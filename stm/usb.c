#include <string.h>

#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"
#include "usbd_pyb_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "usb.h"

#ifdef USE_DEVICE_MODE
extern CDC_IF_Prop_TypeDef VCP_fops;
#endif

USB_OTG_CORE_HANDLE USB_OTG_dev;

static int is_enabled = 0;
static char rx_buf[64];
static int rx_buf_in;
static int rx_buf_out;

void usb_init(void) {
    if (!is_enabled) {
        // only init USB once in the device's power-lifetime
#ifdef USE_DEVICE_MODE
        USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_PYB_cb, &USR_cb);
        //USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_PYB_HID_cb, &USR_cb);
#endif
    }
    rx_buf_in = 0;
    rx_buf_out = 0;
    is_enabled = 1;
}

bool usb_vcp_is_enabled(void) {
    return is_enabled;
}

void usb_vcp_receive(const char *buf, uint32_t len) {
    if (is_enabled) {
        for (int i = 0; i < len; i++) {
            rx_buf[rx_buf_in++] = buf[i];
            if (rx_buf_in >= sizeof(rx_buf)) {
                rx_buf_in = 0;
            }
            if (rx_buf_in == rx_buf_out) {
                rx_buf_out = rx_buf_in + 1;
                if (rx_buf_out >= sizeof(rx_buf)) {
                    rx_buf_out = 0;
                }
            }
        }
    }
}

int usb_vcp_rx_any(void) {
    if (rx_buf_in >= rx_buf_out) {
        return rx_buf_in - rx_buf_out;
    } else {
        return rx_buf_in + sizeof(rx_buf) - rx_buf_out;
    }
}

char usb_vcp_rx_get(void) {
    while (rx_buf_out == rx_buf_in) {
    }
    char c = rx_buf[rx_buf_out];
    rx_buf_out += 1;
    if (rx_buf_out >= sizeof(rx_buf)) {
        rx_buf_out = 0;
    }
    return c;
}

void usb_vcp_send_str(const char *str) {
    usb_vcp_send_strn(str, strlen(str));
}

void usb_vcp_send_strn(const char *str, int len) {
    if (is_enabled) {
#ifdef USE_DEVICE_MODE
        VCP_fops.pIf_DataTx((const uint8_t*)str, len);
#endif
    }
}

#include "usbd_conf.h"

/* These are external variables imported from CDC core to be used for IN 
   transfer management. */
#ifdef USE_DEVICE_MODE
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */
#endif

void usb_vcp_send_strn_cooked(const char *str, int len) {
#ifdef USE_DEVICE_MODE
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            APP_Rx_Buffer[APP_Rx_ptr_in] = '\r';
            APP_Rx_ptr_in = (APP_Rx_ptr_in + 1) & (APP_RX_DATA_SIZE - 1);
        }
        APP_Rx_Buffer[APP_Rx_ptr_in] = *str;
        APP_Rx_ptr_in = (APP_Rx_ptr_in + 1) & (APP_RX_DATA_SIZE - 1);
    }
#endif
}

void usb_hid_send_report(uint8_t *buf) {
#ifdef USE_DEVICE_MODE
    USBD_HID_SendReport(&USB_OTG_dev, buf, 4);
#endif
}

/******************************************************************************/
// code for experimental USB OTG support

#ifdef USE_HOST_MODE

#include "lib-otg/usbh_core.h"
#include "lib-otg/usbh_usr.h"
#include "lib-otg/usbh_hid_core.h"
#include "lib-otg/usb_hcd_int.h"

__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END ;

static int host_is_enabled = 0;
void pyb_usbh_init(void) {
    if (!host_is_enabled) {
        // only init USBH once in the device's power-lifetime
        /* Init Host Library */
        USBH_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USB_Host, &HID_cb, &USR_Callbacks);
    }
    host_is_enabled = 1;
}

mp_obj_t pyb_usbh_process(void) {
    USBH_Process(&USB_OTG_dev, &USB_Host);
    return mp_const_none;
}

mp_obj_t pyb_usbh_connect(void) {
    USBH_HCD_INT_fops->DevConnected(&USB_OTG_dev);
    return mp_const_none;
}

#endif // USE_HOST_MODE
