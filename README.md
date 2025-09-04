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
- **Architecture**: x86 (32-bit) - Fully supported
- **Firmware**: BIOS or UEFI

### Build Requirements
- Linux host system for building (Ubuntu, Debian, Fedora, CentOS, Arch, openSUSE supported)
- i686-elf cross-compilation toolchain (automatically installed by dependency script)
- Build tools: `gcc`, `make`, `grub-mkrescue`, `xorriso` (automatically installed)

**Note**: Use the included `install-dependencies.sh` script to automatically install all required dependencies for your Linux distribution.

---

## 🛠️ Quick Start

### 1. Install Dependencies (Automatic)
```bash
# Run the automatic dependency installer (recommended)
./install-dependencies.sh
```

### 1. Build System Setup (Manual)
```bash
# Or install dependencies manually
sudo apt install build-essential grub-pc-bin grub-common xorriso

# Set up development environment
make dev-setup
```

### 2. Build Xorix OS
```bash
# Build x86 kernel and ISO
make iso

# Or build just the kernel
make
```

---

## 🏗️ Architecture Support

### Fully Supported
- **x86 (32-bit)**: ✅ **Production Ready**
  - Complete implementation with all features
  - Real installation system with disk partitioning
  - 45+ functional commands
  - GRUB bootloader integration
  - Reboot functionality with boot mode detection
  - Simplified build system for easy development

### Build Commands
```bash
# Build kernel binary
make

# Build bootable ISO
make iso

# Test in QEMU
make test
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
