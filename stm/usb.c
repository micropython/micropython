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

USB_OTG_CORE_HANDLE USB_OTG_Core;

static int dev_is_enabled = 0;
int dev_is_connected=0; /* used by usbd_cdc_vcp */
static char rx_buf[64];
static int rx_buf_in;
static int rx_buf_out;

void pyb_usb_dev_init(void) {
#ifdef USE_DEVICE_MODE
    if (!dev_is_enabled) {
        // only init USB once in the device's power-lifetime
        USBD_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_PYB_cb, &USR_cb);
        //USBD_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_PYB_HID_cb, &USR_cb);
    }
    rx_buf_in = 0;
    rx_buf_out = 0;
    dev_is_enabled = 1;
#endif
}

bool usb_vcp_is_enabled(void) {
    return dev_is_enabled;
}

bool usb_vcp_is_connected(void) {
    return dev_is_connected;
}

void usb_vcp_receive(const char *buf, uint32_t len) {
    if (dev_is_enabled) {
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
#ifdef USE_DEVICE_MODE
    if (dev_is_enabled) {
        VCP_fops.pIf_DataTx((const uint8_t*)str, len);
    }
#endif
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
    USBD_HID_SendReport(&USB_OTG_Core, buf, 4);
#endif
}

/******************************************************************************/
// code for experimental USB OTG support

#ifdef USE_HOST_MODE

#include "led.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_hid_core.h"
#include "usbh_hid_keybd.h"
#include "usbh_hid_mouse.h"

__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END ;

static int host_is_enabled = 0;

void pyb_usb_host_init(void) {
    if (!host_is_enabled) {
        // only init USBH once in the device's power-lifetime
        /* Init Host Library */
        USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &HID_cb, &USR_Callbacks);
    }
    host_is_enabled = 1;
}

void pyb_usb_host_process(void) {
    USBH_Process(&USB_OTG_Core, &USB_Host);
}

uint8_t usb_keyboard_key = 0;

// TODO this is an ugly hack to get key presses
uint pyb_usb_host_get_keyboard(void) {
    uint key = usb_keyboard_key;
    usb_keyboard_key = 0;
    return key;
}

void USR_MOUSE_Init(void) {
    led_state(4, 1);
    USB_OTG_BSP_mDelay(100);
    led_state(4, 0);
}

void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef *data) {
    led_state(4, 1);
    USB_OTG_BSP_mDelay(50);
    led_state(4, 0);
}

void USR_KEYBRD_Init(void) {
    led_state(4, 1);
    USB_OTG_BSP_mDelay(100);
    led_state(4, 0);
}

void USR_KEYBRD_ProcessData(uint8_t pbuf) {
    led_state(4, 1);
    USB_OTG_BSP_mDelay(50);
    led_state(4, 0);
    //lcd_print_strn((char*)&pbuf, 1);
    usb_keyboard_key = pbuf;
}

#endif // USE_HOST_MODE
