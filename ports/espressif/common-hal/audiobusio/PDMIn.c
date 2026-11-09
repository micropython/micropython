// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT


#include "bindings/espidf/__init__.h"

#include "common-hal/audiobusio/PDMIn.h"
#include "py/mpprint.h"
#include "py/runtime.h"
#include "shared-bindings/audiobusio/PDMIn.h"


#include "driver/i2s_pdm.h"



#if CIRCUITPY_AUDIOBUSIO_PDMIN



/**
	Note: I think this function needs an additional parameter for the word select
	pin. It takes `mono`, a boolean indicating if it should be mono or
	stereo, but without a word select pin, I don't think one can get
	the other channel.
*/

void common_hal_audiobusio_pdmin_construct(audiobusio_pdmin_obj_t *self,
    const mcu_pin_obj_t *clock_pin,
    const mcu_pin_obj_t *data_pin,
    uint32_t sample_rate,
    uint8_t bit_depth,
    bool mono,
    uint8_t oversample) {

    if (bit_depth != I2S_DATA_BIT_WIDTH_8BIT
        && bit_depth != I2S_DATA_BIT_WIDTH_16BIT
        && bit_depth != I2S_DATA_BIT_WIDTH_24BIT
        && bit_depth != I2S_DATA_BIT_WIDTH_32BIT) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be 8, 16, 24, or 32"), MP_QSTR_bit_depth);
    }

    i2s_chan_config_t chanConfig = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    esp_err_t err = i2s_new_channel(&chanConfig, NULL, &self->rx_chan);
    CHECK_ESP_RESULT(err);

    i2s_pdm_rx_config_t pdm_rx_cfg = {
        .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(sample_rate),
        .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(bit_depth, mono ? I2S_SLOT_MODE_MONO : I2S_SLOT_MODE_STEREO),
        .gpio_cfg =
        {
            .clk = clock_pin->number,
            .din = data_pin->number,
            .invert_flags =
            {
                .clk_inv = false,
            },
        },
    };
    err = i2s_channel_init_pdm_rx_mode(self->rx_chan, &pdm_rx_cfg);
    if (err == ESP_OK) {
        err = i2s_channel_enable(self->rx_chan);
    }
    if (err != ESP_OK) {
        i2s_del_channel(self->rx_chan);
        self->rx_chan = NULL;
        CHECK_ESP_RESULT(err);
    }

    self->clock_pin = clock_pin;
    self->data_pin = data_pin;
    claim_pin(clock_pin);
    claim_pin(data_pin);

    self->sample_rate = sample_rate;
    self->bit_depth = bit_depth;
}

bool common_hal_audiobusio_pdmin_deinited(audiobusio_pdmin_obj_t *self) {
    return self->clock_pin == NULL;
}

void common_hal_audiobusio_pdmin_deinit(audiobusio_pdmin_obj_t *self) {
    if (common_hal_audiobusio_pdmin_deinited(self)) {
        return;
    }

    esp_err_t err = i2s_channel_disable(self->rx_chan);
    CHECK_ESP_RESULT(err);
    err = i2s_del_channel(self->rx_chan);
    CHECK_ESP_RESULT(err);

    // common_hal_audiobusio_i2sout_stop(self);

    if (self->clock_pin) {
        reset_pin_number(self->clock_pin->number);
    }
    self->clock_pin = NULL;

    if (self->data_pin) {
        reset_pin_number(self->data_pin->number);
    }
    self->data_pin = NULL;
}

/**
	`length` is the buffer element count, not the byte count.
*/

uint32_t common_hal_audiobusio_pdmin_record_to_buffer(audiobusio_pdmin_obj_t *self,
    uint16_t *buffer,
    uint32_t length) {
//      mp_printf(MP_PYTHON_PRINTER, "Copying bytes to buffer\n");

    size_t result = 0;
    size_t elementSize = common_hal_audiobusio_pdmin_get_bit_depth(self) / 8;
    esp_err_t err = i2s_channel_read(self->rx_chan, buffer, length * elementSize, &result, 100);
    CHECK_ESP_RESULT(err);
    return result;
}

uint8_t common_hal_audiobusio_pdmin_get_bit_depth(audiobusio_pdmin_obj_t *self) {
    return self->bit_depth;
}

uint32_t common_hal_audiobusio_pdmin_get_sample_rate(audiobusio_pdmin_obj_t *self) {
    return self->sample_rate;
}

#endif
