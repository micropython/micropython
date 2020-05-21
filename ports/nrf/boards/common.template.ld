/*
    GNU linker script for NRF52840 w/ s140 6.0.0 SoftDevice
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = ${FLASH_SIZE} /* entire flash */
    /* nRF SoftDevice */
    FLASH_MBR (rx)  : ORIGIN = ${MBR_START_ADDR}, LENGTH = ${MBR_SIZE}
    FLASH_SD (rx)   : ORIGIN = ${SD_FLASH_START_ADDR}, LENGTH = ${SD_FLASH_SIZE}
    FLASH_ISR (rx)  : ORIGIN = ${ISR_START_ADDR}, LENGTH = ${ISR_SIZE}
    FLASH_FIRMWARE (rx) : ORIGIN = ${CIRCUITPY_FIRMWARE_START_ADDR}, LENGTH = ${CIRCUITPY_FIRMWARE_SIZE}
    FLASH_BLE_CONFIG (r) : ORIGIN = ${CIRCUITPY_BLE_CONFIG_START_ADDR}, LENGTH = ${CIRCUITPY_BLE_CONFIG_SIZE}
    FLASH_NVM (r)   : ORIGIN = ${CIRCUITPY_INTERNAL_NVM_START_ADDR}, LENGTH = ${CIRCUITPY_INTERNAL_NVM_SIZE}
    FLASH_FATFS (r) : ORIGIN = ${CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR}, LENGTH = ${CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE}
    FLASH_BOOTLOADER (rx) : ORIGIN = ${BOOTLOADER_START_ADDR}, LENGTH = ${BOOTLOADER_SIZE}
    FLASH_BOOTLOADER_SETTINGS (r) : ORIGIN = ${BOOTLOADER_SETTINGS_START_ADDR}, LENGTH = ${BOOTLOADER_SETTINGS_SIZE}


    /* 0x2000000 - RAM:ORIGIN is reserved for Softdevice */
    /* SoftDevice 6.1.0 with 5 connections and various increases takes just under 64kiB.
    /* To measure the minimum required amount of memory for given configuration, set this number
       high enough to work and then check the mutation of the value done by sd_ble_enable. */
    SPIM3_RAM (rw)  : ORIGIN = 0x20000000 + ${SOFTDEVICE_RAM_SIZE}, LENGTH = ${SPIM3_BUFFER_SIZE}
    RAM (xrw)       : ORIGIN = 0x20000000 + ${SOFTDEVICE_RAM_SIZE} + ${SPIM3_BUFFER_SIZE}, LENGTH = ${RAM_SIZE} - ${SOFTDEVICE_RAM_SIZE} -${SPIM3_BUFFER_SIZE}

}

/* produce a link error if there is not this amount of RAM available */
_minimum_heap_size = 0;

/* top end of the stack */

/*_stack_end = ORIGIN(RAM) + LENGTH(RAM);*/
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_end = 0x20020000; /* tunable */

/* nrf52840 SPIM3 needs its own area to work around hardware problems. Nothing else may use this space. */
_spim3_ram = ORIGIN(SPIM3_RAM);
_spim3_ram_end = ORIGIN(SPIM3_RAM) + LENGTH(RAM);

/* define output sections */
SECTIONS
{
    /* The startup code goes first into FLASH */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector)) /* Startup code */

        . = ALIGN(4);
    } >FLASH_ISR

    /* The program code and other data goes into FLASH */
    .text :
    {
        . = ALIGN(4);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    /*  *(.glue_7)   */    /* glue arm to thumb code */
    /*  *(.glue_7t)  */    /* glue thumb to arm code */

        . = ALIGN(4);
        _etext = .;        /* define a global symbol at end of code */
    } >FLASH_FIRMWARE

    /* used by the startup to initialize data */
    _sidata = .;

    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH (inidata).
    It is one task of the startup to copy the initial values from FLASH to RAM. */
    .data : AT (_sidata)
    {
        . = ALIGN(4);
        _sdata = .;        /* create a global symbol at data start; used by startup code in order to initialise the .data section in RAM */
        _ram_start = .;    /* create a global symbol at ram start for garbage collector */
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */

        . = ALIGN(4);
        _edata = .;        /* define a global symbol at data end; used by startup code in order to initialise the .data section in RAM */
    } >RAM

    /* Zero-initialized data section */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;         /* define a global symbol at bss start; used by startup code */
        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end; used by startup code and GC */
    } >RAM

    /* Uninitialized data section
    Data placed into this section will remain unchanged across reboots. */
    .uninitialized (NOLOAD) :
    {
        . = ALIGN(4);
        _suninitialized = .; /* define a global symbol at uninitialized start; currently unused */
        *(.uninitialized)
        *(.uninitialized*)

        . = ALIGN(4);
        _euninitialized = .; /* define a global symbol at uninitialized end; currently unused */
    } >RAM

    /* this is to define the start of the heap, and make sure we have a minimum size */
    .heap :
    {
        . = ALIGN(4);
	      PROVIDE ( end = . );
	      PROVIDE ( _end = . );
        _heap_start = .;    /* define a global symbol at heap start */
        . = . + _minimum_heap_size;
    } >RAM

    /* this just checks there is enough RAM for the stack */
    .stack :
    {
        . = ALIGN(4);
        . = . + ${CIRCUITPY_DEFAULT_STACK_SIZE};
        . = ALIGN(4);
    } >RAM

    /* Remove exception unwinding information, since Circuit Python
       does not support this GCC feature. */
    /DISCARD/ :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        *(.ARM.exidx*)
    }

    /* Remove information from the standard libraries */
    /*
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
    */

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
