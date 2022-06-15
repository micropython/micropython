/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * This file provides functions for configuring the clocks.
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
#include "samd_soc.h"

static uint32_t cpu_freq = CPU_FREQ;
static uint32_t apb_freq = APB_FREQ;

int sercom_gclk_id[] = {
    SERCOM0_GCLK_ID_CORE, SERCOM1_GCLK_ID_CORE,
    SERCOM2_GCLK_ID_CORE, SERCOM3_GCLK_ID_CORE,
    SERCOM4_GCLK_ID_CORE, SERCOM5_GCLK_ID_CORE,
    #if defined(SERCOM7_GCLK_ID_CORE)
    SERCOM6_GCLK_ID_CORE, SERCOM7_GCLK_ID_CORE,
    #endif
};

uint32_t get_cpu_freq(void) {
    return cpu_freq;
}

uint32_t get_apb_freq(void) {
    return apb_freq;
}

void set_cpu_freq(uint32_t cpu_freq_arg) {
    cpu_freq = cpu_freq_arg;

    // Setup GCLK0 for 48MHz as default state to keep the MCU running during config change.
    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL0) {
    }

    // Setup DPLL0 for 120 MHz
    // first: disable DPLL0 in case it is running
    OSCCTRL->Dpll[0].DPLLCTRLA.bit.ENABLE = 0;
    while (OSCCTRL->Dpll[0].DPLLSYNCBUSY.bit.ENABLE == 1) {
    }
    // Now configure the registers
    OSCCTRL->Dpll[0].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_DIV(1) | OSCCTRL_DPLLCTRLB_LBYPASS |
        OSCCTRL_DPLLCTRLB_REFCLK(0) | OSCCTRL_DPLLCTRLB_WUF | OSCCTRL_DPLLCTRLB_FILTER(0x01);

    uint32_t div = cpu_freq / DPLLx_REF_FREQ;
    uint32_t frac = (cpu_freq - div * DPLLx_REF_FREQ) / (DPLLx_REF_FREQ / 32);
    OSCCTRL->Dpll[0].DPLLRATIO.reg = (frac << 16) + div - 1;
    // enable it again
    OSCCTRL->Dpll[0].DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE | OSCCTRL_DPLLCTRLA_RUNSTDBY;

    // Per errata 2.13.1
    while (!(OSCCTRL->Dpll[0].DPLLSTATUS.bit.CLKRDY == 1)) {
    }

    // Setup GCLK0 for DPLL0 output (48 or 48-200MHz)
    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DPLL0;
    while (GCLK->SYNCBUSY.bit.GENCTRL0) {
    }
}

void init_clocks(uint32_t cpu_freq) {
    // SAMD51 clock settings
    // GCLK0: 48MHz from DFLL48M or 48 - 200 MHz from DPLL0 (SAMD51)
    // GCLK1: DPLLx_REF_FREQ 32768 Hz from 32KULP or 32k Crystal
    // GCLK2: 48MHz from DFLL48M for Peripheral devices
    // GCLK3: 16Mhz for the us-counter (TC0/TC1)
    // DPLL0: 48 - 200 MHz

    // Steps to set up clocks:
    // Reset Clocks
    // Switch GCLK0 to DFLL 48MHz
    // Setup 32768 Hz source and DFLL48M in closed loop mode, if a crystal is present.
    // Setup GCLK1 to the DPLL0 Reference freq. of 32768 Hz
    // Setup GCLK1 to drive peripheral channel 1
    // Setup DPLL0 to 120MHz
    // Setup GCLK0 to 120MHz
    // Setup GCLK2 to 48MHz for Peripherals
    // Setup GCLK3 to 8MHz for TC0/TC1

    // Setup GCLK0 for 48MHz as default state to keep the MCU running during config change.
    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL0) {
    }

    #if MICROPY_HW_XOSC32K
    // OSCILLATOR CONTROL
    // Setup XOSC32K
    OSC32KCTRL->INTFLAG.reg = OSC32KCTRL_INTFLAG_XOSC32KRDY | OSC32KCTRL_INTFLAG_XOSC32KFAIL;
    OSC32KCTRL->XOSC32K.bit.CGM = OSC32KCTRL_XOSC32K_CGM_HS_Val;
    OSC32KCTRL->XOSC32K.bit.XTALEN = 1; // 0: Generator 1: Crystal
    OSC32KCTRL->XOSC32K.bit.EN32K = 1;
    OSC32KCTRL->XOSC32K.bit.ONDEMAND = 0;
    OSC32KCTRL->XOSC32K.bit.RUNSTDBY = 1;
    OSC32KCTRL->XOSC32K.bit.STARTUP = 4;
    OSC32KCTRL->CFDCTRL.bit.CFDEN = 1; // Fall back to internal Osc on crystal fail
    OSC32KCTRL->XOSC32K.bit.ENABLE = 1;
    // make sure osc32kcrtl is ready
    while (OSC32KCTRL->STATUS.bit.XOSC32KRDY == 0) {
    }

    // Setup GCLK1 for 32kHz crystal
    GCLK->GENCTRL[1].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K;
    while (GCLK->SYNCBUSY.bit.GENCTRL1) {
    }

    // Set-up the DFLL48M in closed loop mode with input from the 32kHz crystal

    // Step 1: Peripheral channel 0 is driven by GCLK1 and it feeds DFLL48M
    GCLK->PCHCTRL[0].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN;
    while (GCLK->PCHCTRL[0].bit.CHEN == 0) {
    }
    // Step 2: Set the multiplication values. The offset of 16384 to the freq is for rounding.
    OSCCTRL->DFLLMUL.reg = OSCCTRL_DFLLMUL_MUL((APB_FREQ + DPLLx_REF_FREQ / 2) / DPLLx_REF_FREQ) |
        OSCCTRL_DFLLMUL_FSTEP(1) | OSCCTRL_DFLLMUL_CSTEP(1);
    while (OSCCTRL->DFLLSYNC.bit.DFLLMUL == 1) {
    }
    // Step 3: Set the mode to closed loop
    OSCCTRL->DFLLCTRLB.reg = OSCCTRL_DFLLCTRLB_BPLCKC | OSCCTRL_DFLLCTRLB_MODE;
    while (OSCCTRL->DFLLSYNC.bit.DFLLCTRLB == 1) {
    }
    // Wait for lock fine
    while (OSCCTRL->STATUS.bit.DFLLLCKF == 0) {
    }
    // Step 4: Start the DFLL.
    OSCCTRL->DFLLCTRLA.reg = OSCCTRL_DFLLCTRLA_RUNSTDBY | OSCCTRL_DFLLCTRLA_ENABLE;
    while (OSCCTRL->DFLLSYNC.bit.ENABLE == 1) {
    }

    #else // MICROPY_HW_XOSC32K

    // Set GCLK1 to DPLL0_REF_FREQ as defined in mpconfigboard.h (e.g. 32768 Hz)
    GCLK->GENCTRL[1].reg = ((APB_FREQ + DPLLx_REF_FREQ / 2) / DPLLx_REF_FREQ) << GCLK_GENCTRL_DIV_Pos
        | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL1) {
    }

    #endif // MICROPY_HW_XOSC32K

    // Peripheral channel 1 is driven by GCLK1 and it feeds DPLL0
    GCLK->PCHCTRL[1].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN;
    while (GCLK->PCHCTRL[1].bit.CHEN == 0) {
    }

    set_cpu_freq(cpu_freq);

    apb_freq = APB_FREQ;  // To be changed if CPU_FREQ < 48M

    // Setup GCLK2 for DPLL1 output (48 MHz)
    GCLK->GENCTRL[2].reg = GCLK_GENCTRL_DIV(1) | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL2) {
    }

    // Setup GCLK3 for 8MHz, Used for TC0/1 counter
    GCLK->GENCTRL[3].reg = GCLK_GENCTRL_DIV(6) | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL3) {
    }
}

void enable_sercom_clock(int id) {
    // Next: Set up the clocks
    GCLK->PCHCTRL[sercom_gclk_id[id]].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK2;
    // no easy way to set the clocks, except enabling all of them
    switch (id) {
        case 0:
            MCLK->APBAMASK.bit.SERCOM0_ = 1;
            break;
        case 1:
            MCLK->APBAMASK.bit.SERCOM1_ = 1;
            break;
        case 2:
            MCLK->APBBMASK.bit.SERCOM2_ = 1;
            break;
        case 3:
            MCLK->APBBMASK.bit.SERCOM3_ = 1;
            break;
        case 4:
            MCLK->APBDMASK.bit.SERCOM4_ = 1;
            break;
        case 5:
            MCLK->APBDMASK.bit.SERCOM5_ = 1;
            break;
        #ifdef SERCOM7_GCLK_ID_CORE
        case 6:
            MCLK->APBDMASK.bit.SERCOM6_ = 1;
            break;
        case 7:
            MCLK->APBDMASK.bit.SERCOM7_ = 1;
            break;
        #endif
    }
}
