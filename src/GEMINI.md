# NoanOS Development Guidelines

Welcome to the NoanOS project. This document outlines the standards, conventions, and workflows for developing the NoanOS kernel and its components.

## General Principles

- **Simplicity:** Keep the implementation straightforward and easy to understand.
- **Modularity:** Organise code into logical modules with clear interfaces (headers in `kernel/include`).
- **Safety:** Be mindful of memory management and interrupt safety.

## Coding Conventions

- **Indentation:** Use 4 spaces for indentation.
- **Naming:**
  - Functions: `snake_case` (e.g., `ata_read_sectors`).
  - Variables: `snake_case` (e.g., `boot_drive`).
  - Types: `snake_case` with `_t` suffix (e.g., `ata_drive_t`).
  - Macros: `UPPER_SNAKE_CASE` (e.g., `NULL`).
- **Types:** Use the fixed-width types defined in `kernel/include/core/types.h` (e.g., `u8`, `u16`, `u32`, `size_t`).

## Build System

- The primary build system is `make`, managed within the `boot/` directory.
- `build.sh` is a convenience script that triggers `make clean` and `make run`.
- Always verify changes by running `make` in `boot/`.

## Kernel Modules

### Adding a new Shell Command

1.  Create a new C file in `kernel/shell/commands/` (e.g., `sh_new.c`).
2.  Implement the command function.
3.  Register the command in `kernel/shell/commands/commands.c`.
4.  Update the `Makefile` in `boot/` to include the new C file in `SH_C`.

### Adding a new Kernel Module

1.  Place the implementation in the appropriate subdirectory under `kernel/`.
2.  Add a corresponding header in `kernel/include/`.
3.  Update the `Makefile` in `boot/` to include the new C file in `KERNEL_C`.
4.  Initialize the module in `kmain` if necessary.

## Testing

- Currently, testing is primarily manual via QEMU.
- When adding new features or fixing bugs, ensure they are verified by running the OS and testing the affected functionality in the shell.
