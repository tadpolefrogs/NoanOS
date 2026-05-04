#ifndef MALLOC_H
#define MALLOC_H

#include "core/types.h"

typedef struct {
    size_t total_size;
    size_t used_size;
    size_t free_size;
    u32 total_blocks;
    u32 free_blocks;
} malloc_info_t;

void malloc_init(u32 start_addr, u32 size);
void* kmalloc(size_t size);
void kfree(void* ptr);

// For debugging
void malloc_stats(void);
void get_malloc_info(malloc_info_t* info);

#endif
