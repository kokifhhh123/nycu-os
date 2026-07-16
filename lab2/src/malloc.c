#include "malloc.h"
#include "utils.h"
#include <stdint.h>

extern char __kernel_end[];
static uintptr_t heap_current;

void heap_init() {
    heap_current = alignup((uintptr_t)__kernel_end, 16);
}

void* simple_malloc(unsigned int size) {
    void *res = (void*)heap_current;
    heap_current += size;
    return res;
}