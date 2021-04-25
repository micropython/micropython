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
#include "supervisor/shared/translate.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"
#include "hal/utils/include/utils.h"

#include "samd/clocks.h"
#include "samd/events.h"
#include "samd/i2s.h"
#include "samd/pins.h"
#include "samd/dma.h"

#include "audio_dma.h"

#define OVERSAMPLING 64
#define SAMPLES_PER_BUFFER 32

// MEMS microphones must be clocked at at least 1MHz.
#define MIN_MIC_CLOCK 1000000

#ifdef SAMD21
#define SERCTRL(name) I2S_SERCTRL_ ## name
#endif

#ifdef SAM_D5X_E5X
#define SERCTRL(name) I2S_RXCTRL_ ## name
#endif

void pdmin_reset(void) {
    while (I2S->SYNCBUSY.reg & I2S_SYNCBUSY_ENABLE) {}
    I2S->INTENCLR.reg = I2S_INTENCLR_MASK;
    I2S->INTFLAG.reg = I2S_INTFLAG_MASK;
    I2S->CTRLA.reg &= ~I2S_SYNCBUSY_ENABLE;
    while (I2S->SYNCBUSY.reg & I2S_SYNCBUSY_ENABLE) {}
    I2S->CTRLA.reg = I2S_CTRLA_SWRST;
}

// Caller validates that pins are free.
void common_hal_audiobusio_pdmin_construct(audiobusio_pdmin_obj_t* self,
                                           const mcu_pin_obj_t* clock_pin,
                                           const mcu_pin_obj_t* data_pin,
                                           uint32_t sample_rate,
                                           uint8_t bit_depth,
                                           bool mono,
                                           uint8_t oversample) {
    self->clock_pin = clock_pin; // PA10, PA20 -> SCK0, PB11 -> SCK1
    #ifdef SAMD21
        if (clock_pin == &pin_PA10
        #if defined(PIN_PA20) && !defined(IGNORE_PIN_PA20)
            || clock_pin == &pin_PA20
        #endif
            ) {
            self->clock_unit = 0;
        #if defined(PIN_PB11) && !defined(IGNORE_PIN_PB11)
        } else if (clock_pin == &pin_PB11) {
            self->clock_unit = 1;
        #endif
    #endif
    #ifdef SAM_D5X_E5X
        if (clock_pin == &pin_PA10 || clock_pin == &pin_PB16) {
            self->clock_unit = 0;
    } else if (clock_pin == &pin_PB12
        #if defined(PIN_PB28) && !defined(IGNORE_PIN_PB28)
        || data_pin == &pin_PB28) {
        #else
        ) {
        #endif
            self->clock_unit = 1;
    #endif
    } else {
        mp_raise_ValueError_varg(translate("Invalid %q pin"), MP_QSTR_clock);
    }

    self->data_pin = data_pin; // PA07, PA19 -> SD0, PA08, PB16 -> SD1

    #ifdef SAMD21
    if (false
        #if defined(PIN_PA07) && !defined(IGNORE_PIN_PA07)
        || data_pin == &pin_PA07
        #endif
        #if defined(PIN_PA19) && !defined(IGNORE_PIN_PA19)
        || data_pin == &pin_PA19
        #endif
        ) {
        self->serializer = 0;
    }
    else if (false
        #if defined(PIN_PA08) && !defined(IGNORE_PIN_PA08)
        || data_pin == &pin_PA08
        #endif
        #if defined (PIN_PB16) && !defined(IGNORE_PIN_PB16)
        || data_pin == &pin_PB16
       #endif
        ) {
        self->serializer = 1;
    #endif
    #ifdef SAM_D5X_E5X
    if (data_pin == &pin_PB10 || data_pin == &pin_PA22) {
        self->serializer = 1;
    #endif
    } else {
        mp_raise_ValueError_varg(translate("Invalid %q pin"), MP_QSTR_data);
    }

    if (!(bit_depth == 16 || bit_depth == 8) || !mono || oversample != OVERSAMPLING) {
        mp_raise_NotImplementedError(translate("Only 8 or 16 bit mono with " MP_STRINGIFY(OVERSAMPLING) "x oversampling is supported."));
    }

    turn_on_i2s();

    if (I2S->CTRLA.bit.ENABLE == 0) {
        I2S->CTRLA.bit.SWRST = 1;
        while (I2S->CTRLA.bit.SWRST == 1) {}
    } else {
        #ifdef SAMD21
        if ((I2S->CTRLA.vec.SEREN & (1 << self->serializer)) != 0) {
            mp_raise_RuntimeError(translate("Serializer in use"));
        }
        #endif
        #ifdef SAM_D5X_E5X
        if (I2S->CTRLA.bit.RXEN == 1) {
            mp_raise_RuntimeError(translate("Serializer in use"));
        }
        #endif
    }
    #ifdef SAM_D5X_E5X
    #define GPIO_I2S_FUNCTION GPIO_PIN_FUNCTION_J
    #endif
    #ifdef SAMD21
    #define GPIO_I2S_FUNCTION GPIO_PIN_FUNCTION_G
    #endif

    uint32_t clock_divisor = (uint32_t) roundf( 48000000.0f / sample_rate / oversample);
    float mic_clock_freq = 48000000.0f / clock_divisor;
    self->sample_rate =  mic_clock_freq / oversample;
    if (mic_clock_freq <  MIN_MIC_CLOCK || clock_divisor == 0) {
        mp_raise_ValueError(translate("sampling rate out of range"));
    }
    // Find a free GCLK to generate the MCLK signal.
    uint8_t gclk = find_free_gclk(clock_divisor);
    if (gclk > GCLK_GEN_NUM) {
        mp_raise_RuntimeError(translate("Unable to find free GCLK"));
    }
    self->gclk = gclk;

    enable_clock_generator(self->gclk, CLOCK_48MHZ, clock_divisor);
    connect_gclk_to_peripheral(self->gclk, I2S_GCLK_ID_0 + self->clock_unit);

    // Clock unit configuration

    uint32_t clkctrl = I2S_CLKCTRL_MCKSEL_GCLK |
                       I2S_CLKCTRL_NBSLOTS(2) |
                       I2S_CLKCTRL_FSWIDTH_SLOT |
                       I2S_CLKCTRL_SLOTSIZE_16;

    // Serializer configuration
    #ifdef SAMD21
    uint32_t serctrl = (self->clock_unit << I2S_SERCTRL_CLKSEL_Pos) | SERCTRL(SERMODE_PDM2) | SERCTRL(DATASIZE_32);
    #endif
    #ifdef SAM_D5X_E5X
    uint32_t serctrl = (self->clock_unit << I2S_RXCTRL_CLKSEL_Pos) | SERCTRL(SERMODE_PDM2) | SERCTRL(DATASIZE_32);
    #endif

    // Configure the I2S peripheral
    i2s_set_enable(false);

    I2S->CLKCTRL[self->clock_unit].reg = clkctrl;
    #ifdef SAMD21
    I2S->SERCTRL[self->serializer].reg = serctrl;
    #endif
    #ifdef SAM_D5X_E5X
    I2S->RXCTRL.reg = serctrl;
    #endif

    i2s_set_enable(true);

    // Run the clock all the time. This eliminates startup delay for the microphone,
    // which can be 10-100ms. Turn serializer on as needed.
    i2s_set_clock_unit_enable(self->clock_unit, true);

    claim_pin(clock_pin);
    claim_pin(data_pin);

    gpio_set_pin_function(self->clock_pin->number, GPIO_I2S_FUNCTION);
    gpio_set_pin_function(self->data_pin->number, GPIO_I2S_FUNCTION);

    self->bytes_per_sample = oversample >> 3;
    self->bit_depth = bit_depth;
}

bool common_hal_audiobusio_pdmin_deinited(audiobusio_pdmin_obj_t* self) {
    return self->clock_pin == NULL;
}

void common_hal_audiobusio_pdmin_deinit(audiobusio_pdmin_obj_t* self) {
    if (common_hal_audiobusio_pdmin_deinited(self)) {
        return;
    }

    i2s_set_serializer_enable(self->serializer, false);
    i2s_set_clock_unit_enable(self->clock_unit, false);

    i2s_set_enable(false);

    disconnect_gclk_from_peripheral(self->gclk, I2S_GCLK_ID_0 + self->clock_unit);
    disable_clock_generator(self->gclk);

    reset_pin_number(self->clock_pin->number);
    reset_pin_number(self->data_pin->number);
    self->clock_pin = NULL;
    self->data_pin = NULL;
}

uint8_t common_hal_audiobusio_pdmin_get_bit_depth(audiobusio_pdmin_obj_t* self) {
    return self->bit_depth;
}

uint32_t common_hal_audiobusio_pdmin_get_sample_rate(audiobusio_pdmin_obj_t* self) {
    return self->sample_rate;
}

static void setup_dma(audiobusio_pdmin_obj_t* self, uint32_t length,
                      DmacDescriptor* descriptor,
                      DmacDescriptor* second_descriptor,
                      uint32_t words_per_buffer, uint8_t words_per_sample,
                      uint32_t* first_buffer, uint32_t* second_buffer) {
    descriptor->BTCTRL.reg = DMAC_BTCTRL_VALID |
                             DMAC_BTCTRL_BLOCKACT_NOACT |
                             DMAC_BTCTRL_EVOSEL_BLOCK |
                             DMAC_BTCTRL_DSTINC |
                             DMAC_BTCTRL_BEATSIZE_WORD;

    // Block transfer count is the number of beats per block (aka descriptor).
    // In this case there are two bytes per beat so divide the length by two.
    uint16_t block_transfer_count = words_per_buffer;
    if (length * words_per_sample < words_per_buffer) {
        block_transfer_count = length * words_per_sample;
    }

    descriptor->BTCNT.reg = block_transfer_count;
    descriptor->DSTADDR.reg = ((uint32_t) first_buffer + sizeof(uint32_t) * block_transfer_count);
    descriptor->DESCADDR.reg = 0;
    if (length * words_per_sample > words_per_buffer) {
        descriptor->DESCADDR.reg = ((uint32_t)second_descriptor);
    }
    #ifdef SAMD21
    descriptor->SRCADDR.reg = (uint32_t)&I2S->DATA[self->serializer];
    #endif
    #ifdef SAM_D5X_E5X
    descriptor->SRCADDR.reg = (uint32_t)&I2S->RXDATA;
    #endif

    // Do we need more values than will fit in the first buffer?
    // If so, set up a second buffer chained to be filled after the first buffer.
    if (length * words_per_sample > words_per_buffer) {
        block_transfer_count = words_per_buffer;
        second_descriptor->DESCADDR.reg = ((uint32_t)descriptor);
        if (length * words_per_sample < 2 * words_per_buffer) {
            // Length needed is more than one buffer but less than two.
            // Subtract off the size of the first buffer, and what remains is the count we need.
            block_transfer_count = length * words_per_sample - words_per_buffer;
            second_descriptor->DESCADDR.reg = 0;
        }
        second_descriptor->DSTADDR.reg = ((uint32_t) second_buffer + sizeof(uint32_t) * block_transfer_count);

        second_descriptor->BTCNT.reg = block_transfer_count;
        #ifdef SAMD21
        second_descriptor->SRCADDR.reg = (uint32_t)&I2S->DATA[self->serializer];
        #endif
        #ifdef SAM_D5X_E5X
        second_descriptor->SRCADDR.reg = (uint32_t)&I2S->RXDATA;
        #endif
        second_descriptor->BTCTRL.reg = DMAC_BTCTRL_VALID |
                                        DMAC_BTCTRL_BLOCKACT_NOACT |
                                        DMAC_BTCTRL_EVOSEL_BLOCK |
                                        DMAC_BTCTRL_DSTINC |
                                        DMAC_BTCTRL_BEATSIZE_WORD;
    }
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
const uint16_t sinc_filter [OVERSAMPLING] = {
    0, 2, 9, 21, 39, 63, 94, 132,
    179, 236, 302, 379, 467, 565, 674, 792,
    920, 1055, 1196, 1341, 1487, 1633, 1776, 1913,
    2042, 2159, 2263, 2352, 2422, 2474, 2506, 2516,
    2506, 2474, 2422, 2352, 2263, 2159, 2042, 1913,
    1776, 1633, 1487, 1341, 1196, 1055, 920, 792,
    674, 565, 467, 379, 302, 236, 179, 132,
    94, 63, 39, 21, 9, 2, 0, 0
};

#ifdef SAMD21
#define REPEAT_16_TIMES(X) do { for(uint8_t j=0; j<4; j++) { X X X X } } while (0)
#else
#define REPEAT_16_TIMES(X) do { X X X X X X X X X X X X X X X X } while(0)
#endif

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
            );
    }
    return running_sum;
}

// output_buffer may be a byte buffer or a halfword buffer.
// output_buffer_length is the number of slots, not the number of bytes.
uint32_t common_hal_audiobusio_pdmin_record_to_buffer(audiobusio_pdmin_obj_t* self,
        uint16_t* output_buffer, uint32_t output_buffer_length) {
    uint8_t dma_channel = dma_allocate_channel();
    uint8_t event_channel = find_sync_event_channel_raise();

    // We allocate two buffers on the stack to use for double buffering.
    const uint8_t samples_per_buffer = SAMPLES_PER_BUFFER;
    // For every word we record, we throw away 2 bytes of a phantom second channel.
    uint8_t words_per_sample = self->bytes_per_sample / 2;
    uint32_t words_per_buffer = samples_per_buffer * words_per_sample;
    uint32_t first_buffer[words_per_buffer];
    uint32_t second_buffer[words_per_buffer];

    turn_on_event_system();

    COMPILER_ALIGNED(16) DmacDescriptor second_descriptor;

    setup_dma(self, output_buffer_length, dma_descriptor(dma_channel), &second_descriptor,
              words_per_buffer, words_per_sample, first_buffer, second_buffer);

    uint8_t trigger_source = I2S_DMAC_ID_RX_0;
    #ifdef SAMD21
    trigger_source += self->serializer;
    #endif

    dma_configure(dma_channel, trigger_source, true);
    init_event_channel_interrupt(event_channel, CORE_GCLK, EVSYS_ID_GEN_DMAC_CH_0 + dma_channel);
    // Turn on serializer now to get it in sync with DMA.
    i2s_set_serializer_enable(self->serializer, true);
    audio_dma_enable_channel(dma_channel);

    // Record
    uint32_t buffers_processed = 0;
    uint32_t values_output = 0;

    uint32_t remaining_samples_needed = output_buffer_length;
    while (values_output < output_buffer_length) {
        if (event_interrupt_overflow(event_channel)) {
            // Looks like we aren't keeping up. We shouldn't skip a buffer so stop early.
            break;
        }
        // Wait for the next buffer to fill
        uint32_t wait_counts = 0;
        #ifdef SAMD21
          #define MAX_WAIT_COUNTS 1000
        #endif
        #ifdef SAM_D5X_E5X
          #define MAX_WAIT_COUNTS 6000
        #endif
        // If wait_counts exceeds the max count, buffer has probably stopped filling;
        // DMA may have missed an I2S trigger event.
        while (!event_interrupt_active(event_channel) && ++wait_counts < MAX_WAIT_COUNTS) {
            RUN_BACKGROUND_TASKS;
        }

        // The mic is running all the time, so we don't need to wait the usual 10msec or 100msec
        // for it to start up.

        // Flip back and forth between processing the first and second buffers.
        uint32_t *buffer = first_buffer;
        DmacDescriptor* descriptor = dma_descriptor(dma_channel);
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

    disable_event_channel(event_channel);
    dma_free_channel(dma_channel);
    // Turn off serializer, but leave clock on, to avoid mic startup delay.
    i2s_set_serializer_enable(self->serializer, false);

    return values_output;
}

void common_hal_audiobusio_pdmin_record_to_file(audiobusio_pdmin_obj_t* self, uint8_t* buffer, uint32_t length) {

}
