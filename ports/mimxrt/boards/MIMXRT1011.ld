/* Memory configuration */
#if BOARD_FLASH_RESERVED
reserved_size = BOARD_FLASH_RESERVED;
#endif

#if BOARD_FLASH_TYPE==qspi_nor
flash_start         = 0x60000000;
#else
#error Unknown BOARD_FLASH_TYPE
#endif
flash_size          = BOARD_FLASH_SIZE;
flash_end           = DEFINED(reserved_size) ? ((flash_start) + (flash_size - reserved_size)) : ((flash_start) + (flash_size));
flash_config_start  = flash_start + 0x00000400;
flash_config_size   = 0x00000C00;
ivt_start           = flash_start + 0x00001000;
ivt_size            = 0x00001000;
interrupts_start    = flash_start + 0x00002000;
interrupts_size     = 0x00000400;
text_start          = flash_start + 0x00002400;
vfs_start           = flash_start + 0x00100000;
text_size           = ((vfs_start) - (text_start));
vfs_size            = ((flash_end) - (vfs_start));
itcm_start          = 0x00000000;
itcm_size           = 0x00008000;
dtcm_start          = 0x20000000;
dtcm_size           = 0x00008000;
ocrm_start          = 0x20200000;
ocrm_size           = 0x00010000;

/* 20kiB stack. */
__stack_size__ = 0x5000;
_estack = __StackTop;
_sstack = __StackLimit;

/* Do not use the traditional C heap. */
__heap_size__ = 0;

/* Use second OCRAM bank for GC heap. */
_gc_heap_start = ORIGIN(m_ocrm);
_gc_heap_end = ORIGIN(m_ocrm) + LENGTH(m_ocrm);
