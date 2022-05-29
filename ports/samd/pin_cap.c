/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * This file provides and checks pin capabilities as required
 * for USART, I2C, SPI, PWM, ADC
 *
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Robert Hammelrath
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

#include "py/runtime.h"
#include "py/misc.h"
#include "pin_cap.h"
#include "sam.h"


extern const uint8_t tcc_channel_count[];

#include PIN_CAP_TABLE_C

// Just look for an table entry for a given pin and raise an error
// in case of no match (which should not happen).

const pin_cap_t *get_pin_cap_info(int pin_id) {
    for (int i = 0; i < MP_ARRAY_SIZE(pin_cap_table); i++) {
        if (pin_cap_table[i].pin_id == pin_id) { // Pin match
            return &pin_cap_table[i];
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("wrong pin"));
}

// Test, wether the given pin is defined and has signals for sercom.
// If that applies return the alt_fct and pad_nr.
// If not, an error will be raised.

sercom_pad_config_t is_sercom_n(int pin_id, uint8_t sercom_nr) {
    const pin_cap_t *pct_ptr = get_pin_cap_info(pin_id);
    if ((pct_ptr->sercom1 >> 4) == sercom_nr) {
        return (sercom_pad_config_t) {ALT_FCT_SERCOM1, pct_ptr->sercom1 & 0x0f};
    } else if ((pct_ptr->sercom2 >> 4) == sercom_nr) {
        return (sercom_pad_config_t) {ALT_FCT_SERCOM2, pct_ptr->sercom2 & 0x0f};
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("wrong serial device"));
    }
}

// Test, wether the given pin is defined as ADC.
// If that applies return the adc instance and channel.
// If not, an error will be raised.

adc_config_t is_adc(int pin_id, int32_t flag) {
    const pin_cap_t *pct_ptr = get_pin_cap_info(pin_id);
    if (pct_ptr->adc0 != 0xff && (flag & (1 << pct_ptr->adc0)) == 0) {
        return (adc_config_t) {0, pct_ptr->adc0};
    #if defined(MUC_SAMD51)
    } else if (pct_ptr->adc1 != 0xff && (flag & (1 << (pct_ptr->adc1 + 16))) == 0) {
        return (adc_config_t) {1, pct_ptr->adc1};
    #endif
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("ADC pin used"));
    }
}

// Test, wether the given pin is defined and has signals for pwm.
// If that applies return the alt_fct, tcc number and channel number.
// If not, an error will be raised.
// The function either supplies a channel from a wanted device, or
// tries to provide an unused device, if available.

pwm_config_t is_pwm(int pin_id, int wanted_dev, uint8_t device_status[]) {
    const pin_cap_t *pct_ptr = get_pin_cap_info(pin_id);
    uint8_t tcc1 = pct_ptr->tcc1;
    uint8_t tcc2 = pct_ptr->tcc2;

    if (wanted_dev != -1) {
        if ((tcc1 >> 4) == wanted_dev) {
            return (pwm_config_t) {ALT_FCT_TCC1, tcc1};
        } else if ((tcc2 >> 4) == wanted_dev) {
            return (pwm_config_t) {ALT_FCT_TCC2, tcc2};
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("wrong device or channel"));
        }
    } else {
        pwm_config_t ret = {};
        if ((tcc1 >> 4) < TCC_INST_NUM) {
            ret = (pwm_config_t) {ALT_FCT_TCC1, tcc1};
            if (tcc2 == 0xff) {
                return ret;
            }
        }
        if ((tcc2 >> 4) < TCC_INST_NUM) {
            // if a device in slot 1 is not available or already in use, use the one in slot 2
            if (tcc1 == 0xff || device_status[(ret.device_channel >> 4)] != 0) {
                return (pwm_config_t) {ALT_FCT_TCC2, tcc2};
            } else {
                return ret;
            }
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("not a PWM pin"));
        }
    }
}
