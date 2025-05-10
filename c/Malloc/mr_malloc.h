#ifndef MR_MALLOC_H
#define MR_MALLOC_H

#include <stddef.h>

void* mr_malloc(size_t size);
void mr_free(void* ptr);

void print_memory_map(void);

#endif // MR_MALLOC_H 