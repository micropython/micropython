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

#include "shared-bindings/frequencyio/FrequencyIn.h"

#include "py/runtime.h"

static void IRAM_ATTR pcnt_overflow_handler(void *self_in) {
    frequencyio_frequencyin_obj_t *self = self_in;
    // reset counter
    pcnt_counter_clear(self->unit);

    // increase multiplier
    self->multiplier++;

    // reset interrupt
    PCNT.int_clr.val = BIT(self->unit);
}

static void IRAM_ATTR timer_interrupt_handler(void *self_in) {
    frequencyio_frequencyin_obj_t *self = self_in;
    // get counter value
    int16_t count;
    pcnt_get_counter_value(self->unit, &count);
    self->frequency = ((count / 2.0) + (self->multiplier * INT16_MAX / 4.0)) / (self->capture_period);

    // reset multiplier
    self->multiplier = 0;

    // reset counter
    pcnt_counter_clear(self->unit);

    // reset interrupt
    timg_dev_t *device = self->timer.group ? &(TIMERG1) : &(TIMERG0);
    if (self->timer.idx) {
        device->int_clr.t1 = 1;
    } else {
        device->int_clr.t0 = 1;
    }
    device->hw_timer[self->timer.idx].config.alarm_en = 1;
}

static void init_pcnt(frequencyio_frequencyin_obj_t *self) {
    // Prepare configuration for the PCNT unit
    const pcnt_config_t pcnt_config = {
        // Set PCNT input signal and control GPIOs
        .pulse_gpio_num = self->pin,
        .ctrl_gpio_num = PCNT_PIN_NOT_USED,
        .channel = PCNT_CHANNEL_0,
        // What to do on the positive / negative edge of pulse input?
        .pos_mode = PCNT_COUNT_INC,  // count both rising and falling edges
        .neg_mode = PCNT_COUNT_INC,
        // Set counter limit
        .counter_h_lim = INT16_MAX,
        .counter_l_lim = 0,
    };

    // initialize PCNT
    const int8_t unit = peripherals_pcnt_init(pcnt_config);
    if (unit == -1) {
        mp_raise_RuntimeError(translate("All PCNT units in use"));
    }

    // set the GPIO back to high-impedance, as pcnt_unit_config sets it as pull-up
    gpio_set_pull_mode(self->pin, GPIO_FLOATING);

    self->unit = (pcnt_unit_t)unit;

    // enable pcnt interrupt
    pcnt_event_enable(self->unit, PCNT_EVT_H_LIM);
    pcnt_isr_register(pcnt_overflow_handler, (void *)self, ESP_INTR_FLAG_IRAM, &self->handle);
    pcnt_intr_enable(self->unit);
}

static void init_timer(frequencyio_frequencyin_obj_t *self) {
    // Prepare configuration for the timer module
    const timer_config_t config = {
        .alarm_en = true,
        .counter_en = false,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = true,
        .divider = 80            // 1 us per tick
    };

    // initialize Timer
    peripherals_timer_init(&config, &self->timer);
    if (self->timer.idx == TIMER_MAX || self->timer.group == TIMER_GROUP_MAX) {
        mp_raise_RuntimeError(translate("All timers in use"));
    }

    timer_idx_t idx = self->timer.idx;
    timer_group_t group = self->timer.group;

    // enable timer interrupt
    timer_set_alarm_value(group, idx, self->capture_period * 1000000);
    timer_isr_register(group, idx, timer_interrupt_handler, (void *)self, ESP_INTR_FLAG_IRAM, &self->handle);
    timer_enable_intr(group, idx);

    // start timer
    timer_start(self->timer.group, self->timer.idx);
}

void common_hal_frequencyio_frequencyin_construct(frequencyio_frequencyin_obj_t *self,
    const mcu_pin_obj_t *pin, const uint16_t capture_period) {
    if ((capture_period == 0) || (capture_period > 500)) {
        mp_raise_ValueError(translate("Invalid capture period. Valid range: 1 - 500"));
    }

    self->pin = pin->number;
    self->handle = NULL;
    self->multiplier = 0;
    self->capture_period = capture_period;

    // initialize pcnt and timer
    init_pcnt(self);
    init_timer(self);

    claim_pin(pin);
}

bool common_hal_frequencyio_frequencyin_deinited(frequencyio_frequencyin_obj_t *self) {
    return self->unit == PCNT_UNIT_MAX;
}

void common_hal_frequencyio_frequencyin_deinit(frequencyio_frequencyin_obj_t *self) {
    if (common_hal_frequencyio_frequencyin_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin);
    peripherals_pcnt_deinit(&self->unit);
    peripherals_timer_deinit(&self->timer);
    if (self->handle) {
        esp_intr_free(self->handle);
        self->handle = NULL;
    }
}

uint32_t common_hal_frequencyio_frequencyin_get_item(frequencyio_frequencyin_obj_t *self) {
    return self->frequency;
}

void common_hal_frequencyio_frequencyin_pause(frequencyio_frequencyin_obj_t *self) {
    pcnt_counter_pause(self->unit);
    timer_pause(self->timer.group, self->timer.idx);
}

void common_hal_frequencyio_frequencyin_resume(frequencyio_frequencyin_obj_t *self) {
    pcnt_counter_resume(self->unit);
    timer_start(self->timer.group, self->timer.idx);
}

void common_hal_frequencyio_frequencyin_clear(frequencyio_frequencyin_obj_t *self) {
    self->frequency = 0;
    pcnt_counter_clear(self->unit);
    timer_set_counter_value(self->timer.group, self->timer.idx, 0);
}

uint16_t common_hal_frequencyio_frequencyin_get_capture_period(frequencyio_frequencyin_obj_t *self) {
    return self->capture_period;
}

void common_hal_frequencyio_frequencyin_set_capture_period(frequencyio_frequencyin_obj_t *self, uint16_t capture_period) {
    if ((capture_period == 0) || (capture_period > 500)) {
        mp_raise_ValueError(translate("Invalid capture period. Valid range: 1 - 500"));
    }
    self->capture_period = capture_period;
    common_hal_frequencyio_frequencyin_clear(self);
    timer_set_alarm_value(self->timer.group, self->timer.idx, capture_period * 1000000);
}
