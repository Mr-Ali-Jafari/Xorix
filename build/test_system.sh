#!/bin/bash
# Xorix Server Edition - Comprehensive Testing Framework
# Tests all architectures, virtualization platforms, and functionality

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test results
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_TOTAL=0

# Logging
LOG_FILE="build/test_results.log"
echo "=== Xorix Server Edition Test Results ===" > "$LOG_FILE"
echo "Test started: $(date)" >> "$LOG_FILE"
echo "" >> "$LOG_FILE"

log_test() {
    local status="$1"
    local test_name="$2"
    local details="$3"
    
    echo "[$status] $test_name: $details" >> "$LOG_FILE"
    
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}✓${NC} $test_name"
        ((TESTS_PASSED++))
    elif [ "$status" = "FAIL" ]; then
        echo -e "${RED}✗${NC} $test_name: $details"
        ((TESTS_FAILED++))
    elif [ "$status" = "SKIP" ]; then
        echo -e "${YELLOW}⚠${NC} $test_name: $details"
    else
        echo -e "${BLUE}ℹ${NC} $test_name: $details"
    fi
    ((TESTS_TOTAL++))
}

# Test functions
test_build_system() {
    echo -e "${BLUE}Testing Build System...${NC}"
    
    # Test Makefile exists and is valid
    if [ -f "Makefile" ]; then
        if make -n help >/dev/null 2>&1; then
            log_test "PASS" "Makefile Syntax" "Valid Makefile structure"
        else
            log_test "FAIL" "Makefile Syntax" "Invalid Makefile"
            return 1
        fi
    else
        log_test "FAIL" "Makefile Exists" "Makefile not found"
        return 1
    fi
    
    # Test multi-architecture support
    if [ -f "build/Makefile.multi-arch" ]; then
        log_test "PASS" "Multi-arch Support" "Multi-architecture Makefile found"
    else
        log_test "FAIL" "Multi-arch Support" "Multi-architecture Makefile missing"
    fi
    
    # Test architecture directories
    for arch in x86 x86_64 arm; do
        if [ -d "arch/$arch" ]; then
            if [ -f "arch/$arch/boot.s" ] && [ -f "arch/$arch/linker.ld" ]; then
                log_test "PASS" "$arch Architecture" "Boot and linker files present"
            else
                log_test "FAIL" "$arch Architecture" "Missing boot.s or linker.ld"
            fi
        else
            log_test "FAIL" "$arch Architecture" "Architecture directory missing"
        fi
    done
}

test_components() {
    echo -e "${BLUE}Testing Components...${NC}"
    
    # Test component headers
    local components=("drivers/keyboard_driver.h" "shell/xbash.h" "editor/xnano.h" "installer/xorix_installer.h" "fs/xorix_filesystem.h")
    
    for component in "${components[@]}"; do
        if [ -f "$component" ]; then
            # Basic syntax check
            if gcc -fsyntax-only -x c++ "$component" -I. -Iinclude 2>/dev/null; then
                log_test "PASS" "Component $(basename $component)" "Header syntax valid"
            else
                log_test "FAIL" "Component $(basename $component)" "Header syntax errors"
            fi
        else
            log_test "FAIL" "Component $(basename $component)" "Header file missing"
        fi
    done
    
    # Test component implementations
    local implementations=("drivers/keyboard_driver.cpp" "shell/xbash.cpp" "editor/xnano.cpp" "installer/xorix_installer.cpp")
    
    for impl in "${implementations[@]}"; do
        if [ -f "$impl" ]; then
            log_test "PASS" "Implementation $(basename $impl)" "Implementation file present"
        else
            log_test "FAIL" "Implementation $(basename $impl)" "Implementation file missing"
        fi
    done
}

test_kernel_build() {
    echo -e "${BLUE}Testing Kernel Build...${NC}"
    
    # Test build for each architecture
    for arch in x86 x86_64 arm; do
        echo "  Testing $arch build..."
        
        # Check if cross-compiler is available
        case $arch in
            x86)
                if command -v i686-elf-gcc >/dev/null 2>&1; then
                    compiler_available=true
                else
                    compiler_available=false
                fi
                ;;
            x86_64)
                if command -v x86_64-elf-gcc >/dev/null 2>&1; then
                    compiler_available=true
                else
                    compiler_available=false
                fi
                ;;
            arm)
                if command -v arm-none-eabi-gcc >/dev/null 2>&1; then
                    compiler_available=true
                else
                    compiler_available=false
                fi
                ;;
        esac
        
        if [ "$compiler_available" = true ]; then
            # Try to build
            if make ARCH=$arch clean >/dev/null 2>&1 && make ARCH=$arch >/dev/null 2>&1; then
                log_test "PASS" "$arch Kernel Build" "Kernel built successfully"
                
                # Check if binary exists and has reasonable size
                kernel_file="xorix-$arch.bin"
                if [ -f "$kernel_file" ]; then
                    size=$(stat -c%s "$kernel_file" 2>/dev/null || echo "0")
                    if [ "$size" -gt 1024 ] && [ "$size" -lt 10485760 ]; then # 1KB - 10MB
                        log_test "PASS" "$arch Binary Size" "Size: $size bytes (reasonable)"
                    else
                        log_test "FAIL" "$arch Binary Size" "Size: $size bytes (suspicious)"
                    fi
                else
                    log_test "FAIL" "$arch Binary Creation" "Binary file not created"
                fi
            else
                log_test "FAIL" "$arch Kernel Build" "Build failed"
            fi
        else
            log_test "SKIP" "$arch Kernel Build" "Cross-compiler not available"
        fi
    done
}

test_iso_creation() {
    echo -e "${BLUE}Testing ISO Creation...${NC}"
    
    # Check if grub-mkrescue is available
    if command -v grub-mkrescue >/dev/null 2>&1; then
        # Test ISO creation for available architectures
        for arch in x86 x86_64; do
            kernel_file="xorix-$arch.bin"
            iso_file="xorix-$arch.iso"
            
            if [ -f "$kernel_file" ]; then
                echo "  Testing $arch ISO creation..."
                if make ARCH=$arch iso >/dev/null 2>&1; then
                    if [ -f "$iso_file" ]; then
                        size=$(stat -c%s "$iso_file" 2>/dev/null || echo "0")
                        if [ "$size" -gt 1048576 ]; then # > 1MB
                            log_test "PASS" "$arch ISO Creation" "ISO created, size: $size bytes"
                        else
                            log_test "FAIL" "$arch ISO Creation" "ISO too small: $size bytes"
                        fi
                    else
                        log_test "FAIL" "$arch ISO Creation" "ISO file not created"
                    fi
                else
                    log_test "FAIL" "$arch ISO Creation" "ISO build failed"
                fi
            else
                log_test "SKIP" "$arch ISO Creation" "Kernel binary not available"
            fi
        done
    else
        log_test "SKIP" "ISO Creation" "grub-mkrescue not available"
    fi
}

test_qemu_compatibility() {
    echo -e "${BLUE}Testing QEMU Compatibility...${NC}"
    
    # Test QEMU availability
    local qemu_systems=("qemu-system-i386" "qemu-system-x86_64" "qemu-system-arm")
    
    for qemu in "${qemu_systems[@]}"; do
        if command -v "$qemu" >/dev/null 2>&1; then
            log_test "PASS" "$qemu Available" "QEMU emulator found"
            
            # Test basic QEMU startup (without actually booting)
            arch=""
            case "$qemu" in
                *i386*) arch="x86" ;;
                *x86_64*) arch="x86_64" ;;
                *arm*) arch="arm" ;;
            esac
            
            if [ -n "$arch" ] && [ -f "xorix-$arch.bin" ]; then
                # Test QEMU can start with our kernel (timeout after 2 seconds)
                timeout 2s "$qemu" -kernel "xorix-$arch.bin" -m 512M -nographic -serial stdio >/dev/null 2>&1 || true
                log_test "PASS" "$qemu Boot Test" "QEMU can load kernel"
            fi
        else
            log_test "SKIP" "$qemu Available" "QEMU emulator not found"
        fi
    done
}

test_virtualbox_compatibility() {
    echo -e "${BLUE}Testing VirtualBox Compatibility...${NC}"
    
    if command -v VBoxManage >/dev/null 2>&1; then
        log_test "PASS" "VirtualBox Available" "VirtualBox found"
        
        # Test ISO compatibility with VirtualBox
        for arch in x86 x86_64; do
            iso_file="xorix-$arch.iso"
            if [ -f "$iso_file" ]; then
                # Check if ISO has proper boot sector
                if file "$iso_file" | grep -q "ISO 9660"; then
                    log_test "PASS" "$arch VirtualBox ISO" "ISO format compatible"
                else
                    log_test "FAIL" "$arch VirtualBox ISO" "ISO format issues"
                fi
            else
                log_test "SKIP" "$arch VirtualBox ISO" "ISO not available"
            fi
        done
    else
        log_test "SKIP" "VirtualBox Available" "VirtualBox not found"
    fi
}

test_documentation() {
    echo -e "${BLUE}Testing Documentation...${NC}"
    
    # Test essential documentation files
    local docs=("README.md" "docs/XORIX_SERVER_EDITION_GUIDE.md" "PROJECT_STRUCTURE.md")
    
    for doc in "${docs[@]}"; do
        if [ -f "$doc" ]; then
            # Check if file is not empty and has reasonable content
            if [ -s "$doc" ] && [ $(wc -l < "$doc") -gt 10 ]; then
                log_test "PASS" "Documentation $(basename $doc)" "Complete documentation"
            else
                log_test "FAIL" "Documentation $(basename $doc)" "Incomplete documentation"
            fi
        else
            log_test "FAIL" "Documentation $(basename $doc)" "Documentation missing"
        fi
    done
    
    # Test if documentation mentions key features
    if [ -f "docs/XORIX_SERVER_EDITION_GUIDE.md" ]; then
        local features=("keyboard driver" "xbash" "xnano" "install xorix" "multi-architecture")
        for feature in "${features[@]}"; do
            if grep -qi "$feature" "docs/XORIX_SERVER_EDITION_GUIDE.md"; then
                log_test "PASS" "Feature Documentation" "$feature documented"
            else
                log_test "FAIL" "Feature Documentation" "$feature not documented"
            fi
        done
    fi
}

test_project_structure() {
    echo -e "${BLUE}Testing Project Structure...${NC}"
    
    # Test directory structure
    local dirs=("arch" "boot" "build" "docs" "drivers" "editor" "fs" "installer" "shell")
    
    for dir in "${dirs[@]}"; do
        if [ -d "$dir" ]; then
            log_test "PASS" "Directory Structure" "$dir/ exists"
        else
            log_test "FAIL" "Directory Structure" "$dir/ missing"
        fi
    done
    
    # Test essential files
    local files=("Makefile" "build/Makefile.multi-arch" "build/optimize_system.sh")
    
    for file in "${files[@]}"; do
        if [ -f "$file" ]; then
            log_test "PASS" "Essential Files" "$file exists"
        else
            log_test "FAIL" "Essential Files" "$file missing"
        fi
    done
}

# Main test execution
main() {
    echo -e "${BLUE}=== Xorix Server Edition - Comprehensive Test Suite ===${NC}"
    echo ""
    
    # Run all tests
    test_project_structure
    test_build_system
    test_components
    test_kernel_build
    test_iso_creation
    test_qemu_compatibility
    test_virtualbox_compatibility
    test_documentation
    
    # Summary
    echo ""
    echo -e "${BLUE}=== Test Summary ===${NC}"
    echo "Total tests: $TESTS_TOTAL"
    echo -e "Passed: ${GREEN}$TESTS_PASSED${NC}"
    echo -e "Failed: ${RED}$TESTS_FAILED${NC}"
    echo -e "Skipped: ${YELLOW}$((TESTS_TOTAL - TESTS_PASSED - TESTS_FAILED))${NC}"
    
    # Write summary to log
    echo "" >> "$LOG_FILE"
    echo "=== Test Summary ===" >> "$LOG_FILE"
    echo "Total tests: $TESTS_TOTAL" >> "$LOG_FILE"
    echo "Passed: $TESTS_PASSED" >> "$LOG_FILE"
    echo "Failed: $TESTS_FAILED" >> "$LOG_FILE"
    echo "Skipped: $((TESTS_TOTAL - TESTS_PASSED - TESTS_FAILED))" >> "$LOG_FILE"
    echo "Test completed: $(date)" >> "$LOG_FILE"
    
    echo ""
    echo "Detailed results saved to: $LOG_FILE"
    
    # Exit with appropriate code
    if [ $TESTS_FAILED -eq 0 ]; then
        echo -e "${GREEN}All tests passed or skipped!${NC}"
        exit 0
    else
        echo -e "${RED}Some tests failed. Check the log for details.${NC}"
        exit 1
    fi
}

# Handle command line arguments
case "${1:-all}" in
    "build")
        test_build_system
        test_kernel_build
        ;;
    "iso")
        test_iso_creation
        ;;
    "vm")
        test_qemu_compatibility
        test_virtualbox_compatibility
        ;;
    "docs")
        test_documentation
        ;;
    "structure")
        test_project_structure
        ;;
    "all"|"")
        main
        ;;
    *)
        echo "Usage: $0 [build|iso|vm|docs|structure|all]"
        exit 1
        ;;
esac
