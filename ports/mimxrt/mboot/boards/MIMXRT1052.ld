/* Memory configuration */
#if MICROPY_HW_FLASH_TYPE == qspi_nor_flash || MICROPY_HW_FLASH_TYPE == qspi_hyper_flash
    flash_start         = 0x60000000;
    flash_size          = MICROPY_HW_FLASH_SIZE;
#else
    #error Unknown MICROPY_HW_FLASH_TYPE
#endif

itcm_start            = 0x00000000;
itcm_size             = 0x00020000;  /* 128KB */
dtcm_start            = 0x20000000;
dtcm_size             = 0x00020000;  /* 128KB */
ocrm_start            = 0x20200000;
ocrm_size             = 0x00040000;  /* 256KB */
ocrm_end              = (ocrm_start + ocrm_size);
