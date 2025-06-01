/* Memory layout for N6 when the application runs from external flash in XIP mode.

    FLASH_APP   .isr_vector
    FLASH_APP   .text
    FLASH_APP   .data

    RAM         .data
    RAM         .bss
    RAM         .heap
    RAM         .stack
*/

ENTRY(Reset_Handler)

REGION_ALIAS("FLASH_ISR", FLASH_APP);
REGION_ALIAS("FLASH_COMMON", FLASH_APP);

/* define output sections */
SECTIONS
{
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
        *(.text.HAL_GetTick)
        *drivers/memory/spiflash.o(.text* .rodata*)
        *boards*(.rodata.spiflash_config*)

        . = ALIGN(4);
        __isr_vector_ram_end = .;
    } >RAM AT >FLASH_ISR

    /* This section is used when waking from STANDBY. */
    .iram_bootloader :
    {
        . = ALIGN(4);
        _siram = .;

        KEEP(*(.*.iram_bootloader_isr_vector));
        *(.*.iram_bootloader_reset);
        *(*.memcpy);
        *(*.mp_hal_gpio_clock_enable);
        *(*.mp_hal_pin_config);
        *(*.mp_hal_pin_config_speed);
        *xspi.o(.text* .rodata*);
        *(*.rodata.pin_N*_obj);
        *(*.board_leave_standby);
        *(.text.LL_AHB4_GRP1_EnableClock);
        *(.text.LL_APB4_GRP2_EnableClock);

        . = ALIGN(4);
        _eiram = .;
    } >IRAM AT> FLASH_COMMON

    INCLUDE common_text.ld
    .gnu.sgstubs :
    {
        . = ALIGN(4);
        *(.gnu.sgstubs*)   /* Secure Gateway stubs */
        . = ALIGN(4);
    } >FLASH_COMMON
    INCLUDE common_extratext_data_in_flash.ld
    INCLUDE common_bss_heap_stack.ld
}

/* Used by the start-up code to initialise data */
__isr_vector_flash_addr = LOADADDR(.isr_vector);

/* Used by the start-up code to initialise data */
_siiram = LOADADDR(.iram_bootloader);
