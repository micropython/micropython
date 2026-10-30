/*
    Add AXISRAM3-6 and the NPU cache RAM to the heap.  This is used after the common
    linker script in LD_FILES.
*/

SECTIONS
{
    /* Extra GC blocks: AXISRAM3-6 and the NPU cache RAM. */
    .gc.blocks.table (READONLY) : {
        . = ALIGN(4);
        _gc_blocks_table_start = .;
        LONG (ORIGIN(NPU_RAM));
        LONG (LENGTH(NPU_RAM));
        _gc_blocks_table_end = .;
        . = ALIGN(4);
    } > FLASH_COMMON
}
