# Sorting comparison implementation plan
Goal: measured, reproducible assignment with honest limitations.
Spec: docs/DESIGN.md
1. Define sorting API and tests (qsort/permutation, edge cases, stability, counts, parallel agreement). Run stubs to observe failure. Implement and pass; run sanitizers.
2. Implement fixed-seed benchmark with timing/counts separation, logging and environment capture. Run primary and parallel experiments; aggregate median/IQR without discarding outliers.
3. Generate figures and Korean 8-page PDF, including AI learning text and sources. Render all pages, check pagination, numbers and fonts.
4. Independent review of code/results/report, resolve substantive findings. Bundle reproducible source, data, PDF and submission instructions. Save deliverables. GitHub publication remains pending if no authenticated connection exists.
Review focus: empty inputs; equal keys; full permutation vs sortedness; RNG thread races; CPU quota and wall-time interpretation; absent repository URL.
