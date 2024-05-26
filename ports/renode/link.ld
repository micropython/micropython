MEMORY
{
    FLASH_FIRMWARE (rx) : ORIGIN = 0x00000000, LENGTH = 512k
    RAM (rwx) : ORIGIN =  0x20000000, LENGTH = 128k
}

SECTIONS
{
    .irqs : {
        . = ALIGN(4);
        KEEP(*(.rodata.interrupt_table .rodata.interrupt_table.*))
        . = ALIGN(4);
    } > FLASH_FIRMWARE

    .text : {
        *(.text*)
        . = ALIGN(4);
    } > FLASH_FIRMWARE

    .rodata : {
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH_FIRMWARE

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH_FIRMWARE

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH_FIRMWARE
    __exidx_end = .;

    /* End of .text-like segments */
    __etext = .;

    .data : {
        __data_start__ = .;
        *(.data*)
        /* All data end */
        __data_end__ = .;
    } > RAM AT> FLASH_FIRMWARE
    _ld_ocram_data_destination = ADDR(.data);
    _ld_ocram_data_flash_copy = LOADADDR(.data);
    _ld_ocram_data_size = SIZEOF(.data);

    /* Uninitialized data section */
    .bss  : {
        . = ALIGN(4);
        __bss_start__ = .;
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.bss*)))
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM
    _ld_ocram_bss_start = ADDR(.bss);
    _ld_ocram_bss_size = SIZEOF(.bss);
    _ld_heap_start = _ld_ocram_bss_start + _ld_ocram_bss_size;
    _ld_ocram_start = ORIGIN(RAM);
    _ld_ocram_end = ORIGIN(RAM) + LENGTH(RAM);
    _ld_heap_end = _ld_ocram_end;
}
