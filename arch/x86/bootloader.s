# Xorix Server Edition - Enhanced Bootloader with Logo and Loading Animation
# Compatible with QEMU and standard bootloaders

.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Multiboot header
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Stack for our kernel
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# VGA text mode constants
.set VGA_WIDTH, 80
.set VGA_HEIGHT, 25
.set VGA_MEMORY, 0xB8000

# Colors
.set COLOR_BLACK, 0
.set COLOR_BLUE, 1
.set COLOR_GREEN, 2
.set COLOR_CYAN, 3
.set COLOR_RED, 4
.set COLOR_MAGENTA, 5
.set COLOR_BROWN, 6
.set COLOR_LIGHT_GREY, 7
.set COLOR_DARK_GREY, 8
.set COLOR_LIGHT_BLUE, 9
.set COLOR_LIGHT_GREEN, 10
.set COLOR_LIGHT_CYAN, 11
.set COLOR_LIGHT_RED, 12
.set COLOR_LIGHT_MAGENTA, 13
.set COLOR_YELLOW, 14
.set COLOR_WHITE, 15

.section .text
.global _start
.type _start, @function

_start:
    # Set up the stack
    mov $stack_top, %esp
    
    # Save multiboot info
    push %eax
    push %ebx
    
    # Show bootloader splash screen
    call show_bootloader_splash
    
    # Show loading animation
    call show_loading_animation
    
    # Restore multiboot info
    pop %ebx
    pop %eax
    
    # Call the kernel
    call kernel_main
    
    # Hang if kernel returns
    cli
1:  hlt
    jmp 1b

# Function to clear screen with specific color
clear_screen:
    pushl %eax
    pushl %ecx
    pushl %edi
    
    mov $VGA_MEMORY, %edi
    mov $0x1F20, %ax        # White on blue background with space
    mov $(VGA_WIDTH * VGA_HEIGHT), %ecx
    rep stosw
    
    popl %edi
    popl %ecx
    popl %eax
    ret

# Function to set cursor position
set_cursor:
    pushl %eax
    pushl %ebx
    pushl %edx
    
    # Calculate position (row * 80 + col)
    mov 16(%esp), %eax      # row
    mov $VGA_WIDTH, %ebx
    mul %ebx
    add 20(%esp), %eax      # add column
    
    # Set cursor position via VGA registers
    mov %ax, %bx
    mov $0x3D4, %dx
    mov $0x0F, %al
    out %al, %dx
    mov $0x3D5, %dx
    mov %bl, %al
    out %al, %dx
    
    mov $0x3D4, %dx
    mov $0x0E, %al
    out %al, %dx
    mov $0x3D5, %dx
    mov %bh, %al
    out %al, %dx
    
    popl %edx
    popl %ebx
    popl %eax
    ret

# Function to print string at specific position with color
print_string_at:
    pushl %eax
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi
    
    mov 32(%esp), %esi      # string address
    mov 36(%esp), %eax      # row
    mov 40(%esp), %ebx      # column
    mov 44(%esp), %cl       # color
    
    # Calculate VGA memory position
    mov $VGA_WIDTH, %edx
    mul %edx
    add %ebx, %eax
    shl $1, %eax            # multiply by 2 (each char takes 2 bytes)
    add $VGA_MEMORY, %eax
    mov %eax, %edi
    
print_loop:
    lodsb                   # load byte from string
    test %al, %al           # check for null terminator
    jz print_done
    
    stosb                   # store character
    mov %cl, %al            # store color
    stosb
    jmp print_loop
    
print_done:
    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax
    ret

# Function to show bootloader splash screen with logo
show_bootloader_splash:
    pushl %eax
    pushl %ebx
    pushl %ecx
    
    # Clear screen with blue background
    call clear_screen
    
    # Draw ASCII art logo
    push $(COLOR_LIGHT_CYAN | (COLOR_BLUE << 4))
    push $25
    push $5
    push $logo_line1
    call print_string_at
    add $16, %esp
    
    push $(COLOR_LIGHT_CYAN | (COLOR_BLUE << 4))
    push $26
    push $6
    push $logo_line2
    call print_string_at
    add $16, %esp
    
    push $(COLOR_LIGHT_CYAN | (COLOR_BLUE << 4))
    push $27
    push $7
    push $logo_line3
    call print_string_at
    add $16, %esp
    
    push $(COLOR_LIGHT_CYAN | (COLOR_BLUE << 4))
    push $28
    push $8
    push $logo_line4
    call print_string_at
    add $16, %esp
    
    push $(COLOR_LIGHT_CYAN | (COLOR_BLUE << 4))
    push $29
    push $9
    push $logo_line5
    call print_string_at
    add $16, %esp
    
    # Title
    push $(COLOR_YELLOW | (COLOR_BLUE << 4))
    push $30
    push $11
    push $title_text
    call print_string_at
    add $16, %esp
    
    # Version
    push $(COLOR_WHITE | (COLOR_BLUE << 4))
    push $32
    push $12
    push $version_text
    call print_string_at
    add $16, %esp
    
    # Copyright
    push $(COLOR_LIGHT_GREY | (COLOR_BLUE << 4))
    push $35
    push $22
    push $copyright_text
    call print_string_at
    add $16, %esp
    
    popl %ecx
    popl %ebx
    popl %eax
    ret

# Function to show loading animation
show_loading_animation:
    pushl %eax
    pushl %ebx
    pushl %ecx
    pushl %edx
    
    mov $20, %ecx           # Animation frames
    
animation_loop:
    # Show loading text
    push $(COLOR_WHITE | (COLOR_BLUE << 4))
    push $25
    push $15
    push $loading_text
    call print_string_at
    add $16, %esp
    
    # Show spinner
    mov %ecx, %eax
    and $3, %eax            # Get animation frame (0-3)
    
    push $(COLOR_LIGHT_GREEN | (COLOR_BLUE << 4))
    push $54
    push $15
    
    cmp $0, %eax
    je spinner_0
    cmp $1, %eax
    je spinner_1
    cmp $2, %eax
    je spinner_2
    jmp spinner_3
    
spinner_0:
    push $spinner_char1
    jmp show_spinner
spinner_1:
    push $spinner_char2
    jmp show_spinner
spinner_2:
    push $spinner_char3
    jmp show_spinner
spinner_3:
    push $spinner_char4
    
show_spinner:
    call print_string_at
    add $16, %esp
    
    # Progress bar
    mov $21, %eax
    sub %ecx, %eax          # Calculate progress
    
    push $(COLOR_LIGHT_GREY | (COLOR_BLUE << 4))
    push $20
    push $17
    push $progress_start
    call print_string_at
    add $16, %esp
    
    # Draw progress
    mov $21, %eax
    sub %ecx, %eax
    mov %eax, %edx
    
progress_loop:
    test %edx, %edx
    jz progress_empty
    
    push $(COLOR_LIGHT_GREEN | (COLOR_BLUE << 4))
    push $60
    push $17
    push $progress_full
    call print_string_at
    add $16, %esp
    
    dec %edx
    jmp progress_loop
    
progress_empty:
    # Delay
    mov $0x100000, %eax
delay_loop:
    dec %eax
    jnz delay_loop
    
    loop animation_loop
    
    # Final message
    push $(COLOR_LIGHT_GREEN | (COLOR_BLUE << 4))
    push $30
    push $19
    push $ready_text
    call print_string_at
    add $16, %esp
    
    # Final delay
    mov $0x500000, %eax
final_delay:
    dec %eax
    jnz final_delay
    
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax
    ret

# Data section
.section .rodata

logo_line1: .asciz "##   ## ####### ######  ## ##   ##"
logo_line2: .asciz " ## ##  ##   ## ##   ## ##  ## ## "
logo_line3: .asciz "  ###   ##   ## ######  ##   ###  "
logo_line4: .asciz " ## ##  ##   ## ##   ## ##  ## ## "
logo_line5: .asciz "##   ## ####### ##   ## ## ##   ##"

title_text: .asciz "SERVER EDITION v1.0"
version_text: .asciz "Unified Stable Kernel"
copyright_text: .asciz "(c) 2025 Xorix Project"

loading_text: .asciz "Loading Xorix Server Edition"
ready_text: .asciz "System Ready - Starting Kernel..."

spinner_char1: .asciz "|"
spinner_char2: .asciz "/"
spinner_char3: .asciz "-"
spinner_char4: .asciz "\\"

progress_start: .asciz "["
progress_full: .asciz "#"
progress_empty: .asciz "-"

.size _start, . - _start
