/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared-bindings/analogio/AnalogOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate/translate.h"
#include "common-hal/microcontroller/Pin.h"
#include "em_vdac.h"

// Set the VDAC to max frequency of 1 MHz
#define CLK_VDAC_FREQ 1000000

// List DAC pin and channel supported
mcu_dac_pin_obj_t mcu_dac_list[DAC_BANK_ARRAY_LEN] = {
    DAC(VDAC0, 0, FN_VDAC0_CH0, false, 0, &pin_PB0),
    DAC(VDAC0, 1, FN_VDAC0_CH1, false, 0, &pin_PB1),
    DAC(VDAC1, 0, FN_VDAC1_CH0, false, 0, &pin_PB2),
    DAC(VDAC1, 1, FN_VDAC1_CH1, false, 0, &pin_PB3),
};

// Construct analogout pin. This function is called when init analogout
void common_hal_analogio_analogout_construct(analogio_analogout_obj_t *self,
    const mcu_pin_obj_t *pin) {
    uint8_t dac_num = DAC_BANK_ARRAY_LEN;
    mcu_dac_pin_obj_t *p_dac;
    uint8_t dac_index;

    if (self->dac == NULL) {
        for (dac_index = 0; dac_index < dac_num; dac_index++) {
            p_dac = &mcu_dac_list[dac_index];

            if (p_dac->pin == pin) {
                self->dac = p_dac;
                self->dac->is_used = true;
                self->dac->value = 0;
                break;
            }
        }
    }

    if (self->dac == NULL) {
        mp_raise_ValueError(translate("DAC Device Init Error"));
    }

    // Use default settings
    VDAC_Init_TypeDef init = VDAC_INIT_DEFAULT;
    VDAC_InitChannel_TypeDef initChannel = VDAC_INITCHANNEL_DEFAULT;

    // Use the HFRCOEM23 to clock the VDAC in order to operate in EM3 mode
    CMU_ClockSelectSet(self->dac->vdac == VDAC0 ?
        cmuClock_VDAC0:cmuClock_VDAC1, cmuSelect_HFRCOEM23);

    // Enable the HFRCOEM23 and VDAC clocks
    CMU_ClockEnable(cmuClock_HFRCOEM23, true);
    CMU_ClockEnable(self->dac->vdac == VDAC0 ?
        cmuClock_VDAC0 : cmuClock_VDAC1, true);

    // Calculate the VDAC clock prescaler value resulting in a 1 MHz VDAC clock
    init.prescaler = VDAC_PrescaleCalc(VDAC0, CLK_VDAC_FREQ);

    init.reference = vdacRef2V5;
    // Clocking is requested on demand
    init.onDemandClk = false;

    // Disable High Capacitance Load mode
    initChannel.highCapLoadEnable = false;

    // Use Low Power mode
    initChannel.powerMode = vdacPowerModeLowPower;

    // Initialize the VDAC and VDAC channel
    VDAC_Init(self->dac->vdac, &init);

    VDAC_InitChannel(self->dac->vdac, &initChannel, self->dac->channel);

    // Enable the VDAC
    VDAC_Enable(self->dac->vdac, self->dac->channel, true);

    for (dac_index = 0; dac_index < dac_num; dac_index++) {
        p_dac = &mcu_dac_list[dac_index];

        if (p_dac->vdac == self->dac->vdac && p_dac->pin != self->dac->pin
            && p_dac->is_used == true) {
            VDAC_InitChannel(p_dac->vdac, &initChannel, p_dac->channel);
            VDAC_Enable(p_dac->vdac, p_dac->channel, true);
            VDAC_ChannelOutputSet(p_dac->vdac, p_dac->channel,
                p_dac->value >> 4);
            break;
        }
    }

    VDAC_ChannelOutputSet(self->dac->vdac, self->dac->channel, 0);

    common_hal_mcu_pin_claim(pin);
}

// Check obj is deinited or not
bool common_hal_analogio_analogout_deinited(analogio_analogout_obj_t *self) {
    return self->dac == NULL;
}

// Deinit analogout obj
void common_hal_analogio_analogout_deinit(analogio_analogout_obj_t *self) {
    uint8_t dac_num = DAC_BANK_ARRAY_LEN;
    mcu_dac_pin_obj_t *p_dac;
    uint8_t dac_index;
    VDAC_Enable(self->dac->vdac, self->dac->channel, false);

    for (dac_index = 0; dac_index < dac_num; dac_index++) {
        p_dac = &mcu_dac_list[dac_index];
        if (p_dac->vdac == self->dac->vdac && p_dac->pin != self->dac->pin
            && p_dac->is_used == false) {
            VDAC_Reset(self->dac->vdac);
        }
    }
    common_hal_reset_pin(self->dac->pin);

    self->dac->value = 0;
    self->dac->is_used = false;
    self->dac = NULL;
}

// Set value for dac pin
// dac value 0 - 65535 (0 - 2.5V)
void common_hal_analogio_analogout_set_value(analogio_analogout_obj_t *self,
    uint16_t value) {
    self->dac->value = value;
    // Write the output value to VDAC DATA register
    VDAC_ChannelOutputSet(self->dac->vdac, self->dac->channel, value >> 4);
}

// Function reset dac peripheral
void analogout_reset(void) {
    uint8_t dac_index;
    mcu_dac_pin_obj_t *p_dac;
    for (dac_index = 0; dac_index < DAC_BANK_ARRAY_LEN; dac_index++) {
        p_dac = &mcu_dac_list[dac_index];
        if (p_dac->is_used == true) {
            VDAC_Reset(p_dac->vdac);
        }
    }
}
