/*
 * Build-time assertion for the MicroPython GC heap size.
 *
 * __GcHeapStart and __GcHeapEnd are defined via --defsym in the Makefile,
 * controlled by MICROPY_C_HEAP_SIZE.  This fragment is appended after the
 * BSP linker script so both symbols are resolved when the ASSERT runs.
 *
 * 64 KB is the practical minimum: LittleFS2 alone allocates ~16 KB of
 * read/write cache on mount, and the interpreter needs headroom on top.
 * Increase MICROPY_C_HEAP_SIZE is too large, or reduce it if this fires.
 */
ASSERT((__GcHeapEnd - __GcHeapStart) >= 64 * 1024,
    "MicroPython GC heap < 64 KB. Reduce MICROPY_C_HEAP_SIZE.")
