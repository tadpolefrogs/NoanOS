#include "io/kprint.h"
#include "io/terminal.h"
#include "cpu/idt.h"
#include "io/keyboard.h"
#include "shell/shell.h"
#include "core/malloc.h"
#include "storage/ata.h"
#include "storage/fat32.h"
#include "shell/installer.h"
#include "system/timer.h"
#include "cpu/gdt.h"
#include "cpu/task.h"
#include "cpu/syscall.h"

void kmain(u32 boot_drive) {
    kprint_init();
    
    kprint("Initializing Malloc (4MB heap)...\n");
    malloc_init(0x200000, 4 * 1024 * 1024);
    kprint("Initializing GDT & TSS...\n");
    gdt_init();
    kprint("Initializing IDT...\n");
    idt_init();
    kprint("Initializing Syscalls...\n");
    syscall_init();
    kprint("Initializing Task Management...\n");
    task_init();
    kprint("Initializing Timer (100Hz)...\n");
    timer_init(100);
    kprint("Initializing Keyboard...\n");
    keyboard_init();
    kprint("Initializing ATA Disk Driver...\n");
    ata_init();
    kprint("Enabling Interrupts...\n");
    asm volatile("sti");

    // Check if we should run the installer (Live Mode)
    int is_live = 0;
    int drive_idx = (boot_drive >= 0x80) ? (boot_drive - 0x80) : 0;
    ata_drive_t* boot_disk = ata_get_drive(drive_idx);
    
    if (boot_disk && boot_disk->exists) {
        u16* buf = (u16*)kmalloc(512);
        ata_read_sectors(boot_disk, 255, 1, buf);
        if (buf[0] == 0x494C && buf[1] == 0x4556) {
            is_live = 1;
        }
        kfree(buf);
    }

    if (is_live) {
        installer_start(boot_drive);
    } else {
        kprint("Booting from installed system...\n");
        if (boot_disk && boot_disk->exists) {
            fat32_init(boot_disk);
        }
    }
    
    kprint("Spawning Shell...\n");
    // Flag 0x1 indicates User Mode (Ring 3)
    task_create(shell_main, 0x1);
    
    while(1) {
        asm volatile("hlt");
    }
}
