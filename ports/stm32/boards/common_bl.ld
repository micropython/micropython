/* Memory layout for bootloader configuration (this here describes the app part):

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
    INCLUDE common_text.ld
    INCLUDE common_extratext_data_in_flash.ld
    INCLUDE common_bss_heap_stack.ld
}
