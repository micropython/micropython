/*
    Add AXISRAM3-6, the NPU cache RAM and the 32MB PSRAM on XSPI1 to the heap.  This is
    used after the common linker script in LD_FILES.  Only the PSRAM that's found at
    runtime is used (see main.c).
*/

MEMORY
{
    PSRAM (xrw) : ORIGIN = 0x90000000, LENGTH = 32M
}

SECTIONS
{
    /* Extra GC blocks: AXISRAM3-6 and the NPU cache RAM, then the PSRAM. */
    .gc.blocks.table (READONLY) : {
        . = ALIGN(4);
        _gc_blocks_table_start = .;
        LONG (ORIGIN(NPU_RAM));
        LONG (LENGTH(NPU_RAM));
        LONG (ORIGIN(PSRAM));
        /* Leave out the last 32 bytes: a cache line fill that includes the PSRAM's last
           address can deadlock XSPI1 (ES0620 erratum 2.4.5). */
        LONG (LENGTH(PSRAM) - 32);
        _gc_blocks_table_end = .;
        . = ALIGN(4);
    } > FLASH_COMMON
}
