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
KERNEL_C = ../kernel/kernel.c ../kernel/kprint.c ../kernel/terminal.c ../kernel/idt.c ../kernel/keyboard.c ../kernel/shell.c ../kernel/malloc.c ../kernel/ata.c ../kernel/fat32.c ../kernel/installer.c ../kernel/timer.c ../kernel/rtc.c ../kernel/power.c ../kernel/gdt.c ../kernel/task.c ../kernel/syscall.c ../kernel/elf.c
SH_C = ../kernel/sh/sh_ls.c ../kernel/sh/sh_cd.c ../kernel/sh/sh_pwd.c ../kernel/sh/sh_mkdir.c ../kernel/sh/sh_rm.c ../kernel/sh/sh_cp.c ../kernel/sh/sh_mv.c ../kernel/sh/sh_touch.c ../kernel/sh/sh_cat.c ../kernel/sh/sh_echo.c ../kernel/sh/sh_clear.c ../kernel/sh/sh_help.c ../kernel/sh/sh_reboot.c ../kernel/sh/sh_shutdown.c ../kernel/sh/commands.c ../kernel/sh/sh_vim.c

# Output files
BOOT_BIN = $(BIN_DIR)/boot.bin
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BIN_DIR)/kernel.bin

# Object files
KERNEL_ASM_O = $(patsubst %.asm,$(BUILD_DIR)/%_asm.o,$(KERNEL_ASM))
KERNEL_C_O = $(patsubst ../kernel/%.c,$(BUILD_DIR)/%.o,$(KERNEL_C))
SH_C_O = $(patsubst ../kernel/sh/%.c,$(BUILD_DIR)/%.o,$(SH_C))

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

# Build kernel C object files
$(BUILD_DIR)/%.o: ../kernel/%.c | $(BUILD_DIR)
	$(eval CURRENT := $(shell echo $$(($(CURRENT)+1))))
	@printf "\033[0;32m[%2d/$(TOTAL_FILES)]\033[0m Compiling \033[0;36m$(<F)\033[0m\n" $(CURRENT)
	@$(CC) $(CFLAGS) -I ../kernel -c $< -o $@

# Build shell command object files
$(BUILD_DIR)/%.o: ../kernel/sh/%.c | $(BUILD_DIR)
	$(eval CURRENT := $(shell echo $$(($(CURRENT)+1))))
	@printf "\033[0;32m[%2d/$(TOTAL_FILES)]\033[0m Compiling \033[0;36m$(<F)\033[0m\n" $(CURRENT)
	@$(CC) $(CFLAGS) -I ../kernel -c $< -o $@

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
