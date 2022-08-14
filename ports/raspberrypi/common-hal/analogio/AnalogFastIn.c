/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2022 Lee Atkinson, MeanStride Technology, Inc.
 * Taken from AnalogIn by Scott Shawcroft for Adafruit Industries
 * Also from DMA_Capture by Luke Wren of Raspberry Pi (Trading) Ltd.
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

#include "common-hal/analogio/AnalogFastIn.h"
#include "shared-bindings/analogio/AnalogFastIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"
#include "supervisor/shared/translate/translate.h"
#include "src/rp2_common/hardware_adc/include/hardware/adc.h"
#include "src/rp2_common/hardware_dma/include/hardware/dma.h"
//     /sdk/src/rp2_common/hardware_dma/include/hardware/dma.h
//     ports/raspberrypi/
#include "sdk/src/common/pico_stdlib/include/pico/stdlib.h"

#define ADC_FIRST_PIN_NUMBER 26
#define ADC_PIN_COUNT 4

void common_hal_analogio_analogfastin_construct(analogio_analogfastin_obj_t *self, const mcu_pin_obj_t *pin, uint8_t *buffer, uint32_t len, uint8_t bytes_per_sample, bool samples_signed, mp_float_t sample_rate) {

    // Set pin and channel
    self->pin = pin;
    claim_pin(pin);
    self->chan = pin->number - ADC_FIRST_PIN_NUMBER;

    // Checks on chan value here

    // Set buffer and length
    self->buffer = buffer;
    self->len = len;

    // checks on length here

    // uint8_t bytes_per_sample
    // Set sample rate
    // self->bits_per_sample = bytes_per_sample * 8;
    self->sample_rate = sample_rate;

    // Standard IO Init
    stdio_init_all();

    // Init GPIO for analogue use: hi-Z, no pulls, disable digital input buffer.
    adc_init();
    adc_gpio_init(pin->number);
    adc_select_input(self->chan); // chan = pin - 26 ??

    // adc_select_input(self->pin->number - ADC_FIRST_PIN_NUMBER);
    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        false,   // We won't see the ERR bit because of 8 bit reads; disable. // ??
        false    // Shift each sample to 8 bits when pushing to FIFO // ??
        );

    // Divisor of 0 -> full speed. Free-running capture with the divider is
    // equivalent to pressing the ADC_CS_START_ONCE button once per `div + 1`
    // cycles (div not necessarily an integer). Each conversion takes 96
    // cycles, so in general you want a divider of 0 (hold down the button
    // continuously) or > 95 (take samples less frequently than 96 cycle
    // intervals). This is all timed by the 48 MHz ADC clock.
    // sample rate determines divisor, not zero.

    adc_set_clkdiv(48000000.0 / self->sample_rate);

    // sleep_ms(1000);
    // Set up the DMA to start transferring data as soon as it appears in FIFO
    uint dma_chan = dma_claim_unused_channel(true);
    self->dma_chan = dma_chan;

    // Set Config
    self->cfg = dma_channel_get_default_config(dma_chan);

    // Reading from constant address, writing to incrementing byte addresses
    channel_config_set_transfer_data_size(&(self->cfg), DMA_SIZE_16);
    channel_config_set_read_increment(&(self->cfg), false);
    channel_config_set_write_increment(&(self->cfg), true);

    // Pace transfers based on availability of ADC samples
    channel_config_set_dreq(&(self->cfg), DREQ_ADC);

    // clear any previous activity
    adc_fifo_drain();
    adc_run(false);

}


bool common_hal_analogio_analogfastin_deinited(analogio_analogfastin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_analogio_analogfastin_deinit(analogio_analogfastin_obj_t *self) {
    if (common_hal_analogio_analogfastin_deinited(self)) {
        return;
    }

    // Release ADC Pin
    reset_pin_number(self->pin->number);
    self->pin = NULL;

    // Release DMA Channel
    dma_channel_unclaim(self->dma_chan);
}

// ================================================================
// capture()
// make this a bool so that later we can perform integrity checking
// ================================================================
bool common_hal_analogio_analogfastin_capture(analogio_analogfastin_obj_t *self) {


    // CONSIDER THESE ISSUES
    // uint16_t value = adc_read();
    // Stretch 12-bit ADC reading to 16-bit range
    // return (value << 4) | (value >> 8);

    uint32_t cdl = self->len / 2 - 1;
    dma_channel_configure(self->dma_chan, &(self->cfg),
        self->buffer,   // dst
        &adc_hw->fifo,  // src
        cdl,            // CAPTURE_DEPTH,  // transfer count
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

    return true;
}
