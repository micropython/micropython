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

#include "rom/gpio.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"

#if MICROPY_PY_MACHINE_BITSTREAM

/******************************************************************************/
// Bit-bang implementation

#define NS_TICKS_OVERHEAD (6)

// This is a translation of the cycle counter implementation in ports/stm32/machine_bitstream.c.
static void IRAM_ATTR machine_bitstream_high_low_bitbang(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    uint32_t pin_mask, gpio_reg_set, gpio_reg_clear;
    #if SOC_GPIO_PIN_COUNT > 32
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
    uint32_t fcpu_mhz = esp_rom_get_cpu_ticks_per_us();
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

#if SOC_RMT_SUPPORTED

/******************************************************************************/
// RMT implementation

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 3, 0)
#include "rmt_private.h"
#endif
#include "driver/rmt_tx.h"
#include "driver/rmt_encoder.h"

static bool machine_bitstream_high_low_rmt(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    // Use 40MHz clock (although 2MHz would probably be sufficient).
    uint32_t clock_div = 2;
    rmt_channel_handle_t channel = NULL;
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = pin,
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .resolution_hz = APB_CLK_FREQ / clock_div,
        .trans_queue_depth = 1,
    };
    if (rmt_new_tx_channel(&tx_chan_config, &channel) != ESP_OK) {
        return false;
    }
    check_esp_err(rmt_enable(channel));

    // Get the tick rate in kHz (this will likely be 40000).
    uint32_t counter_clk_khz = APB_CLK_FREQ / clock_div;
    counter_clk_khz /= 1000;

    // Convert nanoseconds to pulse duration.
    // Example: 500ns = 40000 * 500 / 1e6 = 20 ticks
    // 20 ticks / 40MHz = 500e-9
    rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = {
            .level0 = 1,
            .duration0 = (counter_clk_khz * timing_ns[0]) / 1e6,
            .level1 = 0,
            .duration1 = (counter_clk_khz * timing_ns[1]) / 1e6,
        },
        .bit1 = {
            .level0 = 1,
            .duration0 = (counter_clk_khz * timing_ns[2]) / 1e6,
            .level1 = 0,
            .duration1 = (counter_clk_khz * timing_ns[3]) / 1e6,
        },
        .flags.msb_first = 1
    };

    // Install the bits->highlow encoder.
    rmt_encoder_handle_t encoder;
    check_esp_err(rmt_new_bytes_encoder(&bytes_encoder_config, &encoder));

    rmt_transmit_config_t tx_config = {
        .loop_count = 0,
        .flags.eot_level = 0,
    };

    // Stream the byte data using the encoder.
    rmt_encoder_reset(encoder);
    check_esp_err(rmt_transmit(channel, encoder, buf, len, &tx_config));

    // Wait until completion.
    rmt_tx_wait_all_done(channel, -1);

    // Disable and release channel.
    check_esp_err(rmt_del_encoder(encoder));
    rmt_disable(channel);
    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 3, 0)
    channel->del(channel);
    #else
    rmt_del_channel(channel);
    #endif

    // Cancel RMT output to GPIO pin.
    esp_rom_gpio_connect_out_signal(pin, SIG_GPIO_OUT_IDX, false, false);

    return true;
}

#endif // SOC_RMT_SUPPORTED

/******************************************************************************/
// Interface to machine.bitstream

void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    #if SOC_RMT_SUPPORTED
    if (esp32_rmt_bitstream_enabled && machine_bitstream_high_low_rmt(pin, timing_ns, buf, len)) {
        // Use of RMT was successful.
        return;
    }
    #endif
    machine_bitstream_high_low_bitbang(pin, timing_ns, buf, len);
}

#endif // MICROPY_PY_MACHINE_BITSTREAM
