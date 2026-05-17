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

uint32_t get_peripheral_freq(void) {
    return peripheral_freq;
}

void set_cpu_freq(uint32_t cpu_freq_arg) {

    // Setup GCLK0 for 48MHz as default state to keep the MCU running during config change.
    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL0) {
    }
    // Setup DPLL0 for 120 MHz
    // first: disable DPLL0 in case it is running
    OSCCTRL->Dpll[0].DPLLCTRLA.bit.ENABLE = 0;
    while (OSCCTRL->Dpll[0].DPLLSYNCBUSY.bit.ENABLE == 1) {
    }
    if (cpu_freq_arg > DFLL48M_FREQ) {

        cpu_freq = cpu_freq_arg;
        peripheral_freq = DFLL48M_FREQ;
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
        // Set GCLK 2 back to 48 MHz
        GCLK->GENCTRL[2].reg = GCLK_GENCTRL_DIV(1) | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
        while (GCLK->SYNCBUSY.bit.GENCTRL2) {
        }
    } else {
        int div = DFLL48M_FREQ / cpu_freq_arg;
        // Setup GCLK1 for the low freq
        GCLK->GENCTRL[2].reg = GCLK_GENCTRL_DIV(div) | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
        while (GCLK->SYNCBUSY.bit.GENCTRL2) {
        }
        GCLK->GENCTRL[0].reg = GCLK_GENCTRL_DIV(div) | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
        while (GCLK->SYNCBUSY.bit.GENCTRL0) {
        }
        peripheral_freq = DFLL48M_FREQ / div;
        cpu_freq = DFLL48M_FREQ / div;
    }
    if (cpu_freq >= 8000000) {
        // Setup GCLK5 for DFLL48M output (48 MHz)
        GCLK->GENCTRL[5].reg = GCLK_GENCTRL_DIV(1) | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
        while (GCLK->SYNCBUSY.bit.GENCTRL5) {
        }
    } else {
        // Setup GCLK5 off if CPU Clk < 8 MHz
        GCLK->GENCTRL[5].reg = 0;
        while (GCLK->SYNCBUSY.bit.GENCTRL5) {
        }
    }
    SysTick_Config(cpu_freq / 1000);
}

void check_usb_clock_recovery_mode(void) {
    #if MICROPY_HW_DFLL_USB_SYNC
    // Check USB status for up to 1 second. If not connected,
    // switch DFLL48M back to open loop
    for (int i = 0; i < 100; i++) {
        if (USB->DEVICE.DeviceEndpoint[0].EPCFG.reg != 0) {
            return;
        }
        mp_hal_delay_ms(10);
    }
    // No connection. Switch back to open loop mode.
    // as per Errata 2.8.3
    OSCCTRL->DFLLMUL.reg = 0;
    while (OSCCTRL->DFLLSYNC.bit.DFLLMUL == 1) {
    }
    // Set the mode to open loop mode
    OSCCTRL->DFLLCTRLB.reg = 0;
    while (OSCCTRL->DFLLSYNC.bit.DFLLCTRLB == 1) {
    }
    OSCCTRL->DFLLCTRLA.reg = OSCCTRL_DFLLCTRLA_RUNSTDBY | OSCCTRL_DFLLCTRLA_ENABLE;
    while (OSCCTRL->DFLLSYNC.bit.ENABLE == 1) {
    }
    OSCCTRL->DFLLVAL.reg = dfll48m_calibration; // Reload DFLLVAL register
    while (OSCCTRL->DFLLSYNC.bit.DFLLVAL == 1) {
    }
    // Set the mode to open loop mode
    OSCCTRL->DFLLCTRLB.reg = 0;
    while (OSCCTRL->DFLLSYNC.bit.DFLLCTRLB == 1) {
    }
    #endif
}

// Purpose of the #defines for the clock configuration.
//
// The CPU clock is generated by DPLL0, which takes 32768 Hz as reference frequency,
// supplied through GCLK1.
//
// DFLL48M is used for the peripheral clock, e.g. for PWM, UART, SPI, I2C.
// DFLL48M is either free running, or controlled by the 32kHz crystal, or
// Synchronized with the USB clock.
//
// GCLK1 takes it's input either from the 32kHz crystal, the internal low power
// RC oscillator or from DFLL48M.
//
// #define MICROPY_HW_XOSC32K (0 | 1)
//
// If MICROPY_HW_XOSC32K = 1, the 32kHz crystal is used for the DFLL48M oscillator
// and for GCLK1, feeding the CPU, unless MICROPY_HW_MCU_OSC32KULP is set.
// In that case GCLK1 (and the CPU clock) is driven by the 32K Low power oscillator.
// The reason for offering this option is a design flaw of the Adafruit
// Feather boards, where the RGB Led and Debug signals interfere with the
// crystal, causing the CPU to fail if it is driven by the crystal. The
// peripheral devices are affected as well, but continue it's operation.
//
// If MICROPY_HW_XOSC32K = 0, the 32kHz signal for GCLK1 (and the CPU) is
// created by dividing the 48MHz clock of DFLL48M.
//
// If MICROPY_HW_DFLL_USB_SYNC = 0, the DFLL48M oscillator is free running using
// the pre-configured trim values. In that mode, the peripheral clock is
// not exactly 48Mhz and has a substitutional temperature drift.
//
// If MICROPY_HW_DFLL_USB_SYNC = 1, the DFLL48 is synchronized with the 1 kHz USB sync
// signal. If after boot there is no USB sync within 1000 ms, the configuration falls
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

    // SAMD51 clock settings
    //
    // GCLK0: 48MHz, source: 48 - 200 MHz from DPLL0, usage: CPU
    // GCLK1: 32kHz, source: OSCULP32K or DFLL48M, usage: ref_clk DPLL0
    // GCLK2: 1-48MHz, source:DFLL48M, usage: Peripheral devices
    // GCLK3: 16Mhz, source: DLLL48M, usage: us-counter (TC0/TC1)
    // GCLK4: 32kHz, source: XOSC32K, if crystal present, usage: DFLL48M reference
    // GCLK5: 48MHz, source: DFLL48M, usage: USB
    // DFLL48M: Reference sources:
    //          - in closed loop mode: either XOSC32K or OSCULP32K or USB clock
    //          - in open loop mode: None
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
    // Setup GCLK3 to 16MHz for TC0/TC1
    // Setup GCLK4 to 32kHz crystal, if present
    // Setup GCLK5 to 48 MHz

    // Setup GCLK0 for 48MHz as default state to keep the MCU running during config change.
    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL0) {
    }

    #if MICROPY_HW_XOSC32K
    // OSCILLATOR CONTROL
    // Enable the clock for RTC
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K;
    // Setup XOSC32K
    OSC32KCTRL->INTFLAG.reg = OSC32KCTRL_INTFLAG_XOSC32KRDY | OSC32KCTRL_INTFLAG_XOSC32KFAIL;
    OSC32KCTRL->CFDCTRL.bit.CFDEN = 1; // Fall back to internal oscillator on crystal fail
    OSC32KCTRL->XOSC32K.reg =
        OSC32KCTRL_XOSC32K_CGM_HS |
        OSC32KCTRL_XOSC32K_XTALEN |
        OSC32KCTRL_XOSC32K_EN32K |
        OSC32KCTRL_XOSC32K_EN1K |
        OSC32KCTRL_XOSC32K_RUNSTDBY |
        OSC32KCTRL_XOSC32K_STARTUP(3) |
        OSC32KCTRL_XOSC32K_ENABLE;
    // Wait until the oscillator is running and stable
    while (OSC32KCTRL->STATUS.bit.XOSC32KRDY == 0) {
    }

    #if MICROPY_HW_MCU_OSC32KULP
    // Setup GCLK1 for 32kHz ULP
    GCLK->GENCTRL[1].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K;
    #else
    // Setup GCLK1 for 32kHz crystal
    GCLK->GENCTRL[1].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K;
    #endif

    while (GCLK->SYNCBUSY.bit.GENCTRL1) {
    }

    // Setup GCLK4 for 32kHz crystal
    GCLK->GENCTRL[4].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K;
    while (GCLK->SYNCBUSY.bit.GENCTRL4) {
    }

    // Set-up the DFLL48M in closed loop mode with input from the 32kHz crystal

    // Step 1: Peripheral channel 0 is driven by GCLK4 and it feeds DFLL48M
    GCLK->PCHCTRL[0].reg = GCLK_PCHCTRL_GEN_GCLK4 | GCLK_PCHCTRL_CHEN;
    while (GCLK->PCHCTRL[0].bit.CHEN == 0) {
    }
    // Step 2: Set the multiplication values. The offset of 16384 to the freq is for rounding.
    OSCCTRL->DFLLMUL.reg = OSCCTRL_DFLLMUL_MUL((DFLL48M_FREQ + DPLLx_REF_FREQ / 2) / DPLLx_REF_FREQ) |
        OSCCTRL_DFLLMUL_FSTEP(1) | OSCCTRL_DFLLMUL_CSTEP(1);
    while (OSCCTRL->DFLLSYNC.bit.DFLLMUL == 1) {
    }
    // Step 3: Set the mode to closed loop
    OSCCTRL->DFLLCTRLB.reg = OSCCTRL_DFLLCTRLB_BPLCKC | OSCCTRL_DFLLCTRLB_STABLE | OSCCTRL_DFLLCTRLB_MODE;
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

    // Enable the clock for RTC
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K;

    // Derive GCLK1 from DFLL48M at DPLL0_REF_FREQ as defined in mpconfigboard.h (e.g. 32768 Hz)
    GCLK->GENCTRL[1].reg = ((DFLL48M_FREQ + DPLLx_REF_FREQ / 2) / DPLLx_REF_FREQ) << GCLK_GENCTRL_DIV_Pos
        | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL1) {
    }

    OSCCTRL->DFLLCTRLA.bit.RUNSTDBY = 1;
    OSCCTRL->DFLLCTRLA.bit.ONDEMAND = 0;

    OSCCTRL->DFLLCTRLA.bit.ENABLE = 1;
    while (OSCCTRL->DFLLSYNC.bit.ENABLE == 1) {
    }

    #if MICROPY_HW_DFLL_USB_SYNC
    // Configure the DFLL48M for USB clock recovery.
    // Will have to switch back if no USB
    dfll48m_calibration = OSCCTRL->DFLLVAL.reg;
    // Set the Multiplication factor.
    OSCCTRL->DFLLMUL.reg = OSCCTRL_DFLLMUL_MUL(48000) |
        OSCCTRL_DFLLMUL_FSTEP(1) | OSCCTRL_DFLLMUL_CSTEP(1);
    while (OSCCTRL->DFLLSYNC.bit.DFLLMUL == 1) {
    }
    // Set the mode to closed loop USB Recovery
    OSCCTRL->DFLLCTRLB.reg = OSCCTRL_DFLLCTRLB_USBCRM | OSCCTRL_DFLLCTRLB_CCDIS | OSCCTRL_DFLLCTRLB_MODE;
    while (OSCCTRL->DFLLSYNC.bit.DFLLCTRLB == 1) {
    }
    #endif

    #endif // MICROPY_HW_XOSC32K

    // Peripheral channel 1 is driven by GCLK1 and it feeds DPLL0
    GCLK->PCHCTRL[1].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN;
    while (GCLK->PCHCTRL[1].bit.CHEN == 0) {
    }

    set_cpu_freq(cpu_freq);

    peripheral_freq = DFLL48M_FREQ;  // To be changed if CPU_FREQ < 48M

    // Setup GCLK2 for DFLL48M output (48 MHz), may be scaled down later by calls to set_cpu_freq
    GCLK->GENCTRL[2].reg = GCLK_GENCTRL_DIV(1) | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL2) {
    }

    // Setup GCLK3 for 16MHz, Used for TC0/1 counter
    GCLK->GENCTRL[3].reg = GCLK_GENCTRL_DIV(3) | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL3) {
    }
}

void enable_sercom_clock(int id) {
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
