/* Memory configuration */
flash_start         = 0x60000000;
flash_end           = DEFINED(reserved_size) ? ((flash_start) + (flash_size - reserved_size)) : ((flash_start) + (flash_size));
flash_config_start  = flash_start;
flash_config_size   = 0x00001000;
ivt_start           = 0x60001000;
ivt_size            = 0x00001000;
interrupts_start    = 0x60002000;
interrupts_size     = 0x00000400;
text_start          = 0x60002400;
vfs_start           = 0x60100000;
text_size           = ((vfs_start) - (text_start));
vfs_size            = ((flash_end) - (vfs_start));
itcm_start          = 0x00000000;
itcm_size           = 0x00020000;
dtcm_start          = 0x20000000;
dtcm_size           = 0x00020000;
ocrm_start          = 0x20200000;
ocrm_size           = 0x00040000;

/* 24kiB stack. */
__stack_size__ = 0x6000;
_estack = __StackTop;
_sstack = __StackLimit;

/* Use second OCRAM bank for GC heap. */
_gc_heap_start = ORIGIN(m_ocrm);
_gc_heap_end = ORIGIN(m_ocrm) + LENGTH(m_ocrm);
