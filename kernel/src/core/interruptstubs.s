.code32         
.set IRQ_BASE, 0x20   

.section .text

.global HandleInterruptRequest0x00
.global HandleInterruptRequest0x01
.global IgnoreInterruptRequest

.extern handleInterrupt   


.macro HandleInterruptRequest num
\name:
    movb $\num+IRQ_BASE, interruptnumber  
    jmp int_bottom
.endm


HandleInterruptRequest0x00:
    movb $0x00+IRQ_BASE, interruptnumber
    jmp int_bottom

HandleInterruptRequest0x01:
    movb $0x01+IRQ_BASE, interruptnumber
    jmp int_bottom

IgnoreInterruptRequest:
    movb $0xFF, interruptnumber            
    jmp int_bottom


int_bottom:
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    pushl %esp                
    pushl interruptnumber    
    call handleInterrupt  
    movl %eax, %esp        

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

    iret


.section .data
interruptnumber: .byte 0
