typedef enum {
    USBD_STORAGE_MEDIUM_FLASH,
    USBD_STORAGE_MEDIUM_SDCARD,
} usbd_storage_medium_kind_t;

void usbd_storage_select_medium(usbd_storage_medium_kind_t medium_kind);
