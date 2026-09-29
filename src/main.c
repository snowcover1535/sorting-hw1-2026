#include "sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <omp.h>
static uint64_t rng(uint64_t*s){*s=*s*UINT64_C(6364136223846793005)+UINT64_C(1442695040888963407);return *s;}
static const char*shapes[]={"random","sorted","reversed","few_unique","equal"};
static const char*names[]={"merge","quick_first","heap","quick_random","omp_1","omp_2","omp_4"};
static void input(Record*a,size_t n,int shape){
 uint64_t s=20260929+(uint64_t)n*17+(unsigned)shape;
 for(size_t i=0;i<n;i++)a[i].key=shape==2?(int)(n-i):shape==3?(int)((rng(&s)>>32)%16):shape==4?7:(int)i;
 if(shape==0)for(size_t i=n;i>1;i--){size_t j=(size_t)((rng(&s)>>16)%i);int t=a[i-1].key;a[i-1].key=a[j].key;a[j].key=t;}
 for(size_t i=0;i<n;i++)a[i].tag=(int)i;
}
static int compare(const void*x,const void*y){int a=((const Record*)x)->key,b=((const Record*)y)->key;return (a>b)-(a<b);}
static int verify(Record*a,Record*base,Record*ref,size_t n,unsigned char*seen){
 memset(seen,0,n);for(size_t i=0;i<n;i++)if(a[i].key!=ref[i].key||a[i].tag<0||(size_t)a[i].tag>=n||seen[a[i].tag]++||a[i].key!=base[a[i].tag].key)return 0;return 1;
}
static int stable(Record*a,size_t n){for(size_t i=1;i<n;i++)if(a[i-1].key==a[i].key&&a[i-1].tag>a[i].tag)return 0;return 1;}
static void logProgress(int done,int total,const char*stage,double start,double step){
 char stamp[40];time_t t=time(NULL);strftime(stamp,sizeof(stamp),"%Y-%m-%dT%H:%M:%S%z",localtime(&t));
 fprintf(stderr,"[%s] progress=%.1f%% (%d/%d) status=%s stage_s=%.3f total_s=%.3f\n",stamp,100.0*done/total,done,total,stage,step,omp_get_wtime()-start);
}
int main(int argc,char**argv){
 if(argc!=2||(strcmp(argv[1],"main")&&strcmp(argv[1],"parallel"))){fprintf(stderr,"usage: %s main|parallel\n",argv[0]);return 2;}
 int par=!strcmp(argv[1],"parallel"),reps=7;
 #ifdef COUNT_OPS
 if(par){fprintf(stderr,"Counts are serial only\n");return 2;}reps=1;
 #endif
 const size_t small[]={1000,2000,4000,8000},large[]={10000,100000,1000000,4000000};
 int nshape=par?2:5,nalg=par?4:3,total=4*nshape*nalg*reps,done=0;double start=omp_get_wtime();
 printf("suite,shape,n,algorithm,rep,wall_ms,comparisons,moves,buffer_bytes,max_depth,stable,correct,actual_threads\n");
 for(int si=0;si<4;si++)for(int shape=0;shape<nshape;shape++){
  size_t n=par?large[si]:small[si];Record*base=malloc(n*sizeof(*base)),*a=malloc(n*sizeof(*a)),*ref=malloc(n*sizeof(*ref));unsigned char*seen=malloc(n);
  if(!base||!a||!ref||!seen){fprintf(stderr,"allocation failed\n");return 2;}
  input(base,n,shape);memcpy(ref,base,n*sizeof(*ref));qsort(ref,n,sizeof(*ref),compare);
  /* One warm-up for every algorithm/condition; not written to CSV. */
  for(int rep=-1;rep<reps;rep++)for(int k=0;k<nalg;k++){
   int alg=(k+(rep<0?0:rep))%nalg+(par?3:0),actual=1;Stats s={0};memcpy(a,base,n*sizeof(*a));
   double t=omp_get_wtime();
   if(alg==0)mergeSort(a,n,&s);else if(alg==1)quickSort(a,n,&s);else if(alg==2)heapSort(a,n,&s);
   else if(alg==3)randomQuickSort(a,n,20260929);else actual=parallelQuickSort(a,n,20260929,alg==4?1:alg==5?2:4);
   double ms=(omp_get_wtime()-t)*1000;
   int correct=verify(a,base,ref,n,seen),st=stable(a,n);if(!correct||(alg==0&&!st)){fprintf(stderr,"verification failed\n");return 3;}
   if(rep>=0){printf("%s,%s,%zu,%s,%d,%.9f,%llu,%llu,%zu,%u,%d,%d,%d\n",argv[1],shapes[shape],n,names[alg],rep,ms,(unsigned long long)s.comparisons,(unsigned long long)s.moves,s.buffer_bytes,s.max_depth,st,correct,actual);fflush(stdout);
    char desc[100];snprintf(desc,sizeof(desc),"%s/%s/n=%zu/repeat=%d",names[alg],shapes[shape],n,rep+1);logProgress(++done,total,desc,start,ms/1000);
   }
  }
  free(base);free(a);free(ref);free(seen);
 }
 return 0;
}
