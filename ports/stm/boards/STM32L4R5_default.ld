/*
    GNU linker script for STM32L4R5 with filesystem
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)          : ORIGIN = 0x08000000, LENGTH = 2048K /* entire flash */
    FLASH_ISR (rx)      : ORIGIN = 0x08000000, LENGTH = 4K /* ISR vector. Kind of wasteful. */
    FLASH_FIRMWARE (rx) : ORIGIN = 0x08001000, LENGTH = 1024K-128K-4K  /* For now, limit to 1MB so that bank switching is still possible. */
    FLASH_FS (rw)       : ORIGIN = 0x08100000, LENGTH = 1024K
    RAM (xrw)           : ORIGIN = 0x20000000, LENGTH = 640K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 24K;
_minimum_heap_size = 16K;

/* Define tho top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);

/* ensure the firmware is within bounds */
ASSERT ( (ORIGIN(FLASH_FIRMWARE) + LENGTH(FLASH_FIRMWARE)) <= (ORIGIN(FLASH)+LENGTH(FLASH)), "FLASH_FIRMWARE out of bounds" );
