#ifndef SORT_H
#define SORT_H
#include <stddef.h>
#include <stdint.h>
typedef struct { int key; int tag; } Record;
typedef struct { uint64_t comparisons, moves; unsigned max_depth; size_t buffer_bytes; } Stats;
void mergeSort(Record *, size_t, Stats *);
void quickSort(Record *, size_t, Stats *);
void heapSort(Record *, size_t, Stats *);
void randomQuickSort(Record *, size_t, uint64_t);
int parallelQuickSort(Record *, size_t, uint64_t, int);
#endif
