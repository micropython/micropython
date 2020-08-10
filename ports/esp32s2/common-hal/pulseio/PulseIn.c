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
#include "py/runtime.h"

STATIC uint8_t refcount = 0;
STATIC pulseio_pulsein_obj_t * handles[RMT_CHANNEL_MAX];

// Requires rmt.c void esp32s2_peripherals_reset_all(void) to reset

STATIC void update_internal_buffer(pulseio_pulsein_obj_t* self) {
    //mp_printf(&mp_plat_print, "Update internal Buffer\n");
    uint32_t length = 0;
    rmt_item32_t *items = (rmt_item32_t *) xRingbufferReceive(self->buf_handle, &length, 0);
    if (items) {
        length /= 4;
        //mp_printf(&mp_plat_print, "Length%d\n",length);
        // TODO: If the size of the recieve is larger than the buffer, reset it?

        for (size_t i=0; i < length; i++) {
            //mp_printf(&mp_plat_print, "Item d0:%d, l0:%d, d1:%d, l1:%d\n",(items[i].duration0 * 3),
            //          items[i].level0,(items[i].duration1 * 3),items[i].level1);
            uint16_t pos = (self->start + self->len) % self->maxlen;
            self->buffer[pos] = items[i].duration0 * 3;
            // Check if second item exists before incrementing
            if (items[i].duration1) {
                self->buffer[pos+1] = items[i].duration1 * 3;
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
        vRingbufferReturnItem(self->buf_handle, (void *) items);
    }
}

// We can't access the RMT interrupt, so we need a global service to prevent
// the ringbuffer from overflowing and crashing the peripheral
void pulsein_background(void) {
    //mp_printf(&mp_plat_print, "BG Task!\n");
    for (size_t i = 0; i < RMT_CHANNEL_MAX; i++) {
        if (handles[i]) {
            //mp_printf(&mp_plat_print, "Located viable handle:%d\n",i);
            update_internal_buffer(handles[i]);
            UBaseType_t items_waiting;
            vRingbufferGetInfo(handles[i]->buf_handle, NULL, NULL, NULL, NULL, &items_waiting);
            //mp_printf(&mp_plat_print, "items waiting:%d\n",items_waiting);
            // if (items_waiting > handles[i]->maxlen) {
            //     mp_printf(&mp_plat_print, "Overage!\n");
            //     mp_printf(&mp_plat_print, "Items waiting detected:%d\n", items_waiting);
            //     update_internal_buffer(handles[i]);
            // }
        }
    }
}

void pulsein_reset(void) {
    mp_printf(&mp_plat_print, "Pulsein Reset called\n");
    for (size_t i = 0; i < RMT_CHANNEL_MAX; i++) {
        handles[i] = NULL;
    }
    supervisor_disable_tick();
    refcount = 0;
}

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t* self, const mcu_pin_obj_t* pin,
                                             uint16_t maxlen, bool idle_state) {
    self->buffer = (uint16_t *) m_malloc(maxlen * sizeof(uint16_t), false);
    if (self->buffer == NULL) {
        mp_raise_msg_varg(&mp_type_MemoryError, translate("Failed to allocate RX buffer of %d bytes"), maxlen * sizeof(uint16_t));
    }
    self->pin = pin;
    self->maxlen = maxlen;
    // self->idle_state = idle_state;
    self->start = 0;
    self->len = 0;
    // self->first_edge = true;
    self->paused = false;
    // self->last_overflow = 0;
    // self->last_count = 0;

    rmt_channel_t channel = esp32s2_peripherals_find_and_reserve_rmt();
    mp_printf(&mp_plat_print, "Selected Channel:%d!\n",channel);

    // Configure Channel
    rmt_config_t config = RMT_DEFAULT_CONFIG_RX(pin->number, channel);
    config.rx_config.filter_en = true;
    config.rx_config.idle_threshold = 30000;
    config.clk_div = 240;

    rmt_config(&config);
    size_t len = 1000; //TODO: pick a reasonable number for this?
    // size_t len = maxlen * 4;
    rmt_driver_install(channel, len, 0);

    self->channel = channel;

    // Store handle for background updates
    handles[channel] = self;

    rmt_get_ringbuf_handle(channel, &(self->buf_handle));
    rmt_rx_start(channel, true);

    supervisor_enable_tick();
    refcount++;
}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t* self) {
    return handles[self->channel] ? true : false;
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t* self) {
    handles[self->channel] = NULL;
    esp32s2_peripherals_free_rmt(self->channel);
    reset_pin_number(self->pin);
    refcount--;
    if (refcount == 0) {
        supervisor_disable_tick();
    }
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t* self) {
    self->paused = true;
    rmt_rx_stop();
}

void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t* self, uint16_t trigger_duration) {
    // Make sure we're paused.
    if ( !self->paused ) {
        common_hal_pulseio_pulsein_pause(self);
    }

    self->paused = false;
    rmt_rx_start();
}

void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t* self) {
    self->start = 0;
    self->len = 0;
}

uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t* self, int16_t index) {
    //mp_printf(&mp_plat_print, "Call GetItem\n");
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

uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t* self) {
    mp_printf(&mp_plat_print, "Call PopLeft\n");
    update_internal_buffer(self);

    if (self->len == 0) {
        mp_raise_IndexError(translate("pop from an empty PulseIn"));
    }

    uint16_t value = self->buffer[self->start];
    self->start = (self->start + 1) % self->maxlen;
    self->len--;

    return value;


    // uint32_t length = 0;
    // rmt_item32_t *items = (rmt_item32_t *) xRingbufferReceive(self->buf_handle, &length, 10);
    // mp_printf(&mp_plat_print, "Length%d\n",length);
    // if (items) {
    //     length /= 4;
    //     mp_printf(&mp_plat_print, "Length%d\n",length);
    //     for (size_t i=0; i < length; i++) {
    //         mp_printf(&mp_plat_print, "Item d0:%d, l0:%d, d1:%d, l1:%d\n",(items[i].duration0 * 3),
    //                   items[i].level0,(items[i].duration1 * 3),items[i].level1);
    //     }
    //     vRingbufferReturnItem(self->buf_handle, (void *) items);
    // }
    // // while(items) {
    // //     mp_printf(&mp_plat_print, "Length%d",length);
    // //     mp_printf(&mp_plat_print, "Item val0:%d, val1:%d, val:%d",items->duration0,
    // //                     items->duration1,items->val);
    // //     vRingbufferReturnItem(self->buf_handle, (void *) items);
    // //     items = (rmt_item32_t *) xRingbufferReceive(self->buf_handle, &length, 1);
    // // }
    // return items ? items[0].duration0 : false;
}

uint16_t common_hal_pulseio_pulsein_get_maxlen(pulseio_pulsein_obj_t* self) {
    return self->maxlen;
}

bool common_hal_pulseio_pulsein_get_paused(pulseio_pulsein_obj_t* self) {
    return self->paused;
}

uint16_t common_hal_pulseio_pulsein_get_len(pulseio_pulsein_obj_t* self) {
    return self->len;
}
