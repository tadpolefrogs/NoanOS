#include "cpu/task.h"
#include "core/malloc.h"
#include "cpu/gdt.h"
#include "io/kprint.h"

static process_t* process_list = NULL;
static process_t* current_process = NULL;
static int next_pid = 1;

void task_init(void) {
    current_process = (process_t*)kmalloc(sizeof(process_t));
    current_process->id = next_pid++;
    current_process->state = TASK_RUNNING;
    current_process->kstack = 0x9C00; 
    current_process->ustack = 0;
    current_process->next = current_process;
    process_list = current_process;
}

void task_create(void* entry, u32 flags) {
    process_t* proc = (process_t*)kmalloc(sizeof(process_t));
    if (!proc) return;
    
    proc->id = next_pid++;
    proc->state = TASK_READY;
    
    void* k_ptr = kmalloc(STACK_SIZE);
    if (!k_ptr) return;
    proc->kstack = (u32)k_ptr + STACK_SIZE;
    
    void* u_ptr = kmalloc(STACK_SIZE);
    if (!u_ptr) return;
    proc->ustack = (u32)u_ptr + STACK_SIZE;

    u32* stack = (u32*)proc->kstack;
    // ... rest of the setup

    // --- PUSH THE IRET FRAME (Unified: Always 5 dwords) ---
    // Stack for iret: SS, ESP, EFLAGS, CS, EIP
    u16 ss = (flags & 0x1) ? 0x23 : 0x10;
    u32 esp_val = (flags & 0x1) ? proc->ustack : (u32)stack;
    
    *(--stack) = ss;
    *(--stack) = esp_val;
    *(--stack) = 0x202; // EFLAGS
    *(--stack) = (flags & 0x1) ? 0x1B : 0x08; // CS
    *(--stack) = (u32)entry; // EIP

    // --- PUSH DUMMY ERROR CODE & INT NO ---
    *(--stack) = 0; // err_code
    *(--stack) = 0; // int_no

    // --- PUSH REGISTERS FOR PUSHA ---
    *(--stack) = 0; // EAX
    *(--stack) = 0; // ECX
    *(--stack) = 0; // EDX
    *(--stack) = 0; // EBX
    *(--stack) = 0; // ESP (dummy)
    *(--stack) = 0; // EBP
    *(--stack) = 0; // ESI
    *(--stack) = 0; // EDI

    // --- PUSH DS ---
    *(--stack) = (flags & 0x1) ? 0x23 : 0x10;

    proc->esp = (u32)stack;

    // Add to list
    proc->next = process_list->next;
    process_list->next = proc;
}

u32 task_switch(u32 esp) {
    if (!current_process) return esp;

    // Save current task's esp
    current_process->esp = esp;

    // Move to next task in the circular list
    process_t* next = current_process->next;
    
    // Safety check: if we're pointing to ourselves, don't switch
    if (next == current_process) {
        current_process->state = TASK_RUNNING;
        return esp;
    }
    
    // Find next task that's not terminated (with safety limit)
    process_t* start = next;
    int iterations = 0;
    while (next->state == TASK_TERMINATED && iterations < 100) {
        next = next->next;
        iterations++;
        if (next == start) {
            // All tasks terminated, stay on current
            current_process->state = TASK_RUNNING;
            return esp;
        }
    }

    // Switch to next
    current_process = next;
    current_process->state = TASK_RUNNING;

    tss_set_stack(current_process->kstack);
    return current_process->esp;
}

void task_yield(void) {
    asm volatile("int $0x20");
}

process_t* get_current_process(void) {
    return current_process;
}
