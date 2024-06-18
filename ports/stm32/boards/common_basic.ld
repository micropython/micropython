/* Memory layout for basic configuration:

    FLASH       .isr_vector
    FLASH       .text
    FLASH       .data

    RAM         .data
    RAM         .bss
    RAM         .heap
    RAM         .stack
*/

ENTRY(Reset_Handler)

REGION_ALIAS("FLASH_ISR", FLASH);
REGION_ALIAS("FLASH_COMMON", FLASH);

/* define output sections */
SECTIONS
{
    INCLUDE common_isr.ld
    INCLUDE common_text.ld
  .gnu.sgstubs :
  {
    . = ALIGN(4);
    *(.gnu.sgstubs*)   /* Secure Gateway stubs */
    . = ALIGN(4);
  } >FLASH

    INCLUDE common_extratext_data_in_flash.ld

    INCLUDE common_bss_heap_stack.ld
}
