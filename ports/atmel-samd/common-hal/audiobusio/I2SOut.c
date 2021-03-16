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

#include "mpconfigport.h"

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
#include "supervisor/shared/translate.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"
#include "hpl/gclk/hpl_gclk_base.h"
#include "peripheral_clk_config.h"

#ifdef SAMD21
#include "hpl/pm/hpl_pm_base.h"
#endif

#include "samd/clocks.h"
#include "samd/dma.h"
#include "samd/events.h"
#include "samd/i2s.h"
#include "samd/pins.h"
#include "samd/timers.h"

#include "audio_dma.h"

#ifdef SAMD21
#define SERCTRL(name) I2S_SERCTRL_##name
#endif

#ifdef SAM_D5X_E5X
#define SERCTRL(name) I2S_TXCTRL_##name
#endif

void i2sout_reset(void) {
    // Make sure the I2S peripheral is running so we can see if the resources we need are free.
    #ifdef SAM_D5X_E5X
    // Connect the clock units to the 2mhz clock. It can't disable without it.
    connect_gclk_to_peripheral(5, I2S_GCLK_ID_0);
    connect_gclk_to_peripheral(5, I2S_GCLK_ID_1);
    #endif
    if (I2S->CTRLA.bit.ENABLE == 1) {
        I2S->CTRLA.bit.ENABLE = 0;
        while (I2S->SYNCBUSY.bit.ENABLE == 1) {
        }
    }

    // Make sure the I2S peripheral is running so we can see if the resources we need are free.
    #ifdef SAM_D5X_E5X
    // Connect the clock units to the 2mhz clock by default. They can't reset without it.
    disconnect_gclk_from_peripheral(5, I2S_GCLK_ID_0);
    disconnect_gclk_from_peripheral(5, I2S_GCLK_ID_1);

    hri_mclk_clear_APBDMASK_I2S_bit(MCLK);
    #endif

    #ifdef SAMD21
    _pm_disable_bus_clock(PM_BUS_APBC, I2S);
    #endif
}

// Caller validates that pins are free.
void common_hal_audiobusio_i2sout_construct(audiobusio_i2sout_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, bool left_justified) {
    uint8_t serializer = 0xff;
    uint8_t bc_clock_unit = 0xff;
    uint8_t ws_clock_unit = 0xff;
    #ifdef SAMD21
    if (bit_clock == &pin_PA10
        #ifdef PIN_PA20
        || bit_clock == &pin_PA20
        #endif
        ) { // I2S SCK[0]
        bc_clock_unit = 0;
    }
    #ifdef PIN_PB11
    else if (bit_clock == &pin_PB11) { // I2S SCK[1]
        bc_clock_unit = 1;
    }
    #endif
    if (word_select == &pin_PA11
        #ifdef PIN_PA21
        || word_select == &pin_PA21
        #endif
        ) { // I2S FS[0]
        ws_clock_unit = 0;
    }
    #ifdef PIN_PB12
    else if (word_select == &pin_PB12) { // I2S FS[1]
        ws_clock_unit = 1;
    }
    #endif

    if (data == &pin_PA07 || data == &pin_PA19) { // I2S SD[0]
        serializer = 0;
    } else if (data == &pin_PA08
               #ifdef PIN_PB16
               || data == &pin_PB16
               #endif
               ) { // I2S SD[1]
        serializer = 1;
    }
    #endif
    #ifdef SAM_D5X_E5X
    // Only clock unit 0 can be used for transmission.
    if (bit_clock == &pin_PA10 || bit_clock == &pin_PB16) { // I2S SCK[0]
        bc_clock_unit = 0;
    }
    if (word_select == &pin_PA09 || word_select == &pin_PA20) { // I2S FS[0]
        ws_clock_unit = 0;
    }
    if (data == &pin_PA11 || data == &pin_PA21) { // I2S SDO
        serializer = 0;
    }
    #endif
    if (bc_clock_unit == 0xff) {
        mp_raise_ValueError_varg(translate("Invalid %q pin"), MP_QSTR_bit_clock);
    }
    if (ws_clock_unit == 0xff) {
        mp_raise_ValueError_varg(translate("Invalid %q pin"), MP_QSTR_word_select);
    }
    if (bc_clock_unit != ws_clock_unit) {
        mp_raise_ValueError(translate("Bit clock and word select must share a clock unit"));
    }
    if (serializer == 0xff) {
        mp_raise_ValueError_varg(translate("Invalid %q pin"), MP_QSTR_data);
    }
    self->clock_unit = ws_clock_unit;
    self->serializer = serializer;

    turn_on_i2s();

    if (I2S->CTRLA.bit.ENABLE == 0) {
        I2S->CTRLA.bit.SWRST = 1;
        while (I2S->CTRLA.bit.SWRST == 1) {
        }
    } else {
        #ifdef SAMD21
        if ((I2S->CTRLA.vec.SEREN & (1 << serializer)) != 0) {
            mp_raise_RuntimeError(translate("Serializer in use"));
        }
        #endif
        #ifdef SAM_D5X_E5X
        if (I2S->CTRLA.bit.TXEN == 1) {
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

    self->bit_clock = bit_clock;
    self->word_select = word_select;
    self->data = data;

    claim_pin(bit_clock);
    claim_pin(word_select);
    claim_pin(data);

    gpio_set_pin_function(self->bit_clock->number, GPIO_I2S_FUNCTION);
    gpio_set_pin_function(self->word_select->number, GPIO_I2S_FUNCTION);
    gpio_set_pin_function(self->data->number, GPIO_I2S_FUNCTION);

    self->left_justified = left_justified;
    self->playing = false;
    audio_dma_init(&self->dma);
}

bool common_hal_audiobusio_i2sout_deinited(audiobusio_i2sout_obj_t *self) {
    return self->bit_clock == NULL;
}

void common_hal_audiobusio_i2sout_deinit(audiobusio_i2sout_obj_t *self) {
    if (common_hal_audiobusio_i2sout_deinited(self)) {
        return;
    }

    reset_pin_number(self->bit_clock->number);
    self->bit_clock = NULL;
    reset_pin_number(self->word_select->number);
    self->word_select = NULL;
    reset_pin_number(self->data->number);
    self->data = NULL;
}

void common_hal_audiobusio_i2sout_play(audiobusio_i2sout_obj_t *self,
    mp_obj_t sample, bool loop) {
    if (common_hal_audiobusio_i2sout_get_playing(self)) {
        common_hal_audiobusio_i2sout_stop(self);
    }
    #ifdef SAMD21
    if ((I2S->CTRLA.vec.CKEN & (1 << self->clock_unit)) == 1) {
        mp_raise_RuntimeError(translate("Clock unit in use"));
    }
    #endif
    uint8_t bits_per_sample = audiosample_bits_per_sample(sample);
    // We always output stereo so output twice as many bits.
    uint16_t bits_per_sample_output = bits_per_sample * 2;
    uint16_t divisor = 48000000 / (bits_per_sample_output * audiosample_sample_rate(sample));
    // Find a free GCLK to generate the MCLK signal.
    uint8_t gclk = find_free_gclk(divisor);
    if (gclk > GCLK_GEN_NUM) {
        mp_raise_RuntimeError(translate("Unable to find free GCLK"));
    }
    self->gclk = gclk;

    uint32_t clkctrl = I2S_CLKCTRL_MCKSEL_GCLK |
        I2S_CLKCTRL_NBSLOTS(1) |
        I2S_CLKCTRL_FSWIDTH_HALF;
    if (self->left_justified) {
        clkctrl |= I2S_CLKCTRL_BITDELAY_LJ;
    } else {
        clkctrl |= I2S_CLKCTRL_FSOUTINV | I2S_CLKCTRL_BITDELAY_I2S;
    }
    uint8_t channel_count = audiosample_channel_count(sample);
    if (channel_count > 2) {
        mp_raise_ValueError(translate("Too many channels in sample."));
    }
    #ifdef SAMD21
    uint32_t serctrl = (self->clock_unit << I2S_SERCTRL_CLKSEL_Pos) | SERCTRL(SERMODE_TX) | I2S_SERCTRL_TXSAME_SAME | I2S_SERCTRL_EXTEND_MSBIT | I2S_SERCTRL_TXDEFAULT_ONE | I2S_SERCTRL_SLOTADJ_LEFT;
    #endif
    #ifdef SAM_D5X_E5X
    uint32_t serctrl = (self->clock_unit << I2S_RXCTRL_CLKSEL_Pos) | I2S_TXCTRL_TXSAME_SAME;
    #endif
    if (audiosample_channel_count(sample) == 1) {
        serctrl |= SERCTRL(MONO_MONO);
    } else {
        serctrl |= SERCTRL(MONO_STEREO);
    }
    if (bits_per_sample == 8) {
        serctrl |= SERCTRL(DATASIZE_8C);
        clkctrl |= I2S_CLKCTRL_SLOTSIZE_8;
    } else if (bits_per_sample == 16) {
        serctrl |= SERCTRL(DATASIZE_16C);
        clkctrl |= I2S_CLKCTRL_SLOTSIZE_16;
    }

    // Configure the I2S peripheral
    i2s_set_enable(false);

    I2S->CLKCTRL[self->clock_unit].reg = clkctrl;
    #ifdef SAMD21
    I2S->SERCTRL[self->serializer].reg = serctrl;
    #endif
    #ifdef SAM_D5X_E5X
    I2S->TXCTRL.reg = serctrl;
    #endif

    // The DFLL is always a 48mhz clock
    enable_clock_generator(self->gclk, CLOCK_48MHZ, divisor);
    connect_gclk_to_peripheral(self->gclk, I2S_GCLK_ID_0 + self->clock_unit);

    i2s_set_enable(true);

    #ifdef SAMD21
    uint32_t tx_register = (uint32_t)&I2S->DATA[self->serializer].reg;
    uint8_t dmac_id = I2S_DMAC_ID_TX_0 + self->serializer;
    #endif
    #ifdef SAM_D5X_E5X
    uint32_t tx_register = (uint32_t)&I2S->TXDATA.reg;
    uint8_t dmac_id = I2S_DMAC_ID_TX_0;
    #endif
    audio_dma_result result = audio_dma_setup_playback(&self->dma, sample, loop, false, 0,
        true /* output signed */, tx_register, dmac_id);

    if (result == AUDIO_DMA_DMA_BUSY) {
        common_hal_audiobusio_i2sout_stop(self);
        mp_raise_RuntimeError(translate("No DMA channel found"));
    } else if (result == AUDIO_DMA_MEMORY_ERROR) {
        common_hal_audiobusio_i2sout_stop(self);
        mp_raise_RuntimeError(translate("Unable to allocate buffers for signed conversion"));
    }

    I2S->INTFLAG.reg = I2S_INTFLAG_TXUR0 | I2S_INTFLAG_TXUR1;

    I2S->CTRLA.vec.CKEN = 1 << self->clock_unit;
    while ((I2S->SYNCBUSY.vec.CKEN & (1 << self->clock_unit)) != 0) {
    }

    // Init the serializer after the clock. Otherwise, it will never enable because its unclocked.
    #ifdef SAMD21
    I2S->CTRLA.vec.SEREN = 1 << self->serializer;
    while ((I2S->SYNCBUSY.vec.SEREN & (1 << self->serializer)) != 0) {
    }
    #endif
    #ifdef SAM_D5X_E5X
    I2S->CTRLA.bit.TXEN = 1;
    while (I2S->SYNCBUSY.bit.TXEN == 1) {
    }
    #endif

    self->playing = true;
}

void common_hal_audiobusio_i2sout_pause(audiobusio_i2sout_obj_t *self) {
    audio_dma_pause(&self->dma);
}

void common_hal_audiobusio_i2sout_resume(audiobusio_i2sout_obj_t *self) {
    // Clear any overrun/underrun errors
    #ifdef SAMD21
    I2S->INTFLAG.reg = I2S_INTFLAG_TXUR0 << self->serializer;
    #endif
    #ifdef SAM_D5X_E5X
    I2S->INTFLAG.reg = I2S_INTFLAG_TXUR0 | I2S_INTFLAG_TXUR1;
    #endif

    audio_dma_resume(&self->dma);
}

bool common_hal_audiobusio_i2sout_get_paused(audiobusio_i2sout_obj_t *self) {
    return audio_dma_get_paused(&self->dma);
}

void common_hal_audiobusio_i2sout_stop(audiobusio_i2sout_obj_t *self) {
    audio_dma_stop(&self->dma);

    #ifdef SAMD21
    I2S->CTRLA.vec.SEREN &= ~(1 << self->serializer);
    while ((I2S->SYNCBUSY.vec.SEREN & (1 << self->serializer)) != 0) {
    }
    #endif
    #ifdef SAM_D5X_E5X
    I2S->CTRLA.bit.TXEN = 0;
    while (I2S->SYNCBUSY.bit.TXEN == 1) {
    }
    #endif

    #ifdef SAMD21
    if (self->clock_unit == 0) {
        I2S->CTRLA.bit.CKEN0 = 0;
        while (I2S->SYNCBUSY.bit.CKEN0 == 1) {
        }
    } else {
        I2S->CTRLA.bit.CKEN1 = 0;
        while (I2S->SYNCBUSY.bit.CKEN1 == 1) {
        }
    }
    #endif
    disconnect_gclk_from_peripheral(self->gclk, I2S_GCLK_ID_0 + self->clock_unit);
    disable_clock_generator(self->gclk);

    #ifdef SAM_D5X_E5X
    connect_gclk_to_peripheral(5, I2S_GCLK_ID_0 + self->clock_unit);
    #endif

    self->playing = false;
}

bool common_hal_audiobusio_i2sout_get_playing(audiobusio_i2sout_obj_t *self) {
    bool still_playing = audio_dma_get_playing(&self->dma);
    if (self->playing && !still_playing) {
        common_hal_audiobusio_i2sout_stop(self);
    }
    return still_playing;
}

#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT
