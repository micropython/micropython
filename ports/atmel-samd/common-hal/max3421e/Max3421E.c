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

#include "common-hal/max3421e/Max3421E.h"

#include "lib/tinyusb/src/host/usbh.h"
#include "supervisor/usb.h"

#include "eic_handler.h"
#include "py/runtime.h"
#include "samd/external_interrupts.h"

#include "shared-bindings/microcontroller/Pin.h"

void samd_max3421e_interrupt_handler(uint8_t channel) {
    max3421e_interrupt_handler((max3421e_max3421e_obj_t *)get_eic_channel_data(channel));
}

// Setup irq on self->irq to call tuh_int_handler(rhport, true) on falling edge
void common_hal_max3421e_max3421e_init_irq(max3421e_max3421e_obj_t *self) {
    const mcu_pin_obj_t *pin = self->irq.pin;
    if (!pin->has_extint) {
        raise_ValueError_invalid_pin();
    }
    if (eic_get_enable()) {
        if (!eic_channel_free(pin->extint_channel)) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("Internal resource(s) in use"));
        }
    } else {
        turn_on_external_interrupt_controller();
    }

    set_eic_channel_data(pin->extint_channel, self);
    set_eic_handler(pin->extint_channel, EIC_HANDLER_MAX3421E);
    turn_on_eic_channel(pin->extint_channel, EIC_CONFIG_SENSE0_LOW_Val);
}

void common_hal_max3421e_max3421e_deinit_irq(max3421e_max3421e_obj_t *self) {
    const mcu_pin_obj_t *pin = self->irq.pin;
    set_eic_handler(pin->extint_channel, EIC_HANDLER_NO_INTERRUPT);
    turn_off_eic_channel(pin->extint_channel);
    reset_pin_number(pin->extint_channel);
}

// Enable or disable the irq interrupt.
void common_hal_max3421e_max3421e_irq_enabled(max3421e_max3421e_obj_t *self, bool enabled) {
    const mcu_pin_obj_t *pin = self->irq.pin;
    uint32_t mask = 1 << pin->extint_channel;
    // Don't use the turn_(on|off) API because it may deinit the eic completely.
    if (!enabled) {
        EIC->INTENCLR.reg = mask << EIC_INTENSET_EXTINT_Pos;
    } else {
        EIC->INTENSET.reg = mask << EIC_INTENSET_EXTINT_Pos;
    }
}
