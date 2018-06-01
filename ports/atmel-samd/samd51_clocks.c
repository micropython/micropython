/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "clocks.h"

#include "hpl_gclk_config.h"

#include "bindings/samd/Clock.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "py/runtime.h"

bool gclk_enabled(uint8_t gclk) {
    return GCLK->GENCTRL[gclk].bit.GENEN;
}

void disable_gclk(uint8_t gclk) {
    while ((GCLK->SYNCBUSY.vec.GENCTRL & (1 << gclk)) != 0) {}
    GCLK->GENCTRL[gclk].bit.GENEN = false;
    while ((GCLK->SYNCBUSY.vec.GENCTRL & (1 << gclk)) != 0) {}
}

void connect_gclk_to_peripheral(uint8_t gclk, uint8_t peripheral) {
    GCLK->PCHCTRL[peripheral].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(gclk);
    while(GCLK->SYNCBUSY.reg != 0) {}
}

void disconnect_gclk_from_peripheral(uint8_t gclk, uint8_t peripheral) {
    GCLK->PCHCTRL[peripheral].reg = 0;
}

void enable_clock_generator(uint8_t gclk, uint32_t source, uint16_t divisor) {
    GCLK->GENCTRL[gclk].reg = GCLK_GENCTRL_SRC(source) | GCLK_GENCTRL_DIV(divisor) | GCLK_GENCTRL_GENEN;
    while ((GCLK->SYNCBUSY.vec.GENCTRL & (1 << gclk)) != 0) {}
}

void disable_clock_generator(uint8_t gclk) {
    GCLK->GENCTRL[gclk].reg = 0;
    while ((GCLK->SYNCBUSY.vec.GENCTRL & (1 << gclk)) != 0) {}
}

static bool clk_enabled(uint8_t clk) {
    return GCLK->PCHCTRL[clk].bit.CHEN;
}

static uint8_t clk_get_generator(uint8_t clk) {
    return GCLK->PCHCTRL[clk].bit.GEN;
}

static uint8_t generator_get_source(uint8_t gen) {
    return GCLK->GENCTRL[gen].bit.SRC;
}

static bool osc_enabled(uint8_t index) {
    switch (index) {
        case GCLK_SOURCE_XOSC0:
            return OSCCTRL->XOSCCTRL[0].bit.ENABLE;
        case GCLK_SOURCE_XOSC1:
            return OSCCTRL->XOSCCTRL[1].bit.ENABLE;
        case GCLK_SOURCE_OSCULP32K:
            return true;
        case GCLK_SOURCE_XOSC32K:
            return OSC32KCTRL->XOSC32K.bit.ENABLE;
        case GCLK_SOURCE_DFLL:
            return OSCCTRL->DFLLCTRLA.bit.ENABLE;
        case GCLK_SOURCE_DPLL0:
            return OSCCTRL->Dpll[0].DPLLCTRLA.bit.ENABLE;
        case GCLK_SOURCE_DPLL1:
            return OSCCTRL->Dpll[1].DPLLCTRLA.bit.ENABLE;
    };
    return false;
}

static uint32_t osc_get_source(uint8_t index) {
    uint8_t dpll_index = index - GCLK_SOURCE_DPLL0;
    uint32_t refclk = OSCCTRL->Dpll[dpll_index].DPLLCTRLB.bit.REFCLK;
    switch (refclk) {
        case 0x0:
            return generator_get_source(GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL0 + dpll_index].bit.GEN);
        case 0x1:
            return GCLK_SOURCE_XOSC32K;
        case 0x2:
            return GCLK_SOURCE_XOSC0;
        case 0x3:
            return GCLK_SOURCE_XOSC1;
    }
    return 0;
}

static uint32_t osc_get_frequency(uint8_t index);

static uint32_t generator_get_frequency(uint8_t gen) {
        uint8_t src = GCLK->GENCTRL[gen].bit.SRC;
        uint32_t div;
        if (GCLK->GENCTRL[gen].bit.DIVSEL) {
            div = 1 << (GCLK->GENCTRL[gen].bit.DIV + 1);
        } else {
            div = GCLK->GENCTRL[gen].bit.DIV;
            if (!div)
                div = 1;
        }

        return osc_get_frequency(src) / div;
}

static uint32_t dpll_get_frequency(uint8_t index) {
    uint8_t dpll_index = index - GCLK_SOURCE_DPLL0;
    uint32_t refclk = OSCCTRL->Dpll[dpll_index].DPLLCTRLB.bit.REFCLK;
    uint32_t freq;

    switch (refclk) {
        case 0x0: // GCLK
            freq = generator_get_frequency(GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL0 + dpll_index].bit.GEN);
            break;
        case 0x1: // XOSC32
            freq = 32768;
            break;
        case 0x2: // XOSC0
        case 0x3: // XOSC1
            return 0; // unknown
    }

    return (freq * (OSCCTRL->Dpll[dpll_index].DPLLRATIO.bit.LDR + 1)) +
           (freq * OSCCTRL->Dpll[dpll_index].DPLLRATIO.bit.LDRFRAC / 32);
}

static uint32_t osc_get_frequency(uint8_t index) {
    switch (index) {
        case GCLK_SOURCE_XOSC0:
        case GCLK_SOURCE_XOSC1:
            return 0; // unknown
        case GCLK_SOURCE_OSCULP32K:
        case GCLK_SOURCE_XOSC32K:
            return 32768;
        case GCLK_SOURCE_DFLL:
            return 48000000;
        case GCLK_SOURCE_DPLL0:
        case GCLK_SOURCE_DPLL1:
            return dpll_get_frequency(index);
    }
    return 0;
}

bool clock_get_enabled(uint8_t type, uint8_t index) {
    if (type == 0)
        return osc_enabled(index);
    if (type == 1)
        return clk_enabled(index);
    if (type == 2)
        return SysTick->CTRL & SysTick_CTRL_ENABLE_Msk;
    return false;
}

bool clock_get_parent(uint8_t type, uint8_t index, uint8_t *p_type, uint8_t *p_index) {
    if (type == 0 && osc_enabled(index)) {
         if (index == GCLK_SOURCE_DPLL0 || index == GCLK_SOURCE_DPLL1) {
            *p_type = 0;
            *p_index = osc_get_source(index);
            return true;
         }
         return false;
    }
    if (type == 1 && index <= 47 && clk_enabled(index)) {
        *p_type = 0;
        *p_index = generator_get_source(clk_get_generator(index));
        return true;
    }
    if (type == 2) {
        switch (index) {
            case 0:
            case 1:
                *p_type = 0;
                *p_index = generator_get_source(0);
                return true;
            case 2:
                *p_type = 0;
                switch (OSC32KCTRL->RTCCTRL.bit.RTCSEL) {
                    case 0:
                    case 1:
                        *p_index = GCLK_SOURCE_OSCULP32K;
                        return true;
                    case 4:
                    case 5:
                        *p_index = GCLK_SOURCE_XOSC32K;
                        return true;
                }
                return false;
        }
    }
    return false;
}

uint32_t clock_get_frequency(uint8_t type, uint8_t index) {
    if (type == 0) {
        return osc_get_frequency(index);
    }
    if (type == 1 && index <= 47 && clk_enabled(index)) {
        return generator_get_frequency(clk_get_generator(index));
    }
    if (type == 2) {
        switch (index) {
            case 0:
                return clock_get_frequency(0, generator_get_source(0)) / SysTick->LOAD;
            case 1:
                return clock_get_frequency(0, generator_get_source(0)) / MCLK->CPUDIV.bit.DIV;
            case 2:
                switch (OSC32KCTRL->RTCCTRL.bit.RTCSEL) {
                    case 0:
                    case 4:
                        return 1024;
                    case 1:
                    case 5:
                        return 32768;
                }
        }
    }
    return 0;
}

uint32_t clock_get_calibration(uint8_t type, uint8_t index) {
    if (type == 0) {
        switch (index) {
            case GCLK_SOURCE_OSCULP32K:
                return OSC32KCTRL->OSCULP32K.bit.CALIB;
        };
    }
    if (type == 2 && index == 0) {
        return SysTick->LOAD + 1;
    }
    return 0;
}

int clock_set_calibration(uint8_t type, uint8_t index, uint32_t val) {
    if (type == 0) {
        switch (index) {
            case GCLK_SOURCE_OSCULP32K:
                if (val > 0x3f)
                    return -1;
                OSC32KCTRL->OSCULP32K.bit.CALIB = val;
                return 0;
        };
    }
    if (type == 2 && index == 0) {
        if (val < 0x1000 || val > 0x1000000)
            return -1;
        SysTick->LOAD = val - 1;
        return 0;
    }
    return -2;
}

#include <instance/can0.h>
#include <instance/can1.h>
#include <instance/i2s.h>
#include <instance/sdhc1.h>
#include <instance/sercom6.h>
#include <instance/sercom7.h>
#include <instance/tcc4.h>

CLOCK_SOURCE(XOSC0);
CLOCK_SOURCE(XOSC1);
CLOCK_SOURCE(GCLKIN);
CLOCK_SOURCE(GCLKGEN1);
CLOCK_SOURCE(OSCULP32K);
CLOCK_SOURCE(XOSC32K);
CLOCK_SOURCE(DFLL);
CLOCK_SOURCE(DPLL0);
CLOCK_SOURCE(DPLL1);

CLOCK_GCLK_(OSCCTRL, DFLL48);
CLOCK_GCLK_(OSCCTRL, FDPLL0);
CLOCK_GCLK_(OSCCTRL, FDPLL1);
CLOCK_GCLK_(OSCCTRL, FDPLL032K); // GCLK_OSCCTRL_FDPLL1_32K, GCLK_SDHC0_SLOW, GCLK_SDHC1_SLOW, GCLK_SERCOM[0..7]_SLOW
CLOCK_GCLK(EIC);
CLOCK_GCLK_(FREQM, MSR);
// 6: GCLK_FREQM_REF
CLOCK_GCLK_(SERCOM0, CORE);
CLOCK_GCLK_(SERCOM1, CORE);
CLOCK(TC0_TC1, 1, 9);
CLOCK_GCLK(USB);
CLOCK_GCLK_(EVSYS, 0);
CLOCK_GCLK_(EVSYS, 1);
CLOCK_GCLK_(EVSYS, 2);
CLOCK_GCLK_(EVSYS, 3);
CLOCK_GCLK_(EVSYS, 4);
CLOCK_GCLK_(EVSYS, 5);
CLOCK_GCLK_(EVSYS, 6);
CLOCK_GCLK_(EVSYS, 7);
CLOCK_GCLK_(EVSYS, 8);
CLOCK_GCLK_(EVSYS, 9);
CLOCK_GCLK_(EVSYS, 10);
CLOCK_GCLK_(EVSYS, 11);
CLOCK_GCLK_(SERCOM2, CORE);
CLOCK_GCLK_(SERCOM3, CORE);
CLOCK(TCC0_TCC1, 1, 25);
CLOCK(TC2_TC3, 1, 26);
CLOCK_GCLK(CAN0);
CLOCK_GCLK(CAN1);
CLOCK(TCC2_TCC3, 1, 29);
CLOCK(TC4_TC5, 1, 30);
CLOCK_GCLK(PDEC);
CLOCK_GCLK(AC);
CLOCK_GCLK(CCL);
CLOCK_GCLK_(SERCOM4, CORE);
CLOCK_GCLK_(SERCOM5, CORE);
CLOCK_GCLK_(SERCOM6, CORE);
CLOCK_GCLK_(SERCOM7, CORE);
CLOCK_GCLK(TCC4);
CLOCK(TC6_TC7, 1, 39);
CLOCK_GCLK(ADC0);
CLOCK_GCLK(ADC1);
CLOCK_GCLK(DAC);
CLOCK_GCLK_(I2S, 0);
CLOCK_GCLK_(I2S, 1);
CLOCK_GCLK(SDHC0);
CLOCK_GCLK(SDHC1);
// 47: GCLK_CM4_TRACE

CLOCK(SYSTICK, 2, 0);
CLOCK(CPU, 2, 1);
CLOCK(RTC, 2, 2);


STATIC const mp_rom_map_elem_t samd_clock_global_dict_table[] = {
    CLOCK_ENTRY(XOSC0),
    CLOCK_ENTRY(XOSC1),
    CLOCK_ENTRY(GCLKIN),
    CLOCK_ENTRY(GCLKGEN1),
    CLOCK_ENTRY(OSCULP32K),
    CLOCK_ENTRY(XOSC32K),
    CLOCK_ENTRY(DFLL),
    CLOCK_ENTRY(DPLL0),
    CLOCK_ENTRY(DPLL1),

    CLOCK_ENTRY_(OSCCTRL, DFLL48),
    CLOCK_ENTRY_(OSCCTRL, FDPLL0),
    CLOCK_ENTRY_(OSCCTRL, FDPLL1),
    CLOCK_ENTRY_(OSCCTRL, FDPLL032K),
    CLOCK_ENTRY(EIC),
    CLOCK_ENTRY_(FREQM, MSR),
    CLOCK_ENTRY_(SERCOM0, CORE),
    CLOCK_ENTRY_(SERCOM1, CORE),
    CLOCK_ENTRY(TC0_TC1),
    CLOCK_ENTRY(USB),
    CLOCK_ENTRY_(EVSYS, 0),
    CLOCK_ENTRY_(EVSYS, 1),
    CLOCK_ENTRY_(EVSYS, 2),
    CLOCK_ENTRY_(EVSYS, 3),
    CLOCK_ENTRY_(EVSYS, 4),
    CLOCK_ENTRY_(EVSYS, 5),
    CLOCK_ENTRY_(EVSYS, 6),
    CLOCK_ENTRY_(EVSYS, 7),
    CLOCK_ENTRY_(EVSYS, 8),
    CLOCK_ENTRY_(EVSYS, 9),
    CLOCK_ENTRY_(EVSYS, 10),
    CLOCK_ENTRY_(EVSYS, 11),
    CLOCK_ENTRY_(SERCOM2, CORE),
    CLOCK_ENTRY_(SERCOM3, CORE),
    CLOCK_ENTRY(TCC0_TCC1),
    CLOCK_ENTRY(TC2_TC3),
    CLOCK_ENTRY(CAN0),
    CLOCK_ENTRY(CAN1),
    CLOCK_ENTRY(TCC2_TCC3),
    CLOCK_ENTRY(TC4_TC5),
    CLOCK_ENTRY(PDEC),
    CLOCK_ENTRY(AC),
    CLOCK_ENTRY(CCL),
    CLOCK_ENTRY_(SERCOM4, CORE),
    CLOCK_ENTRY_(SERCOM5, CORE),
    CLOCK_ENTRY_(SERCOM6, CORE),
    CLOCK_ENTRY_(SERCOM7, CORE),
    CLOCK_ENTRY(TCC4),
    CLOCK_ENTRY(TC6_TC7),
    CLOCK_ENTRY(ADC0),
    CLOCK_ENTRY(ADC1),
    CLOCK_ENTRY(DAC),
    CLOCK_ENTRY_(I2S, 0),
    CLOCK_ENTRY_(I2S, 1),
    CLOCK_ENTRY(SDHC0),
    CLOCK_ENTRY(SDHC1),

    CLOCK_ENTRY(SYSTICK),
    CLOCK_ENTRY(CPU),
    CLOCK_ENTRY(RTC),
};
MP_DEFINE_CONST_DICT(samd_clock_globals, samd_clock_global_dict_table);
