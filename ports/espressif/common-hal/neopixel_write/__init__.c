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

#include "shared-bindings/neopixel_write/__init__.h"

#include "py/mphal.h"
#include "py/runtime.h"

#include "bindings/espidf/__init__.h"
#include "supervisor/port.h"

#include "driver/gpio.h"
#include "driver/rmt_tx.h"

// Use closer to WS2812-style timings instead of WS2812B, to accommodate more varieties.
#define WS2812_T0H_NS (316)
#define WS2812_T0L_NS (316 * 3)
#define WS2812_T1H_NS (700)
#define WS2812_T1L_NS (564)

static uint64_t next_start_raw_ticks = 0;

void common_hal_neopixel_write(const digitalio_digitalinout_obj_t *digitalinout, uint8_t *pixels, uint32_t numBytes) {
    // Reserve channel
    rmt_tx_channel_config_t config = {
        .gpio_num = digitalinout->pin->number,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 40000000,
        .trans_queue_depth = 1,
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
    };
    rmt_channel_handle_t channel;
    CHECK_ESP_RESULT(rmt_new_tx_channel(&config, &channel));

    size_t ns_per_tick = 1e9 / 40000000;
    uint16_t ws2812_t0h_ticks = WS2812_T0H_NS / ns_per_tick;
    uint16_t ws2812_t0l_ticks = WS2812_T0L_NS / ns_per_tick;
    uint16_t ws2812_t1h_ticks = WS2812_T1H_NS / ns_per_tick;
    uint16_t ws2812_t1l_ticks = WS2812_T1L_NS / ns_per_tick;

    rmt_symbol_word_t bit0 = {
        .duration0 = ws2812_t0h_ticks,
        .level0 = 1,
        .duration1 = ws2812_t0l_ticks,
        .level1 = 0
    };
    rmt_symbol_word_t bit1 = {
        .duration0 = ws2812_t1h_ticks,
        .level0 = 1,
        .duration1 = ws2812_t1l_ticks,
        .level1 = 0
    };
    rmt_bytes_encoder_config_t encoder_config = {
        .bit0 = bit0,
        .bit1 = bit1,
        .flags = {
            .msb_first = true
        }
    };
    rmt_encoder_handle_t encoder;
    CHECK_ESP_RESULT(rmt_new_bytes_encoder(&encoder_config, &encoder));

    // Wait to make sure we don't append onto the last transmission. This should only be a tick or
    // two.
    while (port_get_raw_ticks(NULL) < next_start_raw_ticks) {
    }

    rmt_enable(channel);

    // Write and wait to finish
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0,
        .flags.eot_level = 0
    };
    esp_err_t result = rmt_transmit(channel, encoder, pixels, (size_t)numBytes, &transmit_config);
    if (result != ESP_OK) {
        rmt_del_encoder(encoder);
        rmt_disable(channel);
        rmt_del_channel(channel);
        raise_esp_error(result);
    }
    result = ESP_ERR_TIMEOUT;
    while (result == ESP_ERR_TIMEOUT) {
        RUN_BACKGROUND_TASKS;
        result = rmt_tx_wait_all_done(channel, 0);
    }

    // Update the next start to +2 ticks. It ensures that we've gone 300+ us.
    next_start_raw_ticks = port_get_raw_ticks(NULL) + 2;

    // Free channel again
    rmt_del_encoder(encoder);
    rmt_disable(channel);
    rmt_del_channel(channel);
    CHECK_ESP_RESULT(result);
    // Swap pin back to GPIO mode
    gpio_set_direction(digitalinout->pin->number, GPIO_MODE_OUTPUT);
}
