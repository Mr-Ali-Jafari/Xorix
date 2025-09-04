# Xorix Server Edition - x86 Build System
# Simplified build system for x86 architecture only

# Project information
PROJECT_NAME = Xorix Server Edition
VERSION = 1.0
BUILD_DATE = $(shell date +"%Y-%m-%d %H:%M:%S")

# x86 toolchain
CC = i686-elf-gcc
CXX = i686-elf-g++
AS = i686-elf-as
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy

# Compiler flags
CFLAGS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASFLAGS = --32
LDFLAGS = -melf_i386

# Include directories
INCLUDES = -Iinclude -Iinstaller -Idrivers -Ishell -Ieditor -Iarch/x86/include

# Source files
KERNEL_SOURCES = kernel/src/core/kernel.cpp kernel/src/core/memory.cpp kernel/src/core/gdt.cpp kernel/src/core/port.cpp
BOOT_SOURCE = arch/x86/boot.s

# Object files
KERNEL_OBJECTS = $(KERNEL_SOURCES:.cpp=.o)
BOOT_OBJECT = arch/x86/boot.o

# Output files
KERNEL_NAME = xorix-x86.bin
ISO_NAME = xorix-x86.iso

# Default target
.DEFAULT_GOAL := all

all: $(KERNEL_NAME)

# Compile C++ source files
%.o: %.cpp
	@echo "Compiling $< for x86..."
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Assemble boot file
$(BOOT_OBJECT): $(BOOT_SOURCE)
	@echo "Assembling $< for x86..."
	$(AS) $(ASFLAGS) -o $@ $<

# Link kernel
$(KERNEL_NAME): $(KERNEL_OBJECTS) $(BOOT_OBJECT)
	@echo "Linking $(KERNEL_NAME) for x86..."
	@echo "Build: $(PROJECT_NAME) v$(VERSION) - $(BUILD_DATE)"
	$(LD) $(LDFLAGS) -T arch/x86/linker.ld -o $@ $^
	@echo "✓ Built $(KERNEL_NAME) successfully ($(shell stat -c%s $(KERNEL_NAME) 2>/dev/null || echo 0) bytes)"

# Create ISO image
$(ISO_NAME): $(KERNEL_NAME)
	@echo "Creating enhanced ISO image $(ISO_NAME)..."
	mkdir -p iso/boot/grub
	cp $(KERNEL_NAME) iso/boot/xorix.bin
	@echo "Creating GRUB configuration..."
	@echo "# GRUB Configuration for $(PROJECT_NAME)" > iso/boot/grub/grub.cfg
	@echo "set timeout=5" >> iso/boot/grub/grub.cfg
	@echo "set default=0" >> iso/boot/grub/grub.cfg
	@echo "" >> iso/boot/grub/grub.cfg
	@echo "menuentry \"$(PROJECT_NAME) (Normal Mode)\" {" >> iso/boot/grub/grub.cfg
	@echo "    multiboot /boot/xorix.bin" >> iso/boot/grub/grub.cfg
	@echo "    boot" >> iso/boot/grub/grub.cfg
	@echo "}" >> iso/boot/grub/grub.cfg
	@echo "" >> iso/boot/grub/grub.cfg
	@echo "menuentry \"$(PROJECT_NAME) (Safe Mode)\" {" >> iso/boot/grub/grub.cfg
	@echo "    multiboot /boot/xorix.bin safe_mode" >> iso/boot/grub/grub.cfg
	@echo "    boot" >> iso/boot/grub/grub.cfg
	@echo "}" >> iso/boot/grub/grub.cfg
	@echo "" >> iso/boot/grub/grub.cfg
	@echo "menuentry \"$(PROJECT_NAME) (Debug Mode)\" {" >> iso/boot/grub/grub.cfg
	@echo "    multiboot /boot/xorix.bin debug_mode verbose" >> iso/boot/grub/grub.cfg
	@echo "    boot" >> iso/boot/grub/grub.cfg
	@echo "}" >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso
	@echo "✓ ISO image $(ISO_NAME) created successfully ($(shell stat -c%s $(ISO_NAME) 2>/dev/null || echo 0) bytes)"

# Testing targets
test: $(ISO_NAME)
	@echo "Testing $(ISO_NAME) in QEMU..."
	qemu-system-i386 -cdrom $(ISO_NAME) -m 512M -serial stdio -boot d

test-debug: $(ISO_NAME)
	@echo "Testing $(ISO_NAME) in debug mode..."
	qemu-system-i386 -cdrom $(ISO_NAME) -m 512M -serial stdio -boot d -s -S

# Build targets
iso: $(ISO_NAME)

# Clean targets
clean:
	@echo "Cleaning build files..."
	find . -name "*.o" -delete
	rm -f $(KERNEL_NAME) $(ISO_NAME)
	rm -rf iso
	@echo "Clean complete"

# Development setup
dev-setup:
	@echo "Setting up development environment..."
	@echo "Run './install-dependencies.sh' to install required dependencies"

# Help target
help:
	@echo "Xorix OS Build System - x86 Architecture"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build kernel binary (default)"
	@echo "  iso        - Build bootable ISO image"
	@echo "  test       - Test in QEMU emulator"
	@echo "  test-debug - Test in QEMU with debugging"
	@echo "  clean      - Clean build files"
	@echo "  dev-setup  - Setup development environment"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make iso   - Build xorix-x86.iso"
	@echo "  make test  - Run in QEMU"

.PHONY: all iso test test-debug clean dev-setup help

# Enhanced help with examples
help:
	@echo "$(PROJECT_NAME) v$(VERSION) - x86 Build System Help"
	@echo ""
	@echo "USAGE:"
	@echo "  make [target]"
	@echo ""
	@echo "ARCHITECTURE:"
	@echo "  x86 (32-bit) - Fully supported"
	@echo ""
	@echo "MAIN TARGETS:"
	@echo "  all           - Build kernel for current architecture"
	@echo "  iso           - Create bootable ISO image"
	@echo "  test-normal   - Test in QEMU (normal mode)"
	@echo "  test-debug    - Test in QEMU (debug mode)"
	@echo "  all-arch      - Build for all architectures"
	@echo "  package       - Create distribution package"
	@echo "  clean         - Clean build files"
	@echo "  clean-all     - Comprehensive cleanup"
	@echo ""
	@echo "DEVELOPMENT:"
	@echo "  dev-setup     - Set up development environment"
	@echo "  check-syntax  - Check syntax of all source files"
	@echo "  analyze-size  - Analyze component sizes"
	@echo "  docs          - Generate documentation"
	@echo "  status        - Show project status"
	@echo ""
	@echo "INSTALLATION:"
	@echo "  install-local - Install to local system"
	@echo ""
	@echo "EXAMPLES:"
	@echo "  make                     # Build x86 kernel"
	@echo "  make ARCH=x86_64 iso     # Build x86_64 ISO"
	@echo "  make arm test-normal     # Build and test ARM"
	@echo "  make all-arch package    # Build all and package"
	@echo "  make clean-all dev-setup # Clean and setup"
	@echo ""
	@echo "For detailed documentation, see docs/XORIX_SERVER_EDITION_GUIDE.md"

.PHONY: all clean install iso test help status docs package dev-setup check-syntax analyze-size clean-all install-local test-normal test-debug test-virtualbox
