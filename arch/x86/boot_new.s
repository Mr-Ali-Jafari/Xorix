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
    
    # Reset EFLAGS
    pushl $0
    popf
    
    # Save multiboot info
    pushl %ebx  # multiboot info structure
    pushl %eax  # multiboot magic number
    
    # Show bootloader splash screen
    call show_bootloader_splash
    
    # Show loading animation
    call show_loading_animation
    
    # Restore multiboot info
    popl %eax
    popl %ebx
    
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
    
    # Draw ASCII art logo - Line 1
    pushl $(COLOR_LIGHT_CYAN | (COLOR_BLUE << 4))
    pushl $25
    pushl $5
    pushl $logo_line1
    call print_string_at
    addl $16, %esp
    
    # Draw ASCII art logo - Line 2
    pushl $(COLOR_LIGHT_CYAN | (COLOR_BLUE << 4))
    pushl $26
    pushl $6
    pushl $logo_line2
    call print_string_at
    addl $16, %esp
    
    # Draw ASCII art logo - Line 3
    pushl $(COLOR_LIGHT_CYAN | (COLOR_BLUE << 4))
    pushl $27
    pushl $7
    pushl $logo_line3
    call print_string_at
    addl $16, %esp
    
    # Draw ASCII art logo - Line 4
    pushl $(COLOR_LIGHT_CYAN | (COLOR_BLUE << 4))
    pushl $28
    pushl $8
    pushl $logo_line4
    call print_string_at
    addl $16, %esp
    
    # Draw ASCII art logo - Line 5
    pushl $(COLOR_LIGHT_CYAN | (COLOR_BLUE << 4))
    pushl $29
    pushl $9
    pushl $logo_line5
    call print_string_at
    addl $16, %esp
    
    # Title
    pushl $(COLOR_YELLOW | (COLOR_BLUE << 4))
    pushl $30
    pushl $11
    pushl $title_text
    call print_string_at
    addl $16, %esp
    
    # Version
    pushl $(COLOR_WHITE | (COLOR_BLUE << 4))
    pushl $32
    pushl $12
    pushl $version_text
    call print_string_at
    addl $16, %esp
    
    # Copyright
    pushl $(COLOR_LIGHT_GREY | (COLOR_BLUE << 4))
    pushl $35
    pushl $22
    pushl $copyright_text
    call print_string_at
    addl $16, %esp
    
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
    pushl $(COLOR_WHITE | (COLOR_BLUE << 4))
    pushl $25
    pushl $15
    pushl $loading_text
    call print_string_at
    addl $16, %esp
    
    # Show spinner
    mov %ecx, %eax
    and $3, %eax            # Get animation frame (0-3)
    
    pushl $(COLOR_LIGHT_GREEN | (COLOR_BLUE << 4))
    pushl $54
    pushl $15
    
    cmp $0, %eax
    je spinner_0
    cmp $1, %eax
    je spinner_1
    cmp $2, %eax
    je spinner_2
    jmp spinner_3
    
spinner_0:
    pushl $spinner_char1
    jmp show_spinner
spinner_1:
    pushl $spinner_char2
    jmp show_spinner
spinner_2:
    pushl $spinner_char3
    jmp show_spinner
spinner_3:
    pushl $spinner_char4
    
show_spinner:
    call print_string_at
    addl $16, %esp
    
    # Progress bar frame
    pushl $(COLOR_LIGHT_GREY | (COLOR_BLUE << 4))
    pushl $20
    pushl $17
    pushl $progress_start
    call print_string_at
    addl $16, %esp
    
    # Draw progress bars
    mov $21, %eax
    sub %ecx, %eax          # Calculate progress (0-20)
    mov %eax, %edx
    mov $21, %ebx           # Start column for progress
    
progress_draw_loop:
    test %edx, %edx
    jz progress_empty_draw
    
    pushl $(COLOR_LIGHT_GREEN | (COLOR_BLUE << 4))
    pushl %ebx
    pushl $17
    pushl $progress_full
    call print_string_at
    addl $16, %esp
    
    inc %ebx
    dec %edx
    jmp progress_draw_loop
    
progress_empty_draw:
    # Draw remaining empty progress
    mov $41, %eax           # End column
    sub %ebx, %eax          # Remaining spaces
    
empty_draw_loop:
    test %eax, %eax
    jz progress_end_bracket
    
    pushl $(COLOR_DARK_GREY | (COLOR_BLUE << 4))
    pushl %ebx
    pushl $17
    pushl $progress_empty
    call print_string_at
    addl $16, %esp
    
    inc %ebx
    dec %eax
    jmp empty_draw_loop
    
progress_end_bracket:
    # End bracket
    pushl $(COLOR_LIGHT_GREY | (COLOR_BLUE << 4))
    pushl $42
    pushl $17
    pushl $progress_end
    call print_string_at
    addl $16, %esp
    
    # Delay
    mov $0x200000, %eax
delay_loop:
    dec %eax
    jnz delay_loop
    
    loop animation_loop
    
    # Final message
    pushl $(COLOR_LIGHT_GREEN | (COLOR_BLUE << 4))
    pushl $30
    pushl $19
    pushl $ready_text
    call print_string_at
    addl $16, %esp
    
    # Final delay
    mov $0x800000, %eax
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
progress_end: .asciz "]"

.size _start, . - _start
