GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASPARAMS = --32
objects = loader.o gdt.o port.o keyboard.o terminal.o kernel.o
LDPARAMS = -melf_i386

%.o : %.cpp
		g++ $(GPPPARAMS) -o $@ -c $<

%.o : %.s
		as $(ASPARAMS) -o $@ $<

xorix.bin: linker.ld $(objects)
		ld $(LDPARAMS) -T $< -o $@ $(objects)

install: xorix.bin
		sudo cp $< /boot/xorix.bin

xorix.iso: xorix.bin
		mkdir iso
		mkdir iso/boot
		mkdir iso/boot/grub
		cp $< iso/boot/
		echo 'set timeout=0' > iso/boot/grub/grub.cfg 
		echo 'set default=0' >> iso/boot/grub/grub.cfg
		echo '' >> iso/boot/grub/grub.cfg
		echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
		echo '	multiboot /boot/xorix.bin' >> iso/boot/grub/grub.cfg
		echo '	boot' >> iso/boot/grub/grub.cfg
		echo '}' >> iso/boot/grub/grub.cfg
		mkdir -p installation
		grub-mkrescue --output=./installation/$@ iso 
		rm -rf iso

.PHONY: clean
clean:
		rm -rf installation
		rm -f ${objects} xorix.bin