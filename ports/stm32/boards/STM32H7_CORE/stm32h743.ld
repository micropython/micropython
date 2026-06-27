/*
    GNU linker script for STM32H743
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 2M      /* sectors (0-15)  */
    FLASH_APP  (rx) : ORIGIN = 0x08020000, LENGTH = 1920K   /* sectors (1-15)  */
    DTCM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K    /* Used for storage cache */
    RAM (xrw)       : ORIGIN = 0x24000000, LENGTH = 512K    /* AXI SRAM */
    RAM_D2 (xrw)    : ORIGIN = 0x30000000, LENGTH = 288K
    SDRAM (xrw)     : ORIGIN = 0xC0000000, LENGTH = 32M     /* external SDRAM */
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* Define the stack.  The stack is full descending so begins just above last byte
   of RAM.  Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM) - _estack_reserve;
_sstack = _estack - 16K; /* tunable */

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;

/* Location of filesystem RAM cache */
_micropy_hw_internal_flash_storage_ram_cache_start = ORIGIN(DTCM);
_micropy_hw_internal_flash_storage_ram_cache_end = ORIGIN(DTCM) + LENGTH(DTCM);

/* Define output sections */
SECTIONS
{
    .sdram_pool (NOLOAD) :
    {
       . = ALIGN(32);
       _sdram_pool_start = .;
    } > SDRAM

    /* Define the end of the SDRAM pool to prevent overflow */
    _sdram_pool_end = ORIGIN(SDRAM) + LENGTH(SDRAM);

    .eth_buffers (NOLOAD) : {
        . = ABSOLUTE(0x30040000);
        *eth.o*(.bss.eth_dma)
    } >RAM_D2
}
