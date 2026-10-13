/* Memory configuration for the OpenMV Cam RT1062.

   The board boots through the OpenMV bootloader, which owns the first 256KiB
   of flash (the flash configuration block and the bootloader) and starts the
   firmware image at 0x60040000.  The image keeps the OpenMV firmware's
   layout: its own flash configuration block, then the IVT at +0x1000 and the
   vector table at +0x2000.

   The rest of the 16MiB QSPI flash is laid out as the OpenMV firmware lays it
   out: the firmware up to 4MiB, then a 4MiB filesystem, then the 8MiB ROMFS
   partition, which is placed in the reserved area at the end of flash. */
reserved_size = MICROPY_HW_ROMFS_BYTES;

flash_start         = 0x60000000;
flash_size          = MICROPY_HW_FLASH_SIZE;
flash_end           = ((flash_start) + (flash_size - reserved_size));
image_start         = flash_start + 0x00040000;
flash_config_start  = image_start;
flash_config_size   = 0x00001000;
ivt_start           = image_start + 0x00001000;
ivt_size            = 0x00001000;
interrupts_start    = image_start + 0x00002000;
interrupts_size     = 0x00000400;
text_start          = image_start + 0x00002400;
vfs_start           = flash_start + 0x00400000;
vfs_size            = ((flash_end) - (vfs_start));
_micropy_hw_romfs_part0_start = flash_end;
_micropy_hw_romfs_part0_size = MICROPY_HW_ROMFS_BYTES;
text_size           = ((vfs_start) - (text_start));
itcm_start          = 0x00000000;
itcm_size           = 0x00020000;
dtcm_start          = 0x20000000;
dtcm_size           = 0x00020000;
ocrm_start          = 0x20200000;
ocrm_size           = 0x000C0000;

#if MICROPY_HW_SDRAM_AVAIL
sdram_start         = 0x80000000;
sdram_size          = MICROPY_HW_SDRAM_SIZE;
#endif

/* 32kiB stack. */
__stack_size__ = 0x8000;
_estack = __StackTop;
_sstack = __StackLimit;

#if MICROPY_HW_SDRAM_AVAIL
_gc_heap_start = ORIGIN(m_sdram);
_gc_heap_end = ORIGIN(m_sdram) + LENGTH(m_sdram);
#else
/* Use second OCRAM bank for GC heap. */
_gc_heap_start = ORIGIN(m_ocrm);
_gc_heap_end = ORIGIN(m_ocrm) + LENGTH(m_ocrm);
#endif
