/* Memory configuration */
#if MICROPY_HW_FLASH_RESERVED
reserved_size = MICROPY_HW_FLASH_RESERVED;
#endif

#if MICROPY_HW_FLASH_TYPE == qspi_nor_flash
    flash_start         = 0x60000000;
    flash_size          = MICROPY_HW_FLASH_SIZE;
#else
    #error Unknown MICROPY_HW_FLASH_TYPE
#endif

/* MIMXRT1011 requires different offset for flash configuration */
flash_config_start  = flash_start + 0x00000400;
flash_config_size   = 0x00000C00;

itcm_start          = 0x00000000;
itcm_size           = 0x00008000;
dtcm_start          = 0x20000000;
dtcm_size           = 0x00008000;
ocrm_start          = 0x20200000;
ocrm_size           = 0x00010000;

/* 20kiB stack. */
__stack_size__ = 0x5000;

/* Do not use the traditional C heap. */
__heap_size__ = 0;
