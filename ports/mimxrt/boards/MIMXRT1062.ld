/* Memory configuration */
#if MICROPY_HW_FLASH_RESERVED
reserved_size = MICROPY_HW_FLASH_RESERVED;
#endif

#if MICROPY_HW_FLASH_TYPE == qspi_nor_flash
    flash_start         = 0x60000000;
    flash_size          = MICROPY_HW_FLASH_SIZE;
#elif MICROPY_HW_FLASH_TYPE == qspi_hyper_flash
    flash_start         = 0x60000000;
    flash_size          = MICROPY_HW_FLASH_SIZE;
#else
#error Unknown MICROPY_HW_FLASH_TYPE
#endif

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
