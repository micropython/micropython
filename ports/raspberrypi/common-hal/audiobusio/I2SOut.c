/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>
#include <string.h>

#include "mpconfigport.h"

#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/audiobusio/I2SOut.h"
#include "shared-bindings/audiobusio/I2SOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/audiocore/__init__.h"
#include "bindings/rp2pio/StateMachine.h"
#include "supervisor/shared/translate.h"

const uint16_t i2s_program[] = {
// ; Load the next set of samples
//                     ;        /--- LRCLK
//                     ;        |/-- BCLK
//                     ;        ||
//     pull noblock      side 0b01 ; Loads OSR with the next FIFO value or X
    0x8880,
//     mov x osr         side 0b01 ; Save the new value in case we need it again
    0xa827,
//     set y 14          side 0b01
    0xe84e,
// bitloop1:
//     out pins 1        side 0b00 [2]
    0x6201,
//     jmp y-- bitloop1  side 0b01 [2]
    0x0a83,
//     out pins 1        side 0b10 [2]
    0x7201,
//     set y 14          side 0b11 [2]
    0xfa4e,
// bitloop0:
//     out pins 1        side 0b10 [2]
    0x7201,
//     jmp y-- bitloop0  side 0b11 [2]
    0x1a87,
//     out pins 1        side 0b00 [2]
    0x6201
};

const uint16_t i2s_program_left_justified[] = {
// ; Load the next set of samples
//                     ;        /--- LRCLK
//                     ;        |/-- BCLK
//                     ;        ||
//     pull noblock      side 0b11 ; Loads OSR with the next FIFO value or X
    0x9880,
//     mov x osr         side 0b11 ; Save the new value in case we need it again
    0xb827,
//     set y 14          side 0b11
    0xf84e,
// bitloop1:
//     out pins 1        side 0b00 [2]
    0x6201,
//     jmp y-- bitloop1  side 0b01 [2]
    0x0a83,
//     out pins 1        side 0b10 [2]
    0x6201,
//     set y 14          side 0b01 [2]
    0xea4e,
// bitloop0:
//     out pins 1        side 0b10 [2]
    0x7201,
//     jmp y-- bitloop0  side 0b11 [2]
    0x1a87,
//     out pins 1        side 0b10 [2]
    0x7201
};

void i2sout_reset(void) {
}

// Caller validates that pins are free.
void common_hal_audiobusio_i2sout_construct(audiobusio_i2sout_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, bool left_justified) {
    if (bit_clock->number != word_select->number - 1) {
        mp_raise_ValueError(translate("Bit clock and word select must be sequential pins"));
    }

    const uint16_t *program = i2s_program;
    size_t program_len = sizeof(i2s_program) / sizeof(i2s_program[0]);
    if (left_justified) {
        program = i2s_program_left_justified;
        program_len = sizeof(i2s_program_left_justified) / sizeof(i2s_program_left_justified[0]);
        ;
    }

    // Use the state machine to manage pins.
    common_hal_rp2pio_statemachine_construct(&self->state_machine,
        program, program_len,
        44100 * 32 * 6, // Clock at 44.1 khz to warm the DAC up.
        NULL, 0,
        data, 1, 0, 0xffffffff, // out pin
        NULL, 0, // in pins
        0, 0, // in pulls
        NULL, 0, 0, 0x1f, // set pins
        bit_clock, 2, 0, 0x1f, // sideset pins
        0, // wait gpio pins
        true, // exclusive pin use
        false, 32, false, // shift out left to start with MSB
        false, // Wait for txstall
        false, 32, false); // in settings

    self->playing = false;
    audio_dma_init(&self->dma);
}

bool common_hal_audiobusio_i2sout_deinited(audiobusio_i2sout_obj_t *self) {
    return common_hal_rp2pio_statemachine_deinited(&self->state_machine);
}

void common_hal_audiobusio_i2sout_deinit(audiobusio_i2sout_obj_t *self) {
    if (common_hal_audiobusio_i2sout_deinited(self)) {
        return;
    }

    if (common_hal_audiobusio_i2sout_get_playing(self)) {
        common_hal_audiobusio_i2sout_stop(self);
    }

    common_hal_rp2pio_statemachine_deinit(&self->state_machine);

    audio_dma_deinit(&self->dma);
}

void common_hal_audiobusio_i2sout_play(audiobusio_i2sout_obj_t *self,
    mp_obj_t sample, bool loop) {
    if (common_hal_audiobusio_i2sout_get_playing(self)) {
        common_hal_audiobusio_i2sout_stop(self);
    }

    uint8_t bits_per_sample = audiosample_bits_per_sample(sample);
    // Make sure we transmit a minimum of 16 bits.
    // TODO: Maybe we need an intermediate object to upsample instead. This is
    // only needed for some I2S devices that expect at least 8.
    if (bits_per_sample < 16) {
        bits_per_sample = 16;
    }
    // We always output stereo so output twice as many bits.
    uint16_t bits_per_sample_output = bits_per_sample * 2;
    size_t clocks_per_bit = 6;
    uint32_t frequency = bits_per_sample_output * audiosample_sample_rate(sample);
    common_hal_rp2pio_statemachine_set_frequency(&self->state_machine, clocks_per_bit * frequency);
    common_hal_rp2pio_statemachine_restart(&self->state_machine);

    uint8_t channel_count = audiosample_channel_count(sample);
    if (channel_count > 2) {
        mp_raise_ValueError(translate("Too many channels in sample."));
    }

    audio_dma_result result = audio_dma_setup_playback(
        &self->dma,
        sample,
        loop,
        false, // single channel
        0, // audio channel
        true,  // output signed
        bits_per_sample,
        (uint32_t)&self->state_machine.pio->txf[self->state_machine.state_machine],  // output register
        self->state_machine.tx_dreq); // data request line

    if (result == AUDIO_DMA_DMA_BUSY) {
        common_hal_audiobusio_i2sout_stop(self);
        mp_raise_RuntimeError(translate("No DMA channel found"));
    } else if (result == AUDIO_DMA_MEMORY_ERROR) {
        common_hal_audiobusio_i2sout_stop(self);
        mp_raise_RuntimeError(translate("Unable to allocate buffers for signed conversion"));
    }

    // Turn on the state machine's clock.

    self->playing = true;
}

void common_hal_audiobusio_i2sout_pause(audiobusio_i2sout_obj_t *self) {
    audio_dma_pause(&self->dma);
}

void common_hal_audiobusio_i2sout_resume(audiobusio_i2sout_obj_t *self) {
    // Maybe: Clear any overrun/underrun errors

    audio_dma_resume(&self->dma);
}

bool common_hal_audiobusio_i2sout_get_paused(audiobusio_i2sout_obj_t *self) {
    return audio_dma_get_paused(&self->dma);
}

void common_hal_audiobusio_i2sout_stop(audiobusio_i2sout_obj_t *self) {
    audio_dma_stop(&self->dma);

    common_hal_rp2pio_statemachine_stop(&self->state_machine);

    self->playing = false;
}

bool common_hal_audiobusio_i2sout_get_playing(audiobusio_i2sout_obj_t *self) {
    bool playing = audio_dma_get_playing(&self->dma);
    if (!playing && self->playing) {
        common_hal_audiobusio_i2sout_stop(self);
    }
    return playing;
}
