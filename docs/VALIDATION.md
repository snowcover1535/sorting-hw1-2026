# Validation record
- Test-first stub run: 1838 checks, 1544 failures (expected sorting behavior missing).
- Implemented sorting suite: 1837 checks, 0 failures. The prior extra failing-large-input assertion is invoked only if a violation exists, hence the check-count difference.
- GCC 13.3.0, -std=c17 -Wall -Wextra -Wpedantic -O2 -fopenmp: clean build.
- ASan + UBSan passed. Initial LeakSanitizer failed due to /proc/<pid>/task access restrictions; final sanitizer target explicitly disables leak detection. No leak-check success claimed.
- Counts: 60 rows; primary timed: 420 rows; parallel timed: 224 rows. Every row passed qsort-key and original-record-permutation checks.
- Parallel actual team sizes: 1, 2, 4 as requested.
- Independent code review found no correctness blocker; reporting caveats applied: fixed input/seed, partial move accounting, stack vs buffers, shared VM/quota, residual recursion-depth updates in time build.
- Compiler/record size metadata describe this execution (GCC 13.3.0, Record 8 bytes); those constants should be revisited on a different ABI or edited build flags.
- PDF rendered to page images and reviewed; seven pages.
- Repository created at https://github.com/snowcover1535/sorting-hw1-2026; actual URL added to PDF and report/submission.json.

- Final report review corrected critical-path wording for the fixed task depth budget; PDF uses supported Korean prose in place of missing Greek glyphs; Markdown table separators escaped.

## 2026-09-30 build-command compatibility update
- Added `make debug` with -O0 -g and OpenMP; readelf confirmed .debug_info.
- Dockerfile includes python3-matplotlib, python3-numpy, python3-reportlab and fonts-dejavu-core. Existing Codespaces require rebuilding the container or installing these packages once.
- Plot dependencies are imported before summary outputs are written.
- Fresh-copy sequence `make clean`, `make run`, `make test`, `make charts`, `make debug`, `make clean`: all exit codes 0 in the available Ubuntu 24.04/GCC environment. Test checks=1837, failures=0. All three PNG/SVG pairs generated; final clean removed executables.
- Two attempts in the synchronized workspace produced truncated measurement files despite a successful child exit. Repeating in /tmp produced the complete 60/420/224 rows and passed the entire sequence. The original submission data were not replaced.
- Docker/Podman are unavailable in the validation environment; a new Debian Codespaces container build was not executed here.
- This update preserves the original measured data, graphs and PDF.
