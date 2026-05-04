// ============================================================================
// fat32.c - Core FAT32 Filesystem (Part 1 of 3)
// ============================================================================
// Contains: initialization, format, FAT management, and helper functions.
// These helpers are used by fat32_dir.c and fat32_file.c which are separate
// compilation units linked together with this module.
// ============================================================================

#include "storage/fat32.h"
#include "io/kprint.h"
#include "io/terminal.h"
#include "core/malloc.h"

// ============================================================================
// MODULE STATE (static variables - shared only within this file and linked modules)
// ============================================================================
static fat32_bpb_t bpb;
static ata_drive_t* current_drive = NULL;
static u32 data_start_sector;
static u32 fat_start_sector;
static u32 current_dir_cluster;

// ============================================================================
// MODULE HELPERS (used by fat32_dir.c and fat32_file.c)
// These are NOT static so they can be called from fat32_dir.c and fat32_file.c
// ============================================================================
u32 _fat32_cluster_to_lba(u32 cluster);
void _fat32_name_to_83(const char* name, u8* dest);
u32 _fat32_get_fat_entry(u32 cluster);
void _fat32_set_fat_entry(u32 cluster, u32 value);
u32 _fat32_find_free_cluster(void);
void _fat32_create_entry(const char* name, u8 attr, u32 first_cluster, u32 size);
int _fat32_find_entry(const char* name, fat32_dir_entry_t* out_entry);

// External accessors for module state
u32 _fat32_get_current_dir_cluster(void) { return current_dir_cluster; }
void _fat32_set_current_dir_cluster(u32 c) { current_dir_cluster = c; }
ata_drive_t* _fat32_get_current_drive(void) { return current_drive; }
fat32_bpb_t* _fat32_get_bpb(void) { return &bpb; }

// ============================================================================
// PUBLIC API: Initialize FAT32 filesystem from a drive
// ============================================================================
void fat32_init(ata_drive_t* drive) {
    if (!drive || !drive->exists) return;
    
    current_drive = drive;
    u16* buf = (u16*)kmalloc(512);
    ata_read_sectors(drive, 0, 1, buf);
    
    u8* byte_buf = (u8*)buf;
    for (int i = 0; i < 512; i++) {
        ((u8*)&bpb)[i] = byte_buf[i];
    }
    
    fat_start_sector = bpb.reserved_sector_count;
    data_start_sector = bpb.reserved_sector_count + (bpb.num_fats * bpb.sectors_per_fat_32);
    current_dir_cluster = bpb.root_cluster;
    
    kprint("FAT32 Initialized on ");
    kprint(drive->name);
    kprint("\n");
    kprint("  OEM Name: ");
    for(int i=0; i<8; i++) terminal_putchar(bpb.oem_name[i]);
    kprint("\n");
    kprint("  Total Sectors: "); kprint_dec(bpb.total_sectors_32); kprint("\n");
    
    kfree(buf);
}

// ============================================================================
// PUBLIC API: Format a drive as FAT32
// ============================================================================
int fat32_format(ata_drive_t* drive) {
    if (!drive || !drive->exists) return 0;
    
    kprint("Formatting "); kprint(drive->name); kprint(" as FAT32...\n");
    
    u16* buf = (u16*)kmalloc(512);
    u8* b = (u8*)buf;
    for(int i=0; i<512; i++) b[i] = 0;
    
    // Minimal BPB for 10MB drive
    fat32_bpb_t* f = (fat32_bpb_t*)buf;
    f->boot_jmp[0] = 0xEB; f->boot_jmp[1] = 0x3C; f->boot_jmp[2] = 0x90;
    for(int i=0; i<8; i++) f->oem_name[i] = "SIMPLEOS"[i];
    f->bytes_per_sector = 512;
    f->sectors_per_cluster = 1;
    f->reserved_sector_count = 128;
    f->num_fats = 2;
    f->media_type = 0xF8;
    f->total_sectors_32 = 20480;
    f->sectors_per_fat_32 = 160; 
    f->root_cluster = 2;
    f->boot_signature = 0x29;
    f->volume_id = 0x12345678;
    for(int i=0; i<11; i++) f->volume_label[i] = "SIMPLE OS  "[i];
    for(int i=0; i<8; i++) f->fs_type[i] = "FAT32   "[i];
    f->boot_sig_2 = 0xAA55;
    
    kprint(" Writing Boot Sector...\n");
    ata_write_sectors(drive, 0, 1, buf);
    
    kprint(" Clearing FAT Tables...\n");
    for(int i=0; i<512; i++) b[i] = 0;
    u32* fat = (u32*)buf;
    fat[0] = 0x0FFFFFF8;
    fat[1] = 0x0FFFFFFF;
    fat[2] = 0x0FFFFFFF;
    
    ata_write_sectors(drive, 128, 1, buf);
    ata_write_sectors(drive, 128 + 160, 1, buf);
    
    fat[0] = 0; fat[1] = 0; fat[2] = 0;
    for(int i=1; i<160; i++) {
        ata_write_sectors(drive, 128 + i, 1, buf);
        ata_write_sectors(drive, 128 + 160 + i, 1, buf);
    }
    
    kprint(" Creating Root Directory...\n");
    for(int i=0; i<512; i++) b[i] = 0;
    ata_write_sectors(drive, 128 + (2 * 160), 1, buf);
    
    kfree(buf);
    kprint("Format Complete!\n");
    return 1;
}

// ============================================================================
// HELPER: Convert cluster number to LBA
// ============================================================================
u32 _fat32_cluster_to_lba(u32 cluster) {
    return data_start_sector + (cluster - 2) * bpb.sectors_per_cluster;
}

// ============================================================================
// HELPER: Convert filename to FAT32 8.3 format
// ============================================================================
void _fat32_name_to_83(const char* name, u8* dest) {
    for(int i=0; i<11; i++) dest[i] = ' ';
    int i = 0, j = 0;
    while(name[i] && name[i] != '.' && j < 8) {
        char c = name[i++];
        if(c >= 'a' && c <= 'z') c -= 32;
        dest[j++] = c;
    }
    if(name[i] == '.') {
        i++;
        j = 8;
        while(name[i] && j < 11) {
            char c = name[i++];
            if(c >= 'a' && c <= 'z') c -= 32;
            dest[j++] = c;
        }
    }
}

// ============================================================================
// HELPER: Read a FAT entry
// ============================================================================
u32 _fat32_get_fat_entry(u32 cluster) {
    u32 fat_offset = cluster * 4;
    u32 fat_sector = fat_start_sector + (fat_offset / 512);
    u32 ent_offset = fat_offset % 512;

    u16* buf = (u16*)kmalloc(512);
    if (!buf) return 0x0FFFFFFF;

    ata_read_sectors(current_drive, fat_sector, 1, buf);
    u32 entry = *(u32*)((u8*)buf + ent_offset);
    kfree(buf);
    return entry & 0x0FFFFFFF;
}

// ============================================================================
// HELPER: Write a FAT entry
// ============================================================================
void _fat32_set_fat_entry(u32 cluster, u32 value) {
    u32 fat_offset = cluster * 4;
    u32 fat_sector = fat_start_sector + (fat_offset / 512);
    u32 ent_offset = fat_offset % 512;

    u16* buf = (u16*)kmalloc(512);
    if (!buf) return;

    ata_read_sectors(current_drive, fat_sector, 1, buf);
    *(u32*)((u8*)buf + ent_offset) = (value & 0x0FFFFFFF);
    
    for(int i=0; i<bpb.num_fats; i++) {
        ata_write_sectors(current_drive, fat_sector + (i * bpb.sectors_per_fat_32), 1, buf);
    }
    kfree(buf);
}

// ============================================================================
// HELPER: Find a free cluster
// ============================================================================
u32 _fat32_find_free_cluster(void) {
    u32 total_clusters = bpb.total_sectors_32 / bpb.sectors_per_cluster;
    u16* buf = (u16*)kmalloc(512);
    if (!buf) return 0;

    u32 current_fat_sector = 0xFFFFFFFF;

    for (u32 i = 2; i < total_clusters; i++) {
        u32 fat_offset = i * 4;
        u32 fat_sector = fat_start_sector + (fat_offset / 512);
        u32 ent_offset = fat_offset % 512;

        if (fat_sector != current_fat_sector) {
            ata_read_sectors(current_drive, fat_sector, 1, buf);
            current_fat_sector = fat_sector;
        }

        u32 entry = *(u32*)((u8*)buf + ent_offset);
        if ((entry & 0x0FFFFFFF) == 0) {
            kfree(buf);
            return i;
        }
    }
    kfree(buf);
    return 0;
}

// ============================================================================
// HELPER: Create a directory entry
// ============================================================================
void _fat32_create_entry(const char* name, u8 attr, u32 first_cluster, u32 size) {
    u16* buf = (u16*)kmalloc(bpb.sectors_per_cluster * 512);
    if (!buf) return;

    ata_read_sectors(current_drive, _fat32_cluster_to_lba(current_dir_cluster), bpb.sectors_per_cluster, buf);
    fat32_dir_entry_t* entries = (fat32_dir_entry_t*)buf;
    int max_entries = (bpb.sectors_per_cluster * 512) / sizeof(fat32_dir_entry_t);

    for (int i = 0; i < max_entries; i++) {
        if (entries[i].name[0] == 0x00 || entries[i].name[0] == 0xE5) {
            _fat32_name_to_83(name, entries[i].name);
            entries[i].attr = attr;
            entries[i].cluster_hi = (u16)(first_cluster >> 16);
            entries[i].cluster_lo = (u16)(first_cluster & 0xFFFF);
            entries[i].file_size = size;
            ata_write_sectors(current_drive, _fat32_cluster_to_lba(current_dir_cluster), bpb.sectors_per_cluster, buf);
            kfree(buf);
            return;
        }
    }
    kfree(buf);
}

// ============================================================================
// HELPER: Find a directory entry
// ============================================================================
int _fat32_find_entry(const char* name, fat32_dir_entry_t* out_entry) {
    u8 fat_name[11];
    _fat32_name_to_83(name, fat_name);

    u16* buf = (u16*)kmalloc(bpb.sectors_per_cluster * 512);
    if (!buf) return 0;
    
    ata_read_sectors(current_drive, _fat32_cluster_to_lba(current_dir_cluster), bpb.sectors_per_cluster, buf);
    fat32_dir_entry_t* entries = (fat32_dir_entry_t*)buf;
    int max_entries = (bpb.sectors_per_cluster * 512) / sizeof(fat32_dir_entry_t);

    for (int i = 0; i < max_entries; i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;
        
        int match = 1;
        for(int j=0; j<11; j++) if(entries[i].name[j] != fat_name[j]) match = 0;
        if (match) {
            if (out_entry) *out_entry = entries[i];
            kfree(buf);
            return 1;
        }
    }
    kfree(buf);
    return 0;
}
