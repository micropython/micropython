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
#include "py/mphal.h"
#include "samd_soc.h"

static uint32_t cpu_freq = CPU_FREQ;
static uint32_t peripheral_freq = DFLL48M_FREQ;
static uint32_t dfll48m_calibration;

int sercom_gclk_id[] = {
    GCLK_CLKCTRL_ID_SERCOM0_CORE, GCLK_CLKCTRL_ID_SERCOM1_CORE,
    GCLK_CLKCTRL_ID_SERCOM2_CORE, GCLK_CLKCTRL_ID_SERCOM3_CORE,
    GCLK_CLKCTRL_ID_SERCOM4_CORE, GCLK_CLKCTRL_ID_SERCOM5_CORE
};

uint32_t get_cpu_freq(void) {
    return cpu_freq;
}

uint32_t get_peripheral_freq(void) {
    return peripheral_freq;
}

void set_cpu_freq(uint32_t cpu_freq_arg) {

    // Set 1 waitstate to be safe
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_MANW | NVMCTRL_CTRLB_RWS(1);

    int div = MAX(DFLL48M_FREQ / cpu_freq_arg, 1);
    peripheral_freq = DFLL48M_FREQ / div;

    // Enable GCLK output: 48MHz from DFLL48M on both CCLK0 and GCLK2
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(0) | GCLK_GENDIV_DIV(div);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(0);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(div);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(2);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    // The comparison is >=, such that for 48MHz still the FDPLL96 is used for the CPU clock.
    if (cpu_freq_arg >= 48000000) {
        cpu_freq = cpu_freq_arg;
        // Connect GCLK1 to the FDPLL96 input.
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_ID_FDPLL | GCLK_CLKCTRL_CLKEN;
        while (GCLK->STATUS.bit.SYNCBUSY) {
        }
        // configure the FDPLL96
        // CtrlB: Set the ref ource to GCLK, set the Wakup-Fast Flag.
        SYSCTRL->DPLLCTRLB.reg = SYSCTRL_DPLLCTRLB_REFCLK_GCLK | SYSCTRL_DPLLCTRLB_WUF;
        // Set the FDPLL ratio and enable the DPLL.
        int ldr = cpu_freq / FDPLL_REF_FREQ;
        int frac = ((cpu_freq - ldr * FDPLL_REF_FREQ) / (FDPLL_REF_FREQ / 16)) & 0x0f;
        SYSCTRL->DPLLRATIO.reg = SYSCTRL_DPLLRATIO_LDR((frac << 16 | ldr) - 1);
        SYSCTRL->DPLLCTRLA.reg = SYSCTRL_DPLLCTRLA_ENABLE;
        // Wait for the DPLL lock.
        while (!SYSCTRL->DPLLSTATUS.bit.LOCK) {
        }
        // Finally switch GCLK0 to FDPLL96M.
        GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DPLL96M | GCLK_GENCTRL_ID(0);
        while (GCLK->STATUS.bit.SYNCBUSY) {
        }
    } else {
        cpu_freq = peripheral_freq;
        // Disable the FDPLL96M in case it was enabled.
        SYSCTRL->DPLLCTRLA.reg = 0;
    }
    if (cpu_freq >= 8000000) {
        // Enable GCLK output: 48MHz on GCLK5 for USB
        GCLK->GENDIV.reg = GCLK_GENDIV_ID(5) | GCLK_GENDIV_DIV(1);
        GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(5);
        while (GCLK->STATUS.bit.SYNCBUSY) {
        }
    } else {
        // Disable GCLK output on GCLK5 for USB, since USB is not reliable below 8 Mhz.
        GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(5);
        while (GCLK->STATUS.bit.SYNCBUSY) {
        }
    }
    // Set 0 waitstates for slower CPU clock
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_MANW | NVMCTRL_CTRLB_RWS(cpu_freq > 24000000 ? 1 : 0);
    SysTick_Config(cpu_freq / 1000);
}

void check_usb_recovery_mode(void) {
    #if !MICROPY_HW_XOSC32K
    mp_hal_delay_ms(500);
    // Check USB status. If not connected, switch DFLL48M back to open loop
    if (USB->DEVICE.DeviceEndpoint[0].EPCFG.reg == 0) {
        // Set/keep the open loop mode of the device.
        SYSCTRL->DFLLVAL.reg = dfll48m_calibration;
        SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_CCDIS | SYSCTRL_DFLLCTRL_ENABLE;
    }
    #endif // MICROPY_HW_XOSC32K
}

// Purpose of the #defines for the clock configuration.
//
// Both CPU and peripheral devices are clocked by the DFLL48M clock.
// DFLL48M is either free running, or controlled by the 32kHz crystal, or
// Synchronized with the USB clock.
//
// #define MICROPY_HW_XOSC32K (0 | 1)
//
// If MICROPY_HW_XOSC32K = 1, the 32kHz crystal is used as input for GCLK 1, which
// serves as reference clock source for the DFLL48M oscillator,
// The crystal is used, unless MICROPY_HW_MCU_OSC32KULP is set.
// In that case GCLK1 (and the CPU clock) is driven by the 32K Low power oscillator.
// The reason for offering this option is a design flaw of the Adafruit
// Feather boards, where the RGB Led and Debug signals interfere with the
// crystal, causing the CPU to fail if it is driven by the crystal.
//
// If MICROPY_HW_XOSC32K = 0, the 32kHz signal for GCLK1 (and the CPU) is
// created by dividing the 48MHz clock of DFLL48M, but not used otherwise.
//
// If MICROPY_HW_DFLL_USB_SYNC = 0, the DFLL48M oscillator is free running using
// the pre-configured trim values. In that mode, the peripheral clock is
// not exactly 48Mhz and has a substantional temperature drift.
//
// If MICROPY_HW_DFLL_USB_SYNC = 1, the DFLL48 is synchronized with the 1 kHz USB sync
// signal. If after boot there is no USB sync within 500ms, the configuration falls
// back to a free running 48Mhz oscillator.
//
// In all modes, the 48MHz signal has a substantial jitter, largest when
// MICROPY_HW_DFLL_USB_SYNC is active. That is caused by the respective
// reference frequencies of 32kHz or 1 kHz being low. That affects most
// PWM. Std Dev at 1kHz 0.156Hz (w. Crystal) up to 0.4 Hz (with USB sync).
//
// If none of the mentioned defines is set, the device uses the internal oscillators.

void init_clocks(uint32_t cpu_freq) {

    dfll48m_calibration = 0; // please the compiler

    // SAMD21 Clock settings
    //
    // GCLK0: 48MHz, source: DFLL48M or FDPLL96M, usage: CPU
    // GCLK1: 32kHz, source: XOSC32K or OSCULP32K, usage: FDPLL96M reference
    // GCLK2: 1-48MHz, source: DFLL48M, usage: Peripherals
    // GCLK3: 2Mhz,  source: DFLL48M, usage: us-counter (TC4/TC5)
    // GCLK4: 32kHz, source: XOSC32K, if crystal present, usage: DFLL48M reference
    // GCLK5: 48MHz, source: DFLL48M, usage: USB
    // GCLK8: 1kHz,  source: XOSC32K or OSCULP32K, usage: WDT and RTC
    // DFLL48M: Reference sources:
    //          - in closed loop mode: either XOSC32K or OSCULP32K or USB clock
    //            from GCLK4.
    //          - in open loop mode: None
    // FDPLL96M: Reference source GCLK1
    //           Used for the CPU clock for freq >= 48Mhz

    NVMCTRL->CTRLB.bit.MANW = 1; // errata "Spurious Writes"
    NVMCTRL->CTRLB.bit.RWS = 1; // 1 read wait state for 48MHz

    #if MICROPY_HW_XOSC32K
    // Set up OSC32K according datasheet 17.6.3
    SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_STARTUP(0x3) | SYSCTRL_XOSC32K_EN32K |
        SYSCTRL_XOSC32K_XTALEN;
    SYSCTRL->XOSC32K.bit.ENABLE = 1;
    while (SYSCTRL->PCLKSR.bit.XOSC32KRDY == 0) {
    }
    // Set up the DFLL48 according to the data sheet 17.6.7.1.2
    // Step 1: Set up the reference clock

    #if MICROPY_HW_MCU_OSC32KULP
    // Connect the GCLK1 to the XOSC32KULP
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(1) | GCLK_GENDIV_DIV(1);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(1);
    #else
    // Connect the GCLK1 to OSC32K
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(1) | GCLK_GENDIV_DIV(1);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_ID(1);
    #endif

    while (GCLK->STATUS.bit.SYNCBUSY) {
    }

    // Connect the GCLK4 to OSC32K
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(4) | GCLK_GENDIV_DIV(1);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_ID(4);
    // Connect GCLK4 to the DFLL input.
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID_DFLL48 | GCLK_CLKCTRL_CLKEN;
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }

    // Enable access to the DFLLCTRL reg acc. to Errata 1.2.1
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while (SYSCTRL->PCLKSR.bit.DFLLRDY == 0) {
    }
    // Step 2: Set the coarse and fine values.
    // Get the coarse value from the calib data. In case it is not set,
    // set a midrange value.
    uint32_t coarse = (*((uint32_t *)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk)
        >> FUSES_DFLL48M_COARSE_CAL_Pos;
    if (coarse == 0x3f) {
        coarse = 0x1f;
    }
    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(512);
    while (SYSCTRL->PCLKSR.bit.DFLLRDY == 0) {
    }
    // Step 3: Set the multiplication values. The offset of 16384 to the freq is for rounding.
    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_MUL((CPU_FREQ + 16384) / 32768) |
        SYSCTRL_DFLLMUL_FSTEP(1) | SYSCTRL_DFLLMUL_CSTEP(1);
    while (SYSCTRL->PCLKSR.bit.DFLLRDY == 0) {
    }
    // Step 4: Start the DFLL and wait for the PLL lock. We just wait for the fine lock, since
    // coarse adjusting is bypassed.
    SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_MODE | SYSCTRL_DFLLCTRL_WAITLOCK | SYSCTRL_DFLLCTRL_STABLE |
        SYSCTRL_DFLLCTRL_BPLCKC | SYSCTRL_DFLLCTRL_ENABLE;
    while (SYSCTRL->PCLKSR.bit.DFLLLCKF == 0) {
    }
    // Set GCLK8 to 1 kHz.
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(8) | GCLK_GENDIV_DIV(32);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_ID(8);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }

    #else // MICROPY_HW_XOSC32K

    // Enable DFLL48M
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {
    }

    uint32_t coarse = (*((uint32_t *)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk)
        >> FUSES_DFLL48M_COARSE_CAL_Pos;
    if (coarse == 0x3f) {
        coarse = 0x1f;
    }
    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(511);

    #if MICROPY_HW_DFLL_USB_SYNC
    // Configure the DFLL48M for USB clock recovery.
    // Will have to switch back if no USB
    SYSCTRL->DFLLSYNC.bit.READREQ = 1;
    dfll48m_calibration = SYSCTRL->DFLLVAL.reg;
    // Set the Multiplication factor.
    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_CSTEP(1) | SYSCTRL_DFLLMUL_FSTEP(1)
        | SYSCTRL_DFLLMUL_MUL(48000);
    // Set the mode to closed loop USB Recovery mode
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_USBCRM | SYSCTRL_DFLLCTRL_CCDIS
        | SYSCTRL_DFLLCTRL_MODE | SYSCTRL_DFLLCTRL_ENABLE;
    #else
    // Set/keep the open loop mode of the device.
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_CCDIS | SYSCTRL_DFLLCTRL_ENABLE;
    #endif

    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {
    }

    // Connect the GCLK1 to the XOSC32KULP
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(1) | GCLK_GENDIV_DIV(1);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(1);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    // Set GCLK8 to 1 kHz.
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(8) | GCLK_GENDIV_DIV(32);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(8);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }

    #endif // MICROPY_HW_XOSC32K

    set_cpu_freq(cpu_freq);

    // Enable GCLK output: 2MHz on GCLK3 for TC4
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(3) | GCLK_GENDIV_DIV(24);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(3);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
}

void enable_sercom_clock(int id) {
    // Enable synchronous clock. The bits are nicely arranged
    PM->APBCMASK.reg |= 0x04 << id;
    // Select multiplexer generic clock source and enable.
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | sercom_gclk_id[id];
    // Wait while it updates synchronously.
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
}
