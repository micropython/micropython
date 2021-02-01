/* 24kiB stack. */
__stack_size__ = 0x6000;
_estack = __StackTop;
_sstack = __StackLimit;

/* Use second OCRAM bank for GC heap. */
_gc_heap_start = ORIGIN(m_data2);
_gc_heap_end = ORIGIN(m_data2) + LENGTH(m_data2);
