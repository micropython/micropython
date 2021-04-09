/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/bitbangio/OneWire.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

// Durations are taken from here: https://www.maximintegrated.com/en/app-notes/index.mvp/id/126

void shared_module_bitbangio_onewire_construct(bitbangio_onewire_obj_t *self,
    const mcu_pin_obj_t *pin) {
    self->pin.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->pin, pin);
}

bool shared_module_bitbangio_onewire_deinited(bitbangio_onewire_obj_t *self) {
    return common_hal_digitalio_digitalinout_deinited(&self->pin);
}

void shared_module_bitbangio_onewire_deinit(bitbangio_onewire_obj_t *self) {
    if (shared_module_bitbangio_onewire_deinited(self)) {
        return;
    }
    common_hal_digitalio_digitalinout_deinit(&self->pin);
}

// We use common_hal_mcu_delay_us(). It should not be dependent on interrupts
// to do accurate timekeeping, since we disable interrupts during the delays below.

bool shared_module_bitbangio_onewire_reset(bitbangio_onewire_obj_t *self) {
    common_hal_mcu_disable_interrupts();
    common_hal_digitalio_digitalinout_switch_to_output(&self->pin, false, DRIVE_MODE_OPEN_DRAIN);
    common_hal_mcu_delay_us(480);
    common_hal_digitalio_digitalinout_switch_to_input(&self->pin, PULL_NONE);
    common_hal_mcu_delay_us(70);
    bool value = common_hal_digitalio_digitalinout_get_value(&self->pin);
    common_hal_mcu_delay_us(410);
    common_hal_mcu_enable_interrupts();
    return value;
}

bool shared_module_bitbangio_onewire_read_bit(bitbangio_onewire_obj_t *self) {
    common_hal_mcu_disable_interrupts();
    common_hal_digitalio_digitalinout_switch_to_output(&self->pin, false, DRIVE_MODE_OPEN_DRAIN);
    common_hal_mcu_delay_us(6);
    common_hal_digitalio_digitalinout_switch_to_input(&self->pin, PULL_NONE);
    // TODO(tannewt): Test with more devices and maybe make the delays
    // configurable. This should be 9 by the datasheet but all bits read as 1
    // then.
    common_hal_mcu_delay_us(6);
    bool value = common_hal_digitalio_digitalinout_get_value(&self->pin);
    common_hal_mcu_delay_us(55);
    common_hal_mcu_enable_interrupts();
    return value;
}

void shared_module_bitbangio_onewire_write_bit(bitbangio_onewire_obj_t *self,
    bool bit) {
    common_hal_mcu_disable_interrupts();
    common_hal_digitalio_digitalinout_switch_to_output(&self->pin, false, DRIVE_MODE_OPEN_DRAIN);
    common_hal_mcu_delay_us(bit? 6 : 60);
    common_hal_digitalio_digitalinout_switch_to_input(&self->pin, PULL_NONE);
    common_hal_mcu_delay_us(bit? 64 : 10);
    common_hal_mcu_enable_interrupts();
}
