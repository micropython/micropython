/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
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

#include "shared-module/max3421e/Max3421E.h"

#include "lib/tinyusb/src/host/usbh.h"
#include "supervisor/usb.h"

#include "nrfx_gpiote.h"

static max3421e_max3421e_obj_t *_active;

static void max3421_int_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    if (!(action == NRF_GPIOTE_POLARITY_HITOLO)) {
        return;
    }
    max3421e_interrupt_handler(_active);
}

// Setup irq on self->irq to call tuh_int_handler(rhport, true) on falling edge
void common_hal_max3421e_max3421e_init_irq(max3421e_max3421e_obj_t *self) {
    nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    size_t pin_number = self->irq.pin->number;

    nrfx_gpiote_in_init(pin_number, &in_config, max3421_int_handler);
    nrfx_gpiote_in_event_enable(pin_number, true);

    _active = self;
}

void common_hal_max3421e_max3421e_deinit_irq(max3421e_max3421e_obj_t *self) {
    size_t pin_number = self->irq.pin->number;
    // Bulk reset may have already reset us but this will raise an assert in
    // debug mode if so.
    nrfx_gpiote_in_event_disable(pin_number);
    nrfx_gpiote_in_uninit(pin_number);
    _active = NULL;
}

// Enable or disable the irq interrupt.
void common_hal_max3421e_max3421e_irq_enabled(max3421e_max3421e_obj_t *self, bool enabled) {
    if (enabled) {
        NVIC_EnableIRQ(GPIOTE_IRQn);
    } else {
        NVIC_DisableIRQ(GPIOTE_IRQn);
    }
}
