# NoanOS Kernel Development

This directory contains the core logic of the NoanOS kernel.

## Architecture

The kernel is structured as follows:

- `core/`: Fundamental kernel logic (entry point, memory allocation).
- `cpu/`: Processor-specific code (GDT, IDT, Tasking, Syscalls).
- `io/`: Drivers for basic I/O (Terminal, Keyboard, Kprint).
- `shell/`: The interactive shell and its built-in commands.
- `storage/`: Disk drivers and filesystem implementations.
- `system/`: Miscellaneous system services (Timer, RTC, Power).

## Important Files

- `core/kernel.c`: The main entry point `kmain`.
- `include/core/types.h`: Essential type definitions.
- `cpu/idt.c`: Interrupt handling logic.
- `storage/fat32.c`: Main filesystem driver.

## Guidelines

- Avoid using standard C library functions unless they are implemented within the kernel.
- Use `kmalloc` and `kfree` for dynamic memory allocation.
- Ensure that interrupts are disabled when performing critical section operations.
- All new kernel features should be documented in `ARCHITECTURE.md`.
