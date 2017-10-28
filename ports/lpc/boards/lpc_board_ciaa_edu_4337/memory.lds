/* Specify the memory areas */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x1a000000, LENGTH = 512K
    RAM (xrw) : ORIGIN = 0x10000000, LENGTH = 32k

    FLASHB (rx) : ORIGIN = 0x1b000000, LENGTH = 512K
    RAMB (rwx) : ORIGIN = 0x10080000, LENGTH = 32K
    RAMC (rwx) : ORIGIN = 0x10088000, LENGTH = 8K
    RAMD (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

_py_heap_start = ORIGIN(RAMD);
_py_heap_end = ORIGIN(RAMD) + LENGTH(RAMD);
