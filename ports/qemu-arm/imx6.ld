/* Vector table is at 0x00000000, entry point is 0x10000000. */

MEMORY
{
    ROM : ORIGIN = 0x00000000, LENGTH = 96K
    RAM : ORIGIN = 0x10000000, LENGTH = 128M
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    .rom : {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > ROM

    .text : {
        . = ALIGN(4);
        *(.text.Reset_Handler)
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } > RAM

    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM
}
