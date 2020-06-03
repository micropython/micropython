/* Memory layout for cases with itcm and dtcm

    FLASH_ISR       .isr_vector
    FLASH_FIRMWARE  .text
    FLASH_FIRMWARE  .data
    FLASH_FS        // ignored for fs use

    RAM             .data
    RAM             .bss
    RAM             .heap

    ITCM            .itcm
    DTCM            .dtcm_data
    DTCM            .dtcm_bss
    DTCM            .stack
*/

ENTRY(Reset_Handler)

/* define output sections */
SECTIONS
{
    /* The startup code goes first into FLASH */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector)) /* Startup code */

        /* This first flash block is 16K annd the isr vectors only take up
           about 400 bytes. Micropython pads this with files, but this didn't
           work with the size of Circuitpython's ff object. */

        . = ALIGN(4);
    } >FLASH_ISR

    /* The program code and other data goes into FLASH */
    .text :
    {
        . = ALIGN(4);
        *(.text*)          /* .text* sections (code) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    /*  *(.glue_7)   */    /* glue arm to thumb code */
    /*  *(.glue_7t)  */    /* glue thumb to arm code */

        . = ALIGN(4);
        _etext = .;        /* define a global symbol at end of code */
    } >FLASH_FIRMWARE

    /* used by the startup to initialize data */
    _sidata = LOADADDR(.data);

    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH (inidata).
    It is one task of the startup to copy the initial values from FLASH to RAM. */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;        /* create a global symbol at data start; used by startup code in order to initialise the .data section in RAM */
        *(.data*)          /* .data* sections */

        . = ALIGN(4);
        _edata = .;        /* define a global symbol at data end; used by startup code in order to initialise the .data section in RAM */
    } >RAM AT> FLASH_FIRMWARE
    _ld_d1_ram_data_destination = ADDR(.data);
    _ld_d1_ram_data_flash_copy = LOADADDR(.data);
    _ld_d1_ram_data_size = SIZEOF(.data);

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;         /* define a global symbol at bss start; used by startup code */
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end; used by startup code and GC */
    } >RAM
    _ld_d1_ram_bss_start = ADDR(.bss);
    _ld_d1_ram_bss_size = SIZEOF(.bss);
    _ld_heap_start = _ld_d1_ram_bss_start + _ld_d1_ram_bss_size;
    _ld_heap_end = ORIGIN(RAM) + LENGTH(RAM);

    /* this is to define the start of the heap, and make sure we have a minimum size */
    .heap :
    {
        . = ALIGN(4);
        . = . + _minimum_heap_size;
        . = ALIGN(4);
    } >RAM

    /* itcm stuff */
    .itcm :
    {
        . = ALIGN(4);

        *(.itcm.*)

        . = ALIGN(4);
    } > ITCM AT> FLASH_FIRMWARE
    _ld_itcm_destination = ADDR(.itcm);
    _ld_itcm_flash_copy = LOADADDR(.itcm);
    _ld_itcm_size = SIZEOF(.itcm);

    .dtcm_data :
    {
        . = ALIGN(4);

        *(.dtcm_data.*)

        . = ALIGN(4);
    } > DTCM AT> FLASH_FIRMWARE
    _ld_dtcm_data_destination = ADDR(.dtcm_data);
    _ld_dtcm_data_flash_copy = LOADADDR(.dtcm_data);
    _ld_dtcm_data_size = SIZEOF(.dtcm_data);

    .dtcm_bss :
    {
        . = ALIGN(4);

        *(.dtcm_bss.*)

        . = ALIGN(4);
    } > DTCM AT> DTCM
    _ld_dtcm_bss_start = ADDR(.dtcm_bss);
    _ld_dtcm_bss_size = SIZEOF(.dtcm_bss);

    .stack :
    {
        . = ALIGN(8);
        _ld_stack_bottom = .;
        . += _ld_default_stack_size;
    } > DTCM
    _ld_stack_top = ORIGIN(DTCM) + LENGTH(DTCM);


    .ARM.attributes 0 : { *(.ARM.attributes) }
}
