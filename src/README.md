# NoanOS

NoanOS is a lightweight, 32-bit x86 operating system It features a custom bootloader, a monolithic kernel, a FAT32 filesystem driver, and a basic shell with various utilities.

## Features

- **Custom Bootloader:** A simple stage-1 bootloader written in assembly.
- **Monolithic Kernel:** Handles memory management, tasking, interrupts, and system calls.
- **Memory Management:** Includes a GDT with TSS support and a custom malloc implementation (4MB heap).
- **Task Management:** Basic multitasking support with user-mode tasks.
- **Storage:** ATA PIO driver for hard disk access.
- **Filesystem:** Comprehensive FAT32 support (files, directories, CRUD operations).
- **Shell:** Interactive command-line interface with various built-in commands.
- **Installer:** Live mode support with an automated installer.
- **Drivers:** Keyboard (PS/2), VGA Terminal (Text mode), PIT (Timer), and RTC.

## Directory Structure

- `boot/`: Bootloader source, assembly entry points, and build system.
- `kernel/`: Core kernel source code.
  - `core/`: Kernel entry point (`kmain`), memory allocation, and memory mapping.
  - `cpu/`: GDT, IDT, Syscalls, and Task management.
  - `include/`: Header files for all kernel modules.
  - `io/`: Input/Output drivers (Keyboard, Terminal, Kprint).
  - `shell/`: Shell implementation and user commands.
  - `storage/`: ATA driver and FAT32 filesystem implementation.
  - `system/`: System utilities (Timer, RTC, Power management).

## Build Instructions

### Prerequisites

- `nasm`: Netwide Assembler
- `gcc`: GNU Compiler Collection (configured for `i686-elf` or with `-m32` support)
- `ld`: GNU Linker
- `objcopy`: GNU Binary Utility
- `qemu-system-i386`: For running the OS

### Building

You can build the project using the provided `build.sh` script or by using `make` in the `boot/` directory.

```bash
# Using the build script
./build.sh

# Or using make directly
cd boot
make
```

## Running NoanOS

To run NoanOS in QEMU, use the `run` target in the `boot/` Makefile:

```bash
cd boot
make run
```

This will launch QEMU with a live disk image and a secondary blank hard drive.

## Shell Commands

NoanOS comes with several shell commands:

- `help`: Display available commands.
- `ls`: List directory contents.
- `cd`: Change directory.
- `pwd`: Print working directory.
- `cat`: Display file contents.
- `echo`: Print text to terminal.
- `mkdir`: Create a directory.
- `touch`: Create an empty file.
- `rm`: Remove a file or directory.
- `cp`: Copy a file.
- `mv`: Move or rename a file.
- `clear`: Clear the terminal screen.
- `uptime`: Show system uptime.
- `reboot`: Reboot the system.
- `shutdown`: Shutdown the system.
- `vim`: A simple text editor.

## License

(Add license information here if applicable)
