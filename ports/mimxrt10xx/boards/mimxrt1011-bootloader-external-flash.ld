ENTRY(Reset_Handler)

_minimum_stack_size = 64K;
_minimum_heap_size = 0;

MEMORY
{
    FLASH (rx)            : ORIGIN = 0x60000000, LENGTH = 8M
    FLASH_BOOTLOADER (rx) : ORIGIN = 0x60000000, LENGTH = 48K
    FLASH_ISR (rx)        : ORIGIN = 0x6000C000, LENGTH = 1K
    FLASH_TEXT (rx)       : ORIGIN = 0x6000C400, LENGTH = 975K
    FLASH_FATFS (r)       : ORIGIN = 0x60100000, LENGTH = 7M
    RAM (rwx)             : ORIGIN = 0x00000000, LENGTH = 32K
    OCRAM (rwx)           : ORIGIN = 0x20200000, LENGTH = 64K
}

_estack = ORIGIN(OCRAM) + LENGTH(OCRAM);
_bootloader_dbl_tap = ORIGIN(RAM) + LENGTH(RAM) - 4;

__fatfs_flash_start_addr = ORIGIN(FLASH_FATFS);
__fatfs_flash_length     = LENGTH(FLASH_FATFS);

__RAM_VECTOR_TABLE_SIZE_BYTES = 0;

SECTIONS
{
    .interrupts :
    {
        __VECTOR_TABLE = .;
        __VECTOR_RAM = .;

        . = ALIGN(4);
        KEEP(*(.isr_vector))         /* Startup code */
        . = ALIGN(4);
    } > FLASH_ISR

    .text :
    {
        . = ALIGN(4);
    *(EXCLUDE_FILE(
        *flexspi_nor_flash_ops.o
        *fsl_flexspi.o
    ) .text*)                /* .text* sections (code) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } > FLASH_TEXT

    .ARM.exidx :
    {
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        _etext = .;        /* define a global symbol at end of code */
        __etext = .;       /* define a global symbol at end of code */
    } > FLASH_TEXT

    /* used by the startup to initialize data */
    _sidata = .;

    .data : AT (_sidata)
    {
        . = ALIGN(4);
        __data_start__ = .;     /* create a global symbol at data start */

        *(.data*)               /* .data* sections */
        *flexspi_nor_flash_ops.o(.text*)
        *fsl_flexspi.o(.text*)
        . = ALIGN(4);

        __data_end__ = .;       /* define a global symbol at data end */
    } > RAM

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;

        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        __bss_end__ = .;
        PROVIDE(end = .);
    } > RAM

    .heap :
    {
        . = ALIGN(8);
        _heap_start = .;    /* define a global symbol at heap start */

        . += _minimum_heap_size;

        __HeapLimit = .;
    } > OCRAM

    .stack :
    {
        . = ALIGN(8);
        . += _minimum_stack_size;
    } > OCRAM

    .ARM.attributes 0 : { *(.ARM.attributes) }
}

ASSERT(__data_end__ <= ORIGIN(FLASH_TEXT) + LENGTH(FLASH_TEXT), "region FLASH_TEXT overflowed with text and data")
ASSERT(_estack - _minimum_stack_size >= __HeapLimit, "region OCRAM overflowed with stack and heap")
