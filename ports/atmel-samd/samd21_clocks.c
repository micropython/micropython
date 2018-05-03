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
    init_clock_source_osc32k();
    enable_clock_generator(0, GCLK_GENCTRL_SRC_DFLL48M_Val, 1);
    enable_clock_generator(1, GCLK_GENCTRL_SRC_DFLL48M_Val, 150);
    init_clock_source_dfll48m();
    enable_clock_generator(2, GCLK_GENCTRL_SRC_OSC32K_Val, 32);
}
