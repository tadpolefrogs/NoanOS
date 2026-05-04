#include "core/memory_map.h"
#include "io/kprint.h"

const char* memory_type_to_string(u32 type) {
    switch (type) {
        case MEMORY_TYPE_AVAILABLE:    return "usable";
        case MEMORY_TYPE_RESERVED:     return "reserved";
        case MEMORY_TYPE_ACPI_RECLAIM: return "ACPI reclaim";
        case MEMORY_TYPE_ACPI_NVS:     return "ACPI NVS";
        case MEMORY_TYPE_BAD:          return "bad";
        default:                       return "unknown";
    }
}

void print_memory_map(memory_map_t* map) {
    kprint("BIOS-provided physical RAM map:\n");
    
    for (u32 i = 0; i < map->count; i++) {
        memory_map_entry_t* entry = &map->entries[i];
        
        // Format similar to Linux: "BIOS-e820: [mem 0x0000000000000000-0x000000000009fbff] usable"
        kprint("BIOS-e820: [mem 0x");
        print_hex64(entry->base_addr);
        kprint("-0x");
        print_hex64(entry->base_addr + entry->length - 1);
        kprint("] ");
        kprint(memory_type_to_string(entry->type));
        kprint("\n");
    }
    
    // Print summary
    u64 total_usable = 0;
    u64 total_reserved = 0;
    
    for (u32 i = 0; i < map->count; i++) {
        if (map->entries[i].type == MEMORY_TYPE_AVAILABLE) {
            total_usable += map->entries[i].length;
        } else {
            total_reserved += map->entries[i].length;
        }
    }
    
    kprint("\nMemory summary:\n");
    kprint("  Usable:   ");
    print_size_kb(total_usable);
    kprint("\n  Reserved: ");
    print_size_kb(total_reserved);
    kprint("\n");
}

// Helper function to print 64-bit hex values
void print_hex64(u64 value) {
    char buffer[17];
    buffer[16] = '\0';
    
    for (int i = 15; i >= 0; i--) {
        u8 digit = value & 0xF;
        buffer[i] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value >>= 4;
    }
    
    kprint(buffer);
}

// Helper function to print sizes in KB/MB/GB
void print_size_kb(u64 bytes) {
    u64 kb = bytes / 1024;
    u64 mb = kb / 1024;
    u64 gb = mb / 1024;
    
    if (gb > 0) {
        print_decimal(gb);
        kprint(" GB");
    } else if (mb > 0) {
        print_decimal(mb);
        kprint(" MB");
    } else {
        print_decimal(kb);
        kprint(" KB");
    }
}

// Simple decimal printing function
void print_decimal(u64 value) {
    if (value == 0) {
        kprint("0");
        return;
    }
    
    char buffer[21]; // Max digits for 64-bit number
    int pos = 20;
    buffer[20] = '\0';
    
    while (value > 0) {
        buffer[--pos] = '0' + (value % 10);
        value /= 10;
    }
    
    kprint(&buffer[pos]);
}

// Detection function - this needs to be called from bootloader or early kernel
void detect_memory_map(memory_map_t* map) {
    // This is a simplified example - in real implementation, you'd get this
    // from the bootloader which calls BIOS INT 0x15, EAX=0xE820
    
    map->count = 0;
    
    // Example entries (replace with actual BIOS detection)
    // These would normally come from your bootloader
    
    // Low memory (0-640KB)
    map->entries[map->count].base_addr = 0x00000000;
    map->entries[map->count].length = 0x0009FC00;  // ~639KB
    map->entries[map->count].type = MEMORY_TYPE_AVAILABLE;
    map->count++;
    
    // BIOS/VGA reserved area
    map->entries[map->count].base_addr = 0x0009FC00;
    map->entries[map->count].length = 0x00060400;  // ~385KB
    map->entries[map->count].type = MEMORY_TYPE_RESERVED;
    map->count++;
    
    // Extended memory (1MB+)
    map->entries[map->count].base_addr = 0x00100000;
    map->entries[map->count].length = 0x07EE0000;  // ~126MB (example)
    map->entries[map->count].type = MEMORY_TYPE_AVAILABLE;
    map->count++;
    
    // ACPI tables area (example)
    map->entries[map->count].base_addr = 0x07FE0000;
    map->entries[map->count].length = 0x00020000;  // 128KB
    map->entries[map->count].type = MEMORY_TYPE_ACPI_RECLAIM;
    map->count++;
}

// Usage in your kernel.c:
/*
#include "core/memory_map.h"

void kmain() {
    terminal_init();
    
    kprint("MyOS Kernel v1.0\n");
    kprint("=================\n\n");
    
    // Detect and print memory map
    memory_map_t memory_map;
    detect_memory_map(&memory_map);
    print_memory_map(&memory_map);
    
    kprint("\nKernel initialization complete.\n");
    
    while (1) {
        __asm__("hlt");
    }
}
*/
