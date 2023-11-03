/*
    GNU linker script for SAMD51x20
*/

/*
_codesize is defined in mpconfigmcu.mk or mpconfigboard.mk as
MICROPY_HW_CODESIZE and is set in Makefile
*/

_flashsize = 1024K; /* The physical flash size */
_bootloader = 16K;  /* Must match the ORIGIN value of FLASH */

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00004000, LENGTH = _codesize
    RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 256K
}

/* Top end of the stack, with room for double-tap variable */
_estack = ORIGIN(RAM) + LENGTH(RAM) - 8;
_sstack = _estack - 16K;

_oflash_fs = ORIGIN(FLASH) + _codesize;
_sflash_fs = _flashsize - _codesize - _bootloader;

_sheap = _ebss;
_eheap = _sstack;
