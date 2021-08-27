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
#include "shared-bindings/microcontroller/__init__.h"
#include "py/runtime.h"

STATIC uint8_t refcount = 0;
STATIC pulseio_pulsein_obj_t *handles[RMT_CHANNEL_MAX];

// Requires rmt.c void esp32s2_peripherals_reset_all(void) to reset

STATIC void update_internal_buffer(pulseio_pulsein_obj_t *self) {
    uint32_t length = 0;
    rmt_item32_t *items = (rmt_item32_t *)xRingbufferReceive(self->buf_handle, &length, 0);
    if (items) {
        length /= 4;
        for (size_t i = 0; i < length; i++) {
            uint16_t pos = (self->start + self->len) % self->maxlen;
            self->buffer[pos] = items[i].duration0 * 3;
            // Check if second item exists before incrementing
            if (items[i].duration1) {
                self->buffer[pos + 1] = items[i].duration1 * 3;
                if (self->len < (self->maxlen - 1)) {
                    self->len += 2;
                } else {
                    self->start += 2;
                }
            } else {
                if (self->len < self->maxlen) {
                    self->len++;
                } else {
                    self->start++;
                }
            }
        }
        vRingbufferReturnItem(self->buf_handle, (void *)items);
    }
}

// We can't access the RMT interrupt, so we need a global service to prevent
// the ringbuffer from overflowing and crashing the peripheral
void pulsein_background(void) {
    for (size_t i = 0; i < RMT_CHANNEL_MAX; i++) {
        if (handles[i]) {
            update_internal_buffer(handles[i]);
            UBaseType_t items_waiting;
            vRingbufferGetInfo(handles[i]->buf_handle, NULL, NULL, NULL, NULL, &items_waiting);
        }
    }
}

void pulsein_reset(void) {
    for (size_t i = 0; i < RMT_CHANNEL_MAX; i++) {
        handles[i] = NULL;
    }
    if (refcount != 0) {
        supervisor_disable_tick();
    }
    refcount = 0;
}

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t *self, const mcu_pin_obj_t *pin,
    uint16_t maxlen, bool idle_state) {
    self->buffer = (uint16_t *)m_malloc(maxlen * sizeof(uint16_t), false);
    if (self->buffer == NULL) {
        mp_raise_msg_varg(&mp_type_MemoryError, translate("Failed to allocate RX buffer of %d bytes"), maxlen * sizeof(uint16_t));
    }
    self->pin = pin;
    self->maxlen = maxlen;
    self->idle_state = idle_state;
    self->start = 0;
    self->len = 0;
    self->paused = false;

    // Set pull settings
    gpio_pullup_dis(pin->number);
    gpio_pulldown_dis(pin->number);
    if (idle_state) {
        gpio_pullup_en(pin->number);
    } else {
        gpio_pulldown_en(pin->number);
    }

    // Find a free RMT Channel and configure it
    rmt_channel_t channel = esp32s2_peripherals_find_and_reserve_rmt();
    if (channel == RMT_CHANNEL_MAX) {
        mp_raise_RuntimeError(translate("All timers in use"));
    }
    rmt_config_t config = RMT_DEFAULT_CONFIG_RX(pin->number, channel);
    config.rx_config.filter_en = true;
    config.rx_config.idle_threshold = 30000; // 30*3=90ms idle required to register a sequence
    config.clk_div = 240; // All measurements are divided by 3 to accomodate 65ms pulses
    rmt_config(&config);
    rmt_driver_install(channel, 1000, 0); // TODO: pick a more specific buffer size?

    // Store this object and the buffer handle for background updates
    self->channel = channel;
    handles[channel] = self;
    rmt_get_ringbuf_handle(channel, &(self->buf_handle));

    // start RMT RX, and enable ticks so the core doesn't turn off.
    rmt_rx_start(channel, true);
    refcount++;
    if (refcount == 1) {
        supervisor_enable_tick();
    }
}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t *self) {
    return handles[self->channel] ? false : true;
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t *self) {
    handles[self->channel] = NULL;
    esp32s2_peripherals_free_rmt(self->channel);
    reset_pin_number(self->pin->number);
    refcount--;
    if (refcount == 0) {
        supervisor_disable_tick();
    }
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t *self) {
    self->paused = true;
    rmt_rx_stop(self->channel);
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

    self->paused = false;
    rmt_rx_start(self->channel, false);
}

void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t *self) {
    // Buffer only updates in BG tasks or fetches, so no extra protection is needed
    self->start = 0;
    self->len = 0;
}

uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t *self, int16_t index) {
    update_internal_buffer(self);
    if (index < 0) {
        index += self->len;
    }
    if (index < 0 || index >= self->len) {
        mp_raise_IndexError(translate("index out of range"));
    }
    uint16_t value = self->buffer[(self->start + index) % self->maxlen];
    return value;
}

uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t *self) {
    update_internal_buffer(self);

    if (self->len == 0) {
        mp_raise_IndexError(translate("pop from an empty PulseIn"));
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
