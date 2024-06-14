#ifndef KSORT_H
#define KSORT_H

#include <linux/types.h>
#define QSORT 0
#define LIBSORT 1
#define TIMSORT 2
#define PDQSORT 3
typedef int cmp_t(const void *, const void *);

extern struct workqueue_struct *workqueue;

void sort_main(void *sort_buffer,
               size_t size,
               size_t es,
               cmp_t cmp,
               int sort_type);

#endif
