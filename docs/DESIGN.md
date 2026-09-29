# Sorting experiment design
Request: create code and a 2-10 page Korean PDF comparing two taught sorts and one new sort, with AI learning appendix and a GitHub URL. The user explicitly requested design AND execution without unnecessary questions.
Selected: merge sort, lecture first-pivot quicksort, heapsort; randomized quicksort serial/OpenMP extension.
Primary: tagged integer records, five input shapes, n=1000/2000/4000/8000, seven repeats with fixed inputs per condition. Rotate/shuffle execution order. Median and IQR of wall time. Counts from separately compiled instrumented code, not timed code.
Extension: same randomized two-way partition and per-subtree deterministic seed; serial, OpenMP 1/2/4 threads. Unique random/sorted arrays; n=10000/100000/1000000/4000000, seven repeats. Threshold=16384, depth budget=8. Pool/region overhead included, process startup excluded, one warm-up per condition. No claim of speedup before measurement.
Correctness: qsort key comparison AND full tagged permutation, stable merge, counter known cases, randomized property cases, threaded equivalence, ASan/UBSan.
Memory: theoretical auxiliary arrays and actual recursion depth; no misleading RSS or total-byte claim. Tail-recursion elimination in lecture quicksort bounds stack independently of quadratic work.
Artifacts: C source, Makefile, raw CSVs, environment JSON, plots, reproducible runner, AI appendix, editable report source, PDF, ZIP, submission instructions. GitHub URL stays explicitly pending until a real repository is available.
