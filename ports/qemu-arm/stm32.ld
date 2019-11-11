INCLUDE build/ld_vars.ld;

MEMORY
{
    ROM : ORIGIN = 0x00000000, LENGTH = 1M
    RAM : ORIGIN = 0x20000000, LENGTH = 128K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    .text : {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        ASSERT(. == MP_SMALL_DATA_CONST_NONE, "bad MP_SMALL_DATA_CONST_NONE");
        *(.rodata.mp_const_none_obj)
        ASSERT(. == MP_SMALL_DATA_CONST_FALSE, "bad MP_SMALL_DATA_CONST_FALSE");
        *(.rodata.mp_const_false_obj)
        ASSERT(. == MP_SMALL_DATA_CONST_TRUE, "bad MP_SMALL_DATA_CONST_TRUE");
        *(.rodata.mp_const_true_obj)
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } > ROM

    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM
}
