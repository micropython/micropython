/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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

#include "common-hal/pulseio/PulseIn.h"
#include "bindings/espidf/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "py/runtime.h"

#include "driver/gpio.h"
#include "driver/rmt_rx.h"

// Save IRAM by not working during flash writes or erases.
#ifdef CONFIG_RMT_ISR_IRAM_SAFE
#error "CircuitPython RMT callback is not IRAM safe"
#endif

static const rmt_receive_config_t rx_config = {
    .signal_range_min_ns = 1250, // 1.25 microseconds
    .signal_range_max_ns = 0xffff * 1000, // ~65 milliseconds
};

static bool _done_callback(rmt_channel_handle_t rx_chan,
    const rmt_rx_done_event_data_t *edata, void *user_ctx) {
    pulseio_pulsein_obj_t *self = (pulseio_pulsein_obj_t *)user_ctx;
    if (self->paused) {
        return false;
    }

    for (size_t i = 0; i < edata->num_symbols; i++) {
        uint16_t pos = (self->start + self->len) % self->maxlen;
        rmt_symbol_word_t symbol = edata->received_symbols[i];
        uint32_t val = symbol.duration0 * 2;

        bool done = val == 0;
        // Duration of zero indicates the end of transmission which is longer
        // than we can capture. So, set the max value.
        if (val == 0) {
            val = 65535;
        }
        if (!self->find_first || symbol.level0 == !self->idle_state) {
            self->find_first = false;
            self->buffer[pos] = (uint16_t)val;
            self->len++;
            if (done) {
                break;
            }
        }

        pos = (self->start + self->len) % self->maxlen;
        val = symbol.duration1 * 2;
        done = val == 0;
        // Duration of zero indicates the end of transmission which is longer
        // than we can capture. So, set the max value.
        if (val == 0) {
            val = 65535;
        }
        self->buffer[pos] = (uint16_t)val;
        self->len++;
        self->find_first = false;
        if (done) {
            break;
        }
    }

    if (!self->paused) {
        rmt_receive(self->channel, self->raw_symbols, self->raw_symbols_size, &rx_config);
    }
    return false;
}

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t *self, const mcu_pin_obj_t *pin,
    uint16_t maxlen, bool idle_state) {
    self->buffer = (uint16_t *)m_malloc(maxlen * sizeof(uint16_t));
    if (self->buffer == NULL) {
        m_malloc_fail(maxlen * sizeof(uint16_t));
    }
    // We add one to the maxlen version to ensure that two symbols at lease are
    // captured because we may skip the first portion of a symbol.
    self->raw_symbols_size = MIN(64, maxlen / 2 + 1) * sizeof(rmt_symbol_word_t);
    self->raw_symbols = (rmt_symbol_word_t *)m_malloc(self->raw_symbols_size);
    if (self->raw_symbols == NULL) {
        m_free(self->buffer);
        m_malloc_fail(self->raw_symbols_size);
    }
    self->pin = pin;
    self->maxlen = maxlen;
    self->idle_state = idle_state;
    self->start = 0;
    self->len = 0;
    self->paused = false;
    self->find_first = true;

    // Set pull settings
    gpio_pullup_dis(pin->number);
    gpio_pulldown_dis(pin->number);
    if (idle_state) {
        gpio_pullup_en(pin->number);
    } else {
        gpio_pulldown_en(pin->number);
    }

    // Find a free RMT Channel and configure it
    rmt_rx_channel_config_t config = {
        .gpio_num = pin->number,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        // 2 us resolution so we can capture 65ms pulses. The RMT period is only 15 bits.
        .resolution_hz = 1000000 / 2,
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
    };
    // If we fail here, the buffers allocated above will be garbage collected.
    CHECK_ESP_RESULT(rmt_new_rx_channel(&config, &self->channel));

    rmt_rx_event_callbacks_t rx_callback = {
        .on_recv_done = _done_callback
    };
    rmt_rx_register_event_callbacks(self->channel, &rx_callback, self);
    rmt_enable(self->channel);
    rmt_receive(self->channel, self->raw_symbols, self->raw_symbols_size, &rx_config);
}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t *self) {
    return self->channel == NULL;
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t *self) {
    rmt_disable(self->channel);
    reset_pin_number(self->pin->number);
    rmt_del_channel(self->channel);
    self->channel = NULL;
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t *self) {
    self->paused = true;
    rmt_disable(self->channel);
}

void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t *self, uint16_t trigger_duration) {
    // Make sure we're paused.
    if (!self->paused) {
        common_hal_pulseio_pulsein_pause(self);
    }

    if (trigger_duration > 0) {
        gpio_set_direction(self->pin->number, GPIO_MODE_DEF_OUTPUT);
        gpio_set_level(self->pin->number, !self->idle_state);
        common_hal_mcu_delay_us((uint32_t)trigger_duration);
        gpio_set_level(self->pin->number, self->idle_state);
        gpio_set_direction(self->pin->number, GPIO_MODE_INPUT); // should revert to pull direction
    }

    self->find_first = true;
    self->paused = false;
    rmt_enable(self->channel);
    rmt_receive(self->channel, self->raw_symbols, self->raw_symbols_size, &rx_config);
}

void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t *self) {
    // Buffer only updates in BG tasks or fetches, so no extra protection is needed
    self->start = 0;
    self->len = 0;
}

uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t *self, int16_t index) {
    if (index < 0) {
        index += self->len;
    }
    if (index < 0 || index >= self->len) {
        mp_raise_IndexError(MP_ERROR_TEXT("index out of range"));
    }
    uint16_t value = self->buffer[(self->start + index) % self->maxlen];
    return value;
}

uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t *self) {
    if (self->len == 0) {
        mp_raise_IndexError(MP_ERROR_TEXT("pop from an empty PulseIn"));
    }

    uint16_t value = self->buffer[self->start];
    self->start = (self->start + 1) % self->maxlen;
    self->len--;

    return value;
}

uint16_t common_hal_pulseio_pulsein_get_maxlen(pulseio_pulsein_obj_t *self) {
    return self->maxlen;
}

bool common_hal_pulseio_pulsein_get_paused(pulseio_pulsein_obj_t *self) {
    return self->paused;
}

uint16_t common_hal_pulseio_pulsein_get_len(pulseio_pulsein_obj_t *self) {
    return self->len;
}
