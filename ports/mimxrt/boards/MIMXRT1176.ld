/* Memory configuration */

/* Reserved Area
 * Users can create a reserved area at the end of the flash memory via
 * 'reserved_size' variable. The size of the reserved area should be a multiple
 * of the sector size of the flash memory!
 */
#if defined MICROPY_HW_FLASH_RESERVED
reserved_size = MICROPY_HW_FLASH_RESERVED;
#endif

#if MICROPY_HW_FLASH_TYPE == qspi_nor
flash_start         = 0x30000000;
#else
#error Unknown MICROPY_HW_FLASH_TYPE
#endif
flash_size      = MICROPY_HW_FLASH_SIZE;
flash_end           = DEFINED(reserved_size) ? ((flash_start) + (flash_size - reserved_size)) : ((flash_start) + (flash_size));
flash_config_start  = flash_start + 0x400;  /* TODO: check why config_start is on 0x400 instead of 0x0 offset */
flash_config_size   = 0x00000C00;
ivt_start           = flash_start + 0x00001000;
ivt_size            = 0x00001000;
interrupts_start    = flash_start + 0x00002000;
interrupts_size     = 0x00000400;
text_start          = flash_start + 0x00002400;
vfs_start           = flash_start + 0x00200000;

#if defined MULTI_CORE
m_core1_image_start = vfs_start - 0x00040000;
m_core1_image_size  = 0x00040000;
#endif

text_size           = ((vfs_start) - (text_start));
vfs_size            = ((flash_end) - (vfs_start));
itcm_start          = 0x00000000;
itcm_size           = 0x00020000;
dtcm_start          = 0x20000000;
dtcm_size           = 0x00020000;
ocrm_start          = 0x20240000;
ocrm_size           = 0x00100000;

#ifdef MICROPY_HW_SDRAM_AVAIL
sdram_start         = 0x80000000;
sdram_size          = MICROPY_HW_SDRAM_SIZE;
#endif

/* 32kiB stack. */
__stack_size__ = 0x8000;
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
