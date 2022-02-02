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

/* Uses code from Espressif RGB LED Strip demo and drivers
 * Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "py/mphal.h"
#include "py/runtime.h"
#include "shared-bindings/neopixel_write/__init__.h"
#include "supervisor/port.h"
#include "components/driver/include/driver/rmt.h"
#include "peripherals/rmt.h"

// 416 ns is 1/3 of the 1250ns period of a 800khz signal.
#define WS2812_T0H_NS (416)
#define WS2812_T0L_NS (416 * 2)
#define WS2812_T1H_NS (416 * 2)
#define WS2812_T1L_NS (416)

static uint32_t ws2812_t0h_ticks = 0;
static uint32_t ws2812_t1h_ticks = 0;
static uint32_t ws2812_t0l_ticks = 0;
static uint32_t ws2812_t1l_ticks = 0;

static uint64_t next_start_raw_ticks = 0;

static void IRAM_ATTR ws2812_rmt_adapter(const void *src, rmt_item32_t *dest, size_t src_size,
    size_t wanted_num, size_t *translated_size, size_t *item_num) {
    if (src == NULL || dest == NULL) {
        *translated_size = 0;
        *item_num = 0;
        return;
    }
    const rmt_item32_t bit0 = {{{ ws2812_t0h_ticks, 1, ws2812_t0l_ticks, 0 }}}; // Logical 0
    const rmt_item32_t bit1 = {{{ ws2812_t1h_ticks, 1, ws2812_t1l_ticks, 0 }}}; // Logical 1
    size_t size = 0;
    size_t num = 0;
    uint8_t *psrc = (uint8_t *)src;
    rmt_item32_t *pdest = dest;
    while (size < src_size && num < wanted_num) {
        for (int i = 0; i < 8; i++) {
            // MSB first
            if (*psrc & (1 << (7 - i))) {
                pdest->val = bit1.val;
            } else {
                pdest->val = bit0.val;
            }
            num++;
            pdest++;
        }
        size++;
        psrc++;
    }
    *translated_size = size;
    *item_num = num;
}

void common_hal_neopixel_write(const digitalio_digitalinout_obj_t *digitalinout, uint8_t *pixels, uint32_t numBytes) {
    // Reserve channel
    uint8_t number = digitalinout->pin->number;
    rmt_channel_t channel = peripherals_find_and_reserve_rmt(TRANSMIT_MODE);
    if (channel == RMT_CHANNEL_MAX) {
        mp_raise_RuntimeError(translate("All timers in use"));
    }

    // Configure Channel
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(number, channel);
    config.clk_div = 2; // set counter clock to 40MHz
    rmt_config(&config);
    rmt_driver_install(config.channel, 0, 0);

    // Convert NS timings to ticks
    uint32_t counter_clk_hz = 0;
    if (rmt_get_counter_clock(config.channel, &counter_clk_hz) != ESP_OK) {
        mp_raise_RuntimeError(translate("Could not retrieve clock"));
    }
    size_t ns_per_tick = 1e9 / counter_clk_hz;
    ws2812_t0h_ticks = WS2812_T0H_NS / ns_per_tick;
    ws2812_t0l_ticks = WS2812_T0L_NS / ns_per_tick;
    ws2812_t1h_ticks = WS2812_T1H_NS / ns_per_tick;
    ws2812_t1l_ticks = WS2812_T1L_NS / ns_per_tick;

    // Initialize automatic timing translator
    rmt_translator_init(config.channel, ws2812_rmt_adapter);

    // Wait to make sure we don't append onto the last transmission. This should only be a tick or
    // two.
    while (port_get_raw_ticks(NULL) < next_start_raw_ticks) {
    }

    // Write and wait to finish
    if (rmt_write_sample(config.channel, pixels, (size_t)numBytes, true) != ESP_OK) {
        mp_raise_RuntimeError(translate("Input/output error"));
    }
    rmt_wait_tx_done(config.channel, pdMS_TO_TICKS(100));

    // Update the next start to +2 ticks. It ensures that we've gone 300+ us.
    next_start_raw_ticks = port_get_raw_ticks(NULL) + 2;

    // Free channel again
    peripherals_free_rmt(config.channel);
    // Swap pin back to GPIO mode
    gpio_set_direction(digitalinout->pin->number, GPIO_MODE_OUTPUT);
}
