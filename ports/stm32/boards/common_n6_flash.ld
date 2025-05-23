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
    INCLUDE common_isr.ld

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
        . = ALIGN(4);
        _eiram = .;
    } >IRAM AT> FLASH_COMMON

    /* Used by the start-up code to initialise data */
    _siiram = LOADADDR(.iram_bootloader);

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
