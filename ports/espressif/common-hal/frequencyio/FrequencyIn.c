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

#include "bindings/espidf/__init__.h"
#include "py/runtime.h"

#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "soc/timer_group_struct.h"
#include "esp_heap_caps.h"

// All gptimer handlers are in RAM so that protomatter can run during flash writes.
static IRAM_ATTR bool timer_interrupt_handler(gptimer_handle_t timer,
    const gptimer_alarm_event_data_t *edata, void *internal_data_ptr) {
    _internal_data_t *internal_data = (_internal_data_t *)internal_data_ptr;
    // get counter value
    pcnt_unit_get_count(internal_data->unit, &internal_data->pulse_count);

    // reset counter
    pcnt_unit_clear_count(internal_data->unit);
    return false;
}

static esp_err_t init_pcnt(frequencyio_frequencyin_obj_t *self) {
    pcnt_unit_config_t unit_config = {
        // Set counter limit
        .low_limit = -INT16_MAX + 1,
        .high_limit = INT16_MAX
    };
    // The pulse count driver automatically counts roll overs.
    unit_config.flags.accum_count = true;

    // initialize PCNT
    esp_err_t result = pcnt_new_unit(&unit_config, &self->internal_data->unit);
    if (result != ESP_OK) {
        return result;
    }

    pcnt_chan_config_t channel_config = {
        .edge_gpio_num = self->pin,
        .level_gpio_num = -1
    };
    result = pcnt_new_channel(self->internal_data->unit, &channel_config, &self->channel);
    if (result != ESP_OK) {
        pcnt_del_unit(self->internal_data->unit);
        return result;
    }
    // Count both edges of the signal.
    pcnt_channel_set_edge_action(self->channel, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);

    pcnt_unit_enable(self->internal_data->unit);
    pcnt_unit_start(self->internal_data->unit);

    // set the GPIO back to high-impedance, as pcnt_unit_config sets it as pull-up
    gpio_set_pull_mode(self->pin, GPIO_FLOATING);
    return ESP_OK;
}

static esp_err_t init_timer(frequencyio_frequencyin_obj_t *self) {
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000 * 1000, // 1,000 counts per millisecond.
        .flags = {
            .intr_shared = true
        }
    };

    // initialize Timer
    CHECK_ESP_RESULT(gptimer_new_timer(&config, &self->timer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0, // counter will reload with 0 on alarm event
        .alarm_count = self->capture_period_ms * 1000, // period in ms
        .flags.auto_reload_on_alarm = true, // enable auto-reload
    };
    esp_err_t result = gptimer_set_alarm_action(self->timer, &alarm_config);
    if (result != ESP_OK) {
        gptimer_del_timer(self->timer);
        return result;
    }

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_interrupt_handler, // register user callback
    };
    result = gptimer_register_event_callbacks(self->timer, &cbs, self->internal_data);
    if (result != ESP_OK) {
        gptimer_del_timer(self->timer);
        return result;
    }
    result = gptimer_enable(self->timer);
    if (result != ESP_OK) {
        gptimer_del_timer(self->timer);
        return result;
    }
    result = gptimer_start(self->timer);
    if (result != ESP_OK) {
        gptimer_del_timer(self->timer);
        return result;
    }
    return ESP_OK;
}

void common_hal_frequencyio_frequencyin_construct(frequencyio_frequencyin_obj_t *self,
    const mcu_pin_obj_t *pin, const uint16_t capture_period_ms) {
    mp_arg_validate_int_range(capture_period_ms, 1, 500, MP_QSTR_capture_period);

    self->pin = pin->number;
    self->capture_period_ms = capture_period_ms;
    self->internal_data = heap_caps_malloc(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL, sizeof(_internal_data_t));
    if (self->internal_data == NULL) {
        raise_esp_error(ESP_ERR_NO_MEM);
    }
    self->internal_data->pulse_count = 0;

    // initialize pcnt and timer
    esp_err_t result = init_pcnt(self);
    if (result != ESP_OK) {
        heap_caps_free(self->internal_data);
        self->internal_data = NULL;
        raise_esp_error(result);
    }
    result = init_timer(self);
    if (result != ESP_OK) {
        pcnt_del_channel(self->channel);
        pcnt_del_unit(self->internal_data->unit);
        heap_caps_free(self->internal_data);
        self->internal_data = NULL;
        raise_esp_error(result);
    }

    claim_pin(pin);
}

bool common_hal_frequencyio_frequencyin_deinited(frequencyio_frequencyin_obj_t *self) {
    return self->internal_data == NULL;
}

void common_hal_frequencyio_frequencyin_deinit(frequencyio_frequencyin_obj_t *self) {
    if (common_hal_frequencyio_frequencyin_deinited(self)) {
        return;
    }
    gptimer_stop(self->timer);
    gptimer_disable(self->timer);
    gptimer_del_timer(self->timer);
    pcnt_unit_disable(self->internal_data->unit);
    pcnt_del_channel(self->channel);
    reset_pin_number(self->pin);
    pcnt_del_unit(self->internal_data->unit);
    self->internal_data->unit = NULL;
    heap_caps_free(self->internal_data);
    self->internal_data = NULL;
}

uint32_t common_hal_frequencyio_frequencyin_get_item(frequencyio_frequencyin_obj_t *self) {
    // pulse_count / capture_period_ms is pulses per ms. * 1000 is pulses per second.
    // We have two pulse counts (one for each edge) per cycle so / 2. Combine to do
    // * 500 instead of * 1000 / 2.
    return self->internal_data->pulse_count * 500 / self->capture_period_ms;
}

void common_hal_frequencyio_frequencyin_pause(frequencyio_frequencyin_obj_t *self) {
    pcnt_unit_stop(self->internal_data->unit);
    gptimer_stop(self->timer);
}

void common_hal_frequencyio_frequencyin_resume(frequencyio_frequencyin_obj_t *self) {
    pcnt_unit_start(self->internal_data->unit);
    gptimer_start(self->timer);
}

void common_hal_frequencyio_frequencyin_clear(frequencyio_frequencyin_obj_t *self) {
    self->internal_data->pulse_count = 0;
    pcnt_unit_clear_count(self->internal_data->unit);
    gptimer_set_raw_count(self->timer, 0);
}

uint16_t common_hal_frequencyio_frequencyin_get_capture_period(frequencyio_frequencyin_obj_t *self) {
    return self->capture_period_ms;
}

void common_hal_frequencyio_frequencyin_set_capture_period(frequencyio_frequencyin_obj_t *self, uint16_t capture_period_ms) {
    mp_arg_validate_int_range(capture_period_ms, 1, 500, MP_QSTR_capture_period);

    common_hal_frequencyio_frequencyin_clear(self);

    self->capture_period_ms = capture_period_ms;
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = capture_period_ms * 1000,
        .reload_count = 0, // counter will reload with 0 on alarm event
        .flags.auto_reload_on_alarm = true, // enable auto-reload
    };
    gptimer_set_alarm_action(self->timer, &alarm_config);
}
