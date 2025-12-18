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

REGION_ALIAS("FLASH_COMMON", FLASH_APP);

/* define output sections */
SECTIONS
{
    .isr_vector :
    {
        _siram = .;

        /* This ISR is used for normal application mode. */
        . = ALIGN(1024);
        KEEP(*(.isr_vector));

        /* This ISR is used when waking from STANDBY. */
        . = ALIGN(1024);
        KEEP(*(.rodata.iram_bootloader_isr_vector));

        /* Need to place in RAM all the code necessary to write to
         * flash, and to resume from STANDBY. */
        *(.*.iram_bootloader_reset);
        *(.*.memcpy);
        *(.*.mp_hal_gpio_clock_enable);
        *(.*.mp_hal_pin_config);
        *(.*.mp_hal_pin_config_speed);
        *drivers/memory/spiflash.o(.text.* .rodata.*)
        *xspi.o(.text.* .rodata.*);
        *boards*(.rodata.spiflash_config*)
        *boards*(.*.board_leave_standby);
        *(*.rodata.pin_N*_obj);
        *(.text.LL_AHB4_GRP1_EnableClock);
        *(.text.LL_APB4_GRP2_EnableClock);

        . = ALIGN(4);
        _eiram = .;
    } >IRAM AT> FLASH_COMMON

    INCLUDE common_text.ld
    INCLUDE common_extratext_data_in_flash.ld
    INCLUDE common_bss_heap_stack.ld
}

/* Used by the start-up code to initialise data */
_siiram = LOADADDR(.isr_vector);
