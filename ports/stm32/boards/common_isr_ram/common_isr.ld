/* This linker script fragment is intended to be included in SECTIONS. */

/* The startup code goes first into FLASH */
.isr_vector :
{
    . = ALIGN(4);
    __isr_vector_ram_start = .;

    KEEP(*(.isr_vector)) /* Startup code */

    /* These functions need to run from ram to enable uart
       reception during flash erase/write operations.
       Defining them here ensures they're copied from
       flash (in main.c) along with the isr_vector above.
    */
    . = ALIGN(4);
    *(.text.mp_sched_keyboard_interrupt)
    *(.text.pendsv_schedule_dispatch)
    *(.text.storage_systick_callback)
    *(.text.SysTick_Handler)
    *(.text.uart_irq_handler)
    *(.text.UART*_IRQHandler)
    *(.text.USART*_IRQHandler)
    *(.text.FLASH_PageErase)
    *(.text.FLASH_SectorErase)
    *(.text.FLASH_WaitForLastOperation)
    *(.text.HAL_FLASHEx_Erase)
    *(.text.HAL_GetTick)

    __isr_vector_ram_end = .;
    . = ALIGN(4);

} >RAM AT >FLASH_ISR

/* Used by the start-up code to initialise data */
__isr_vector_flash_addr = LOADADDR(.isr_vector);
