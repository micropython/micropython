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

#include "peripherals/clocks.h"

#include "hpl_gclk_config.h"

#include "bindings/samd/Clock.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "py/runtime.h"

bool gclk_enabled(uint8_t gclk) {
    common_hal_mcu_disable_interrupts();
    // Explicitly do a byte write so the peripheral knows we're just wanting to read the channel
    // rather than write to it.
    *((uint8_t*) &GCLK->GENCTRL.reg) = gclk;
    while (GCLK->STATUS.bit.SYNCBUSY == 1) {}
    bool enabled = GCLK->GENCTRL.bit.GENEN;
    common_hal_mcu_enable_interrupts();
    return enabled;
}

void disable_gclk(uint8_t gclk) {
    while (GCLK->STATUS.bit.SYNCBUSY == 1) {}
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(gclk);
    while (GCLK->STATUS.bit.SYNCBUSY == 1) {}
}

void connect_gclk_to_peripheral(uint8_t gclk, uint8_t peripheral) {
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(peripheral) | GCLK_CLKCTRL_GEN(gclk) | GCLK_CLKCTRL_CLKEN;
}

void disconnect_gclk_from_peripheral(uint8_t gclk, uint8_t peripheral) {
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(peripheral) | GCLK_CLKCTRL_GEN(gclk);
}

void enable_clock_generator(uint8_t gclk, uint32_t source, uint16_t divisor) {
    uint32_t divsel = 0;
    if (gclk == 2 && divisor > 31) {
        divsel = GCLK_GENCTRL_DIVSEL;
        for (int i = 15; i > 4; i++) {
            if (divisor & (1 << i)) {
                divisor = i - 1;
                break;
            }
        }
    }
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(gclk) | GCLK_GENDIV_DIV(divisor);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(gclk) | GCLK_GENCTRL_SRC(source) | divsel | GCLK_GENCTRL_OE | GCLK_GENCTRL_GENEN;
    while (GCLK->STATUS.bit.SYNCBUSY != 0) {}
}

void disable_clock_generator(uint8_t gclk) {
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(gclk);
    while (GCLK->STATUS.bit.SYNCBUSY != 0) {}
}

static void init_clock_source_osc8m(void) {
    // Preserve CALIB and FRANGE
    SYSCTRL->OSC8M.bit.ONDEMAND = 0;
    SYSCTRL->OSC8M.bit.PRESC = 3;
    SYSCTRL->OSC8M.bit.ENABLE = 1;
    while (!SYSCTRL->PCLKSR.bit.OSC8MRDY) {}
}

static void init_clock_source_osc32k(void) {
    uint32_t calib = (*((uint32_t *)FUSES_OSC32K_CAL_ADDR) & FUSES_OSC32K_CAL_Msk) >> FUSES_OSC32K_CAL_Pos;
    SYSCTRL->OSC32K.reg = SYSCTRL_OSC32K_CALIB(calib) |
                          SYSCTRL_OSC32K_EN32K |
                          SYSCTRL_OSC32K_ENABLE;
    while (!SYSCTRL->PCLKSR.bit.OSC32KRDY) {}
}

static void init_clock_source_xosc32k(void) {
    SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_EN32K |
                           SYSCTRL_XOSC32K_XTALEN |
                           SYSCTRL_XOSC32K_ENABLE;
    while (!SYSCTRL->PCLKSR.bit.XOSC32KRDY) {}
}

static void init_clock_source_dfll48m(void) {
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {}
    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_CSTEP(1) |
                           SYSCTRL_DFLLMUL_FSTEP(1) |
                           SYSCTRL_DFLLMUL_MUL(48000);
    uint32_t coarse = (*((uint32_t *)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk) >> FUSES_DFLL48M_COARSE_CAL_Pos;
    if (coarse == 0x3f)
        coarse = 0x1f;
    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) |
                           SYSCTRL_DFLLVAL_FINE(512);
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_CCDIS |
                            SYSCTRL_DFLLCTRL_USBCRM |
                            SYSCTRL_DFLLCTRL_MODE |
                            SYSCTRL_DFLLCTRL_ENABLE;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {}
    while (GCLK->STATUS.bit.SYNCBUSY) {}
}

void clock_init(void)
{
    init_clock_source_osc8m();
    if (board_has_crystal())
        init_clock_source_xosc32k();
    else
        init_clock_source_osc32k();
    enable_clock_generator(0, GCLK_GENCTRL_SRC_DFLL48M_Val, 1);
    enable_clock_generator(1, GCLK_GENCTRL_SRC_DFLL48M_Val, 150);
    init_clock_source_dfll48m();
    if (board_has_crystal())
        enable_clock_generator(2, GCLK_GENCTRL_SRC_XOSC32K_Val, 32);
    else
        enable_clock_generator(2, GCLK_GENCTRL_SRC_OSC32K_Val, 32);
}

static bool clk_enabled(uint8_t clk) {
    common_hal_mcu_disable_interrupts();
    *((uint8_t*) &GCLK->CLKCTRL.reg) = clk;
    while (GCLK->STATUS.bit.SYNCBUSY == 1) {}
    bool enabled = GCLK->CLKCTRL.bit.CLKEN;
    common_hal_mcu_enable_interrupts();
    return enabled;
}

static uint8_t clk_get_generator(uint8_t clk) {
    common_hal_mcu_disable_interrupts();
    *((uint8_t*) &GCLK->CLKCTRL.reg) = clk;
    while (GCLK->STATUS.bit.SYNCBUSY == 1) {}
    uint8_t gen = GCLK->CLKCTRL.bit.GEN;
    common_hal_mcu_enable_interrupts();
    return gen;
}

static uint8_t generator_get_source(uint8_t gen) {
    common_hal_mcu_disable_interrupts();
    *((uint8_t*) &GCLK->GENCTRL.reg) = gen;
    while (GCLK->STATUS.bit.SYNCBUSY == 1) {}
    uint8_t src = GCLK->GENCTRL.bit.SRC;
    common_hal_mcu_enable_interrupts();
    return src;
}

static bool osc_enabled(uint8_t index) {
    switch (index) {
        case GCLK_SOURCE_XOSC:
            return SYSCTRL->XOSC.bit.ENABLE;
        // TODO: GCLK_SOURCE_GCLKIN
        // TODO: GCLK_SOURCE_GCLKGEN1
        case GCLK_SOURCE_OSCULP32K:
            return true;
        case GCLK_SOURCE_OSC32K:
            return SYSCTRL->OSC32K.bit.ENABLE;
        case GCLK_SOURCE_XOSC32K:
            return SYSCTRL->XOSC32K.bit.ENABLE;
        case GCLK_SOURCE_OSC8M:
            return SYSCTRL->OSC8M.bit.ENABLE;
        case GCLK_SOURCE_DFLL48M:
            return SYSCTRL->DFLLCTRL.bit.ENABLE;
        case GCLK_SOURCE_DPLL96M:
            return SYSCTRL->DPLLCTRLA.bit.ENABLE;
    };
    return false;
}

static uint32_t osc_get_frequency(uint8_t index) {
    switch (index) {
        case GCLK_SOURCE_XOSC:
            return 0; // unknown 0.4-32MHz
        // TODO: GCLK_SOURCE_GCLKIN
        // TODO: GCLK_SOURCE_GCLKGEN1
        case GCLK_SOURCE_OSCULP32K:
        case GCLK_SOURCE_OSC32K:
        case GCLK_SOURCE_XOSC32K:
            return 32768;
        case GCLK_SOURCE_OSC8M:
            return 8000000;
        case GCLK_SOURCE_DFLL48M:
            return 48000000;
        case GCLK_SOURCE_DPLL96M:
            return 96000000;
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
    if (type == 1 && index <= 0x24 && clk_enabled(index)) {
        *p_type = 0;
        *p_index = generator_get_source(clk_get_generator(index));
        return true;
    }
    if (type == 2 && index == 0) {
        *p_type = 0;
        *p_index = generator_get_source(0);
        return true;
    }
    return false;
}

uint32_t clock_get_frequency(uint8_t type, uint8_t index) {
    if (type == 0) {
        return osc_get_frequency(index);
    }
    if (type == 1) {
        if (!clk_enabled(index))
            return 0;

        uint8_t gen = clk_get_generator(index);

        common_hal_mcu_disable_interrupts();
        *((uint8_t*) &GCLK->GENCTRL.reg) = gen;
        *((uint8_t*) &GCLK->GENDIV.reg) = gen;
        while (GCLK->STATUS.bit.SYNCBUSY == 1) {}

        uint8_t src = GCLK->GENCTRL.bit.SRC;
        uint32_t div;
        if (GCLK->GENCTRL.bit.DIVSEL) {
            div = 1 << (GCLK->GENDIV.bit.DIV + 1);
        } else {
            div = GCLK->GENDIV.bit.DIV;
            if (!div)
                div = 1;
        }
        common_hal_mcu_enable_interrupts();

        return osc_get_frequency(src) / div;
    }
    if (type == 2 && index == 0) {
        return clock_get_frequency(0, generator_get_source(0)) / SysTick->LOAD;
    }
    return 0;
}

uint32_t clock_get_calibration(uint8_t type, uint8_t index) {
    if (type == 0) {
        switch (index) {
            case GCLK_SOURCE_OSCULP32K:
                return SYSCTRL->OSCULP32K.bit.CALIB;
            case GCLK_SOURCE_OSC32K:
                return SYSCTRL->OSC32K.bit.CALIB;
            case GCLK_SOURCE_OSC8M:
                return SYSCTRL->OSC8M.bit.CALIB;
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
                if (val > 0x1f)
                    return -1;
                SYSCTRL->OSCULP32K.bit.CALIB = val;
                return 0;
            case GCLK_SOURCE_OSC32K:
                if (val > 0x7f)
                    return -1;
                SYSCTRL->OSC32K.bit.CALIB = val;
                return 0;
            case GCLK_SOURCE_OSC8M:
                if (val > 0xfff)
                    return -1;
                SYSCTRL->OSC8M.bit.CALIB = val;
                return 0;
        };
    }
    if (type == 2 && index == 0) {
        if (val < 0x1000 || val > 0x1000000)
            return -1;
        SysTick->LOAD = val - 1;
        return 0;
    }
    return -2; // calibration is read only
}

#ifdef SAMD21_EXPOSE_ALL_CLOCKS
CLOCK_SOURCE(XOSC);
CLOCK_SOURCE(GCLKIN);
CLOCK_SOURCE(GCLKGEN1);
CLOCK_SOURCE(OSCULP32K);
#endif
CLOCK_SOURCE(OSC32K);
CLOCK_SOURCE(XOSC32K);
#ifdef SAMD21_EXPOSE_ALL_CLOCKS
CLOCK_SOURCE(OSC8M);
CLOCK_SOURCE(DFLL48M);
CLOCK_SOURCE(DPLL96M);

CLOCK_GCLK_(SYSCTRL, DFLL48);
CLOCK_GCLK_(SYSCTRL, FDPLL);
CLOCK_GCLK_(SYSCTRL, FDPLL32K);
CLOCK_GCLK(WDT);
#endif
CLOCK_GCLK(RTC);
#ifdef SAMD21_EXPOSE_ALL_CLOCKS
CLOCK_GCLK(EIC);
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
CLOCK(SERCOMx_SLOW, 1, 19);
CLOCK_GCLK_(SERCOM0, CORE);
CLOCK_GCLK_(SERCOM1, CORE);
CLOCK_GCLK_(SERCOM2, CORE);
CLOCK_GCLK_(SERCOM3, CORE);
CLOCK_GCLK_(SERCOM4, CORE);
CLOCK_GCLK_(SERCOM5, CORE);
CLOCK(TCC0_TCC1, 1, 26);
CLOCK(TCC2_TCC3, 1, 27);
CLOCK(TC4_TC5, 1, 28);
CLOCK(TC6_TC7, 1, 29);
CLOCK_GCLK(ADC);
CLOCK_GCLK_(AC, DIG);
CLOCK_GCLK_(AC, ANA);
CLOCK_GCLK(DAC);
CLOCK_GCLK(PTC);
CLOCK_GCLK_(I2S, 0);
CLOCK_GCLK_(I2S, 1);

CLOCK(SYSTICK, 2, 0);
#endif

STATIC const mp_rom_map_elem_t samd_clock_global_dict_table[] = {
#ifdef SAMD21_EXPOSE_ALL_CLOCKS
    CLOCK_ENTRY(XOSC),
    CLOCK_ENTRY(GCLKIN),
    CLOCK_ENTRY(GCLKGEN1),
    CLOCK_ENTRY(OSCULP32K),
#endif
    CLOCK_ENTRY(OSC32K),
    CLOCK_ENTRY(XOSC32K),
#ifdef SAMD21_EXPOSE_ALL_CLOCKS
    CLOCK_ENTRY(OSC8M),
    CLOCK_ENTRY(DFLL48M),
    CLOCK_ENTRY(DPLL96M),
    CLOCK_ENTRY_(SYSCTRL, DFLL48),
    CLOCK_ENTRY_(SYSCTRL, FDPLL),
    CLOCK_ENTRY_(SYSCTRL, FDPLL32K),
    CLOCK_ENTRY(WDT),
#endif
    CLOCK_ENTRY(RTC),
#ifdef SAMD21_EXPOSE_ALL_CLOCKS
    CLOCK_ENTRY(EIC),
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
    CLOCK_ENTRY(SERCOMx_SLOW),
    CLOCK_ENTRY_(SERCOM0, CORE),
    CLOCK_ENTRY_(SERCOM1, CORE),
    CLOCK_ENTRY_(SERCOM2, CORE),
    CLOCK_ENTRY_(SERCOM3, CORE),
    CLOCK_ENTRY_(SERCOM4, CORE),
    CLOCK_ENTRY_(SERCOM5, CORE),
    CLOCK_ENTRY(TCC0_TCC1),
    CLOCK_ENTRY(TCC2_TCC3),
    CLOCK_ENTRY(TC4_TC5),
    CLOCK_ENTRY(TC6_TC7),
    CLOCK_ENTRY(ADC),
    CLOCK_ENTRY_(AC, DIG),
    CLOCK_ENTRY_(AC, ANA),
    CLOCK_ENTRY(DAC),
    CLOCK_ENTRY(PTC),
    CLOCK_ENTRY_(I2S, 0),
    CLOCK_ENTRY_(I2S, 1),

    CLOCK_ENTRY(SYSTICK),
#endif
};
MP_DEFINE_CONST_DICT(samd_clock_globals, samd_clock_global_dict_table);
