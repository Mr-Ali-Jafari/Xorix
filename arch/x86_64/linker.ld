/* x86_64 Linker script for Xorix Server Edition */

ENTRY(_start)

SECTIONS
{
    . = 1M;

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
        . = ALIGN(4K);
    }

    /* Higher half kernel mapping */
    . += 0xFFFFFFFF80000000;

    .text.high : AT(ADDR(.text.high) - 0xFFFFFFFF80000000)
    {
        *(.text.high)
    }
}
