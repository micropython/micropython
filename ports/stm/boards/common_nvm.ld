/* Memory layout for case with dedicated nvm sector (inefficient)
TODO: rewrite nvm module, remove this file entirely

    FLASH_ISR       .isr_vector
    FLASH_FIRMWARE  .text
    FLASH_FIRMWARE  .data
    FLASH_NVM       .nvm_data
    FLASH_FS        // ignored for fs use

    RAM             .data
    RAM             .bss
    RAM             .heap
    RAM             .stack
*/

ENTRY(Reset_Handler)

_ld_default_stack_size = _minimum_stack_size;

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

    /* Non-volitile memory */
    .nvm_data :
    {
        . = ALIGN(4);
        KEEP(*(.nvm_data))
        . = ALIGN(4);
    } >FLASH_NVM

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

    /* this is to define the start of the heap, and make sure we have a minimum size */
    .heap :
    {
        . = ALIGN(4);
        _ld_heap_start = .;
        . = . + _minimum_heap_size;
        . = ALIGN(4);
    } >RAM
    _ld_heap_end = ORIGIN(RAM) + LENGTH(RAM);

    /* this just checks there is enough RAM for the stack */
    .stack :
    {
        . = ALIGN(4);
        _ld_stack_bottom = .;
        . = . + _minimum_stack_size;
        . = ALIGN(4);
    } >RAM
    _ld_stack_top = ORIGIN(RAM) + LENGTH(RAM);


    .ARM.attributes 0 : { *(.ARM.attributes) }
}
