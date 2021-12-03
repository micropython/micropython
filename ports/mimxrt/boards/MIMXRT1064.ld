/* Memory configuration */
#if MICROPY_HW_FLASH_RESERVED
reserved_size = MICROPY_HW_FLASH_RESERVED;
#endif

#if MICROPY_HW_FLASH_TYPE==qspi_nor
flash_start         = 0x60000000;
#elif MICROPY_HW_FLASH_TYPE==hyperflash
flash_start         = 0x60000000;
#elif MICROPY_HW_FLASH_TYPE==internal
flash_start         = 0x70000000;
#else
#error Unknown MICROPY_HW_FLASH_TYPE
#endif
flash_size          = MICROPY_HW_FLASH_SIZE;
flash_end           = DEFINED(reserved_size) ? ((flash_start) + (flash_size - reserved_size)) : ((flash_start) + (flash_size));
flash_config_start  = flash_start;
flash_config_size   = 0x00001000;
ivt_start           = flash_start + 0x00001000;
ivt_size            = 0x00001000;
interrupts_start    = flash_start + 0x00002000;
interrupts_size     = 0x00000400;
text_start          = flash_start + 0x00002400;
vfs_start           = flash_start + 0x00100000;
text_size           = ((vfs_start) - (text_start));
vfs_size            = ((flash_end) - (vfs_start));
itcm_start          = 0x00000000;
itcm_size           = 0x00020000;
dtcm_start          = 0x20000000;
dtcm_size           = 0x00020000;
ocrm_start          = 0x20200000;
ocrm_size           = 0x000C0000;

#ifdef MICROPY_HW_SDRAM_AVAIL
sdram_start         = 0x80000000;
sdram_size          = MICROPY_HW_SDRAM_SIZE;
#endif

/* 24kiB stack. */
__stack_size__ = 0x6000;
_estack = __StackTop;
_sstack = __StackLimit;

#ifdef MICROPY_HW_SDRAM_AVAIL
_gc_heap_start = ORIGIN(m_sdram);
_gc_heap_end = ORIGIN(m_sdram) + LENGTH(m_sdram);
#else
/* Use second OCRAM bank for GC heap. */
_gc_heap_start = ORIGIN(m_ocrm);
_gc_heap_end = ORIGIN(m_ocrm) + LENGTH(m_ocrm);
#endif
