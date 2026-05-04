# Compiler and tools
ASM = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy

# Directories
BUILD_DIR = build
BIN_DIR = bin

# Flags
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32 -march=i686 -fno-pie -fno-stack-protector -nostdlib -nostartfiles
LDFLAGS = -T linker.ld --oformat elf32-i386 -m elf_i386
ASMFLAGS = -f elf32

# Source files
BOOT_SRC = boot.asm
KERNEL_ASM = kernel.asm interrupts.asm
KERNEL_C = ../kernel/core/kernel.c ../kernel/core/malloc.c ../kernel/core/memory_map.c ../kernel/core/panic.c ../kernel/cpu/gdt.c ../kernel/cpu/idt.c ../kernel/cpu/task.c ../kernel/cpu/syscall.c ../kernel/io/kprint.c ../kernel/io/terminal.c ../kernel/io/keyboard.c ../kernel/storage/ata.c ../kernel/storage/fat32.c ../kernel/storage/fat32_dir.c ../kernel/storage/fat32_file.c ../kernel/storage/elf.c ../kernel/system/timer.c ../kernel/system/rtc.c ../kernel/system/power.c ../kernel/shell/shell.c ../kernel/shell/installer.c
SH_C = ../kernel/shell/commands/sh_uptime.c ../kernel/shell/commands/commands.c ../kernel/shell/commands/sh_ls.c ../kernel/shell/commands/sh_cd.c ../kernel/shell/commands/sh_pwd.c ../kernel/shell/commands/sh_mkdir.c ../kernel/shell/commands/sh_rm.c ../kernel/shell/commands/sh_cp.c ../kernel/shell/commands/sh_mv.c ../kernel/shell/commands/sh_touch.c ../kernel/shell/commands/sh_cat.c ../kernel/shell/commands/sh_echo.c ../kernel/shell/commands/sh_clear.c ../kernel/shell/commands/sh_help.c ../kernel/shell/commands/sh_reboot.c ../kernel/shell/commands/sh_shutdown.c ../kernel/shell/commands/sh_vim.c ../kernel/shell/commands/sh_panic.c ../kernel/shell/commands/sh_date.c ../kernel/shell/commands/sh_mem.c ../kernel/shell/commands/sh_disks.c ../kernel/shell/commands/sh_stat.c

# Output files
BOOT_BIN = $(BIN_DIR)/boot.bin
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BIN_DIR)/kernel.bin

# Object files - extract just the filename
KERNEL_ASM_O = $(patsubst %.asm,$(BUILD_DIR)/%_asm.o,$(KERNEL_ASM))
KERNEL_C_O = $(addprefix $(BUILD_DIR)/,$(notdir $(KERNEL_C:.c=.o)))
SH_C_O = $(addprefix $(BUILD_DIR)/,$(notdir $(SH_C:.c=.o)))

# vpath allows make to find source files in subdirectories
vpath %.c ../kernel/core ../kernel/cpu ../kernel/io ../kernel/storage ../kernel/system ../kernel/shell ../kernel/shell/commands

DISK_IMG = $(BIN_DIR)/disk.img
HDD_IMG = $(BIN_DIR)/hdd.img

# Progress tracking
TOTAL_FILES := $(words $(KERNEL_C) $(SH_C) $(KERNEL_ASM))
CURRENT := 0

# Default target
all: $(DISK_IMG) $(HDD_IMG)
	@printf "\033[0;32mBuild complete!\033[0m\n"

# Create necessary directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build bootloader binary
$(BOOT_BIN): $(BOOT_SRC) | $(BIN_DIR)
	@printf "\033[0;33m[ASM]\033[0m Assembling \033[0;36mboot.asm\033[0m\n"
	@$(ASM) -f bin $(BOOT_SRC) -o $(BOOT_BIN)

# Build kernel assembly object files
$(BUILD_DIR)/%_asm.o: %.asm | $(BUILD_DIR)
	$(eval CURRENT := $(shell echo $$(($(CURRENT)+1))))
	@printf "\033[0;33m[%2d/$(TOTAL_FILES)]\033[0m Assembling \033[0;36m$(<F)\033[0m\n" $(CURRENT)
	@$(ASM) $(ASMFLAGS) $< -o $@

# Build kernel C object files (includes all subdirectories via vpath)
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(eval CURRENT := $(shell echo $$(($(CURRENT)+1))))
	@printf "\033[0;32m[%2d/$(TOTAL_FILES)]\033[0m Compiling \033[0;36m$(<F)\033[0m\n" $(CURRENT)
	@$(CC) $(CFLAGS) -I ../kernel/include -c $< -o $@

# Link kernel ELF
$(KERNEL_ELF): $(KERNEL_ASM_O) $(KERNEL_C_O) $(SH_C_O)
	@printf "\033[0;35m[LD]\033[0m Linking kernel...\n"
	@$(LD) $(LDFLAGS) $^ -o $@

# Convert ELF to flat binary
$(KERNEL_BIN): $(KERNEL_ELF) | $(BIN_DIR)
	@printf "\033[0;35m[BIN]\033[0m Converting to binary...\n"
	@$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_BIN)

# Create bootable disk image (Live Disk)
$(DISK_IMG): $(BOOT_BIN) $(KERNEL_BIN) | $(BIN_DIR)
	@printf "\033[0;33m[IMG]\033[0m Building disk image...\n"
	@rm -f $(DISK_IMG)
	@dd if=/dev/zero of=$(DISK_IMG) bs=512 count=256 2>/dev/null
	@dd if=$(BOOT_BIN)   of=$(DISK_IMG) conv=notrunc bs=512 seek=0 2>/dev/null
	@dd if=$(KERNEL_BIN) of=$(DISK_IMG) conv=notrunc bs=512 seek=1 2>/dev/null
	@printf "LIVE" | dd of=$(DISK_IMG) bs=1 seek=130560 conv=notrunc 2>/dev/null

# Create a blank 10MB secondary hard drive for installation
$(HDD_IMG): | $(BIN_DIR)
	@printf "\033[0;33m[IMG]\033[0m Creating blank HDD image...\n"
	@dd if=/dev/zero of=$(HDD_IMG) bs=1M count=10 2>/dev/null

# Run in QEMU (Live Disk is index 0, HDD is index 1)
run: $(DISK_IMG) $(HDD_IMG)
	pkill -f "qemu-system-i[3]86" || true
	qemu-system-i386 -drive format=raw,file=$(DISK_IMG),index=0 -drive format=raw,file=$(HDD_IMG),index=1 -vnc :0 -k en-us


# Run with GDB debug info
debug: $(DISK_IMG) $(HDD_IMG)
	qemu-system-i386 -drive format=raw,file=$(DISK_IMG),index=0,media=disk -drive format=raw,file=$(HDD_IMG),index=1,media=disk -s -S

# Clean build artifacts
clean:
	@printf "\033[0;31m[CLEAN]\033[0m Removing build artifacts...\n"
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all run debug clean
