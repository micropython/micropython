/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Vekatech Ltd.
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

/* FSP has available API for DAC (r_dac)

R_DAC_Open ( ... )
R_DAC_Stop ( ... )
R_DAC_Start ( ... )
R_DAC_Write ( ... )
R_DAC_Close ( ... )

and this is (The Lazy way)

   ... but looking to other drivers implementation (for example AGT [ra_timer.c/h]), Renesas want to be hard, so ...

   (The Hard way it is)
*/

#include "hal_data.h"
#include "ra_config.h"
#include "ra_gpio.h"
#include "ra_sci.h"
// #include "ra_int.h"
#include "ra_utils.h"
#include "ra_dac.h"



#if defined(RA4M1) || defined(RA4W1)
#define DAC_CH_SIZE 1
#elif defined(RA6M1) || defined(RA6M2) || defined(RA6M3) || defined(RA6M5)
#define DAC_CH_SIZE 2
#else
#error "CMSIS MCU Series is not specified."
#endif

#define DAC_PINS_SIZE sizeof(ra_dac_pins) / sizeof(ra_af_pin_t)

static const ra_af_pin_t ra_dac_pins[] = {
    #if defined(RA4M1) || defined(RA4W1)
    { AF_GPIO, 0, P014 }, // (A3)
    #elif defined(RA6M1) || defined(RA6M2) || defined(RA6M3) || defined(RA6M5)
    { AF_GPIO, 0, P014 }, // (A4)
    { AF_GPIO, 1, P015 }, // (A5)
    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};

static void ra_dac_set_pin(uint32_t pin) {
    bool find = false;
    uint32_t ch;
    uint32_t af;
    find = ra_af_find_ch_af((ra_af_pin_t *)&ra_dac_pins, DAC_PINS_SIZE, pin, &ch, &af);
    if (find) {
        ra_gpio_config(pin, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_LOW_POWER, af);
        // pwpr_unprotect();
        // _PXXPFS(GPIO_PORT(pin), GPIO_BIT(pin)) &= ~(PDR_MASK);
        // _PXXPFS(GPIO_PORT(pin), GPIO_BIT(pin)) |= PDR_MASK;    // output
        // pwpr_protect();
    }
}

static void ra_dac_release_pin(uint32_t pin) {
    bool find = false;
    uint32_t ch;
    uint32_t af;
    find = ra_af_find_ch_af((ra_af_pin_t *)&ra_dac_pins, DAC_PINS_SIZE, pin, &ch, &af);
    if (find) {
        ra_gpio_config(pin, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_LOW_POWER, AF_GPIO);
        // pwpr_unprotect();
        // _PXXPFS(GPIO_PORT(pin), GPIO_BIT(pin)) &= ~(PDR_MASK);
        // pwpr_protect();
    }
}

bool ra_dac_is_dac_pin(uint32_t pin) {
    uint32_t ch;
    uint32_t af;
    return ra_af_find_ch_af((ra_af_pin_t *)&ra_dac_pins, DAC_PINS_SIZE, pin, &ch, &af);
}

uint8_t ra_dac_is_running(uint8_t ch) {
    if (ch < DAC_CH_SIZE) {
        return ch? R_DAC->DACR_b.DAOE1 : R_DAC->DACR_b.DAOE0;
    } else {
        return 0;
    }
}

void ra_dac_start(uint8_t ch) {
    if (ch < DAC_CH_SIZE) {
        if (ch) {
            R_DAC->DACR_b.DAOE1 = 1U;
        } else {
            R_DAC->DACR_b.DAOE0 = 1U;
        }
    }
}

void ra_dac_stop(uint8_t ch) {
    if (ch < DAC_CH_SIZE) {
        if (ch) {
            R_DAC->DACR_b.DAOE1 = 0U;
        } else {
            R_DAC->DACR_b.DAOE0 = 0U;
        }
    }
}

void ra_dac_write(uint8_t ch, uint16_t val) {
    if ((ch < DAC_CH_SIZE) || (val < 4096)) {
        R_DAC->DADR[ch] = val;
    }
}

uint16_t ra_dac_read(uint8_t ch) {
    if (ch < DAC_CH_SIZE) {
        return R_DAC->DADR[ch];
    } else {
        return 0;
    }
}

void ra_dac_init(uint32_t dac_pin, uint8_t ch) {
    if (ch < DAC_CH_SIZE) {
        ra_mstpcrd_start(R_MSTP_MSTPCRD_MSTPD20_Msk);

        R_DAC->DADPR_b.DPSEL = 0;    // Right-justified format
        R_DAC->DAADSCR_b.DAADST = 0;  // Do not synchronize with ADC14
        R_DAC->DAVREFCR_b.REF = 1;   // AVCC0/AVSS0 selected
        R_DAC->DADR[ch] = 0;         // Output 0 Volts

        ra_dac_set_pin(dac_pin);
        ra_dac_start(ch);
    }
}

void ra_dac_deinit(uint32_t dac_pin, uint8_t ch) {
    if (ch < DAC_CH_SIZE) {
        ra_dac_stop(ch);
        R_DAC->DAVREFCR_b.REF = 0;   // No reference voltage selected
        ra_dac_release_pin(dac_pin);

        ra_mstpcrd_stop(R_MSTP_MSTPCRD_MSTPD20_Msk);
    }
}
