ENTRY(loader)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    . = 0x0100000;

    .text :
    {
        *(.multiboot)
        *(.text*)
        *(.rodata)
    }

    .data :
    {
        __start_ctors = .;
        KEEP(*(.init_array))
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        __end_ctors = .;
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }


    .init_array : {
        PROVIDE(start_ctors = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE(end_ctors = .);
    }

    
    /DISCARD/ :
    {
        *(.fini_array*)
        *(.comment)
    }
}
