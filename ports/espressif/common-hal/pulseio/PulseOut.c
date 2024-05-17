// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/pulseio/PulseOut.h"
#include "shared-bindings/pulseio/PulseOut.h"

#include "bindings/espidf/__init__.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "py/runtime.h"

#include "driver/rmt_tx.h"

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint32_t frequency,
    uint16_t duty_cycle) {

    // Reserve channel
    rmt_tx_channel_config_t config = {
        .gpio_num = pin->number,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .trans_queue_depth = 1,
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
    };
    CHECK_ESP_RESULT(rmt_new_tx_channel(&config, &self->channel));

    rmt_copy_encoder_config_t encoder_config = {};
    esp_err_t result = rmt_new_copy_encoder(&encoder_config, &self->encoder);
    if (result != ESP_OK) {
        rmt_del_channel(self->channel);
        self->channel = NULL;
        raise_esp_error(result);
    }

    if (duty_cycle != 0xffff) {
        rmt_carrier_config_t tx_carrier_cfg = {
            .duty_cycle = (duty_cycle * 1.0) / (1 << 16), // duty cycle as a float 0-1
            .frequency_hz = frequency,
            .flags.polarity_active_low = false, // carrier should be modulated to high level
        };
        // modulate carrier to TX channel
        ESP_ERROR_CHECK(rmt_apply_carrier(self->channel, &tx_carrier_cfg));
    }

    rmt_enable(self->channel);
}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t *self) {
    return self->channel == NULL;
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t *self) {
    rmt_disable(self->channel);
    rmt_del_encoder(self->encoder);
    rmt_del_channel(self->channel);
    self->channel = NULL;
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t *self, uint16_t *pulses, uint16_t length) {
    rmt_symbol_word_t symbols[length];

    // Circuitpython allows 16 bit pulse values, while ESP32 only allows 15 bits
    // Thus, we use entire items for one pulse, rather than switching inside each item
    for (size_t i = 0; i < length; i++) {
        // Setting the RMT duration to 0 has undefined behavior, so avoid that pre-emptively.
        if (pulses[i] == 0) {
            continue;
        }
        uint32_t level = (i % 2) ? 0 : 1;
        rmt_symbol_word_t symbol = {
            .duration0 = (pulses[i] & 0x8000 ? 0x7FFF : 1),
            .level0 = level,
            .duration1 = (pulses[i] & 0x7FFF),
            .level1 = level
        };
        symbols[i] = symbol;
    }

    // Write and wait to finish
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0,
        .flags.eot_level = 0
    };
    CHECK_ESP_RESULT(rmt_transmit(self->channel, self->encoder, symbols, length * sizeof(rmt_symbol_word_t), &transmit_config));

    esp_err_t result = ESP_ERR_TIMEOUT;
    while (result == ESP_ERR_TIMEOUT) {
        RUN_BACKGROUND_TASKS;
        result = rmt_tx_wait_all_done(self->channel, 0);
    }
    CHECK_ESP_RESULT(result);
}
