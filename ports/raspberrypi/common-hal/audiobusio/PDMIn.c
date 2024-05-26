// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>
#include <math.h>

#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared-bindings/audiobusio/PDMIn.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "audio_dma.h"

#define OVERSAMPLING 64
#define SAMPLES_PER_BUFFER 32

// MEMS microphones must be clocked at at least 1MHz.
#define MIN_MIC_CLOCK 1000000

const uint16_t pdmin[] = {
    // in pins 1        side 0b1
    0x5001,
    // push iffull      side 0b0
    0x8040
};

// Caller validates that pins are free.
void common_hal_audiobusio_pdmin_construct(audiobusio_pdmin_obj_t *self,
    const mcu_pin_obj_t *clock_pin,
    const mcu_pin_obj_t *data_pin,
    uint32_t sample_rate,
    uint8_t bit_depth,
    bool mono,
    uint8_t oversample) {
    if (!(bit_depth == 16 || bit_depth == 8) || !mono || oversample != OVERSAMPLING) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Only 8 or 16 bit mono with %dx oversampling supported."), OVERSAMPLING);
    }

    // Use the state machine to manage pins.
    common_hal_rp2pio_statemachine_construct(&self->state_machine,
        pdmin, MP_ARRAY_SIZE(pdmin),
        sample_rate * 32 * 2, // Frequency based on sample rate
        NULL, 0,
        NULL, 0, // may_exec
        NULL, 1, 0, 0xffffffff, // out pin
        data_pin, 1, // in pins
        0, 0, // in pulls
        NULL, 0, 0, 0x1f, // set pins
        clock_pin, 1, 0, 0x1f, // sideset pins
        false, // No sideset enable
        NULL, PULL_NONE, // jump pin
        0, // wait gpio pins
        true, // exclusive pin use
        false, 32, false, // out settings
        false, // Wait for txstall
        false, 32, true, // in settings
        false, // Not user-interruptible.
        0, -1, // wrap settings
        PIO_ANY_OFFSET);
    uint32_t actual_frequency = common_hal_rp2pio_statemachine_get_frequency(&self->state_machine);
    if (actual_frequency < MIN_MIC_CLOCK) {
        mp_raise_ValueError(MP_ERROR_TEXT("sampling rate out of range"));
    }

    self->sample_rate = actual_frequency / oversample;
    self->bit_depth = bit_depth;
}

bool common_hal_audiobusio_pdmin_deinited(audiobusio_pdmin_obj_t *self) {
    return common_hal_rp2pio_statemachine_deinited(&self->state_machine);
}

void common_hal_audiobusio_pdmin_deinit(audiobusio_pdmin_obj_t *self) {
    if (common_hal_audiobusio_pdmin_deinited(self)) {
        return;
    }
    return common_hal_rp2pio_statemachine_deinit(&self->state_machine);
}

uint8_t common_hal_audiobusio_pdmin_get_bit_depth(audiobusio_pdmin_obj_t *self) {
    return self->bit_depth;
}

uint32_t common_hal_audiobusio_pdmin_get_sample_rate(audiobusio_pdmin_obj_t *self) {
    return self->sample_rate;
}

// a windowed sinc filter for 44 khz, 64 samples
//
// This filter is good enough to use for lower sample rates as
// well. It does not increase the noise enough to be a problem.
//
// In the long run we could use a fast filter like this to do the
// decimation and initial filtering in real time, filtering to a
// higher sample rate than specified.  Then after the audio is
// recorded, a more expensive filter non-real-time filter could be
// used to down-sample and low-pass.
const uint16_t sinc_filter[OVERSAMPLING] = {
    0, 2, 9, 21, 39, 63, 94, 132,
    179, 236, 302, 379, 467, 565, 674, 792,
    920, 1055, 1196, 1341, 1487, 1633, 1776, 1913,
    2042, 2159, 2263, 2352, 2422, 2474, 2506, 2516,
    2506, 2474, 2422, 2352, 2263, 2159, 2042, 1913,
    1776, 1633, 1487, 1341, 1196, 1055, 920, 792,
    674, 565, 467, 379, 302, 236, 179, 132,
    94, 63, 39, 21, 9, 2, 0, 0
};

#define REPEAT_32_TIMES(X) do { X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X } while (0)

static uint16_t filter_sample(uint32_t pdm_samples[2]) {
    uint16_t running_sum = 0;
    const uint16_t *filter_ptr = sinc_filter;
    for (uint8_t i = 0; i < 2; i++) {
        uint32_t pdm_sample = pdm_samples[i];
        REPEAT_32_TIMES({
            if (pdm_sample & 0x1) {
                running_sum += *filter_ptr;
            }
            filter_ptr++;
            pdm_sample >>= 1;
        }
            );
    }
    return running_sum;
}

// output_buffer may be a byte buffer or a halfword buffer.
// output_buffer_length is the number of slots, not the number of bytes.
uint32_t common_hal_audiobusio_pdmin_record_to_buffer(audiobusio_pdmin_obj_t *self,
    uint16_t *output_buffer, uint32_t output_buffer_length) {
    uint32_t samples[2];
    size_t output_count = 0;
    common_hal_rp2pio_statemachine_clear_rxfifo(&self->state_machine);
    // Do one read to get the mic going and throw it away.
    common_hal_rp2pio_statemachine_readinto(&self->state_machine, (uint8_t *)samples, 2 * sizeof(uint32_t), sizeof(uint32_t), false);
    while (output_count < output_buffer_length && !common_hal_rp2pio_statemachine_get_rxstall(&self->state_machine)) {
        common_hal_rp2pio_statemachine_readinto(&self->state_machine, (uint8_t *)samples, 2 * sizeof(uint32_t), sizeof(uint32_t), false);
        // Call filter_sample just one place so it can be inlined.
        uint16_t value = filter_sample(samples);
        if (self->bit_depth == 8) {
            // Truncate to 8 bits.
            ((uint8_t *)output_buffer)[output_count] = value >> 8;
        } else {
            output_buffer[output_count] = value;
        }
        output_count++;
    }

    return output_count;
}
