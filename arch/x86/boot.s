# Xorix Server Edition - Clean Frame-by-Frame Logo Bootloader
# Elegant loading animation with progressive XORIX logo display

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

# Stack
.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.type _start, @function

_start:
    mov $stack_top, %esp
    pushl $0
    popf
    
    pushl %ebx
    pushl %eax
    
    call show_logo_animation
    
    popl %eax
    popl %ebx
    
    call kernel_main
    
    cli
1:  hlt
    jmp 1b

# Frame-by-frame XORIX logo loading animation
show_logo_animation:
    pushl %eax
    pushl %ecx
    pushl %edi
    
    # Clear screen - dark blue background
    mov $0xB8000, %edi
    mov $0x1120, %ax        # Dark blue background
    mov $2000, %ecx
    rep stosw
    
    # Frame 1: Show 'X'
    mov $0xB8000, %edi
    add $1638, %edi         # Center position (row 10, col 37)
    mov $0x3F58, %ax        # 'X' bright cyan on blue
    stosw
    call frame_delay
    
    # Frame 2: Show 'XO'
    mov $0x3F4F, %ax        # 'O' bright cyan on blue
    stosw
    call frame_delay
    
    # Frame 3: Show 'XOR'
    mov $0x3F52, %ax        # 'R' bright cyan on blue
    stosw
    call frame_delay
    
    # Frame 4: Show 'XORI'
    mov $0x3F49, %ax        # 'I' bright cyan on blue
    stosw
    call frame_delay
    
    # Frame 5: Show 'XORIX'
    mov $0x3F58, %ax        # 'X' bright cyan on blue
    stosw
    call frame_delay
    
    # Frame 6: Add subtitle line 1
    mov $0xB8000, %edi
    add $1792, %edi         # Next line center (row 11, col 35)
    mov $0x1E53, %ax        # 'S' yellow on blue
    stosw
    mov $0x1E45, %ax        # 'E'
    stosw
    mov $0x1E52, %ax        # 'R'
    stosw
    mov $0x1E56, %ax        # 'V'
    stosw
    mov $0x1E45, %ax        # 'E'
    stosw
    mov $0x1E52, %ax        # 'R'
    stosw
    call frame_delay
    
    # Frame 7: Complete subtitle
    mov $0x1E20, %ax        # space
    stosw
    mov $0x1E45, %ax        # 'E'
    stosw
    mov $0x1E44, %ax        # 'D'
    stosw
    mov $0x1E49, %ax        # 'I'
    stosw
    mov $0x1E54, %ax        # 'T'
    stosw
    mov $0x1E49, %ax        # 'I'
    stosw
    mov $0x1E4F, %ax        # 'O'
    stosw
    mov $0x1E4E, %ax        # 'N'
    stosw
    call frame_delay
    
    # Frame 8: Loading indicator
    mov $0xB8000, %edi
    add $3040, %edi         # Bottom area (row 19, col 35)
    mov $0x0F4C, %ax        # 'L' white on blue
    stosw
    mov $0x0F6F, %ax        # 'o'
    stosw
    mov $0x0F61, %ax        # 'a'
    stosw
    mov $0x0F64, %ax        # 'd'
    stosw
    mov $0x0F69, %ax        # 'i'
    stosw
    mov $0x0F6E, %ax        # 'n'
    stosw
    mov $0x0F67, %ax        # 'g'
    stosw
    call frame_delay
    
    # Frame 9-12: Animated dots
    mov $4, %ecx
dot_animation:
    mov $0x0F2E, %ax        # '.' white on blue
    stosw
    call frame_delay
    loop dot_animation
    
    # Final pause before kernel
    call long_delay
    
    popl %edi
    popl %ecx
    popl %eax
    ret

# Frame delay (longer for visibility)
frame_delay:
    pushl %eax
    mov $0x800000, %eax
frame_delay_loop:
    dec %eax
    jnz frame_delay_loop
    popl %eax
    ret

# Long delay (final pause)
long_delay:
    pushl %eax
    mov $0x1000000, %eax
long_delay_loop:
    dec %eax
    jnz long_delay_loop
    popl %eax
    ret

.size _start, . - _start
