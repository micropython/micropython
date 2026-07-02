SECTIONS
{
    . = 0;
    _start = .;
    .head : {
        KEEP(*(.head))
    }
    /* Put this at 0x1700 which is right after our exception
     * vectors in head.S.
     */
    . = 0x1700;
    .text : {
        *(.text)
        *(.text.*)
        . = ALIGN(4K);
        *(.rodata)
        *(.rodata.*)
    }
    . = ALIGN(4K);
    .data : {
        *(.data)
        *(.data.*)
        . = ALIGN(4K);
        *(.bss)
        *(.bss.*)
    }
    . = ALIGN(4K);
    .stack : {
        PROVIDE(_sstack = .);
        . = . + 128K;
        PROVIDE(_estack = .);
    } = 0x0000000000000000
}
