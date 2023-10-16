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
#include "shared/runtime/interrupt_char.h"
#include "py/runtime.h"
#include "src/rp2_common/hardware_adc/include/hardware/adc.h"
#include "src/rp2_common/hardware_dma/include/hardware/dma.h"
#include "src/common/pico_stdlib/include/pico/stdlib.h"

#define ADC_FIRST_PIN_NUMBER 26
#define ADC_PIN_COUNT 4

#define ADC_CLOCK_INPUT 48000000
#define ADC_MAX_CLOCK_DIV (1 << (ADC_DIV_INT_MSB - ADC_DIV_INT_LSB + 1))

void common_hal_analogbufio_bufferedin_construct(analogbufio_bufferedin_obj_t *self, const mcu_pin_obj_t *pin, uint32_t sample_rate) {
    // Make sure pin number is in range for ADC
    if (pin->number < ADC_FIRST_PIN_NUMBER || pin->number >= (ADC_FIRST_PIN_NUMBER + ADC_PIN_COUNT)) {
        raise_ValueError_invalid_pins();
    }

    // Validate sample rate here
    sample_rate = (uint32_t)mp_arg_validate_int_range(sample_rate, ADC_CLOCK_INPUT / ADC_MAX_CLOCK_DIV, ADC_CLOCK_INPUT / 96, MP_QSTR_sample_rate);

    // Set pin and channel
    self->pin = pin;
    claim_pin(pin);

    // TODO: find a way to accept ADC4 for temperature
    self->chan = pin->number - ADC_FIRST_PIN_NUMBER;

    // Init GPIO for analogue use: hi-Z, no pulls, disable digital input buffer.
    // TODO: Make sure we share the ADC well. Right now we just assume it is
    // unused.
    adc_init();
    adc_gpio_init(pin->number);
    adc_select_input(self->chan); // chan = pin - 26 ??

    // Divisor of 0 -> full speed. Free-running capture with the divider is
    // equivalent to pressing the ADC_CS_START_ONCE button once per `div + 1`
    // cycles (div not necessarily an integer). Each conversion takes 96
    // cycles, so in general you want a divider of 0 (hold down the button
    // continuously) or > 95 (take samples less frequently than 96 cycle
    // intervals). This is all timed by the 48 MHz ADC clock.
    // sample rate determines divisor, not zero.

    // sample_rate is forced to be >= 1 in shared-bindings
    float clk_div = (float)ADC_CLOCK_INPUT / (float)sample_rate;
    adc_set_clkdiv(clk_div);

    // Set up the DMA to start transferring data as soon as it appears in FIFO
    uint dma_chan = dma_claim_unused_channel(true);
    self->dma_chan = dma_chan;

    // Set Config
    self->cfg = dma_channel_get_default_config(dma_chan);

    // Reading from constant address, writing to incrementing byte addresses
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

uint32_t common_hal_analogbufio_bufferedin_readinto(analogbufio_bufferedin_obj_t *self, uint8_t *buffer, uint32_t len, uint8_t bytes_per_sample) {
    // RP2040 Implementation Detail
    // Fills the supplied buffer with ADC values using DMA transfer.
    // If the buffer is 8-bit, then values are 8-bit shifted and error bit is off.
    // If buffer is 16-bit, then values are 12-bit and error bit is present. We
    // stretch the 12-bit value to 16-bits and truncate the number of valid
    // samples at the first sample with the error bit set.
    // Number of transfers is always the number of samples which is the array
    // byte length divided by the bytes_per_sample.
    uint dma_size = DMA_SIZE_8;
    bool show_error_bit = false;
    if (bytes_per_sample == 2) {
        dma_size = DMA_SIZE_16;
        show_error_bit = true;
    }

    adc_fifo_setup(
        true,                 // Write each completed conversion to the sample FIFO
        true,                 // Enable DMA data request (DREQ)
        1,                    // DREQ (and IRQ) asserted when at least 1 sample present
        show_error_bit,       // See the ERR bit
        bytes_per_sample == 1 // Shift each sample to 8 bits when pushing to FIFO
        );

    uint32_t sample_count = len / bytes_per_sample;

    channel_config_set_transfer_data_size(&(self->cfg), dma_size);

    dma_channel_configure(self->dma_chan, &(self->cfg),
        buffer,   // dst
        &adc_hw->fifo,  // src
        sample_count,   // transfer count
        true            // start immediately
        );

    // Start the ADC
    adc_run(true);

    // Once DMA finishes, stop any new conversions from starting, and clean up
    // the FIFO in case the ADC was still mid-conversion.
    uint32_t remaining_transfers = sample_count;
    while (dma_channel_is_busy(self->dma_chan) &&
           !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    remaining_transfers = dma_channel_hw_addr(self->dma_chan)->transfer_count;

    //  Clean up
    adc_run(false);
    // Stopping early so abort.
    if (dma_channel_is_busy(self->dma_chan)) {
        dma_channel_abort(self->dma_chan);
    }
    adc_fifo_drain();

    size_t captured_count = sample_count - remaining_transfers;
    if (dma_size == DMA_SIZE_16) {
        uint16_t *buf16 = (uint16_t *)buffer;
        for (size_t i = 0; i < captured_count; i++) {
            uint16_t value = buf16[i];
            // Check the error bit and "truncate" the buffer if there is an error.
            if ((value & ADC_FIFO_ERR_BITS) != 0) {
                captured_count = i;
                break;
            }
            // Scale the values to the standard 16 bit range.
            buf16[i] = (value << 4) | (value >> 8);
        }
    }
    return captured_count;
}
