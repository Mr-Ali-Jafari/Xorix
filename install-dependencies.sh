#!/bin/bash

# Xorix OS - Linux Dependency Installer
# This script installs all required dependencies for building Xorix OS on Linux

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_header() {
    echo -e "${CYAN}================================${NC}"
    echo -e "${CYAN}  Xorix OS Dependency Installer${NC}"
    echo -e "${CYAN}================================${NC}"
    echo ""
}

# Detect Linux distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        VERSION=$VERSION_ID
    elif type lsb_release >/dev/null 2>&1; then
        DISTRO=$(lsb_release -si | tr '[:upper:]' '[:lower:]')
        VERSION=$(lsb_release -sr)
    elif [ -f /etc/redhat-release ]; then
        DISTRO="centos"
    else
        DISTRO="unknown"
    fi
    
    print_status "Detected distribution: $DISTRO $VERSION"
}

# Check if running as root
check_root() {
    if [ "$EUID" -eq 0 ]; then
        print_warning "Running as root. This is not recommended for development."
        read -p "Continue anyway? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_error "Aborted by user"
            exit 1
        fi
    fi
}

# Install dependencies for Ubuntu/Debian
install_ubuntu_debian() {
    print_status "Installing dependencies for Ubuntu/Debian..."
    
    # Update package list
    sudo apt-get update
    
    # Essential build tools
    sudo apt-get install -y \
        build-essential \
        gcc \
        g++ \
        make \
        nasm \
        ld-gold \
        libc6-dev \
        git
    
    # Cross-compilation toolchain
    sudo apt-get install -y \
        gcc-multilib \
        g++-multilib
    
    # Try to install i686-elf cross-compiler (if available)
    if apt-cache search gcc-i686-linux-gnu | grep -q gcc-i686-linux-gnu; then
        sudo apt-get install -y gcc-i686-linux-gnu g++-i686-linux-gnu
        print_success "Installed i686 cross-compiler from package manager"
    else
        print_warning "i686 cross-compiler not available in package manager"
        print_status "You may need to build it manually or use gcc-multilib"
    fi
    
    # GRUB tools for ISO creation
    sudo apt-get install -y \
        grub-pc-bin \
        grub-common \
        xorriso \
        mtools
    
    # Additional utilities
    sudo apt-get install -y \
        qemu-system-x86 \
        qemu-utils \
        gdb \
        objdump \
        hexdump \
        tree
}

# Install dependencies for Fedora/CentOS/RHEL
install_fedora_centos() {
    print_status "Installing dependencies for Fedora/CentOS/RHEL..."
    
    # Determine package manager
    if command -v dnf &> /dev/null; then
        PKG_MGR="dnf"
    else
        PKG_MGR="yum"
    fi
    
    # Essential build tools
    sudo $PKG_MGR install -y \
        gcc \
        gcc-c++ \
        make \
        nasm \
        binutils \
        glibc-devel \
        git
    
    # Cross-compilation support
    sudo $PKG_MGR install -y \
        glibc-devel.i686 \
        libgcc.i686
    
    # GRUB tools for ISO creation
    sudo $PKG_MGR install -y \
        grub2-tools \
        grub2-tools-extra \
        xorriso
    
    # Additional utilities
    sudo $PKG_MGR install -y \
        qemu-system-x86 \
        gdb \
        binutils \
        util-linux
}

# Install dependencies for Arch Linux
install_arch() {
    print_status "Installing dependencies for Arch Linux..."
    
    # Update package database
    sudo pacman -Sy
    
    # Essential build tools
    sudo pacman -S --needed \
        base-devel \
        gcc \
        make \
        nasm \
        git
    
    # Cross-compilation support
    sudo pacman -S --needed \
        lib32-gcc-libs \
        multilib-devel
    
    # GRUB tools for ISO creation
    sudo pacman -S --needed \
        grub \
        xorriso \
        mtools
    
    # Additional utilities
    sudo pacman -S --needed \
        qemu \
        gdb \
        tree
}

# Install dependencies for openSUSE
install_opensuse() {
    print_status "Installing dependencies for openSUSE..."
    
    # Essential build tools
    sudo zypper install -y \
        gcc \
        gcc-c++ \
        make \
        nasm \
        binutils \
        glibc-devel \
        git
    
    # Cross-compilation support
    sudo zypper install -y \
        glibc-devel-32bit \
        gcc-32bit
    
    # GRUB tools for ISO creation
    sudo zypper install -y \
        grub2 \
        xorriso
    
    # Additional utilities
    sudo zypper install -y \
        qemu \
        gdb
}

# Verify installation
verify_installation() {
    print_status "Verifying installation..."
    
    # Check essential tools
    TOOLS=("gcc" "g++" "make" "nasm" "ld" "grub-mkrescue" "xorriso")
    MISSING_TOOLS=()
    
    for tool in "${TOOLS[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            MISSING_TOOLS+=("$tool")
        fi
    done
    
    if [ ${#MISSING_TOOLS[@]} -eq 0 ]; then
        print_success "All essential tools are installed!"
    else
        print_error "Missing tools: ${MISSING_TOOLS[*]}"
        return 1
    fi
    
    # Check GCC version
    GCC_VERSION=$(gcc --version | head -n1)
    print_status "GCC Version: $GCC_VERSION"
    
    # Check if 32-bit compilation works
    if echo 'int main(){return 0;}' | gcc -m32 -x c - -o /tmp/test32 2>/dev/null; then
        print_success "32-bit compilation support verified"
        rm -f /tmp/test32
    else
        print_warning "32-bit compilation may not work properly"
    fi
}

# Create build test
create_build_test() {
    print_status "Creating build test..."
    
    cat > /tmp/xorix_build_test.c << 'EOF'
#include <stdio.h>

int main() {
    printf("Xorix OS build environment test successful!\n");
    return 0;
}
EOF
    
    if gcc -m32 /tmp/xorix_build_test.c -o /tmp/xorix_build_test 2>/dev/null; then
        /tmp/xorix_build_test
        print_success "Build test completed successfully!"
        rm -f /tmp/xorix_build_test.c /tmp/xorix_build_test
    else
        print_error "Build test failed!"
        rm -f /tmp/xorix_build_test.c
        return 1
    fi
}

# Main installation function
main() {
    print_header
    
    print_status "Starting Xorix OS dependency installation..."
    print_status "This script will install all required tools for building Xorix OS"
    echo ""
    
    # Check if user wants to continue
    read -p "Continue with installation? (Y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Nn]$ ]]; then
        print_error "Installation cancelled by user"
        exit 1
    fi
    
    check_root
    detect_distro
    
    case "$DISTRO" in
        ubuntu|debian|linuxmint|pop)
            install_ubuntu_debian
            ;;
        fedora|centos|rhel|rocky|almalinux)
            install_fedora_centos
            ;;
        arch|manjaro|endeavouros)
            install_arch
            ;;
        opensuse*|sles)
            install_opensuse
            ;;
        *)
            print_error "Unsupported distribution: $DISTRO"
            print_status "Please install the following packages manually:"
            echo "  - gcc, g++, make, nasm, binutils"
            echo "  - 32-bit development libraries"
            echo "  - grub-mkrescue, xorriso"
            echo "  - qemu (optional, for testing)"
            exit 1
            ;;
    esac
    
    print_status "Installation completed!"
    echo ""
    
    verify_installation
    create_build_test
    
    echo ""
    print_success "Xorix OS build environment is ready!"
    print_status "You can now build Xorix OS using: make"
    print_status "To create an ISO image, use: make iso"
    print_status "To test in QEMU, use: make test"
    echo ""
    print_status "For more information, see README.md"
}

# Run main function
main "$@"
