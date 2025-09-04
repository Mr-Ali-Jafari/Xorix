# Xorix Server Edition v1.0

Xorix Server Edition is a complete, minimal, multi-architecture operating system designed for server environments. Built from scratch with modern components including a full keyboard driver, user management, text editor, and single-command installation system.

---

## 🚀 Key Features

### Core System
- **Single-Command Installation**: `install xorix` - Complete automated installation
- **Multi-Architecture Support**: x86 (32-bit), x86_64 (64-bit), ARM Cortex-A8
- **Modern Keyboard Driver**: Full scancode support, modifiers, function keys, layouts
- **User Management**: User accounts, passwords, admin mode, privilege escalation
- **GRUB2 Bootloader**: BIOS and UEFI support with multiple boot options

### Applications
- **xbash Shell**: Unix-like commands, scripting, command history, environment variables
- **xnano Editor**: Lightweight nano-like text editor for configuration files
- **Filesystem**: Simple but robust filesystem with permissions and ownership
- **Installation System**: Automated partitioning, formatting, and system setup

### Architecture Support
- **x86**: 32-bit Intel/AMD processors
- **x86_64**: 64-bit processors with long mode support  
- **ARM**: Cortex-A8 and compatible ARM processors

---

## 📋 Requirements

### Minimum System Requirements
- **RAM**: 512MB minimum
- **Storage**: 512MB for installation
- **Architecture**: x86, x86_64, or ARM Cortex-A8
- **Firmware**: BIOS or UEFI

### Build Requirements
- Linux host system for building
- Cross-compilation toolchain (i686-elf-gcc, x86_64-elf-gcc, arm-none-eabi-gcc)
- Build tools: `gcc`, `make`, `grub-mkrescue`, `xorriso`

---

## 🛠️ Quick Start

### 1. Build System Setup
```bash
# Install dependencies
sudo apt install build-essential grub-pc-bin grub-common xorriso

# Set up development environment
make dev-setup
```

### 2. Build for Your Architecture
```bash
# Build x86 kernel and ISO
make ARCH=x86 iso

## Running on Different Platforms

- **QEMU (Recommended for Development and Testing)**  
  Run Xorix in QEMU with:

  ```bash
  qemu-system-i386 -cdrom xorix.iso
  ```

  QEMU is a fast and flexible emulator supporting multiple architectures.

- **VirtualBox / VMware**  
  You can use the generated ISO (`xorix.iso`) as a boot disk in most virtualization software such as VirtualBox or VMware. Just create a new VM and select the ISO as the boot medium.

- **Real Hardware**  
  Burn the ISO to a USB drive using tools like Rufus or Etcher, then boot from the USB on compatible x86 hardware. Make sure your motherboard supports legacy BIOS or UEFI boot modes.

---

## License

This project is licensed under the [MIT License](LICENSE).  
You are free to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the software under the terms of the MIT License.

---

## More Information

Visit the official website: [https://xorix.ir](https://xorix.ir)  
Contact: XorixSoftware@gmail.com

---

## Contact and Contribution

Feel free to contribute or open issues on GitHub. For direct inquiries, email: XorixSoftware@gmail.com
Follow updates and community discussions on the website.

---
