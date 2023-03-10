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

#include "shared-bindings/neopixel_write/__init__.h"

#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/time/__init__.h"

#include "peripherals/broadcom/cpu.h"

#include "supervisor/port.h"

uint64_t next_start_raw_ticks = 0;

// NeoPixels are 800khz bit streams. Zeroes are 1/3 duty cycle (~416ns) and ones
// are 2/3 duty cycle (~833ns).

void common_hal_neopixel_write(const digitalio_digitalinout_obj_t *digitalinout,
    uint8_t *pixels, uint32_t num_bytes) {
    // Wait to make sure we don't append onto the last transmission. This should only be a tick or
    // two.
    int icnt;
    while ((port_get_raw_ticks(NULL) < next_start_raw_ticks) &&
           (next_start_raw_ticks - port_get_raw_ticks(NULL) < 100)) {
        RUN_BACKGROUND_TASKS;
    }

    BP_Function_Enum alt_function = GPIO_FUNCTION_OUTPUT;
    uint8_t index = 0;
    uint8_t channel = 0;
    bool found = false;
    for (size_t i = 0; i < NUM_ALT_FUNC; i++) {
        const pin_function_t *f = &digitalinout->pin->functions[i];
        if (f->type == PIN_FUNCTION_PWM) {
            index = f->index;
            channel = f->function;
            alt_function = FSEL_VALUES[i];
            found = true;
            break;
        }
    }
    if (!found) {
        mp_raise_ValueError(translate("NeoPixel not supported on pin"));
        return;
    }

    // Turn on the PWM clock. The speed is NeoPixel specific.
    if (CM_PWM->CS_b.BUSY == 0) {
        uint32_t source_clock;
        #if BCM_VERSION == 2711
        source_clock = 54000000;
        #else
        source_clock = 19200000;
        #endif
        // Three clocks per 800khz bit to get the 1/3 or 2/3 timing.
        uint32_t target_clock = 3 * 800000;
        uint32_t int_div = source_clock / target_clock;

        CM_PWM->DIV = CM_PCM_DIV_PASSWD_PASSWD << CM_PCM_DIV_PASSWD_Pos |
                    (int_div) << CM_PCM_DIV_DIVI_Pos;

        CM_PWM->CS = CM_PCM_CS_PASSWD_PASSWD << CM_PCM_CS_PASSWD_Pos |
            CM_PCM_CS_SRC_XOSC << CM_PCM_CS_SRC_Pos;

        // Set enable after setting the source to ensure it is stable.
        CM_PWM->CS = CM_PCM_CS_PASSWD_PASSWD << CM_PCM_CS_PASSWD_Pos |
            CM_PCM_CS_SRC_XOSC << CM_PCM_CS_SRC_Pos |
            CM_PCM_CS_ENAB_Msk;

        // Wait for the clock to start up.
        COMPLETE_MEMORY_READS;
        icnt = 0;
        while ((CM_PWM->CS_b.BUSY == 0) && (icnt++ < 1000)) {
        }
    }

    PWM0_Type *pwm = PWM0;
    #if BCM_VERSION == 2711
    if (index == 1) {
        pwm = PWM1;
    }
    #else
    (void)index;
    #endif

    pwm->RNG1 = 24;
    pwm->RNG2 = 24;
    COMPLETE_MEMORY_READS;
    pwm->CTL = PWM0_CTL_CLRF1_Msk;
    COMPLETE_MEMORY_READS;

    // Even though we're only transmitting one channel, we enable both. Without
    // the second channel enabled, the output is repeated forever.
    pwm->CTL =
        PWM0_CTL_USEF2_Msk |
        PWM0_CTL_MODE2_Msk |
        PWM0_CTL_USEF1_Msk |
        PWM0_CTL_MODE1_Msk;

    COMPLETE_MEMORY_READS;
    pwm->CTL |= PWM0_CTL_PWEN1_Msk | PWM0_CTL_PWEN2_Msk;

    gpio_set_function(digitalinout->pin->number, alt_function);

    for (size_t i = 0; i < num_bytes; i++) {
        uint32_t expanded = 0;
        for (size_t j = 0; j < 8; j++) {
            expanded = expanded >> 3;
            if ((pixels[i] & (1 << j)) != 0) {
                expanded |= 0xc0000000;
            } else {
                expanded |= 0x80000000;
            }
        }
        if (channel == 1) {
            icnt = 0;
            while ((pwm->STA_b.FULL1 == 1) && (icnt++ < 150)) {
                RUN_BACKGROUND_TASKS;
            }
            // Dummy value for the first channel.
            pwm->FIF1 = 0x000000;
        }
        icnt = 0;
        while ((pwm->STA_b.FULL1 == 1) && (icnt++ < 150)) {
            RUN_BACKGROUND_TASKS;
        }
        pwm->FIF1 = expanded;
        if (channel == 0) {
            icnt = 0;
            while ((pwm->STA_b.FULL1 == 1) && (icnt++ < 150)) {
                RUN_BACKGROUND_TASKS;
            }
            // Dummy value for the second channel.
            pwm->FIF1 = 0x000000;
        }
    }

    icnt = 0;
    while ((pwm->STA_b.EMPT1 == 0) && (icnt++ < 2500)) {
        RUN_BACKGROUND_TASKS;
    }
    // Wait for transmission to start.
    icnt = 0;
    while (((pwm->STA_b.STA1 == 0) && (pwm->STA_b.STA2 == 0)) && (icnt++ < 150)) {
        RUN_BACKGROUND_TASKS;
    }
    // Wait for transmission to complete.
    icnt = 0;
    while (((pwm->STA_b.STA1 == 1) || (pwm->STA_b.STA2 == 1)) && (icnt++ < 150)) {
        RUN_BACKGROUND_TASKS;
    }
    // Shouldn't be anything left in queue but clear it so the clock doesn't crash if there is
    pwm->CTL = PWM0_CTL_CLRF1_Msk;

    gpio_set_function(digitalinout->pin->number, GPIO_FUNCTION_OUTPUT);

    // Update the next start.
    next_start_raw_ticks = port_get_raw_ticks(NULL) + 1;
}
