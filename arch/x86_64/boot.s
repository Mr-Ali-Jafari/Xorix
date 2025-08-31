# x86_64 Boot loader for Xorix Server Edition
# Multiboot2-compliant boot sequence with long mode setup

.set ALIGN,    1<<0             
.set MEMINFO,  1<<1             
.set FLAGS,    ALIGN | MEMINFO  
.set MAGIC,    0x1BADB002       
.set CHECKSUM, -(MAGIC + FLAGS) 

# Multiboot header
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Stack for x86_64
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB stack
stack_top:

# Page tables for long mode
.align 4096
boot_page_directory:
.skip 4096
boot_page_table:
.skip 4096

# Entry point (32-bit mode initially)
.section .text
.global _start
.type _start, @function

_start:
    # Set up stack
    mov $stack_top, %esp
    
    # Save multiboot info
    push %ebx
    push %eax
    
    # Check for long mode support
    call check_long_mode
    test %eax, %eax
    jz no_long_mode
    
    # Set up paging for long mode
    call setup_page_tables
    call enable_paging
    
    # Load GDT and switch to long mode
    lgdt gdt64_pointer
    ljmp $0x08, $long_mode_start

no_long_mode:
    # Fall back to 32-bit mode
    call kernel_main_32
    jmp halt

check_long_mode:
    # Check CPUID support
    pushfl
    pop %eax
    mov %eax, %ecx
    xor $0x200000, %eax
    push %eax
    popfl
    pushfl
    pop %eax
    push %ecx
    popfl
    cmp %eax, %ecx
    je no_cpuid
    
    # Check for long mode
    mov $0x80000000, %eax
    cpuid
    cmp $0x80000001, %eax
    jb no_long_mode_support
    
    mov $0x80000001, %eax
    cpuid
    test $0x20000000, %edx
    jz no_long_mode_support
    
    mov $1, %eax
    ret

no_cpuid:
no_long_mode_support:
    mov $0, %eax
    ret

setup_page_tables:
    # Identity map first 2MB
    mov $boot_page_table, %edi
    mov $0x83, %eax  # Present, writable, huge page
    mov $512, %ecx
    
fill_page_table:
    mov %eax, (%edi)
    add $0x200000, %eax
    add $8, %edi
    loop fill_page_table
    
    # Set up page directory
    mov $boot_page_directory, %edi
    mov $boot_page_table, %eax
    or $0x83, %eax
    mov %eax, (%edi)
    
    ret

enable_paging:
    # Load page directory
    mov $boot_page_directory, %eax
    mov %eax, %cr3
    
    # Enable PAE
    mov %cr4, %eax
    or $0x20, %eax
    mov %eax, %cr4
    
    # Enable long mode
    mov $0xC0000080, %ecx
    rdmsr
    or $0x100, %eax
    wrmsr
    
    # Enable paging
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    
    ret

.code64
long_mode_start:
    # Set up 64-bit segments
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    
    # Set up 64-bit stack
    mov $stack_top, %rsp
    
    # Call 64-bit kernel
    call kernel_main
    
halt:
    cli
1:  hlt
    jmp 1b

# GDT for 64-bit mode
.section .data
gdt64:
    .quad 0x0000000000000000  # Null descriptor
    .quad 0x00AF9A000000FFFF  # Code segment
    .quad 0x00CF92000000FFFF  # Data segment

gdt64_pointer:
    .word gdt64_pointer - gdt64 - 1
    .quad gdt64

.size _start, . - _start
