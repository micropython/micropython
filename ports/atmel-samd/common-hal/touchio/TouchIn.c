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

#include <string.h>

#include "py/nlr.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "shared-bindings/touchio/TouchIn.h"
#include "supervisor/shared/translate.h"

// Native touchio only exists for SAMD21
#ifdef SAMD21

#include "hpl/pm/hpl_pm_base.h"

#include "samd/clocks.h"
#include "samd/pins.h"

#include "adafruit_ptc.h"

bool touch_enabled = false;

static uint16_t get_raw_reading(touchio_touchin_obj_t *self) {
    adafruit_ptc_start_conversion(PTC, &self->config);

    while (!adafruit_ptc_is_conversion_finished(PTC)) {
        // wait
        RUN_BACKGROUND_TASKS;
    }

    return adafruit_ptc_get_conversion_result(PTC);
}

void common_hal_touchio_touchin_construct(touchio_touchin_obj_t *self,
    const mcu_pin_obj_t *pin) {
    if (!pin->has_touch) {
        mp_raise_ValueError(translate("Invalid pin"));
    }
    claim_pin(pin);

    // Turn on the PTC if its not in use. We won't turn it off until reset.
    if (((Ptc *)PTC)->CTRLA.bit.ENABLE == 0) {
        // We run the PTC at 8mhz so divide the 48mhz clock by 6.
        uint8_t gclk = find_free_gclk(6);
        if (gclk > GCLK_GEN_NUM) {
            mp_raise_RuntimeError(translate("No free GCLKs"));
        }
        enable_clock_generator(gclk, CLOCK_48MHZ, 6);

        /* Setup and enable generic clock source for PTC module. */
        connect_gclk_to_peripheral(gclk, PTC_GCLK_ID);

        _pm_enable_bus_clock(PM_BUS_APBC, PTC);
    }

    adafruit_ptc_get_config_default(&self->config);
    self->config.pin = pin->number;
    self->config.yline = pin->touch_y_line;

    adafruit_ptc_init(PTC, &self->config);

    // Initial values for pins will vary, depending on what peripherals the pins
    // share on-chip.
    //
    // Set a "touched" threshold not too far above the initial value.
    // For simple finger touch, the values may vary as much as a factor of two,
    // but for touches using fruit or other objects, the difference is much less.

    self->threshold = get_raw_reading(self) + 100;
}

bool common_hal_touchio_touchin_deinited(touchio_touchin_obj_t *self) {
    return self->config.pin == NO_PIN;
}

void common_hal_touchio_touchin_deinit(touchio_touchin_obj_t *self) {
    // TODO(tannewt): Reset the PTC.
    if (common_hal_touchio_touchin_deinited(self)) {
        return;
    }
    // We leave the clocks running because they may be in use by others.

    reset_pin_number(self->config.pin);
    self->config.pin = NO_PIN;
}

void touchin_reset() {
    Ptc *ptc = ((Ptc *)PTC);
    if (ptc->CTRLA.bit.ENABLE == 1) {
        ptc->CTRLA.bit.ENABLE = 0;
        while (ptc->CTRLA.bit.ENABLE == 1) {
        }

        ptc->CTRLA.bit.SWRESET = 1;
        while (ptc->CTRLA.bit.SWRESET == 1) {
        }
    }
}

bool common_hal_touchio_touchin_get_value(touchio_touchin_obj_t *self) {
    uint16_t reading = get_raw_reading(self);
    return reading > self->threshold;
}

uint16_t common_hal_touchio_touchin_get_raw_value(touchio_touchin_obj_t *self) {
    return get_raw_reading(self);
}

uint16_t common_hal_touchio_touchin_get_threshold(touchio_touchin_obj_t *self) {
    return self->threshold;
}

void common_hal_touchio_touchin_set_threshold(touchio_touchin_obj_t *self, uint16_t new_threshold) {
    self->threshold = new_threshold;
}

#endif // SAMD21
