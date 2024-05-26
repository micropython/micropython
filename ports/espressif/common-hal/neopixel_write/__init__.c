// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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

#include "esp_clk_tree.h"
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
    uint32_t clock_speed;
    esp_clk_tree_src_get_freq_hz(RMT_CLK_SRC_DEFAULT,
        ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED,
        &clock_speed);
    rmt_tx_channel_config_t config = {
        .gpio_num = digitalinout->pin->number,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = clock_speed,
        .trans_queue_depth = 1,
    };

    // Greedily try and grab as much RMT memory as we can. The more we get, the
    // smoother the output will be because we'll trigger fewer interrupts. We'll
    // give it all back once we're done.
    rmt_channel_handle_t channel;
    esp_err_t result = ESP_ERR_NOT_FOUND;
    // If no other channels are in use, we can use all of the RMT RAM including the RX channels.
    config.mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL * SOC_RMT_CHANNELS_PER_GROUP;
    while (result == ESP_ERR_NOT_FOUND && config.mem_block_symbols > 0) {
        result = rmt_new_tx_channel(&config, &channel);
        config.mem_block_symbols -= SOC_RMT_MEM_WORDS_PER_CHANNEL;
    }
    CHECK_ESP_RESULT(result);

    size_t ns_per_tick = 1e9 / clock_speed;
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
    result = rmt_new_bytes_encoder(&encoder_config, &encoder);
    if (result != ESP_OK) {
        rmt_del_channel(channel);
        return;
    }

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
    result = rmt_transmit(channel, encoder, pixels, (size_t)numBytes, &transmit_config);
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
