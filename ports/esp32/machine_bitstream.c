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
#include "modesp32.h"

#if MICROPY_PY_MACHINE_BITSTREAM

/******************************************************************************/
// Bit-bang implementation

#define NS_TICKS_OVERHEAD (6)

// This is a translation of the cycle counter implementation in ports/stm32/machine_bitstream.c.
STATIC void IRAM_ATTR machine_bitstream_high_low_bitbang(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    uint32_t pin_mask, gpio_reg_set, gpio_reg_clear;
    #if !CONFIG_IDF_TARGET_ESP32C3
    if (pin >= 32) {
        pin_mask = 1 << (pin - 32);
        gpio_reg_set = GPIO_OUT1_W1TS_REG;
        gpio_reg_clear = GPIO_OUT1_W1TC_REG;
    } else
    #endif
    {
        pin_mask = 1 << pin;
        gpio_reg_set = GPIO_OUT_W1TS_REG;
        gpio_reg_clear = GPIO_OUT_W1TC_REG;
    }

    // Convert ns to cpu ticks [high_time_0, period_0, high_time_1, period_1].
    uint32_t fcpu_mhz = ets_get_cpu_frequency();
    for (size_t i = 0; i < 4; ++i) {
        timing_ns[i] = fcpu_mhz * timing_ns[i] / 1000;
        if (timing_ns[i] > NS_TICKS_OVERHEAD) {
            timing_ns[i] -= NS_TICKS_OVERHEAD;
        }
        if (i % 2 == 1) {
            // Convert low_time to period (i.e. add high_time).
            timing_ns[i] += timing_ns[i - 1];
        }
    }

    uint32_t irq_state = mp_hal_quiet_timing_enter();

    for (size_t i = 0; i < len; ++i) {
        uint8_t b = buf[i];
        for (size_t j = 0; j < 8; ++j) {
            GPIO_REG_WRITE(gpio_reg_set, pin_mask);
            uint32_t start_ticks = mp_hal_ticks_cpu();
            uint32_t *t = &timing_ns[b >> 6 & 2];
            while (mp_hal_ticks_cpu() - start_ticks < t[0]) {
                ;
            }
            GPIO_REG_WRITE(gpio_reg_clear, pin_mask);
            b <<= 1;
            while (mp_hal_ticks_cpu() - start_ticks < t[1]) {
                ;
            }
        }
    }

    mp_hal_quiet_timing_exit(irq_state);
}

/******************************************************************************/
// RMT implementation

#include "driver/rmt.h"

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
STATIC void machine_bitstream_high_low_rmt(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len, uint8_t channel_id) {
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(pin, channel_id);

    // Use 40MHz clock (although 2MHz would probably be sufficient).
    config.clk_div = 2;

    // Install the driver on this channel & pin.
    check_esp_err(rmt_config(&config));
    check_esp_err(rmt_driver_install_core1(config.channel));

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

/******************************************************************************/
// Interface to machine.bitstream

void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    if (esp32_rmt_bitstream_channel_id < 0) {
        machine_bitstream_high_low_bitbang(pin, timing_ns, buf, len);
    } else {
        machine_bitstream_high_low_rmt(pin, timing_ns, buf, len, esp32_rmt_bitstream_channel_id);
    }
}

#endif // MICROPY_PY_MACHINE_BITSTREAM
