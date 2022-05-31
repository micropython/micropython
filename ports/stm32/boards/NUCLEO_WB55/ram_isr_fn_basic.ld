/* Memory layout for basic configuration with isr ram functions:

    FLASH       .isr_vector
    FLASH       .text
    FLASH       .data

    RAM         .data
    RAM         .bss
    RAM         .heap
    RAM         .stack
*/

ENTRY(Reset_Handler)

/* define output sections */
SECTIONS
{
    /* The startup code here is run from RAM */
    .isr_vector :
    {
        . = ALIGN(4);
        _start_isr = .;
        KEEP(*(.isr_vector)) /* Startup code */

        /* These functions need to run from ram.
           Defining them here ensures they're copied from
           flash (in ram_irq.c) along with the isr_vector above.
        */
        . = ALIGN(4);
        *(.text.pendsv_kbd_intr)
        *(.text.pendsv_schedule_dispatch)
        *(.text.UART*_IRQHandler)
        *(.text.USART*_IRQHandler)
        *(.text.storage_systick_callback)
        *(.text.SysTick_Handler)
        *(.text.uart_irq_handler)
        *(.text.HAL_GetTick)
        *(.text.FLASH_PageErase)
        *(.text.FLASH_WaitForLastOperation)
        *(.text.HAL_FLASHEx_Erase)
        _end_isr = .;
        . = ALIGN(4);
    } >RAM AT >FLASH
    _start_init_isr = LOADADDR(.isr_vector); /* Used by the start-up code to initialise data */

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
    } >FLASH

    INCLUDE common_extratext_data_in_flash.ld
    INCLUDE common_bss_heap_stack.ld
}
