#include "core/panic.h"
#include "io/kprint.h"
#include "io/terminal.h"

void panic(const char* message) {
    // Disable interrupts to prevent further system activity
    asm volatile("cli");

    // Set terminal color to White on Red
    terminal_set_color(vga_entry_color(15, 4)); // 15 = White, 4 = Red
    
    kprint("\n\n");
    kprint("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    kprint("!!                                KERNEL PANIC                                !!\n");
    kprint("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    kprint("\nReason: ");
    kprint(message);
   
    kprint("Please reboot your computer.\n");

    // Halt the CPU
    while (1) {
        asm volatile("hlt");
    }
}
