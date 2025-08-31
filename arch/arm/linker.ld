/* ARM Linker script for Xorix Server Edition */

ENTRY(_start)

SECTIONS
{
    . = 0x8000;  /* ARM kernel load address */

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    __bss_start = .;
    .bss : {
        *(COMMON)
        *(.bss)
    }
    __bss_end = .;
}
