"""Standard-library-only runner; stdout logs, CSV data under results/."""
from pathlib import Path
import datetime, hashlib, json, os, platform, subprocess, time
ROOT=Path(__file__).resolve().parents[1]
def log(pct,status,start,stage):
    print(f'[{datetime.datetime.now().astimezone().isoformat(timespec="seconds")}] progress={pct}% status={status} stage_s={time.perf_counter()-stage:.3f} total_s={time.perf_counter()-start:.3f}',flush=True)
def main():
    start=stage=time.perf_counter(); log(0,'environment',start,stage)
    os.chdir(ROOT); out=ROOT/'results'; out.mkdir(exist_ok=True)
    env=os.environ.copy(); env.update(OMP_DYNAMIC='FALSE',OMP_PROC_BIND='spread',OMP_PLACES='cores',OMP_WAIT_POLICY='PASSIVE',TZ='Asia/Seoul')
    metadata={'measured_at':datetime.datetime.now().astimezone().isoformat(),'platform':platform.platform(),'compiler':subprocess.check_output(['gcc','--version'],text=True).splitlines()[0],'flags':'-std=c17 -Wall -Wextra -Wpedantic -O2 -fopenmp','logical_cpus':os.cpu_count(),'affinity':sorted(os.sched_getaffinity(0)) if hasattr(os,'sched_getaffinity') else None,'record_bytes':8,'repeats':7,'input_seed':20260929,'pivot_seed':20260929,'omp':{k:env[k] for k in ['OMP_DYNAMIC','OMP_PROC_BIND','OMP_PLACES','OMP_WAIT_POLICY']},'sources_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted((ROOT/'src').glob('*.[ch]'))}}
    for name in ['cpu.max','cpu.stat','cpuset.cpus.effective']:
        p=Path('/sys/fs/cgroup')/name
        metadata[name]=p.read_text().strip() if p.exists() else 'unavailable'
    metadata['lscpu']=subprocess.getoutput('lscpu')
    (out/'environment.json').write_text(json.dumps(metadata,indent=2,ensure_ascii=False)+'\n')
    jobs=[('src/count.out','main','counts.csv'),('src/main.out','main','timings.csv'),('src/main.out','parallel','parallel.csv')]
    for i,(exe,suite,filename) in enumerate(jobs):
        stage=time.perf_counter();log(round(i/3*100),f'begin {filename}',start,stage)
        with (out/filename).open('w') as data,(out/(filename+'.log')).open('w') as logs:
            # Child stderr already includes per-trial progress and stage durations.
            process=subprocess.Popen([str(ROOT/exe),suite],stdout=data,stderr=subprocess.PIPE,text=True,env=env)
            for line in process.stderr:
                print(line,end='',flush=True);logs.write(line);logs.flush()
            if process.wait()!=0:raise RuntimeError(f'{filename}: benchmark failed')
        log(round((i+1)/3*100),f'finished {filename}',start,stage)
    p=Path('/sys/fs/cgroup/cpu.stat');metadata['cpu.stat.after']=p.read_text() if p.exists() else 'unavailable'
    metadata['duration_s']=time.perf_counter()-start
    (out/'environment.json').write_text(json.dumps(metadata,indent=2,ensure_ascii=False)+'\n')
if __name__=='__main__':main()
