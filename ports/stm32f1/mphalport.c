#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/misc.h"
#include "usb.h"
#include "uart.h"

// this table converts from HAL_StatusTypeDef to POSIX errno
const byte mp_hal_status_to_errno_table[4] = {
    [HAL_OK] = 0,
    [HAL_ERROR] = MP_EIO,
    [HAL_BUSY] = MP_EBUSY,
    [HAL_TIMEOUT] = MP_ETIMEDOUT,
};

NORETURN void mp_hal_raise(HAL_StatusTypeDef status) {
    mp_raise_OSError(mp_hal_status_to_errno_table[status]);
}

MP_WEAK uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
        int errcode;
        const mp_stream_p_t *stream_p = mp_get_stream(MP_STATE_PORT(pyb_stdio_uart));
        ret = stream_p->ioctl(MP_STATE_PORT(pyb_stdio_uart), MP_STREAM_POLL, poll_flags, &errcode);
    }
    return ret | mp_uos_dupterm_poll(poll_flags);
}

MP_WEAK int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        if (MP_STATE_PORT(pyb_stdio_uart) != NULL && uart_rx_any(MP_STATE_PORT(pyb_stdio_uart))) {
            return uart_rx_char(MP_STATE_PORT(pyb_stdio_uart));
        }
        int dupterm_c = mp_uos_dupterm_rx_chr();
        if (dupterm_c >= 0) {
            return dupterm_c;
        }
        MICROPY_EVENT_POLL_HOOK
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

MP_WEAK void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
        uart_tx_strn(MP_STATE_PORT(pyb_stdio_uart), str, len);
    }
    mp_uos_dupterm_tx_strn(str, len);
}

// Efficiently convert "\n" to "\r\n"
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    const char *last = str;
    while (len--) {
        if (*str == '\n') {
            if (str > last) {
                mp_hal_stdout_tx_strn(last, str - last);
            }
            mp_hal_stdout_tx_strn("\r\n", 2);
            ++str;
            last = str;
        } else {
            ++str;
        }
    }
    if (str > last) {
        mp_hal_stdout_tx_strn(last, str - last);
    }
}

void mp_hal_ticks_cpu_enable(void) {
    if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio) {
    uint32_t gpio_idx = ((uint32_t)gpio - GPIOA_BASE) / (GPIOB_BASE - GPIOA_BASE);
    BIT_BAND(RCC->APB2ENR , (RCC_APB2ENR_IOPAEN_Pos + gpio_idx)) = 1;
    volatile uint32_t tmp = RCC->APB2ENR; // Delay after enabling clock
    (void)tmp;
}

#define MP_HAL_PIN_MODE_INVALID    (4)
// this table converts from MP_HAL_PIN_MODE_xxx to STM32F1 CRL/CRH value, output use highest speed
const byte mp_hal_pin_mod_cfg[7] = {
    [MP_HAL_PIN_MODE_IN]      = 0x04, // floating input
    [MP_HAL_PIN_MODE_OUT]     = 0x03, // output PP, 50MHz
    [MP_HAL_PIN_MODE_ALT]     = 0x0b, // alternat PP, 50MHz
    [MP_HAL_PIN_MODE_ANALOG]  = 0x00, // analog input
    [MP_HAL_PIN_MODE_INVALID] = 0x00, // not used
    [MP_HAL_PIN_MODE_OUT_OD]  = 0x07, // output OD, 50MHz
    [MP_HAL_PIN_MODE_ALT_OD]  = 0x0f, // alternat OD, 50MHz
};

void mp_hal_pin_config(mp_hal_pin_obj_t pin_obj, uint32_t mode, uint32_t pull, uint32_t alt) {
    GPIO_TypeDef *gpio = pin_obj->gpio;
    uint32_t pin = pin_obj->pin;
    uint32_t moder = mp_hal_pin_mod_cfg[mode];
    uint32_t pos = (pin % 8) * 4;

    mp_hal_gpio_clock_enable(gpio);
    
    // input with pull up/down
    if ( moder == 0x04  && pull ) {
        moder = 0x08;
    }

    // get register CRL/CRH (pin0~pin7 use CRL, pin8~pin15 use CRH)
    register uint32_t *pReg = (uint32_t *)((uint32_t)(&gpio->CRL) + (pin / 8 * 4));
    *pReg = (*pReg & ~(0x0f << pos) ) | ( (moder & 0x0f) << pos );

    // set pull
    if (moder == 0x08) {
        gpio->BSRR= 1 << (pin + (1-pull%2) * 16);
    }
    
    if (mode == MP_HAL_PIN_MODE_ALT || mode == MP_HAL_PIN_MODE_ALT_OD) {
        // TODO: alt contains AFIO remap infomation, but no value provided
    }
}

bool mp_hal_pin_config_alt(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint8_t fn, uint8_t unit) {
    const pin_af_obj_t *af = pin_find_af(pin, fn, unit);
    if (af == NULL) {
        return false;
    }
    mp_hal_pin_config(pin, mode, pull, af->idx);
    return true;
}

void mp_hal_pin_config_speed(mp_hal_pin_obj_t pin_obj, uint32_t speed) {
    GPIO_TypeDef *gpio = pin_obj->gpio;
    uint32_t pin = pin_obj->pin;
    uint32_t pos = (pin % 8) * 4;

    register uint32_t *pReg = (uint32_t *)((uint32_t)(&gpio->CRL) + (pin / 8 * 4));
    if ( (*pReg >> pos) & 3 ) {
        *pReg = (*pReg & ~(3 << pos)) | (((speed + 1) & 3) << pos);
    }
}
