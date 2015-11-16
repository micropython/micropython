/*
    GNU linker script for MPY external modules
*/

SECTIONS
{
    .all :
    {
        . = ALIGN(4);
        KEEP(*(.mpyheader))
        KEEP(*(.mpytext))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    }
}
