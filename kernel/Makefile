# Xorix Kernel Makefile - Working Organized Structure
GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Iinclude
ASPARAMS = --32
LDPARAMS = -melf_i386

# Core objects (organized structure)
CORE_OBJECTS = src/core/loader.o src/core/gdt.o src/core/port.o \
               src/core/interruptstubs.o src/core/interrupts.o src/core/kernel.o src/core/memory.o

# Driver objects (only compatible drivers)
DRIVER_OBJECTS = src/drivers/keyboard/keyboard.o src/drivers/drivers_simple.o

# All objects
objects = $(CORE_OBJECTS) $(DRIVER_OBJECTS)

# Compilation rules
%.o : %.cpp
	g++ $(GPPPARAMS) -o $@ -c $<

%.o : %.s
	as $(ASPARAMS) -o $@ $<

# Main target
xorix.bin: src/core/linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

# Install target
install: xorix.bin
	sudo cp $< /boot/xorix.bin

# ISO creation
xorix.iso: xorix.bin
	mkdir -p iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg 
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "Xorix Operating System" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/xorix.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	mkdir -p installation
	grub-mkrescue --output=./installation/$@ iso 
	rm -rf iso

# Show organized file structure
structure:
	@echo "Xorix Kernel Organization:"
	@echo "├── src/"
	@echo "│   ├── core/          # Core kernel components"
	@echo "│   │   ├── kernel.cpp # Main kernel with driver integration"
	@echo "│   │   ├── gdt.cpp    # Global Descriptor Table"
	@echo "│   │   ├── interrupts.cpp # Interrupt management"
	@echo "│   │   └── port.cpp   # Hardware port access"
	@echo "│   └── drivers/       # Hardware drivers"
	@echo "│       ├── keyboard/  # PS/2 Keyboard driver"
	@echo "│       ├── mouse/     # PS/2 Mouse driver (organized)"
	@echo "│       ├── network/   # Network drivers (organized)"
	@echo "│       ├── storage/   # ATA/IDE storage drivers (organized)"
	@echo "│       ├── audio/     # Audio drivers (organized)"
	@echo "│       ├── video/     # Video drivers (organized)"
	@echo "│       └── drivers_simple.cpp # Simplified driver manager"
	@echo "├── include/           # Header files"
	@echo "│   ├── types.h        # Basic type definitions"
	@echo "│   ├── port.h         # Port access definitions"
	@echo "│   ├── interrupts.h   # Interrupt definitions"
	@echo "│   ├── gdt.h          # GDT definitions"
	@echo "│   └── drivers_simple.h # Simplified driver interface"
	@echo "└── installation/      # ISO images"

# Clean targets
clean:
	rm -f $(objects) xorix.bin
	rm -rf installation

.PHONY: clean install structure
