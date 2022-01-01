/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jim Mussared
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

#include "py/mpconfig.h"
#include "py/mphal.h"

#if MICROPY_PY_MACHINE_BITSTREAM

#include "driver/rmt.h"

#include "modesp32.h"

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 1, 0)
// This convenience macro was not available in earlier IDF versions.
#define RMT_DEFAULT_CONFIG_TX(gpio, channel_id)      \
    {                                                \
        .rmt_mode = RMT_MODE_TX,                     \
        .channel = channel_id,                       \
        .clk_div = 80,                               \
        .gpio_num = gpio,                            \
        .mem_block_num = 1,                          \
        .tx_config = {                               \
            .loop_en = false,                        \
            .carrier_freq_hz = 38000,                \
            .carrier_duty_percent = 33,              \
            .carrier_level = RMT_CARRIER_LEVEL_HIGH, \
            .carrier_en = false,                     \
            .idle_level = RMT_IDLE_LEVEL_LOW,        \
            .idle_output_en = true,                  \
        }                                            \
    }
#endif

// Logical 0 and 1 values (encoded as a rmt_item32_t).
// The duration fields will be set later.
STATIC rmt_item32_t bitstream_high_low_0 = {{{ 0, 1, 0, 0 }}};
STATIC rmt_item32_t bitstream_high_low_1 = {{{ 0, 1, 0, 0 }}};

// See https://github.com/espressif/esp-idf/blob/master/examples/common_components/led_strip/led_strip_rmt_ws2812.c
// This is called automatically by the IDF during rmt_write_sample in order to
// convert the byte stream to rmt_item32_t's.
STATIC void IRAM_ATTR bitstream_high_low_rmt_adapter(const void *src, rmt_item32_t *dest, size_t src_size, size_t wanted_num, size_t *translated_size, size_t *item_num) {
    if (src == NULL || dest == NULL) {
        *translated_size = 0;
        *item_num = 0;
        return;
    }

    size_t size = 0;
    size_t num = 0;
    uint8_t *psrc = (uint8_t *)src;
    rmt_item32_t *pdest = dest;
    while (size < src_size && num < wanted_num) {
        for (int i = 0; i < 8; i++) {
            // MSB first
            if (*psrc & (1 << (7 - i))) {
                pdest->val = bitstream_high_low_1.val;
            } else {
                pdest->val = bitstream_high_low_0.val;
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

// Use the reserved RMT channel to stream high/low data on the specified pin.
void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(pin, MICROPY_HW_ESP32_RMT_CHANNEL_BITSTREAM);

    // Use 40MHz clock (although 2MHz would probably be sufficient).
    config.clk_div = 2;

    // Install the driver on this channel & pin.
    check_esp_err(rmt_config(&config));
    check_esp_err(rmt_driver_install(config.channel, 0, 0));

    // Get the tick rate in kHz (this will likely be 40000).
    uint32_t counter_clk_khz = 0;
    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 1, 0)
    uint8_t div_cnt;
    check_esp_err(rmt_get_clk_div(config.channel, &div_cnt));
    counter_clk_khz = APB_CLK_FREQ / div_cnt;
    #else
    check_esp_err(rmt_get_counter_clock(config.channel, &counter_clk_khz));
    #endif

    counter_clk_khz /= 1000;

    // Convert nanoseconds to pulse duration.
    bitstream_high_low_0.duration0 = (counter_clk_khz * timing_ns[0]) / 1e6;
    bitstream_high_low_0.duration1 = (counter_clk_khz * timing_ns[1]) / 1e6;
    bitstream_high_low_1.duration0 = (counter_clk_khz * timing_ns[2]) / 1e6;
    bitstream_high_low_1.duration1 = (counter_clk_khz * timing_ns[3]) / 1e6;

    // Install the bits->highlow translator.
    rmt_translator_init(config.channel, bitstream_high_low_rmt_adapter);

    // Stream the byte data using the translator.
    check_esp_err(rmt_write_sample(config.channel, buf, len, true));

    // Wait 50% longer than we expect (if every bit takes the maximum time).
    uint32_t timeout_ms = (3 * len / 2) * (1 + (8 * MAX(timing_ns[0] + timing_ns[1], timing_ns[2] + timing_ns[3])) / 1000);
    check_esp_err(rmt_wait_tx_done(config.channel, pdMS_TO_TICKS(timeout_ms)));

    // Uninstall the driver.
    check_esp_err(rmt_driver_uninstall(config.channel));

    // Cancel RMT output to GPIO pin.
    gpio_matrix_out(pin, SIG_GPIO_OUT_IDX, false, false);
}

#endif // MICROPY_PY_MACHINE_BITSTREAM
