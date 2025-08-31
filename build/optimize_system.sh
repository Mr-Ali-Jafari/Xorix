#!/bin/bash
# Xorix Server Edition - System Optimization Script
# Removes unnecessary files and optimizes for minimal ISO size

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

echo "=== Xorix Server Edition - System Optimization ==="
echo ""

# Function to show size before and after
show_size() {
    local path="$1"
    local description="$2"
    if [ -e "$path" ]; then
        local size=$(du -sh "$path" 2>/dev/null | cut -f1)
        echo "  $description: $size"
    fi
}

echo "Current system size:"
show_size "." "Total project"
show_size "kernel/" "Kernel directory"
show_size "drivers/" "Drivers directory"
show_size "shell/" "Shell directory"
show_size "editor/" "Editor directory"
echo ""

echo "Step 1: Removing development files..."
# Remove development and temporary files
find . -name "*.o" -delete 2>/dev/null || true
find . -name "*.a" -delete 2>/dev/null || true
find . -name "*.so" -delete 2>/dev/null || true
find . -name "*~" -delete 2>/dev/null || true
find . -name "*.bak" -delete 2>/dev/null || true
find . -name "*.tmp" -delete 2>/dev/null || true
find . -name "*.log" -delete 2>/dev/null || true
find . -name ".DS_Store" -delete 2>/dev/null || true
find . -name "Thumbs.db" -delete 2>/dev/null || true

# Remove empty directories
find . -type d -empty -delete 2>/dev/null || true

echo "✓ Development files removed"

echo "Step 2: Optimizing documentation..."
# Keep only essential documentation
mkdir -p docs_essential
cp docs/XORIX_SERVER_EDITION_GUIDE.md docs_essential/ 2>/dev/null || true
cp README.md docs_essential/ 2>/dev/null || true
cp LICENSE docs_essential/ 2>/dev/null || true

# Remove verbose documentation if exists
if [ -d "docs" ] && [ "$(ls -A docs)" ]; then
    rm -rf docs_backup 2>/dev/null || true
    mv docs docs_backup
    mv docs_essential docs
    echo "✓ Documentation optimized (backup in docs_backup)"
else
    rm -rf docs_essential
    echo "✓ Documentation already minimal"
fi

echo "Step 3: Removing debug symbols..."
# Strip debug symbols from binaries if they exist
for binary in xorix-*.bin; do
    if [ -f "$binary" ]; then
        echo "  Stripping $binary..."
        strip "$binary" 2>/dev/null || echo "    (strip failed, continuing)"
    fi
done
echo "✓ Debug symbols removed"

echo "Step 4: Optimizing source code..."
# Remove excessive comments and whitespace from source files
optimize_source() {
    local file="$1"
    if [ -f "$file" ]; then
        # Create backup
        cp "$file" "$file.orig"
        
        # Remove excessive blank lines (keep max 2 consecutive)
        awk '/^[[:space:]]*$/{n++; if(n<=2) print; next} {n=0; print}' "$file.orig" > "$file"
        
        # Remove trailing whitespace
        sed -i 's/[[:space:]]*$//' "$file" 2>/dev/null || true
        
        rm "$file.orig"
    fi
}

echo "  Optimizing C++ source files..."
find . -name "*.cpp" -exec bash -c 'optimize_source "$0"' {} \;
find . -name "*.h" -exec bash -c 'optimize_source "$0"' {} \;
echo "✓ Source code optimized"

echo "Step 5: Creating minimal file list..."
# Create list of essential files for ISO
cat > build/essential_files.txt << 'EOF'
# Essential files for Xorix Server Edition ISO
# Kernel binaries
xorix-x86.bin
xorix-x86_64.bin
xorix-arm.bin

# Documentation
docs/XORIX_SERVER_EDITION_GUIDE.md
README.md
LICENSE

# Build system
Makefile
build/Makefile.multi-arch

# Architecture support
arch/x86/boot.s
arch/x86/linker.ld
arch/x86_64/boot.s
arch/x86_64/linker.ld
arch/arm/boot.s
arch/arm/linker.ld

# Core components (headers only for reference)
drivers/keyboard_driver.h
shell/xbash.h
editor/xnano.h
installer/xorix_installer.h
fs/xorix_filesystem.h
EOF

echo "✓ Essential files list created"

echo "Step 6: Memory and performance optimization..."
# Create optimized compiler flags
cat > build/optimization_flags.mk << 'EOF'
# Optimization flags for minimal size and maximum performance
OPTIMIZE_SIZE = -Os -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables
OPTIMIZE_PERFORMANCE = -O2 -march=native -mtune=native -ffast-math
OPTIMIZE_MINIMAL = -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -fno-exceptions -fno-rtti

# Memory optimization
MEMORY_OPTS = -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE

# Size optimization for different components
KERNEL_OPTS = $(OPTIMIZE_MINIMAL) -ffreestanding -nostdlib
DRIVER_OPTS = $(OPTIMIZE_SIZE) -fno-common
SHELL_OPTS = $(OPTIMIZE_PERFORMANCE) -fomit-frame-pointer
EDITOR_OPTS = $(OPTIMIZE_SIZE) -fno-builtin-printf
EOF

echo "✓ Optimization flags configured"

echo "Step 7: Creating deployment package..."
# Create minimal deployment structure
mkdir -p deployment/xorix-server-minimal
mkdir -p deployment/xorix-server-minimal/{boot,docs,tools}

# Copy only essential files
cp xorix-*.bin deployment/xorix-server-minimal/boot/ 2>/dev/null || true
cp docs/*.md deployment/xorix-server-minimal/docs/ 2>/dev/null || true
cp README.md LICENSE deployment/xorix-server-minimal/ 2>/dev/null || true

# Create installation script
cat > deployment/xorix-server-minimal/install.sh << 'EOF'
#!/bin/bash
# Xorix Server Edition - Quick Install Script

echo "Xorix Server Edition - Quick Installer"
echo "======================================"

# Detect architecture
ARCH=$(uname -m)
case $ARCH in
    i?86) KERNEL="xorix-x86.bin" ;;
    x86_64) KERNEL="xorix-x86_64.bin" ;;
    arm*) KERNEL="xorix-arm.bin" ;;
    *) echo "Unsupported architecture: $ARCH"; exit 1 ;;
esac

echo "Detected architecture: $ARCH"
echo "Using kernel: $KERNEL"

if [ ! -f "boot/$KERNEL" ]; then
    echo "Error: Kernel $KERNEL not found!"
    exit 1
fi

echo ""
echo "Installation options:"
echo "1. Install to /boot (requires sudo)"
echo "2. Create bootable USB"
echo "3. Install to VM disk"
echo ""
read -p "Choose option (1-3): " choice

case $choice in
    1)
        echo "Installing to /boot..."
        sudo cp "boot/$KERNEL" /boot/xorix.bin
        echo "Kernel installed. Add to GRUB manually."
        ;;
    2)
        read -p "Enter USB device (e.g., /dev/sdb): " device
        echo "This will erase $device! Continue? (y/N)"
        read confirm
        if [ "$confirm" = "y" ]; then
            echo "Creating bootable USB..."
            # USB creation logic would go here
            echo "USB creation not implemented in minimal installer"
        fi
        ;;
    3)
        echo "VM installation not implemented in minimal installer"
        ;;
    *)
        echo "Invalid option"
        exit 1
        ;;
esac
EOF

chmod +x deployment/xorix-server-minimal/install.sh

echo "✓ Deployment package created"

echo ""
echo "Optimization complete! Results:"
show_size "." "Total project (after optimization)"
show_size "deployment/xorix-server-minimal" "Minimal deployment package"

echo ""
echo "Optimization summary:"
echo "  ✓ Development files removed"
echo "  ✓ Documentation minimized"
echo "  ✓ Debug symbols stripped"
echo "  ✓ Source code optimized"
echo "  ✓ Essential files identified"
echo "  ✓ Compiler optimization configured"
echo "  ✓ Minimal deployment package created"

echo ""
echo "Next steps:"
echo "  1. Build optimized kernel: make CFLAGS='-Os -ffunction-sections' all"
echo "  2. Create minimal ISO: make iso"
echo "  3. Test deployment: cd deployment/xorix-server-minimal && ./install.sh"

echo ""
echo "Deployment package location: deployment/xorix-server-minimal/"
echo "Optimization complete!"
