# NoanOS Architecture

This document provides a technical overview of the NoanOS architecture and its core components.

## Boot Process

1.  **Stage 1 (boot.asm):** The BIOS loads the first sector (512 bytes) of the boot disk into memory at address `0x7C00`. This stage switches the CPU to Protected Mode (32-bit) and loads the kernel binary from the subsequent sectors into memory at `0x100000` (1MB).
2.  **Kernel Entry (kernel.asm):** Sets up a temporary stack and calls the C entry point `kmain`.
3.  **Kernel Initialization (kmain):**
    - Initializes VGA terminal and printing utilities.
    - Sets up the Heap (Malloc) at `0x200000`.
    - Configures the Global Descriptor Table (GDT) and Task State Segment (TSS).
    - Configures the Interrupt Descriptor Table (IDT) and sets up ISRs/IRQs.
    - Initializes System Calls.
    - Starts Task Management and the System Timer (PIT).
    - Initializes the Keyboard and ATA Disk drivers.
    - Enables interrupts.
4.  **Live Mode Detection:** Checks sector 255 of the boot drive for a "LIVE" signature. If found, it launches the installer; otherwise, it mounts the FAT32 filesystem from the boot drive.
5.  **User Mode Shell:** Creates the first user-mode task to run the shell.

## Memory Management

- **Physical Memory:** NoanOS uses a flat memory model.
- **Heap Allocation:** A simple linked-list based memory allocator manages a 4MB heap starting at `0x200000`.
- **GDT:** Defines segments for Kernel Code/Data and User Code/Data. Includes a TSS for hardware task switching (though software task switching is primarily used).

## CPU & Interrupts

- **IDT:** Handles processor exceptions (0-31) and hardware interrupts (32-47).
- **System Calls:** Implemented via `int 0x80`. Provides an interface for user-mode tasks to interact with the kernel.
- **Task Management:** Supports basic multitasking. Each task has its own stack and execution context.

## Storage & Filesystem

- **ATA Driver:** Communicates with IDE/PATA hard drives using PIO mode. Supports reading and writing sectors.
- **FAT32:** A full implementation of the FAT32 filesystem, including support for long file names (LFN), subdirectories, and file CRUD operations.

## Input/Output

- **Keyboard:** PS/2 keyboard driver handling scan codes and converting them to ASCII characters.
- **Terminal:** VGA text mode driver (80x25) with support for scrolling and basic colors.
- **Kprint:** A `printf`-like utility for kernel logging.

## System Services

- **Timer:** Uses the PIT (Programmable Interval Timer) to provide a 100Hz system tick for task switching and uptime tracking.
- **RTC:** Accesses the Real-Time Clock to provide current date and time.
- **Power Management:** Basic ACPI/APM support for shutdown and reboot.
