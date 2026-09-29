"""Aggregate existing data without re-running benchmarks. Plots need matplotlib."""
from pathlib import Path
import csv, collections, statistics, json, datetime, time
ROOT=Path(__file__).resolve().parents[1]
def aggregate(rows):
    groups=collections.defaultdict(list)
    for r in rows: groups[(r['suite'],r['shape'],int(r['n']),r['algorithm'])].append(r)
    out=[]
    for key,rs in sorted(groups.items()):
        vals=[float(r['wall_ms']) for r in rs];q=statistics.quantiles(vals,n=4,method='inclusive')
        out.append(dict(zip(['suite','shape','n','algorithm'],key),median_ms=statistics.median(vals),q1_ms=q[0],q3_ms=q[2],min_ms=min(vals),max_ms=max(vals),repeats=len(vals)))
    return out

def main():
    start=time.perf_counter()
    def log(p,s):print(f'[{datetime.datetime.now().astimezone().isoformat(timespec="seconds")}] progress={p}% status={s} stage_s={time.perf_counter()-start:.3f}',flush=True)
    log(0,'aggregate raw CSV');rows=[]
    for f in ['timings.csv','parallel.csv']:
        with (ROOT/'results'/f).open() as fp: rows+=list(csv.DictReader(fp))
    assert len(rows)==644 and all(r['correct']=='1' for r in rows)
    summary=aggregate(rows)
    with (ROOT/'results/summary.csv').open('w',newline='') as fp:
        w=csv.DictWriter(fp,summary[0].keys());w.writeheader();w.writerows(summary)
    counts=list(csv.DictReader((ROOT/'results/counts.csv').open()))
    (ROOT/'results/summary.json').write_text(json.dumps(summary,indent=2))
    log(30,'plot figures')
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    import numpy as np
    plt.rcParams.update({'font.family':'DejaVu Sans','font.size':10,'axes.spines.top':False,'axes.spines.right':False,'savefig.dpi':180})
    colors=['#197c80','#df763a','#5165a4'];algs=['merge','quick_first','heap'];labels=['Merge','Quick (first pivot)','Heap']
    def save(fig,name):
        fig.tight_layout();fig.savefig(ROOT/'report'/f'{name}.png',bbox_inches='tight');fig.savefig(ROOT/'report'/f'{name}.svg',bbox_inches='tight');plt.close(fig)
    fig,ax=plt.subplots(figsize=(8,3.4));shapes=['random','sorted','reversed','few_unique','equal'];xs=np.arange(5)
    for j,alg in enumerate(algs):
        rr=[next(r for r in summary if r['suite']=='main' and r['n']==8000 and r['shape']==s and r['algorithm']==alg) for s in shapes]
        vals=np.array([r['median_ms'] for r in rr]);errs=np.array([[r['median_ms']-r['q1_ms'] for r in rr],[r['q3_ms']-r['median_ms'] for r in rr]])
        ax.bar(xs+(j-1)*.23,vals,.23,label=labels[j],color=colors[j],yerr=errs,capsize=2)
    ax.set_xticks(xs,['Random','Sorted','Reverse','16 values','All equal']);ax.set_yscale('log');ax.set_ylabel('Wall time (ms, log scale)');ax.legend(ncol=3,fontsize=9,loc='upper center',bbox_to_anchor=(.5,1.16),frameon=False);ax.grid(axis='y',alpha=.2);save(fig,'input_shapes')
    fig,axs=plt.subplots(1,2,figsize=(8,3.3));sizes=[1000,2000,4000,8000]
    for j,alg in enumerate(algs):
        for ax,shape in zip(axs,['random','sorted']):
            vals=[int(next(r for r in counts if r['algorithm']==alg and r['shape']==shape and int(r['n'])==n)['comparisons']) for n in sizes]
            ax.plot(sizes,vals,'o-',label=labels[j],color=colors[j]);ax.set_xscale('log',base=2);ax.set_yscale('log');ax.set_title(shape.capitalize());ax.set_xlabel('Records n');ax.set_ylabel('Key comparisons');ax.grid(alpha=.2)
    axs[0].legend(fontsize=8);save(fig,'growth_counts')
    fig,axs=plt.subplots(1,2,figsize=(8,3.4));ps=[10000,100000,1000000,4000000]
    for ax,shape in zip(axs,['random','sorted']):
        for j,alg in enumerate(['omp_1','omp_2','omp_4']):
            vals=[];lo=[];hi=[]
            for n in ps:
                by={a:{int(r['rep']):float(r['wall_ms']) for r in rows if r['suite']=='parallel' and r['shape']==shape and int(r['n'])==n and r['algorithm']==a} for a in ['quick_random',alg]}
                ratios=[by['quick_random'][r]/by[alg][r] for r in range(7)];m=statistics.median(ratios);q=statistics.quantiles(ratios,n=4,method='inclusive');vals.append(m);lo.append(m-q[0]);hi.append(q[2]-m)
            ax.errorbar(ps,vals,yerr=[lo,hi],fmt='o-',capsize=2,label=f'{[1,2,4][j]} thread(s)',color=colors[j])
        ax.axhline(1,color='#888',linestyle='--');ax.set_xscale('log');ax.set_title(shape.capitalize());ax.set_xlabel('Records n');ax.set_ylabel('Speedup vs serial random quick');ax.grid(alpha=.2)
    axs[0].legend(fontsize=8);save(fig,'parallel_speedup');log(100,'summary and figures complete')
if __name__=='__main__':main()
