@ ARM Boot loader for Xorix Server Edition
@ Cortex-A8 compatible boot sequence

.section .text
.global _start

_start:
    @ Disable interrupts
    cpsid if
    
    @ Set up stack pointer
    ldr sp, =stack_top
    
    @ Set up vector table
    ldr r0, =vector_table
    mcr p15, 0, r0, c12, c0, 0  @ Set VBAR
    
    @ Enable VFP if available
    mrc p15, 0, r0, c1, c0, 2   @ Read CPACR
    orr r0, r0, #0x300000        @ Enable CP10 and CP11
    mcr p15, 0, r0, c1, c0, 2   @ Write CPACR
    isb
    mov r0, #0x40000000          @ Enable VFP
    vmsr fpexc, r0
    
    @ Clear BSS section
    ldr r0, =__bss_start
    ldr r1, =__bss_end
    mov r2, #0
clear_bss:
    cmp r0, r1
    strlo r2, [r0], #4
    blo clear_bss
    
    @ Call kernel main
    bl kernel_main
    
    @ Halt if kernel returns
halt:
    wfi
    b halt

@ Exception vectors
.align 5
vector_table:
    ldr pc, =_start          @ Reset
    ldr pc, =undefined_handler
    ldr pc, =svc_handler
    ldr pc, =prefetch_handler
    ldr pc, =data_handler
    nop                      @ Reserved
    ldr pc, =irq_handler
    ldr pc, =fiq_handler

@ Default exception handlers
undefined_handler:
    b undefined_handler

svc_handler:
    b svc_handler

prefetch_handler:
    b prefetch_handler

data_handler:
    b data_handler

irq_handler:
    push {r0-r12, lr}
    bl handle_irq
    pop {r0-r12, lr}
    subs pc, lr, #4

fiq_handler:
    b fiq_handler

@ Stack space
.section .bss
.align 4
stack_bottom:
.space 16384
stack_top:
