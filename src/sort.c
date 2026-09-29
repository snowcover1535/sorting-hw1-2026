#include "sort.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#ifdef COUNT_OPS
#define COUNT(s,field,k) do { if(s)(s)->field+=(k); } while(0)
#else
#define COUNT(s,field,k) ((void)(s))
#endif
static int less(Record a,Record b,Stats*s){COUNT(s,comparisons,1);return a.key<b.key;}
static void swap(Record*a,Record*b,Stats*s){if(a==b)return;Record t=*a;*a=*b;*b=t;COUNT(s,moves,3);}
static void depth(Stats*s,unsigned d){if(s&&d>s->max_depth)s->max_depth=d;}
static void mergeRange(Record*a,Record*b,size_t lo,size_t hi,Stats*s,unsigned d){
 depth(s,d);if(hi-lo<2)return;size_t mid=lo+(hi-lo)/2;
 mergeRange(a,b,lo,mid,s,d+1);mergeRange(a,b,mid,hi,s,d+1);
 size_t i=lo,j=mid,k=lo;
 while(i<mid&&j<hi){/* Equality selects the left record: stable. */
  if(less(a[j],a[i],s))b[k++]=a[j++];else b[k++]=a[i++];COUNT(s,moves,1);
 }
 while(i<mid){b[k++]=a[i++];COUNT(s,moves,1);}while(j<hi){b[k++]=a[j++];COUNT(s,moves,1);}
 memcpy(a+lo,b+lo,(hi-lo)*sizeof(*a));COUNT(s,moves,hi-lo);
}
void mergeSort(Record*a,size_t n,Stats*s){
 if(n<2)return;
 Record*b=malloc(n*sizeof(*b));if(!b){perror("merge buffer");exit(2);}
 if(s)s->buffer_bytes=n*sizeof(*b);
 mergeRange(a,b,0,n,s,1);free(b);
}
static size_t partition(Record*a,size_t lo,size_t hi,Stats*s){
 Record pivot=a[lo];size_t i=lo;for(size_t j=lo+1;j<hi;j++)if(less(a[j],pivot,s)){i++;swap(a+i,a+j,s);}swap(a+lo,a+i,s);return i;
}
static void quickRange(Record*a,size_t lo,size_t hi,Stats*s,unsigned d){
 depth(s,d);
 while(hi-lo>1){size_t p=partition(a,lo,hi,s);
  /* Recurse only into the shorter side. Quadratic work still exists,
     but even degenerate inputs cannot cause a linear-depth C stack. */
  if(p-lo<hi-p-1){if(p>lo)quickRange(a,lo,p,s,d+1);lo=p+1;}
  else{if(hi>p+1)quickRange(a,p+1,hi,s,d+1);hi=p;}
 }
}
void quickSort(Record*a,size_t n,Stats*s){if(n>1)quickRange(a,0,n,s,1);}
static void sink(Record*a,size_t root,size_t n,Stats*s){
 while(root<n/2){size_t child=2*root+1;
  if(child+1<n&&less(a[child],a[child+1],s))child++;
  if(!less(a[root],a[child],s))return;
  swap(a+root,a+child,s);root=child;
 }
}
void heapSort(Record*a,size_t n,Stats*s){
 for(size_t i=n/2;i>0;i--)sink(a,i-1,n,s);
 for(size_t end=n;end>1;){swap(a,a+end-1,s);end--;sink(a,0,end,s);}
}
/* SplitMix64; local per-subtree state, no global RNG and no RNG race. */
static uint64_t next(uint64_t*x){uint64_t z=(*x+=UINT64_C(0x9e3779b97f4a7c15));z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9);z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb);return z^(z>>31);}
static size_t randomPartition(Record*a,size_t lo,size_t hi,uint64_t*seed){size_t p=lo+(size_t)(next(seed)%(hi-lo));swap(a+lo,a+p,NULL);return partition(a,lo,hi,NULL);}
static void randomRange(Record*a,size_t lo,size_t hi,uint64_t seed){
 while(hi-lo>1){size_t p=randomPartition(a,lo,hi,&seed);uint64_t left=next(&seed),right=next(&seed);
  if(p-lo<hi-p-1){randomRange(a,lo,p,left);lo=p+1;seed=right;}
  else{randomRange(a,p+1,hi,right);hi=p;seed=left;}
 }
}
void randomQuickSort(Record*a,size_t n,uint64_t seed){randomRange(a,0,n,seed);}
static void taskRange(Record*a,size_t lo,size_t hi,uint64_t seed,int budget){
 if(hi-lo<16384||budget==0){randomRange(a,lo,hi,seed);return;}
 size_t p=randomPartition(a,lo,hi,&seed);uint64_t left=next(&seed),right=next(&seed);
 #pragma omp task firstprivate(a,lo,p,left,budget)
 taskRange(a,lo,p,left,budget-1);
 #pragma omp task firstprivate(a,p,hi,right,budget)
 taskRange(a,p+1,hi,right,budget-1);
 #pragma omp taskwait
}
int parallelQuickSort(Record*a,size_t n,uint64_t seed,int threads){
 int actual=0;omp_set_dynamic(0);
 #pragma omp parallel num_threads(threads) shared(actual)
 {
  #pragma omp single
  {actual=omp_get_num_threads();taskRange(a,0,n,seed,8);}
 }
 return actual;
}
