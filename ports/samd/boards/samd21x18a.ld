/*
    GNU linker script for SAMD21
*/

/*
_codesize is defined in mpconfigmcu.mk or mpconfigboard.mk as
MICROPY_HW_CODESIZE and is set in Makefile
*/

_flashsize = 256K;  /* The physical flash size */
_bootloader = 8K;   /* Must match the ORIGIN value of FLASH */

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00002000, LENGTH = _codesize
    RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 32K
}

/* Top end of the stack, with room for double-tap variable */
_estack = ORIGIN(RAM) + LENGTH(RAM) - 8;
_sstack = _estack - 8K;

_oflash_fs = ORIGIN(FLASH) + _codesize;
_sflash_fs = _flashsize - _codesize - _bootloader;

_sheap = _ebss;
_eheap = _sstack;
