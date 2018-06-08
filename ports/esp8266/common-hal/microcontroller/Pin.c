/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/microcontroller/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "py/mphal.h"

#include "eagle_soc.h"

bool adc_in_use;
bool gpio16_in_use;

typedef struct {
    void (*func)(void *);
    void *data;
} pin_intr_handler_t;

static pin_intr_handler_t _pin_intr_handlers[GPIO_PIN_COUNT];

void microcontroller_pin_call_intr_handlers(uint32_t status) {
    status &= (1 << GPIO_PIN_COUNT) - 1;
    for (int p = 0; status; ++p, status >>= 1) {
        if ((status & 1) && _pin_intr_handlers[p].func) {
            _pin_intr_handlers[p].func(_pin_intr_handlers[p].data);
        }
    }
}

void microcontroller_pin_register_intr_handler(uint8_t gpio_number, void (*func)(void *), void *data) {
    common_hal_mcu_disable_interrupts();
    _pin_intr_handlers[gpio_number] = (pin_intr_handler_t){ func, data };
    common_hal_mcu_enable_interrupts();
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t* pin) {
    if (pin == &pin_TOUT) {
        return !adc_in_use;
    }
    if (pin == &pin_XPD_DCDC) {
        return !gpio16_in_use;
    }
    if (pin->gpio_number == NO_GPIO) {
        return false;
    }
    return (READ_PERI_REG(pin->peripheral) &
                (PERIPHS_IO_MUX_FUNC<<PERIPHS_IO_MUX_FUNC_S)) == 0 &&
             (GPIO_REG_READ(GPIO_ENABLE_ADDRESS) & (1 << pin->gpio_number)) == 0 &&
             (READ_PERI_REG(pin->peripheral) & PERIPHS_IO_MUX_PULLUP) == 0;     
}

void claim_pin(const mcu_pin_obj_t* pin) {
    if (pin == &pin_XPD_DCDC) {
        gpio16_in_use = true;
    }
    if (pin == &pin_TOUT) {
        adc_in_use = true;
    }
}

void reset_pin(const mcu_pin_obj_t* pin) {
    if (pin == &pin_XPD_DCDC) {
        // Set GPIO16 as input
        WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | 1); 	// mux configuration for XPD_DCDC and rtc_gpio0 connection
        WRITE_PERI_REG(RTC_GPIO_CONF, READ_PERI_REG(RTC_GPIO_CONF) & ~1);	//mux configuration for out enable
        WRITE_PERI_REG(RTC_GPIO_ENABLE, READ_PERI_REG(RTC_GPIO_ENABLE) & ~1);	//out disable
        gpio16_in_use = false;
    }
    if (pin == &pin_TOUT) {
        adc_in_use = false;
    }
}

void reset_pins(void) {
    for (int i = 0; i < 16; i++) {
        // 5 is RXD, 6 is TXD
        if ((i > 4 && i < 13) || i == 12) {
            continue;
        }
        uint32_t peripheral = PERIPHS_IO_MUX + i * 4;
        PIN_FUNC_SELECT(peripheral, 0);
        PIN_PULLUP_DIS(peripheral);
        // Disable the pin.
        gpio_output_set(0x0, 0x0, 0x0, 1 << i);
    }
    // Set GPIO16 as input
    WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | 1); 	// mux configuration for XPD_DCDC and rtc_gpio0 connection
    WRITE_PERI_REG(RTC_GPIO_CONF, READ_PERI_REG(RTC_GPIO_CONF) & ~1);	//mux configuration for out enable
    WRITE_PERI_REG(RTC_GPIO_ENABLE, READ_PERI_REG(RTC_GPIO_ENABLE) & ~1);	//out disable

    adc_in_use = false;
    gpio16_in_use = false;
}
