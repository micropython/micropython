// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "MICROpi"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (15 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x2E8A)
#define MICROPY_HW_USB_PID (0x1065)

#ifndef MICROPY_HW_USB_MANUFACTURER_STRING
#define MICROPY_HW_USB_MANUFACTURER_STRING      "Pi-Plates"
#endif

#ifndef MICROPY_HW_USB_PRODUCT_FS_STRING
#define MICROPY_HW_USB_PRODUCT_FS_STRING        "MICROpi in FS Mode"
#endif
