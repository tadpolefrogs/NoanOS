#include "include/kprint.h"
#include "include/terminal.h"
#include "include/idt.h"
#include "include/keyboard.h"
#include "include/shell.h"
#include "include/malloc.h"
#include "include/ata.h"
#include "include/fat32.h"
#include "include/installer.h"
#include "include/timer.h"
#include "include/gdt.h"
#include "include/task.h"
#include "include/syscall.h"

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
    
    kprint("Spawning User Mode Shell...\n");
    // Flag 0x1 indicates User Mode
    task_create(shell_main, 0x1);
    
    kprint("Kernel entering idle loop...\n");
    while(1) {
        asm volatile("hlt");
    }
}
