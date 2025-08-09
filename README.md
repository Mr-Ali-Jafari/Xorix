# Xorix OS

Xorix is a minimalist, original operating system kernel built from scratch with zero external dependencies. It’s designed for modern x86 hardware and provides a simple terminal interface and custom drivers.

---

## Features

- Zero-dependency kernel architecture
- Custom keyboard and terminal drivers
- Support for 32-bit x86 architecture
- Simple command-line interface with built-in commands (help, clear, about, echo)
- Basic VGA text output
- Multiboot compatible

---

## Requirements

- A 64-bit x86 CPU (runs in 32-bit mode)
- Linux host for building
- Required tools: `gcc`, `g++`, `nasm`, `binutils`, `grub`, `xorriso`

---

## Setup

Install the dependencies by running the setup script:

```bash
./setup_dependencies.sh
```

---

## Building

To build the kernel binary, run:

```bash
make
```

This will compile the source and generate `xorix.bin`.

---

## Creating a Bootable ISO

To create a bootable ISO image, run:

```bash
make xorix.iso
```

---

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

This project is licensed under the [Xorix Open License](LICENSE).  
You are free to use and modify the kernel source code under the condition that it is not used for unethical purposes.

---

## More Information

Visit the official website: [https://xorix.ir](https://xorix.ir)  
Contact: info@xorix.ir

---

## Contact and Contribution

Feel free to contribute or open issues on GitHub. For direct inquiries, email: XorixSoftware@gmail.com
Follow updates and community discussions on the website.

---
