#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_pyb_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"

//extern CDC_IF_Prop_TypeDef APP_FOPS;

USB_OTG_CORE_HANDLE USB_OTG_dev;

void usb_vcp_init() {
    //USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);
}

void usb_vcp_send(const char* str, int len) {
    //APP_FOPS.pIf_DataTx(str, len);
}

void usb_init() {
    USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_PYB_cb, &USR_cb);
}
