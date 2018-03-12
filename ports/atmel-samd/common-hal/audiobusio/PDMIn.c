/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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
#include <math.h>

#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/analogio/AnalogOut.h"
#include "common-hal/audiobusio/PDMIn.h"
#include "shared-bindings/analogio/AnalogOut.h"
#include "shared-bindings/audiobusio/PDMIn.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "samd21_pins.h"

#include "shared_dma.h"
#include "tick.h"

#define OVERSAMPLING 64
#define SAMPLES_PER_BUFFER 32

// MEMS microphones must be clocked at at least 1MHz.
#define MIN_MIC_CLOCK 1000000

void pdmin_reset(void) {
    while (I2S->SYNCBUSY.reg & I2S_SYNCBUSY_ENABLE) {}
    I2S->INTENCLR.reg = I2S_INTENCLR_MASK;
    I2S->INTFLAG.reg = I2S_INTFLAG_MASK;
    I2S->CTRLA.reg &= ~I2S_SYNCBUSY_ENABLE;
    while (I2S->SYNCBUSY.reg & I2S_SYNCBUSY_ENABLE) {}
    I2S->CTRLA.reg = I2S_CTRLA_SWRST;
}

void common_hal_audiobusio_pdmin_construct(audiobusio_pdmin_obj_t* self,
                                           const mcu_pin_obj_t* clock_pin,
                                           const mcu_pin_obj_t* data_pin,
                                           uint32_t frequency,
                                           uint8_t bit_depth,
                                           bool mono,
                                           uint8_t oversample) {
    self->clock_pin = clock_pin; // PA10, PA20 -> SCK0, PB11 -> SCK1
    if (clock_pin == &pin_PA10
    #ifdef PIN_PA20
        || clock_pin == &pin_PA20
    #endif
        ) {
        self->clock_unit = 0;
    #ifdef PIN_PB11
    } else if (clock_pin == &pin_PB11) {
        self->clock_unit = 1;
    #endif
    } else {
        mp_raise_ValueError("Invalid clock pin");
    }

    self->data_pin = data_pin; // PA07, PA19 -> SD0, PA08, PB16 -> SD1

    if (data_pin == &pin_PA07 || data_pin == &pin_PA19) {
        self->serializer = 0;
    } else if (data_pin == &pin_PA08
        #ifdef PB16
        || data_pin == &pin_PB16) {
        #else
        ) {
        #endif
        self->serializer = 1;
    } else {
        mp_raise_ValueError("Invalid data pin");
    }

    claim_pin(clock_pin);
    claim_pin(data_pin);

    if (MP_STATE_VM(audiodma_block_counter) == NULL &&
        !allocate_block_counter()) {
        mp_raise_RuntimeError("Unable to allocate audio DMA block counter.");
    }

    if (!(bit_depth == 16 || bit_depth == 8) || !mono || oversample != OVERSAMPLING) {
        mp_raise_NotImplementedError("Only 8 or 16 bit mono with " MP_STRINGIFY(OVERSAMPLING) "x oversampling is supported.");
    }

    // TODO(tannewt): Use the DPLL to get a more precise sampling rate.
    // DFLL -> GCLK (/600 for 8khz, /300 for 16khz and /150 for 32khz) -> DPLL (*(63 + 1)) -> GCLK ( / 10) -> 512khz

    i2s_init(&self->i2s_instance, I2S);
    struct i2s_clock_unit_config config_clock_unit;
    i2s_clock_unit_get_config_defaults(&config_clock_unit);
    config_clock_unit.clock.gclk_src = GCLK_GENERATOR_3;

    config_clock_unit.clock.mck_src = I2S_MASTER_CLOCK_SOURCE_GCLK;
    config_clock_unit.clock.mck_out_enable = false;

    config_clock_unit.clock.sck_src = I2S_SERIAL_CLOCK_SOURCE_MCKDIV;
    uint32_t clock_divisor = (uint32_t) roundf( 8000000.0f / frequency / oversample);
    config_clock_unit.clock.sck_div = clock_divisor;
    float mic_clock_freq = 8000000.0f / clock_divisor;
    self->frequency =  mic_clock_freq / oversample;
    if (mic_clock_freq <  MIN_MIC_CLOCK || clock_divisor == 0 || clock_divisor > 255) {
        mp_raise_ValueError("sampling frequency out of range");
    }

    config_clock_unit.frame.number_slots = 2;
    config_clock_unit.frame.slot_size = I2S_SLOT_SIZE_16_BIT;
    config_clock_unit.frame.data_delay = I2S_DATA_DELAY_0;

    config_clock_unit.frame.frame_sync.width = I2S_FRAME_SYNC_WIDTH_SLOT;

    config_clock_unit.mck_pin.enable = false;
    config_clock_unit.sck_pin.enable = true;
    config_clock_unit.sck_pin.gpio = self->clock_pin->pin;
    // Mux is always the same.
    config_clock_unit.sck_pin.mux = 6L;
    config_clock_unit.fs_pin.enable = false;
    i2s_clock_unit_set_config(&self->i2s_instance, self->clock_unit, &config_clock_unit);

    struct i2s_serializer_config config_serializer;
    i2s_serializer_get_config_defaults(&config_serializer);
    config_serializer.clock_unit = self->clock_unit;
    config_serializer.mode = I2S_SERIALIZER_PDM2;
    config_serializer.data_size = I2S_DATA_SIZE_32BIT;
    config_serializer.data_pin.gpio = self->data_pin->pin;
    // Mux is always the same.
    config_serializer.data_pin.mux = 6L;
    config_serializer.data_pin.enable = true;
    i2s_serializer_set_config(&self->i2s_instance, self->serializer, &config_serializer);
    i2s_enable(&self->i2s_instance);

    // Run the serializer all the time. This eliminates startup delay for the microphone.
    i2s_clock_unit_enable(&self->i2s_instance, self->clock_unit);
    i2s_serializer_enable(&self->i2s_instance, self->serializer);

    self->bytes_per_sample = oversample >> 3;
    self->bit_depth = bit_depth;
}

bool common_hal_audiobusio_pdmin_deinited(audiobusio_pdmin_obj_t* self) {
    return self->clock_pin == mp_const_none;
}

void common_hal_audiobusio_pdmin_deinit(audiobusio_pdmin_obj_t* self) {
    if (common_hal_audiobusio_pdmin_deinited(self)) {
        return;
    }
    i2s_disable(&self->i2s_instance);
    i2s_serializer_disable(&self->i2s_instance, self->serializer);
    i2s_clock_unit_disable(&self->i2s_instance, self->clock_unit);
    i2s_reset(&self->i2s_instance);
    reset_pin(self->clock_pin->pin);
    reset_pin(self->data_pin->pin);
    self->clock_pin = mp_const_none;
    self->data_pin = mp_const_none;
}

uint8_t common_hal_audiobusio_pdmin_get_bit_depth(audiobusio_pdmin_obj_t* self) {
    return self->bit_depth;
}

uint32_t common_hal_audiobusio_pdmin_get_frequency(audiobusio_pdmin_obj_t* self) {
    return self->frequency;
}

static void setup_dma(audiobusio_pdmin_obj_t* self, uint32_t length,
        DmacDescriptor* second_descriptor,
        uint8_t words_per_buffer, uint8_t words_per_sample,
        uint32_t* first_buffer, uint32_t* second_buffer) {
    // Set up the DMA
    struct dma_descriptor_config descriptor_config;
    dma_descriptor_get_config_defaults(&descriptor_config);
    descriptor_config.beat_size = DMA_BEAT_SIZE_WORD;
    descriptor_config.step_selection = DMA_STEPSEL_SRC;
    descriptor_config.source_address = (uint32_t)&I2S->DATA[self->serializer];
    descriptor_config.src_increment_enable = false;
    // Block transfer count is the number of beats per block (aka descriptor).
    // In this case there are two bytes per beat so divide the length by two.
    uint16_t block_transfer_count = words_per_buffer;
    if (length * words_per_sample < words_per_buffer) {
        block_transfer_count = length * words_per_sample;
    }
    descriptor_config.block_transfer_count = block_transfer_count;
    descriptor_config.destination_address = ((uint32_t) first_buffer + sizeof(uint32_t) * block_transfer_count);
    descriptor_config.event_output_selection = DMA_EVENT_OUTPUT_BLOCK;
    descriptor_config.next_descriptor_address = 0;
    if (length * words_per_sample > words_per_buffer) {
        descriptor_config.next_descriptor_address = ((uint32_t)second_descriptor);
    }
    dma_descriptor_create(audio_dma.descriptor, &descriptor_config);

    // Do we need more values than will fit in the first buffer?
    // If so, set up a second buffer chained to be filled after the first buffer.
    if (length * words_per_sample > words_per_buffer) {
        block_transfer_count = words_per_buffer;
        descriptor_config.next_descriptor_address = ((uint32_t)audio_dma.descriptor);
        if (length * words_per_sample < 2 * words_per_buffer) {
            // Length needed is more than one buffer but less than two.
            // Subtract off the size of the first buffer, and what remains is the count we need.
            block_transfer_count = length * words_per_sample - words_per_buffer;
            descriptor_config.next_descriptor_address = 0;
        }
        descriptor_config.block_transfer_count = block_transfer_count;
        descriptor_config.destination_address = ((uint32_t) second_buffer + sizeof(uint32_t) * block_transfer_count);
        dma_descriptor_create(second_descriptor, &descriptor_config);
    }

    switch_audiodma_trigger(I2S_DMAC_ID_RX_0 + self->serializer);
}

void start_dma(audiobusio_pdmin_obj_t* self) {
    dma_start_transfer_job(&audio_dma);
    tc_start_counter(MP_STATE_VM(audiodma_block_counter));
    I2S->DATA[1].reg = I2S->DATA[1].reg;
}

void stop_dma(audiobusio_pdmin_obj_t* self) {
    // Shutdown the DMA: serializer keeps running.
    tc_stop_counter(MP_STATE_VM(audiodma_block_counter));
    dma_abort_job(&audio_dma);
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
uint16_t sinc_filter [OVERSAMPLING] = {
    0, 2, 9, 21, 39, 63, 94, 132,
    179, 236, 302, 379, 467, 565, 674, 792,
    920, 1055, 1196, 1341, 1487, 1633, 1776, 1913,
    2042, 2159, 2263, 2352, 2422, 2474, 2506, 2516,
    2506, 2474, 2422, 2352, 2263, 2159, 2042, 1913,
    1776, 1633, 1487, 1341, 1196, 1055, 920, 792,
    674, 565, 467, 379, 302, 236, 179, 132,
    94, 63, 39, 21, 9, 2, 0, 0
};

#define REPEAT_16_TIMES(X) X X X X X X X X X X X X X X X X

static uint16_t filter_sample(uint32_t pdm_samples[4]) {
    uint16_t running_sum = 0;
    const uint16_t *filter_ptr = sinc_filter;
    for (uint8_t i = 0; i < OVERSAMPLING/16; i++) {
        // The sample is 16-bits right channel in the upper two bytes and 16-bits left channel
        // in the lower two bytes.
        // We just ignore the upper bits
        uint32_t pdm_sample = pdm_samples[i];
        REPEAT_16_TIMES( {
                if (pdm_sample & 0x8000) {
                    running_sum += *filter_ptr;
                }
                filter_ptr++;
                pdm_sample <<= 1;
            }
            )
    }
    return running_sum;
}

// output_buffer may be a byte buffer or a halfword buffer.
// output_buffer_length is the number of slots, not the number of bytes.
uint32_t common_hal_audiobusio_pdmin_record_to_buffer(audiobusio_pdmin_obj_t* self,
        uint16_t* output_buffer, uint32_t output_buffer_length) {
    // We allocate two buffers on the stack to use for double buffering.
    const uint8_t samples_per_buffer = SAMPLES_PER_BUFFER;
    // For every word we record, we throw away 2 bytes of a phantom second channel.
    const uint8_t words_per_sample = self->bytes_per_sample / 2;
    const uint8_t words_per_buffer = samples_per_buffer * words_per_sample;
    uint32_t first_buffer[words_per_buffer];
    uint32_t second_buffer[words_per_buffer];

    COMPILER_ALIGNED(16) DmacDescriptor second_descriptor;

    setup_dma(self, output_buffer_length, &second_descriptor, words_per_buffer,
       words_per_sample, first_buffer, second_buffer);

    start_dma(self);

    // Record
    uint32_t buffers_processed = 0;
    uint32_t values_output = 0;

    uint32_t remaining_samples_needed = output_buffer_length;
    while (values_output < output_buffer_length) {
        // Wait for the next buffer to fill
        uint32_t block_counter;
        while ((block_counter = tc_get_count_value(MP_STATE_VM(audiodma_block_counter))) == buffers_processed) {
            #ifdef MICROPY_VM_HOOK_LOOP
                MICROPY_VM_HOOK_LOOP
            #endif
        }
        if (block_counter != (buffers_processed + 1)) {
            // Looks like we aren't keeping up. We shouldn't skip a buffer.
            break;
        }

        // The mic is running all the time, so we don't need to wait the usual 10msec or 100msec
        // for it to start up.

        // Flip back and forth between processing the first and second buffers.
        uint32_t *buffer = first_buffer;
        DmacDescriptor* descriptor = audio_dma.descriptor;
        if (buffers_processed % 2 == 1) {
            buffer = second_buffer;
            descriptor = &second_descriptor;
        }
        // Decimate and filter the buffer that was just filled.
        uint32_t samples_gathered = descriptor->BTCNT.reg / words_per_sample;
        // Don't run off the end of output buffer. Process only as many as needed.
        uint32_t samples_to_process = min(remaining_samples_needed, samples_gathered);
        for (uint32_t i = 0; i < samples_to_process; i++) {
            // Call filter_sample just one place so it can be inlined.
            uint16_t value = filter_sample(buffer + i * words_per_sample);
            if (self->bit_depth == 8) {
                // Truncate to 8 bits.
                ((uint8_t*) output_buffer)[values_output] = value >> 8;
            } else {
                output_buffer[values_output] = value;
            }
            values_output++;
        }

        buffers_processed++;

        // Compute how many more samples we need, and if the last buffer is the last
        // set of samples needed, adjust the DMA count to only fetch as necessary.
        remaining_samples_needed = output_buffer_length - values_output;
        if (remaining_samples_needed <= samples_per_buffer*2 &&
            remaining_samples_needed > samples_per_buffer) {
            // Adjust the DMA settings for the current buffer, which will be processed
            // after the other buffer, which is now receiving samples via DMA.
            // We don't adjust the DMA in progress, but the one after that.
            // Timeline:
            // 1. current buffer (already processed)
            // 2. alternate buffer (DMA in progress)
            // 3. current buffer (last set of samples needed)

            // Set up to receive the last set of samples (don't include the alternate buffer, now in use).
            uint32_t samples_needed_for_last_buffer = remaining_samples_needed - samples_per_buffer;
            descriptor->BTCNT.reg = samples_needed_for_last_buffer * words_per_sample;
            descriptor->DSTADDR.reg = ((uint32_t) buffer)
                + samples_needed_for_last_buffer * words_per_sample * sizeof(buffer[0]);

            // Break chain to alternate buffer.
            descriptor->DESCADDR.reg = 0;
        }
    }

    stop_dma(self);

    return values_output;
}

void common_hal_audiobusio_pdmin_record_to_file(audiobusio_pdmin_obj_t* self, uint8_t* buffer, uint32_t length) {

}
