/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "common-hal/countio/Counter.h"
#include "shared-bindings/countio/Counter.h"
#include "common-hal/microcontroller/Pin.h"

#include "bindings/espidf/__init__.h"

#include "py/runtime.h"

#include "driver/gpio.h"

void common_hal_countio_counter_construct(countio_counter_obj_t *self,
    const mcu_pin_obj_t *pin, countio_edge_t edge, digitalio_pull_t pull) {
    claim_pin(pin);

    pcnt_unit_config_t unit_config = {
        // Set counter limit
        .low_limit = -1,
        .high_limit = INT16_MAX
    };
    // The pulse count driver automatically counts roll overs.
    unit_config.flags.accum_count = true;

    // initialize PCNT
    CHECK_ESP_RESULT(pcnt_new_unit(&unit_config, &self->unit));

    self->pin = pin->number;
    pcnt_chan_config_t channel_config = {
        .edge_gpio_num = self->pin,
        .level_gpio_num = -1
    };
    CHECK_ESP_RESULT(pcnt_new_channel(self->unit, &channel_config, &self->channel));
    pcnt_channel_edge_action_t pos = (edge == EDGE_RISE || edge == EDGE_RISE_AND_FALL) ? PCNT_CHANNEL_EDGE_ACTION_INCREASE : PCNT_CHANNEL_EDGE_ACTION_HOLD;
    pcnt_channel_edge_action_t neg = (edge == EDGE_FALL || edge == EDGE_RISE_AND_FALL) ? PCNT_CHANNEL_EDGE_ACTION_INCREASE : PCNT_CHANNEL_EDGE_ACTION_HOLD;
    pcnt_channel_set_edge_action(self->channel, pos, neg);

    gpio_pullup_dis(pin->number);
    gpio_pulldown_dis(pin->number);
    if (pull == PULL_UP) {
        gpio_pullup_en(pin->number);
    } else if (pull == PULL_DOWN) {
        gpio_pulldown_en(pin->number);
    }


    pcnt_unit_enable(self->unit);
    pcnt_unit_start(self->unit);
}

bool common_hal_countio_counter_deinited(countio_counter_obj_t *self) {
    return self->unit == NULL;
}

void common_hal_countio_counter_deinit(countio_counter_obj_t *self) {
    if (common_hal_countio_counter_deinited(self)) {
        return;
    }
    pcnt_unit_disable(self->unit);
    pcnt_del_channel(self->channel);
    reset_pin_number(self->pin);
    pcnt_del_unit(self->unit);
    self->unit = NULL;
}

mp_int_t common_hal_countio_counter_get_count(countio_counter_obj_t *self) {
    int count;
    pcnt_unit_get_count(self->unit, &count);
    return count + self->count;
}

void common_hal_countio_counter_set_count(countio_counter_obj_t *self,
    mp_int_t new_count) {
    self->count = new_count;
    pcnt_unit_clear_count(self->unit);
}
