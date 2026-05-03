#include "include/power.h"
#include "include/io.h"
#include "include/kprint.h"

void reboot(void) {
    kprint("Rebooting...\n");
    
    // Clear PS/2 output buffer before sending reset command
    while (inb(0x64) & 0x01) {
        inb(0x60);
    }
    
    // Pulse the reset line
    u8 good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE);
    
    // If PS/2 reset fails, try a triple fault (standard fallback)
    kprint("PS/2 reset failed, trying triple fault...\n");
    asm volatile("lidt (,%0,)"::"r"(0));
    asm volatile("int $3");
}

void shutdown(void) {
    kprint("Shutting down...\n");
    // QEMU shutdown
    outw(0x604, 0x2000);
    // VirtualBox shutdown
    outw(0x4004, 0x3400);
    // Bochs/Older QEMU
    outw(0xB004, 0x2000);
}
