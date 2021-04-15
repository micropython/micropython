/*
    GNU linker script for STM32L431xC, STM32L432xC, STM32L433xC and STM443xC.
    FLASH 256K with 2K blocks, RAM 64K.
*/

/* Specify the memory areas.
   FS_SIZE is not a region, it is only used to define the File System length
*/
MEMORY
{
    ALL_FLASH (rx)  : ORIGIN = 0x08000000,  LENGTH = 256K
    FS_SIZE (r)     : ORIGIN = 0,           LENGTH = 0 /* Adjust the File System size here */

    FLASH (rx)      : ORIGIN = ORIGIN(ALL_FLASH), LENGTH = LENGTH(ALL_FLASH) - LENGTH(FS_SIZE)
    FLASH_FS (r)    : ORIGIN = ORIGIN(FLASH) + LENGTH(FLASH), LENGTH = LENGTH(FS_SIZE)

    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K /* SRAM1, 48K + SRAM2, 16K */
}
_stack_size = 10K; /* Adjust stack size here */