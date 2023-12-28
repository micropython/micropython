/* Template for SAMD21/SAMD51 linking. dollar-sign-curly-bracket items are replaced with strings. */

/* Specify the memory areas */
MEMORY
{
    FLASH_BOOTLOADER (rx): ORIGIN = ${BOOTLOADER_START_ADDR}, LENGTH = ${BOOTLOADER_SIZE}

    FLASH_FIRMWARE (rx) : ORIGIN = ${CIRCUITPY_FIRMWARE_START_ADDR}, LENGTH = ${CIRCUITPY_FIRMWARE_SIZE}
    FLASH_FILESYSTEM (r) : ORIGIN = ${CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR}, LENGTH = ${CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE}
    FLASH_CONFIG (r) : ORIGIN = ${CIRCUITPY_INTERNAL_CONFIG_START_ADDR}, LENGTH = ${CIRCUITPY_INTERNAL_CONFIG_SIZE}
    FLASH_NVM (r) : ORIGIN = ${CIRCUITPY_INTERNAL_NVM_START_ADDR}, LENGTH = ${CIRCUITPY_INTERNAL_NVM_SIZE}

    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = ${RAM_SIZE}
}

/* top end of the stack */
/* stack must be double-word (8 byte) aligned */
_estack = ORIGIN(RAM) + LENGTH(RAM) - 8;
_bootloader_dbl_tap = ORIGIN(RAM) + LENGTH(RAM) - 4;

/* define output sections */
SECTIONS
{
    /* The program code and other data goes into FLASH */
    .text :
    {
        . = ALIGN(4);
        _sfixed = .;
        KEEP(*(.vectors)) /* isr vector table */

        __property_getter_start = .;
        *(.property_getter)
        __property_getter_end = .;
        __property_getset_start = .;
        *(.property_getset)
        __property_getset_end = .;

        /* Sort text sections so that they have fewer *fill* bytes needed. */
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.text)))           /* .text sections (code) */
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.text*)))          /* .text* sections (code) */

        /* Don't sort rodata because it impacts codegen size. */
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */

        . = ALIGN(4);
    } >FLASH_FIRMWARE

    .ARM.exidx :
    {
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        _etext = .;        /* define a global symbol at end of code */
        _sidata = .;       /* start of .data section */
    } >FLASH_FIRMWARE

    /* Data accessed by the CAN peripheral must be in the first 64kB RAM */
    /* place it at the very start of RAM, before the .data section */
    /* it is zeroed by reset_port */
    .canram (NOLOAD) :
    {
        . = ALIGN(4);
        *(.canram)
    } > RAM

    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH_FIRMWARE (inidata).
    It is one task of the startup to copy the initial values from FLASH_FIRMWARE to RAM. */
    .data :
    {
        . = ALIGN(4);
        _srelocate = .;        /* create a global symbol at data start; used by startup code in order to initialize the .data section in RAM */
        *(.ramfunc)
        *(.ramfunc*)
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.data)))           /* .data sections */
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.data*)))          /* .data* sections */

        . = ALIGN(4);
        _erelocate = .;        /* define a global symbol at data end; used by startup code in order to initialize the .data section in RAM */
    } >RAM AT> FLASH_FIRMWARE

    /* Uninitialized data section */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;
        _szero = .;         /* define a global symbol at bss start; used by startup code */
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.bss)))
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.bss*)))
        *(COMMON)

        . = ALIGN(4);
        _ezero = .;         /* define a global symbol at bss end; used by startup code */
        _ebss = .;
    } >RAM

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
