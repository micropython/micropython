/*
    Linker script for PYBD with STM32F722/STM32F723/STM32F732/STM32F733

    Memory layout for mboot configuration (this here describes the app part):

    FLASH_APP   .isr_vector
    FLASH_APP   .text
    FLASH_APP   .data

    FLASH_EXT   .text_ext

    RAM         .data
    RAM         .bss
    RAM         .heap
    RAM         .stack
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 32K     /* sectors 0,1 */
    FLASH_APP (rx)  : ORIGIN = 0x08008000, LENGTH = 480K    /* sectors 2-7 */
    FLASH_EXT (rx)  : ORIGIN = 0x90000000, LENGTH = 2048K   /* external QSPI */
    RAM (rwx)       : ORIGIN = 0x20000000, LENGTH = 256K    /* DTCM+SRAM1+SRAM2 */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack.  The stack is full descending so begins just above last byte
   of RAM.  Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM) - _estack_reserve;
_sstack = _estack - 16K;

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

/* Define output sections */
SECTIONS
{
    .text_ext :
    {
        . = ALIGN(4);
        *lib/btstack/*(.text* .rodata*)
        *lib/mbedtls/*(.text* .rodata*)
        *lib/mynewt-nimble/*(.text* .rodata*)
        *lib/cyw43-driver/*(.rodata.w4343*_combined)
        *drivers/cyw43/*(.rodata.cyw43_btfw_*)
        . = ALIGN(4);
    } >FLASH_EXT
}

INCLUDE common_bl.ld
