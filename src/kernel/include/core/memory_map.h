// memory_map.h
#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include "core/types.h"

#define MEMORY_MAP_MAX_ENTRIES 32

typedef struct {
    u64 base_addr;
    u64 length;
    u32 type;
    u32 attributes;
} __attribute__((packed)) memory_map_entry_t;

typedef struct {
    u32 count;
    memory_map_entry_t entries[MEMORY_MAP_MAX_ENTRIES];
} memory_map_t;

// Memory types (E820 standard)
#define MEMORY_TYPE_AVAILABLE    1
#define MEMORY_TYPE_RESERVED     2
#define MEMORY_TYPE_ACPI_RECLAIM 3
#define MEMORY_TYPE_ACPI_NVS     4
#define MEMORY_TYPE_BAD          5

void print_memory_map(memory_map_t* map);
void detect_memory_map(memory_map_t* map);
const char* memory_type_to_string(u32 type);
void print_hex64(u64 value);
void print_decimal(u64 value);
void print_size_kb(u64 bytes);

#endif
