/*
    GNU linker script for STM32F411 with nvm and an external flash chip.
    No space is reserved for a filesystem.
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)          : ORIGIN = 0x08000000, LENGTH = 512K /* entire flash */
    FLASH_ISR (rx)      : ORIGIN = 0x08000000, LENGTH = 16K /* sector 0 */
    FLASH_NVM (rwx)     : ORIGIN = 0x08004000, LENGTH = 16K /* sector 1 is 16K */
    FLASH_FIRMWARE (rx) : ORIGIN = 0x08008000, LENGTH = 480K /* sector 2,3 is 16k, sector 4 is 64K, sectors 5,6,7 are 128K */
    RAM (xrw)           : ORIGIN = 0x20000000, LENGTH = 128K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 24K;
_minimum_heap_size = 16K;

/* Define the top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
