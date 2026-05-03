#include "include/syscall.h"
#include "include/kprint.h"
#include "include/keyboard.h"
#include "include/malloc.h"
#include "include/task.h"
#include "include/fat32.h"
#include "include/terminal.h"
#include "include/power.h"

extern void syscall_stub(void);

u32 syscall_handler(u32 esp) {
    struct registers* regs = (struct registers*)esp;
    u32 num = regs->eax;
    u32 arg1 = regs->ebx;
    u32 arg2 = regs->ecx;
    u32 arg3 = regs->edx;

    int ret = 0;

    switch (num) {
        case SYS_EXIT:
            kprint("Process terminated\n");
            return task_switch(esp);
        case SYS_PRINT:
            kprint((const char*)arg1);
            break;
        case SYS_READ:
            ret = keyboard_getchar();
            break;
        case SYS_MALLOC:
            ret = (u32)kmalloc(arg1);
            break;
        case SYS_FREE:
            kfree((void*)arg1);
            break;
        case SYS_LS:
            fat32_ls();
            break;
        case SYS_CD:
            fat32_cd((const char*)arg1);
            break;
        case SYS_CAT:
            fat32_cat((const char*)arg1);
            break;
        case SYS_PUTCHAR:
            terminal_putchar((char)arg1);
            break;
        case SYS_CLEAR:
            kprint_init();
            break;
        case SYS_YIELD:
            return task_switch(esp);
        case SYS_REBOOT:
            reboot();
            break;
        case SYS_SHUTDOWN:
            shutdown();
            break;
        case SYS_MKDIR:
            fat32_mkdir((const char*)arg1);
            break;
        case SYS_RM:
            fat32_rm((const char*)arg1);
            break;
        case SYS_TOUCH:
            fat32_touch((const char*)arg1);
            break;
        case SYS_ECHO_FILE:
            fat32_echo((const char*)arg1, (const char*)arg2);
            break;
        case SYS_CP:
            fat32_copy((const char*)arg1, (const char*)arg2);
            break;
        case SYS_MV:
            fat32_move((const char*)arg1, (const char*)arg2);
            break;
        case SYS_READ_FILE:
            ret = fat32_read((const char*)arg1, (char*)arg2, arg3);
            break;
        default:
            kprint("Unknown syscall: ");
            kprint_dec(num);
            kprint("\n");
            break;
    }

    regs->eax = (u32)ret;
    return esp;
}

void syscall_init(void) {
    kprint("[Setting up syscall gate 0x80]\n");
    idt_set_gate(0x80, (u32)syscall_stub, 0x08, 0xEE); // Ring 3 accessible
    kprint("[Syscall gate installed]\n");
}
