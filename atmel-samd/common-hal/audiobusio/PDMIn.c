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

#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/analogio/AnalogOut.h"
#include "common-hal/audiobusio/PDMIn.h"
#include "shared-bindings/analogio/AnalogOut.h"
#include "shared-bindings/audiobusio/PDMIn.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "asf/sam0/drivers/port/port.h"
#include "samd21_pins.h"

#include "shared_dma.h"
#include "tick.h"

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

    if (bit_depth != 16 || !mono || oversample != 64) {
        mp_raise_NotImplementedError("");
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
    config_clock_unit.clock.sck_div = 8000000 / frequency / oversample;
    self->frequency = 8000000 / config_clock_unit.clock.sck_div / oversample;

    config_clock_unit.frame.number_slots = 2;
    config_clock_unit.frame.slot_size = I2S_SLOT_SIZE_16_BIT;
    config_clock_unit.frame.data_delay = I2S_DATA_DELAY_1;

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

    self->bytes_per_sample = oversample >> 3;
    self->bit_depth = bit_depth;
}

void common_hal_audiobusio_pdmin_deinit(audiobusio_pdmin_obj_t* self) {
    i2s_disable(&self->i2s_instance);
    i2s_reset(&self->i2s_instance);
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

    if (length * words_per_sample > words_per_buffer) {
        block_transfer_count = words_per_buffer;
        descriptor_config.next_descriptor_address = ((uint32_t)audio_dma.descriptor);
        if (length * words_per_sample < 2 * words_per_buffer) {
            block_transfer_count = 2 * words_per_buffer - length * words_per_sample;
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
    i2s_clock_unit_enable(&self->i2s_instance, self->clock_unit);
    i2s_serializer_enable(&self->i2s_instance, self->serializer);
    I2S->DATA[1].reg = I2S->DATA[1].reg;
}

void stop_dma(audiobusio_pdmin_obj_t* self) {
    // Turn off the I2S clock and serializer. Peripheral is still enabled.
    i2s_serializer_disable(&self->i2s_instance, self->serializer);
    i2s_clock_unit_disable(&self->i2s_instance, self->clock_unit);

    // Shutdown the DMA
    tc_stop_counter(MP_STATE_VM(audiodma_block_counter));
    dma_abort_job(&audio_dma);
}

static const uint16_t sinc_filter[64] = {
    0, 1, 6, 16, 29, 49, 75, 108,
    149, 200, 261, 334, 418, 514, 622, 742,
    872, 1012, 1161, 1315, 1472, 1631, 1787, 1938,
    2081, 2212, 2329, 2429, 2509, 2568, 2604, 2616,
    2604, 2568, 2509, 2429, 2329, 2212, 2081, 1938,
    1787, 1631, 1472, 1315, 1161, 1012, 872, 742,
    622, 514, 418, 334, 261, 200, 149, 108,
    75, 49, 29, 16, 6, 1, 0, 0
};

static uint16_t filter_sample(uint32_t pdm_samples[4]) {
    uint16_t sample = 0;
    for (uint8_t i = 0; i < 4; i++) {
        uint16_t pdm = pdm_samples[i] & 0xffff;
        for (uint8_t j = 0; j < 16; j++) {
            if ((pdm & 0x8000) != 0) {
                sample += sinc_filter[i * 16 + j];
            }
            pdm <<= 1;
        }
    }
    return sample;
}

uint32_t common_hal_audiobusio_pdmin_record_to_buffer(audiobusio_pdmin_obj_t* self,
        uint16_t* output_buffer, uint32_t length) {
    // Write the wave file header.

    // We allocate two 256 byte buffers on the stack to use for double buffering.
    // Our oversample rate is 64 (bits) so each buffer produces 32 samples.
    // TODO(tannewt): Can the compiler optimize better if we fix the size of
    // these buffers?
    uint8_t samples_per_buffer = 32;
    // For every word we record, we throw away 2 bytes of a phantom second channel.
    uint8_t words_per_sample = self->bytes_per_sample / 2;
    uint8_t words_per_buffer = samples_per_buffer * words_per_sample;
    uint32_t first_buffer[words_per_buffer];
    uint32_t second_buffer[words_per_buffer];

    COMPILER_ALIGNED(16) DmacDescriptor second_descriptor;

    setup_dma(self, length, &second_descriptor, words_per_buffer,
       words_per_sample, first_buffer, second_buffer);

    start_dma(self);

    // Record
    uint32_t buffers_processed = 0;
    uint32_t total_bytes = 0;

    uint64_t start_ticks = ticks_ms;
    while (total_bytes < length) {
        // Wait for the next buffer to fill
        while (tc_get_count_value(MP_STATE_VM(audiodma_block_counter)) == buffers_processed) {
            #ifdef MICROPY_VM_HOOK_LOOP
                MICROPY_VM_HOOK_LOOP
            #endif
        }
        if (tc_get_count_value(MP_STATE_VM(audiodma_block_counter)) != (buffers_processed + 1)) {
            break;
        }
        // Throw away the first ~10ms of data because thats during mic start up.
        if (ticks_ms - start_ticks < 10) {
            buffers_processed++;
            continue;
        }
        uint32_t* buffer = first_buffer;
        DmacDescriptor* descriptor = audio_dma.descriptor;
        if (buffers_processed % 2 == 1) {
            buffer = second_buffer;
            descriptor = &second_descriptor;
        }
        // Decimate and filter the last buffer
        int32_t samples_gathered = descriptor->BTCNT.reg / words_per_sample;
        for (uint16_t i = 0; i < samples_gathered; i++) {
            if (self->bit_depth == 8) {
                ((uint8_t*) output_buffer)[total_bytes] = filter_sample(buffer + i * words_per_sample) >> 8;
                total_bytes += 1;
            } else if (self->bit_depth == 16) {
                output_buffer[total_bytes / 2] = filter_sample(buffer + i * words_per_sample);
                total_bytes += 2;
            }
        }
        buffers_processed++;

        if (length - total_bytes < samples_per_buffer) {
            descriptor->BTCNT.reg = (length - total_bytes) * words_per_sample;
            descriptor->DSTADDR.reg = ((uint32_t) buffer) + (length - total_bytes) * self->bytes_per_sample;
            descriptor->DESCADDR.reg = 0;
        }
    }

    stop_dma(self);

    return total_bytes;
}

void common_hal_audiobusio_pdmin_record_to_file(audiobusio_pdmin_obj_t* self, uint8_t* buffer, uint32_t length) {

}
