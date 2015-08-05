#include <errno.h>
#include <string.h>

#include "py/mpstate.h"
#include "usb.h"
#include "uart.h"
#include "mphal.h"

// this table converts from HAL_StatusTypeDef to POSIX errno
const byte mp_hal_status_to_errno_table[4] = {
    [HAL_OK] = 0,
    [HAL_ERROR] = EIO,
    [HAL_BUSY] = EBUSY,
    [HAL_TIMEOUT] = ETIMEDOUT,
};

NORETURN void mp_hal_raise(HAL_StatusTypeDef status) {
    nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(mp_hal_status_to_errno_table[status])));
}

void mp_hal_set_interrupt_char(int c) {
    usb_vcp_set_interrupt_char(c);
}

int mp_hal_stdin_rx_chr(void) {
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

        byte c;
        if (usb_vcp_recv_byte(&c) != 0) {
            return c;
        } else if (MP_STATE_PORT(pyb_stdio_uart) != NULL && uart_rx_any(MP_STATE_PORT(pyb_stdio_uart))) {
            return uart_rx_char(MP_STATE_PORT(pyb_stdio_uart));
        }
        __WFI();
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
        uart_tx_strn(MP_STATE_PORT(pyb_stdio_uart), str, len);
    }
#if 0 && defined(USE_HOST_MODE) && MICROPY_HW_HAS_LCD
    lcd_print_strn(str, len);
#endif
    if (usb_vcp_is_enabled()) {
        usb_vcp_send_strn(str, len);
    }
}

void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
    // send stdout to UART and USB CDC VCP
    if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
        uart_tx_strn_cooked(MP_STATE_PORT(pyb_stdio_uart), str, len);
    }
    if (usb_vcp_is_enabled()) {
        usb_vcp_send_strn_cooked(str, len);
    }
}

void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio) {
    if (0) {
    #ifdef __GPIOA_CLK_ENABLE
    } else if (gpio == GPIOA) {
        __GPIOA_CLK_ENABLE();
    #endif
    #ifdef __GPIOB_CLK_ENABLE
    } else if (gpio == GPIOB) {
        __GPIOB_CLK_ENABLE();
    #endif
    #ifdef __GPIOC_CLK_ENABLE
    } else if (gpio == GPIOC) {
        __GPIOC_CLK_ENABLE();
    #endif
    #ifdef __GPIOD_CLK_ENABLE
    } else if (gpio == GPIOD) {
        __GPIOD_CLK_ENABLE();
    #endif
    #ifdef __GPIOE_CLK_ENABLE
    } else if (gpio == GPIOE) {
        __GPIOE_CLK_ENABLE();
    #endif
    #ifdef __GPIOF_CLK_ENABLE
    } else if (gpio == GPIOF) {
        __GPIOF_CLK_ENABLE();
    #endif
    #ifdef __GPIOG_CLK_ENABLE
    } else if (gpio == GPIOG) {
        __GPIOG_CLK_ENABLE();
    #endif
    #ifdef __GPIOH_CLK_ENABLE
    } else if (gpio == GPIOH) {
        __GPIOH_CLK_ENABLE();
    #endif
    #ifdef __GPIOI_CLK_ENABLE
    } else if (gpio == GPIOI) {
        __GPIOI_CLK_ENABLE();
    #endif
    #ifdef __GPIOJ_CLK_ENABLE
    } else if (gpio == GPIOJ) {
        __GPIOJ_CLK_ENABLE();
    #endif
    #ifdef __GPIOK_CLK_ENABLE
    } else if (gpio == GPIOK) {
        __GPIOK_CLK_ENABLE();
    #endif
    }
}
