// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/rotaryio/IncrementalEncoder.h"

#include "bindings/espidf/__init__.h"
#include "common-hal/microcontroller/Pin.h"

#include "py/runtime.h"

void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t *self,
    const mcu_pin_obj_t *pin_a, const mcu_pin_obj_t *pin_b) {
    // This configuration counts on all edges of the quadrature signal: Channel 0
    // counts on rising and falling edges of channel A, with the direction set by the
    // polarity of channel B. Channel 1 does likewise, counting edges of channel B according
    // to the polarity of channel A. A little pencil work suffices to show that this
    // counts correctly on all 8 correct quadrature state transitions.
    //
    // These routines also implicitly configure the weak internal pull-ups, as expected
    // in CircuitPython.
    pcnt_unit_config_t unit_config = {
        // Set counter limit
        .low_limit = INT16_MIN,
        .high_limit = INT16_MAX
    };
    // Enable PCNT internal accumulator to count overflows.
    unit_config.flags.accum_count = true;

    // initialize PCNT
    CHECK_ESP_RESULT(pcnt_new_unit(&unit_config, &self->unit));

    // Set watchpoints at limits, to auto-accumulate overflows.
    pcnt_unit_add_watch_point(self->unit, INT16_MIN);
    pcnt_unit_add_watch_point(self->unit, INT16_MAX);

    pcnt_chan_config_t channel_a_config = {
        .edge_gpio_num = pin_a->number,
        .level_gpio_num = pin_b->number
    };
    esp_err_t result = pcnt_new_channel(self->unit, &channel_a_config, &self->channel_a);
    if (result != ESP_OK) {
        pcnt_del_unit(self->unit);
        self->unit = NULL;
        raise_esp_error(result);
    }
    pcnt_channel_set_edge_action(self->channel_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE /* pos */, PCNT_CHANNEL_EDGE_ACTION_INCREASE /* neg */);
    pcnt_channel_set_level_action(self->channel_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP /* high */, /* low */ PCNT_CHANNEL_LEVEL_ACTION_INVERSE);

    pcnt_chan_config_t channel_b_config = {
        .edge_gpio_num = pin_b->number,
        .level_gpio_num = pin_a->number
    };
    result = pcnt_new_channel(self->unit, &channel_b_config, &self->channel_b);
    if (result != ESP_OK) {
        pcnt_del_channel(self->channel_a);
        pcnt_del_unit(self->unit);
        self->unit = NULL;
        raise_esp_error(result);
    }
    pcnt_channel_set_edge_action(self->channel_b, PCNT_CHANNEL_EDGE_ACTION_DECREASE /* pos */, PCNT_CHANNEL_EDGE_ACTION_INCREASE /* neg */);
    pcnt_channel_set_level_action(self->channel_b, PCNT_CHANNEL_LEVEL_ACTION_INVERSE /* high */, /* low */ PCNT_CHANNEL_LEVEL_ACTION_KEEP);

    self->pin_a = pin_a->number;
    self->pin_b = pin_b->number;

    claim_pin(pin_a);
    claim_pin(pin_b);

    pcnt_unit_enable(self->unit);
    pcnt_unit_start(self->unit);
}

bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t *self) {
    return self->unit == NULL;
}

void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t *self) {
    if (common_hal_rotaryio_incrementalencoder_deinited(self)) {
        return;
    }
    pcnt_unit_disable(self->unit);
    reset_pin_number(self->pin_a);
    reset_pin_number(self->pin_b);
    pcnt_del_channel(self->channel_a);
    pcnt_del_channel(self->channel_b);
    pcnt_del_unit(self->unit);
    self->unit = NULL;
}

mp_int_t common_hal_rotaryio_incrementalencoder_get_position(rotaryio_incrementalencoder_obj_t *self) {
    int count;
    pcnt_unit_get_count(self->unit, &count);

    return (count + self->position) / self->divisor;
}

void common_hal_rotaryio_incrementalencoder_set_position(rotaryio_incrementalencoder_obj_t *self,
    mp_int_t new_position) {
    self->position = new_position * self->divisor;
    pcnt_unit_clear_count(self->unit);
}

mp_int_t common_hal_rotaryio_incrementalencoder_get_divisor(rotaryio_incrementalencoder_obj_t *self) {
    return self->divisor;
}

void common_hal_rotaryio_incrementalencoder_set_divisor(rotaryio_incrementalencoder_obj_t *self, mp_int_t divisor) {
    self->divisor = divisor;
}
