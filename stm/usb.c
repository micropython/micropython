#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"
#include "usbd_pyb_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb.h"

extern CDC_IF_Prop_TypeDef VCP_fops;

int is_enabled = 0;
USB_OTG_CORE_HANDLE USB_OTG_dev;

void usb_init() {
    USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_PYB_cb, &USR_cb);
    is_enabled = 1;
}

void usb_vcp_send(const char* str, int len) {
    if (is_enabled) {
        //VCP_fops.pIf_DataTx((const uint8_t*)str, len);
    }
}
