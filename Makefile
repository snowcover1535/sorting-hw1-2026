CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -O2 -fopenmp
.PHONY: all test run bench charts clean sanitize
all: test
src/main.out: src/main.c src/sort.c src/sort.h
	$(CC) $(CFLAGS) -Isrc src/main.c src/sort.c -o $@
src/count.out: src/main.c src/sort.c src/sort.h
	$(CC) $(CFLAGS) -DCOUNT_OPS -Isrc src/main.c src/sort.c -o $@
tests/test_sort.out: tests/test_sort.c src/sort.c src/sort.h
	$(CC) $(CFLAGS) -DCOUNT_OPS -Isrc tests/test_sort.c src/sort.c -o $@
test: tests/test_sort.out
	./tests/test_sort.out
run: bench
bench: src/main.out src/count.out
	python3 tools/run_benchmark.py
charts:
	python3 tools/analyze.py
sanitize:
	$(CC) -std=c17 -Wall -Wextra -O1 -g -fopenmp -DCOUNT_OPS -fsanitize=address,undefined -fno-omit-frame-pointer -Isrc tests/test_sort.c src/sort.c -o tests/asan.out
	ASAN_OPTIONS=detect_leaks=0 ./tests/asan.out
clean:
	rm -f src/*.out tests/*.out
