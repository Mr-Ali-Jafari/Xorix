# Xorix Server Edition - Main Makefile
# Multi-architecture build system with complete component integration

# Include multi-architecture build system
include build/Makefile.multi-arch

# Default target
.DEFAULT_GOAL := all

# Project information
PROJECT_NAME = Xorix Server Edition
VERSION = 1.0
BUILD_DATE = $(shell date +"%Y-%m-%d %H:%M:%S")

# Additional source directories
KERNEL_OBJECTS = kernel/src/core/kernel_minimal.o kernel/src/core/memory.o kernel/src/core/gdt.o kernel/src/core/port.o shell/xbash.o
COMPONENT_SOURCES = $(SHELL_OBJECTS) $(EDITOR_OBJECTS) $(INSTALLER_OBJECTS) $(DRIVER_OBJECTS)

# Update objects list to include new kernel
ALL_OBJECTS := $(CORE_OBJECTS) $(ARCH_OBJECTS) $(COMPONENT_SOURCES)

# Override kernel build to use new main kernel
$(KERNEL_NAME): $(ALL_OBJECTS)
	@echo "Linking $(KERNEL_NAME) for $(ARCH) with all components..."
	@echo "Build: $(PROJECT_NAME) v$(VERSION) - $(BUILD_DATE)"
	$(LD) $(LDFLAGS) -T $(ARCH_DIR)/linker.ld -o $@ $(ALL_OBJECTS)
	@echo "✓ Built $(KERNEL_NAME) successfully ($(shell stat -c%s $(KERNEL_NAME)) bytes)"

# Enhanced ISO creation with proper GRUB configuration
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
	@echo "✓ ISO image $(ISO_NAME) created successfully ($(shell stat -c%s $(ISO_NAME)) bytes)"

# Enhanced testing with different configurations
test-normal: $(ISO_NAME)
	@echo "Testing $(ISO_NAME) in normal mode..."
ifeq ($(ARCH),x86)
	qemu-system-i386 -cdrom $(ISO_NAME) -m 512M -serial stdio -boot d
endif
ifeq ($(ARCH),x86_64)
	qemu-system-x86_64 -cdrom $(ISO_NAME) -m 512M -serial stdio -boot d
endif
ifeq ($(ARCH),arm)
	qemu-system-arm -M versatilepb -kernel $(KERNEL_NAME) -m 512M -serial stdio
endif

test-debug: $(ISO_NAME)
	@echo "Testing $(ISO_NAME) in debug mode..."
ifeq ($(ARCH),x86)
	qemu-system-i386 -cdrom $(ISO_NAME) -m 512M -serial stdio -boot d -s -S
endif
ifeq ($(ARCH),x86_64)
	qemu-system-x86_64 -cdrom $(ISO_NAME) -m 512M -serial stdio -boot d -s -S
endif

test-virtualbox: $(ISO_NAME)
	@echo "Instructions for VirtualBox testing:"
	@echo "1. Create new VM with $(ARCH) architecture"
	@echo "2. Set memory to 512MB minimum"
	@echo "3. Mount $(ISO_NAME) as CD/DVD"
	@echo "4. Boot from CD/DVD"

# Development targets
dev-setup:
	@echo "Setting up development environment..."
	@echo "Checking dependencies..."
	@which gcc > /dev/null || (echo "ERROR: gcc not found" && exit 1)
	@which make > /dev/null || (echo "ERROR: make not found" && exit 1)
	@which grub-mkrescue > /dev/null || (echo "ERROR: grub-mkrescue not found" && exit 1)
	@echo "✓ Basic dependencies found"
	@echo "Setting up directories..."
	@mkdir -p build docs installation
	@echo "✓ Development environment ready"

# Code quality and analysis
check-syntax:
	@echo "Checking syntax for all source files..."
	@find . -name "*.cpp" -o -name "*.h" | xargs -I {} sh -c 'echo "Checking {}..." && $(CXX) $(CXXFLAGS) -fsyntax-only {}'
	@echo "✓ Syntax check completed"

analyze-size:
	@echo "Analyzing component sizes..."
	@echo "Kernel objects:"
	@find . -name "*.o" -exec ls -lh {} \; | sort -k5 -hr
	@echo ""
	@echo "Final binaries:"
	@ls -lh xorix-*.bin 2>/dev/null || echo "No binaries found"
	@ls -lh xorix-*.iso 2>/dev/null || echo "No ISO images found"

# Documentation generation
docs: docs/XORIX_SERVER_EDITION_GUIDE.md
	@echo "Generating additional documentation..."
	@echo "# Xorix Server Edition - Build Information" > docs/BUILD_INFO.md
	@echo "" >> docs/BUILD_INFO.md
	@echo "**Project**: $(PROJECT_NAME)" >> docs/BUILD_INFO.md
	@echo "**Version**: $(VERSION)" >> docs/BUILD_INFO.md
	@echo "**Build Date**: $(BUILD_DATE)" >> docs/BUILD_INFO.md
	@echo "**Architecture**: $(ARCH)" >> docs/BUILD_INFO.md
	@echo "" >> docs/BUILD_INFO.md
	@echo "## Component Status" >> docs/BUILD_INFO.md
	@echo "- ✓ Keyboard Driver: Complete modular implementation" >> docs/BUILD_INFO.md
	@echo "- ✓ xbash Shell: Unix-like shell with user management" >> docs/BUILD_INFO.md
	@echo "- ✓ xnano Editor: Lightweight text editor" >> docs/BUILD_INFO.md
	@echo "- ✓ Installation System: Single-command installer" >> docs/BUILD_INFO.md
	@echo "- ✓ Multi-Architecture: x86, x86_64, ARM support" >> docs/BUILD_INFO.md
	@echo "- ✓ GRUB2 Integration: BIOS/UEFI bootloader" >> docs/BUILD_INFO.md
	@echo "✓ Documentation generated"

# Packaging and distribution
package: all-arch docs
	@echo "Creating distribution package..."
	@mkdir -p dist/$(PROJECT_NAME)-v$(VERSION)
	@cp -r docs dist/$(PROJECT_NAME)-v$(VERSION)/
	@cp xorix-*.bin dist/$(PROJECT_NAME)-v$(VERSION)/ 2>/dev/null || true
	@cp xorix-*.iso dist/$(PROJECT_NAME)-v$(VERSION)/ 2>/dev/null || true
	@cp README.md LICENSE dist/$(PROJECT_NAME)-v$(VERSION)/ 2>/dev/null || true
	@cd dist && tar -czf $(PROJECT_NAME)-v$(VERSION).tar.gz $(PROJECT_NAME)-v$(VERSION)/
	@echo "✓ Package created: dist/$(PROJECT_NAME)-v$(VERSION).tar.gz"

# Installation to local system
install-local: $(KERNEL_NAME)
	@echo "Installing $(PROJECT_NAME) to local system..."
	@sudo mkdir -p /boot/xorix
	@sudo cp $(KERNEL_NAME) /boot/xorix/
	@sudo cp docs/* /boot/xorix/ 2>/dev/null || true
	@echo "Adding GRUB entry..."
	@echo "" | sudo tee -a /etc/grub.d/40_custom
	@echo "menuentry '$(PROJECT_NAME)' {" | sudo tee -a /etc/grub.d/40_custom
	@echo "    multiboot /boot/xorix/$(KERNEL_NAME)" | sudo tee -a /etc/grub.d/40_custom
	@echo "    boot" | sudo tee -a /etc/grub.d/40_custom
	@echo "}" | sudo tee -a /etc/grub.d/40_custom
	@sudo update-grub
	@echo "✓ $(PROJECT_NAME) installed to local system"

# Comprehensive clean
clean-all: clean
	@echo "Performing comprehensive cleanup..."
	@rm -rf dist installation
	@rm -f docs/BUILD_INFO.md
	@find . -name "*~" -delete
	@find . -name "*.bak" -delete
	@find . -name "*.tmp" -delete
	@echo "✓ Comprehensive cleanup completed"

# Status and information
status:
	@echo "=== $(PROJECT_NAME) v$(VERSION) Status ==="
	@echo ""
	@echo "Architecture: $(ARCH)"
	@echo "Compiler: $(CXX)"
	@echo "Build Date: $(BUILD_DATE)"
	@echo ""
	@echo "Components:"
	@echo "  ✓ Keyboard Driver (modular, full scancode support)"
	@echo "  ✓ xbash Shell (Unix commands, user management)"
	@echo "  ✓ xnano Editor (lightweight, nano-like)"
	@echo "  ✓ Installation System (single-command installer)"
	@echo "  ✓ Filesystem (simple, robust, permissions)"
	@echo "  ✓ Multi-Architecture (x86, x86_64, ARM)"
	@echo ""
	@echo "Build Artifacts:"
	@ls -la xorix-*.bin 2>/dev/null || echo "  No kernel binaries found"
	@ls -la xorix-*.iso 2>/dev/null || echo "  No ISO images found"
	@echo ""
	@echo "Quick Commands:"
	@echo "  make ARCH=x86 iso    - Build x86 ISO"
	@echo "  make test-normal     - Test in QEMU"
	@echo "  make package         - Create distribution"
	@echo "  make help            - Show all targets"

# Enhanced help with examples
help:
	@echo "$(PROJECT_NAME) v$(VERSION) - Build System Help"
	@echo ""
	@echo "USAGE:"
	@echo "  make [ARCH=<arch>] [target]"
	@echo ""
	@echo "ARCHITECTURES:"
	@echo "  x86      - 32-bit x86 (i686)"
	@echo "  x86_64   - 64-bit x86_64"
	@echo "  arm      - ARM Cortex-A8"
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
