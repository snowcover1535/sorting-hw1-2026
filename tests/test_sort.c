#include "sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <omp.h>
static int checks, failures;
static void check(int ok, const char *name) { checks++; if (!ok) { failures++; fprintf(stderr,"FAIL %s\n",name); } }
static int cmp(const void *x, const void *y) { int a=((const Record*)x)->key,b=((const Record*)y)->key; return (a>b)-(a<b); }
static void run(Record *base, size_t n) {
 Record a[600], expected[600]; unsigned char seen[600]; Stats s;
 memcpy(expected,base,n*sizeof(*base)); qsort(expected,n,sizeof(*base),cmp);
 for (int alg=0;alg<6;alg++) {
  memcpy(a,base,n*sizeof(*base)); memset(seen,0,sizeof(seen)); memset(&s,0,sizeof(s));
  if(alg==0)mergeSort(a,n,&s); else if(alg==1)quickSort(a,n,&s); else if(alg==2)heapSort(a,n,&s);
  else if(alg==3)randomQuickSort(a,n,42); else parallelQuickSort(a,n,42,alg==4?1:4);
  int ok=1,stable=1;
  for(size_t i=0;i<n;i++) { if(a[i].key!=expected[i].key||a[i].tag<0||(size_t)a[i].tag>=n) {ok=0;break;}
   if(seen[a[i].tag]++||base[a[i].tag].key!=a[i].key)ok=0;
   if(i&&a[i-1].key==a[i].key&&a[i-1].tag>a[i].tag)stable=0;
  }
  check(ok,"sorted keys AND tagged permutation"); if(alg==0)check(stable,"merge stable");
 }
}
int main(void) {
 double start=omp_get_wtime(); time_t now=time(NULL);
 fprintf(stderr,"[%ld] progress=0%% stage=correctness elapsed=0s\n",(long)now);
 Record a[600]; unsigned x=1;
 for(size_t n=0;n<=257;n++) {for(size_t i=0;i<n;i++){x=x*1664525u+1013904223u;a[i]=(Record){(int)(x%17)-8,(int)i};}run(a,n);}
 for(int shape=0;shape<4;shape++){ for(int i=0;i<500;i++)a[i]=(Record){shape==0?i:shape==1?500-i:shape==2?7:(i%2?INT_MAX:INT_MIN),i};run(a,500); }
 Stats s={0};for(int i=0;i<10;i++)a[i]=(Record){i,i};quickSort(a,10,&s);
 #ifdef COUNT_OPS
 check(s.comparisons==45,"lecture sorted n=10 comparisons=45");
 #endif
 /* Exercise actual task creation, beyond the threshold. */
 size_t n=100000;Record *b=malloc(n*sizeof(*b)),*c=malloc(n*sizeof(*c));
 check(b&&c,"allocation");if(!b||!c)return 2;
 for(size_t i=0;i<n;i++){x=x*1664525u+1013904223u;b[i]=(Record){(int)(x%100003),(int)i};}
 memcpy(c,b,n*sizeof(*b));randomQuickSort(b,n,2718);parallelQuickSort(c,n,2718,4);
 check(memcmp(b,c,n*sizeof(*b))==0,"parallel same full output as serial");
 for(size_t i=1;i<n;i++)if(b[i-1].key>b[i].key){check(0,"large sorted");break;}
 free(b);free(c);
 fprintf(stderr,"[%ld] progress=100%% stage=correctness elapsed=%.3fs checks=%d failures=%d\n",(long)time(NULL),omp_get_wtime()-start,checks,failures);
 return failures?1:0;
}
