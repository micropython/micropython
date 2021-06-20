/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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

#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-bindings/analogio/AnalogOut.h"
#include "shared-bindings/audioio/AudioOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_dac_sync.h"
#include "hpl/gclk/hpl_gclk_base.h"
#include "peripheral_clk_config.h"

#ifdef SAMD21
#include "hpl/pm/hpl_pm_base.h"
#endif

#define HAVE_ANALOGOUT ( \
    (defined(PIN_PA02) && !defined(IGNORE_PA02)) || \
    (defined(SAM_D5X_E5X) && defined(PIN_PA05) && !defined(IGNORE_PA05)) \
    )

void common_hal_analogio_analogout_construct(analogio_analogout_obj_t *self,
    const mcu_pin_obj_t *pin) {
    #if !HAVE_ANALOGOUT
    mp_raise_NotImplementedError(translate("No DAC on chip"));
    #else

    uint8_t channel;
    switch (pin->number) {
        #if defined(PIN_PA02) && !defined(IGNORE_PIN_PA02)
        case PIN_PA02:
            channel = 0;
            break;
        #endif

        #if defined(SAM_D5X_E5X) && defined(PIN_PA05) && !defined(IGNORE_PIN_PA05)
        case PIN_PA05:
            channel = 1;
            break;
        #endif

        default:
            mp_raise_ValueError(translate("AnalogOut not supported on given pin"));
            return;
    }

    self->channel = channel;

    #ifdef SAM_D5X_E5X
    hri_mclk_set_APBDMASK_DAC_bit(MCLK);
    #endif

    #ifdef SAMD21
    _pm_enable_bus_clock(PM_BUS_APBC, DAC);
    #endif

    // SAMD21: This clock should be <= 12 MHz, per datasheet section 47.6.3.
    // SAMD51: This clock should be <= 350kHz, per datasheet table 37-6.
    _gclk_enable_channel(DAC_GCLK_ID, CONF_GCLK_DAC_SRC);

    // Don't double init the DAC on the SAMD51 when both outputs are in use. We use the free state
    // of each output pin to determine DAC state.
    int32_t result = ERR_NONE;
    #ifdef SAM_D5X_E5X
    if (!common_hal_mcu_pin_is_free(&pin_PA02) || !common_hal_mcu_pin_is_free(&pin_PA05)) {
    #endif
    // Fake the descriptor if the DAC is already initialized.
    self->descriptor.device.hw = DAC;
    #ifdef SAM_D5X_E5X
} else {
    #endif
    result = dac_sync_init(&self->descriptor, DAC);
    #ifdef SAM_D5X_E5X
}
    #endif
    if (result != ERR_NONE) {
        mp_raise_OSError(MP_EIO);
        return;
    }
    claim_pin(pin);

    gpio_set_pin_function(pin->number, GPIO_PIN_FUNCTION_B);

    dac_sync_enable_channel(&self->descriptor, self->channel);
    #endif
}

bool common_hal_analogio_analogout_deinited(analogio_analogout_obj_t *self) {
    #if !HAVE_ANALOGOUT
    return false;
    #else
    return self->deinited;
    #endif
}

void common_hal_analogio_analogout_deinit(analogio_analogout_obj_t *self) {
    #if HAVE_ANALOGOUT
    if (common_hal_analogio_analogout_deinited(self)) {
        return;
    }
    dac_sync_disable_channel(&self->descriptor, self->channel);
    reset_pin_number(PIN_PA02);
    // Only deinit the DAC on the SAMD51 if both outputs are free.
    #ifdef SAM_D5X_E5X
    if (common_hal_mcu_pin_is_free(&pin_PA02) && common_hal_mcu_pin_is_free(&pin_PA05)) {
    #endif
    dac_sync_deinit(&self->descriptor);
    #ifdef SAM_D5X_E5X
}
    #endif
    self->deinited = true;
    // TODO(tannewt): Turn off the DAC clocks to save power.
    #endif
}

void common_hal_analogio_analogout_set_value(analogio_analogout_obj_t *self,
    uint16_t value) {
    #if HAVE_ANALOGOUT
    // Input is 16 bit so make sure and set LEFTADJ to 1 so it takes the top
    // bits. This is currently done in asf4_conf/*/hpl_dac_config.h.
    dac_sync_write(&self->descriptor, self->channel, &value, 1);
    #endif
}

void analogout_reset(void) {
    // audioout_reset also resets the DAC, and does a smooth ramp down to avoid clicks
    // if it was enabled, so do that instead if AudioOut is enabled.
    #if CIRCUITPY_AUDIOIO
    audioout_reset();
    #elif HAVE_ANALOGOUT
    #ifdef SAMD21
    while (DAC->STATUS.reg & DAC_STATUS_SYNCBUSY) {
    }
    #endif
    #ifdef SAM_D5X_E5X
    while (DAC->SYNCBUSY.reg & DAC_SYNCBUSY_SWRST) {
    }
    #endif
    DAC->CTRLA.reg |= DAC_CTRLA_SWRST;
    #endif
}
