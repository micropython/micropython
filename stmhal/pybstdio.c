#include <stm32f4xx_hal.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "misc.h"
#include "obj.h"
#include "pybstdio.h"
#include "storage.h"
#include "usb.h"
#include "usart.h"

void stdout_tx_str(const char *str) {
    if (pyb_usart_global_debug != PYB_USART_NONE) {
        usart_tx_str(pyb_usart_global_debug, str);
    }
#if defined(USE_HOST_MODE) && MICROPY_HW_HAS_LCD
    lcd_print_str(str);
#endif
    usb_vcp_send_str(str);
}

void stdout_tx_strn(const char *str, uint len) {
    if (pyb_usart_global_debug != PYB_USART_NONE) {
        usart_tx_strn(pyb_usart_global_debug, str, len);
    }
#if defined(USE_HOST_MODE) && MICROPY_HW_HAS_LCD
    lcd_print_strn(str, len);
#endif
    usb_vcp_send_strn(str, len);
}

int stdin_rx_chr(void) {
    for (;;) {
#if 0
#ifdef USE_HOST_MODE
        pyb_usb_host_process();
        int c = pyb_usb_host_get_keyboard();
        if (c != 0) {
            return c;
        }
#endif
#endif
        if (usb_vcp_rx_num() != 0) {
            return usb_vcp_rx_get();
        } else if (pyb_usart_global_debug != PYB_USART_NONE && usart_rx_any(pyb_usart_global_debug)) {
            return usart_rx_char(pyb_usart_global_debug);
        }
        HAL_Delay(1);
        if (storage_needs_flush()) {
            storage_flush();
        }
    }
}
