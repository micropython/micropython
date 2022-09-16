/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2022 Lee Atkinson, MeanStride Technology, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 * https://github.com/raspberrypi/pico-examples/blob/master/adc/dma_capture/dma_capture.c
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

#include <stdio.h>
#include "common-hal/analogbufio/BufferedIn.h"
#include "shared-bindings/analogbufio/BufferedIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"
#include "supervisor/shared/translate/translate.h"
#include "src/rp2_common/hardware_adc/include/hardware/adc.h"
#include "src/rp2_common/hardware_dma/include/hardware/dma.h"
#include "src/common/pico_stdlib/include/pico/stdlib.h"

#define ADC_FIRST_PIN_NUMBER 26
#define ADC_PIN_COUNT 4

void common_hal_analogbufio_bufferedin_construct(analogbufio_bufferedin_obj_t *self, const mcu_pin_obj_t *pin, uint8_t *buffer, uint32_t len, uint8_t bytes_per_sample, bool samples_signed, uint32_t sample_rate) {

    // Make sure pin number is in range for ADC
    if (pin->number < ADC_FIRST_PIN_NUMBER || pin->number >= (ADC_FIRST_PIN_NUMBER + ADC_PIN_COUNT)) {
        raise_ValueError_invalid_pins();
    }

    // Set pin and channel
    self->pin = pin;
    claim_pin(pin);

    // TODO: find a way to accept ADC4 for temperature
    self->chan = pin->number - ADC_FIRST_PIN_NUMBER;

    // Set buffer and length
    self->buffer = buffer;
    self->len = len;

    // Set sample rate - used in read
    self->bytes_per_sample = bytes_per_sample;
    self->sample_rate = sample_rate;

    // Init GPIO for analogue use: hi-Z, no pulls, disable digital input buffer.
    adc_init();
    adc_gpio_init(pin->number);
    adc_select_input(self->chan); // chan = pin - 26 ??

    // RP2040 Implementation Detail
    // Fills the supplied buffer with ADC values using DMA transfer.
    // If the buffer is 8-bit, then values are 8-bit shifted and error bit is off.
    // If buffer is 16-bit, then values are not shifted and error bit is present.
    // Number of transfers is always the number of samples which is the array
    // byte length divided by the bytes_per_sample.

    // self->bytes_per_sample == 1
    uint dma_size = DMA_SIZE_8;
    bool show_error_bit = false;
    bool shift_sample_8_bits = true;
    if (self->bytes_per_sample == 2) {
        dma_size = DMA_SIZE_16;
        show_error_bit = true;
        shift_sample_8_bits = false;
    }

    // adc_select_input(self->pin->number - ADC_FIRST_PIN_NUMBER);
    adc_fifo_setup(
        true,               // Write each completed conversion to the sample FIFO
        true,               // Enable DMA data request (DREQ)
        1,                  // DREQ (and IRQ) asserted when at least 1 sample present
        show_error_bit,     // See the ERR bit
        shift_sample_8_bits // Shift each sample to 8 bits when pushing to FIFO
        );

    // Divisor of 0 -> full speed. Free-running capture with the divider is
    // equivalent to pressing the ADC_CS_START_ONCE button once per `div + 1`
    // cycles (div not necessarily an integer). Each conversion takes 96
    // cycles, so in general you want a divider of 0 (hold down the button
    // continuously) or > 95 (take samples less frequently than 96 cycle
    // intervals). This is all timed by the 48 MHz ADC clock.
    // sample rate determines divisor, not zero.

    // sample_rate is forced to be >= 1 in shared-bindings
    adc_set_clkdiv((float)48000000.0 / (float)self->sample_rate);

    // Set up the DMA to start transferring data as soon as it appears in FIFO
    uint dma_chan = dma_claim_unused_channel(true);
    self->dma_chan = dma_chan;

    // Set Config
    self->cfg = dma_channel_get_default_config(dma_chan);

    // Reading from constant address, writing to incrementing byte addresses
    channel_config_set_transfer_data_size(&(self->cfg), dma_size);
    channel_config_set_read_increment(&(self->cfg), false);
    channel_config_set_write_increment(&(self->cfg), true);

    // Pace transfers based on availability of ADC samples
    channel_config_set_dreq(&(self->cfg), DREQ_ADC);

    // clear any previous activity
    adc_fifo_drain();
    adc_run(false);
}

bool common_hal_analogbufio_bufferedin_deinited(analogbufio_bufferedin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_analogbufio_bufferedin_deinit(analogbufio_bufferedin_obj_t *self) {
    if (common_hal_analogbufio_bufferedin_deinited(self)) {
        return;
    }

    // Release ADC Pin
    reset_pin_number(self->pin->number);
    self->pin = NULL;

    // Release DMA Channel
    dma_channel_unclaim(self->dma_chan);
}

void common_hal_analogbufio_bufferedin_read(analogbufio_bufferedin_obj_t *self) {

    uint32_t cdl = self->len / self->bytes_per_sample;

    dma_channel_configure(self->dma_chan, &(self->cfg),
        self->buffer,   // dst
        &adc_hw->fifo,  // src
        cdl,            // transfer count
        true            // start immediately
        );

    // Start the ADC
    adc_run(true);

    // Once DMA finishes, stop any new conversions from starting, and clean up
    // the FIFO in case the ADC was still mid-conversion.
    dma_channel_wait_for_finish_blocking(self->dma_chan);

    //  Clean up
    adc_run(false);
    adc_fifo_drain();
}
