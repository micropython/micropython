// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>

#include "mpconfigport.h"

#include "bindings/espidf/__init__.h"

// Some boards don't implement I2SOut, so suppress any routines from here.
#if CIRCUITPY_AUDIOBUSIO_I2SOUT

#include "extmod/vfs_fat.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/audiobusio/I2SOut.h"
#include "shared-bindings/audiobusio/I2SOut.h"
#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "driver/i2s_std.h"

// Caller validates that pins are free.
void common_hal_audiobusio_i2sout_construct(audiobusio_i2sout_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, const mcu_pin_obj_t *main_clock, bool left_justified) {
    port_i2s_allocate_init(&self->i2s, left_justified);

    i2s_std_config_t i2s_config = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = main_clock != NULL ? main_clock->number : I2S_GPIO_UNUSED,
            .bclk = bit_clock->number,
            .ws = word_select->number,
            .dout = data->number,
            .din = I2S_GPIO_UNUSED,
        }
    };
    CHECK_ESP_RESULT(i2s_channel_init_std_mode(self->i2s.handle, &i2s_config));
    self->bit_clock = bit_clock;
    self->word_select = word_select;
    self->mclk = main_clock;
    self->data = data;
    claim_pin(bit_clock);
    claim_pin(word_select);
    if (main_clock) {
        claim_pin(main_clock);
    }
    claim_pin(data);
}

bool common_hal_audiobusio_i2sout_deinited(audiobusio_i2sout_obj_t *self) {
    return self->data == NULL;
}

void common_hal_audiobusio_i2sout_deinit(audiobusio_i2sout_obj_t *self) {
    if (common_hal_audiobusio_i2sout_deinited(self)) {
        return;
    }

    common_hal_audiobusio_i2sout_stop(self);

    port_i2s_deinit(&self->i2s);

    if (self->bit_clock) {
        reset_pin_number(self->bit_clock->number);
    }
    self->bit_clock = NULL;

    if (self->word_select) {
        reset_pin_number(self->word_select->number);
    }
    self->word_select = NULL;

    if (self->mclk) {
        reset_pin_number(self->mclk->number);
    }
    self->mclk = NULL;

    if (self->data) {
        reset_pin_number(self->data->number);
    }
    self->data = NULL;

}

void common_hal_audiobusio_i2sout_play(audiobusio_i2sout_obj_t *self,
    mp_obj_t sample, bool loop) {
    if (common_hal_audiobusio_i2sout_get_playing(self)) {
        common_hal_audiobusio_i2sout_stop(self);
    }
    port_i2s_play(&self->i2s, sample, loop);
}

void common_hal_audiobusio_i2sout_pause(audiobusio_i2sout_obj_t *self) {
    port_i2s_pause(&self->i2s);
}

void common_hal_audiobusio_i2sout_resume(audiobusio_i2sout_obj_t *self) {
    port_i2s_resume(&self->i2s);
}

bool common_hal_audiobusio_i2sout_get_paused(audiobusio_i2sout_obj_t *self) {
    return port_i2s_paused(&self->i2s);
}

void common_hal_audiobusio_i2sout_stop(audiobusio_i2sout_obj_t *self) {
    port_i2s_stop(&self->i2s);
}

bool common_hal_audiobusio_i2sout_get_playing(audiobusio_i2sout_obj_t *self) {
    return port_i2s_playing(&self->i2s);
}

#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT
