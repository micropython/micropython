/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * This file initialises the USB (tinyUSB) and USART (SERCOM). Board USART settings
 * are set in 'boards/<board>/mpconfigboard.h.
 *
 * IMPORTANT: Please refer to "I/O Multiplexing and Considerations" chapters
 *            in device datasheets for I/O Pin functions and assignments.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "py/runtime.h"
#include "modmachine.h"
#include "samd_soc.h"
#include "sam.h"
#include "tusb.h"

uint8_t enable_uart_repl = false;
uint32_t cpu_freq = CPU_FREQ;

// "MP" macros defined in "boards/$(BOARD)/mpconfigboard.h"
mp_obj_t machine_uart_init(void) {
    // Firstly, assign alternate function SERCOM PADs to GPIO pins.
    PORT->Group[MP_PIN_GRP].PINCFG[MP_TX_PIN].bit.PMUXEN = 1; // Enable
    PORT->Group[MP_PIN_GRP].PINCFG[MP_RX_PIN].bit.PULLEN = 1; // Enable Pull avoiding crosstalk
    PORT->Group[MP_PIN_GRP].PINCFG[MP_RX_PIN].bit.PMUXEN = 1; // Enable MUX
    PORT->Group[MP_PIN_GRP].PMUX[MP_PERIPHERAL_MUX].reg = MP_PORT_FUNC; // Sets PMUXE & PMUXO in 1 hit.
    uint32_t rxpo = MP_RXPO_PAD; // 1=Pad1,3=Pad3 Rx data
    uint32_t txpo = MP_TXPO_PAD; // 0=pad0,1=Pad2 Tx data

    // Initialise the clocks
    #if defined(MCU_SAMD21)
    PM->APBCMASK.bit.MP_SERCOMx = 1; // Enable synchronous clock
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | MP_SERCOM_GCLK_ID_x_CORE; // Select multiplexer generic clock source and enable.
    // Wait while it updates synchronously.
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    #elif defined(MCU_SAMD51)
    GCLK->PCHCTRL[MP_SERCOM_GCLK_ID_x_CORE].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK2;
    MCLK->APBBMASK.bit.MP_SERCOMx = 1;
    #endif

    // Setup the Peripheral.
    // Reset (clear) the peripheral registers.
    while (USARTx->USART.SYNCBUSY.bit.SWRST) {
    }
    USARTx->USART.CTRLA.bit.SWRST = 1; // Reset all Registers, disable peripheral
    while (USARTx->USART.SYNCBUSY.bit.SWRST) {
    }

    // Set the register bits as needed
    // (CMODE (async),CHSIZE (8),FORM (no parity),SBMODE (1 stop) already 0).
    USARTx->USART.CTRLA.reg =   // USARTx = SERCOMx set in "boards/$(BOARD)/mpconfigboard.h"
        SERCOM_USART_CTRLA_DORD // Data order
        | SERCOM_USART_CTRLA_RXPO(rxpo) // Set Pad#
        | SERCOM_USART_CTRLA_TXPO(txpo) // Set Pad#
        | SERCOM_USART_CTRLA_MODE(1) // USART with internal clock
    ;
    USARTx->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN; // Enable Rx & Tx
    while (USARTx->USART.SYNCBUSY.bit.CTRLB) {
    }

    // USART is driven by the 48MHz clock of GCLK Generator 2
    uint32_t baud = 63019; // 115200 baud; 65536*(1 - 16 * 115200/48e6)
    USARTx->USART.BAUD.bit.BAUD = baud; // Set Baud
    USARTx->USART.CTRLA.bit.ENABLE = 1; // Enable the peripheral
    // Wait for the Registers to update.
    while (USARTx->USART.SYNCBUSY.bit.ENABLE) {
    }

    enable_uart_repl = true;
    return mp_const_none;
}

// Disconnect SERCOM from GPIO pins. (Can't SWRST, as that will totally kill USART).
mp_obj_t machine_uart_deinit(void) {
    // Reset
    enable_uart_repl = false;
    printf("Disabling the Alt-Funct, releasing the USART pins for GPIO... \n");
    PORT->Group[MP_PIN_GRP].PINCFG[MP_TX_PIN].bit.PMUXEN = 0; // Disable
    PORT->Group[MP_PIN_GRP].PINCFG[MP_RX_PIN].bit.PMUXEN = 0; // Disable
    PORT->Group[MP_PIN_GRP].PINCFG[MP_RX_PIN].bit.PULLEN = 0; // Disable Pull

    return mp_const_none;
}

static void usb_init(void) {
    // Init USB clock
    #if defined(MCU_SAMD21)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_USB;
    PM->AHBMASK.bit.USB_ = 1;
    PM->APBBMASK.bit.USB_ = 1;
    uint8_t alt = 6; // alt G, USB
    #elif defined(MCU_SAMD51)
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK3;
    while (GCLK->PCHCTRL[USB_GCLK_ID].bit.CHEN == 0) {
    }
    MCLK->AHBMASK.bit.USB_ = 1;
    MCLK->APBBMASK.bit.USB_ = 1;
    uint8_t alt = 7; // alt H, USB
    #endif

    // Init USB pins
    PORT->Group[0].DIRSET.reg = 1 << 25 | 1 << 24;
    PORT->Group[0].OUTCLR.reg = 1 << 25 | 1 << 24;
    PORT->Group[0].PMUX[12].reg = alt << 4 | alt;
    PORT->Group[0].PINCFG[24].reg = PORT_PINCFG_PMUXEN;
    PORT->Group[0].PINCFG[25].reg = PORT_PINCFG_PMUXEN;

    tusb_init();
}

// Initialize the µs counter on TC 0/1
void init_us_counter(void) {
    #if defined(MCU_SAMD51)

    // Peripheral channel 9 is driven by GCLK4, 2 MHz.
    GCLK->PCHCTRL[TC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK4 | GCLK_PCHCTRL_CHEN;

    MCLK->APBAMASK.bit.TC0_ = 1; // Enable TC0 clock
    MCLK->APBAMASK.bit.TC1_ = 1; // Enable TC1 clock

    // configure the timer
    TC0->COUNT32.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT32_Val;
    TC0->COUNT32.CTRLA.bit.RUNSTDBY = 1;
    TC0->COUNT32.CTRLA.bit.ENABLE = 1;
    while (TC0->COUNT32.SYNCBUSY.bit.ENABLE) {
    }

    #endif
}

void init_clocks(uint32_t cpu_freq, uint8_t full_config) {
    #if defined(MCU_SAMD21)

    // SAMD21 Clock settings
    // GCLK0: 48MHz from DFLL
    // GCLK1: 32768 Hz from 32K ULP or 32k Crystal
    // GCLK2: 48MHz from DFLL for USB and SERCOM
    // GCLK3: 48Mhz from DFLL
    // GCLK4: 2Mhz for TC0/1 counter

    NVMCTRL->CTRLB.bit.MANW = 1; // errata "Spurious Writes"
    NVMCTRL->CTRLB.bit.RWS = 1; // 1 read wait state for 48MHz

    // Enable DFLL48M
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {
    }
    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_CSTEP(1) | SYSCTRL_DFLLMUL_FSTEP(1)
        | SYSCTRL_DFLLMUL_MUL(48000);
    uint32_t coarse = (*((uint32_t *)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk)
        >> FUSES_DFLL48M_COARSE_CAL_Pos;
    if (coarse == 0x3f) {
        coarse = 0x1f;
    }
    uint32_t fine = 512;
    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(fine);
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_CCDIS | SYSCTRL_DFLLCTRL_USBCRM
        | SYSCTRL_DFLLCTRL_MODE | SYSCTRL_DFLLCTRL_ENABLE;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {
    }
    // Enable GCLK output: 48M on both CCLK0 and GCLK2
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(0) | GCLK_GENDIV_DIV(1);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(0);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(1);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(2);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    // Enable 32768 Hz on GCLK1
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(1) | GCLK_GENDIV_DIV(1);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(1);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    // Enable GCLK output: 2M on GCLK4
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(4) | GCLK_GENDIV_DIV(24);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(4);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }

    #elif defined(MCU_SAMD51)

    // SAMD51 clock settings
    // GCLK0: 48MHz from DFLL or 48 - 200 MHz from DPLL0 (SAMD51)
    // GCLK1: DPLLx_REF_FREQ 32768 Hz from 32KULP or 32k Crystal
    // GCLK2: 48MHz from DPLL1 for USB and SERCOM
    // GCLK3: 48Mhz from DFLL
    // GCLK4: 2Mhz for TC0/1 counter
    // DPLL0: 48 - 200 MHz
    // DPLL1: 48 MHz

    // Steps to set up clocks:
    // Reset Clocks
    // Switch GCLK0 to DFLL 48MHz
    // Setup 32768 Hz source, if a crystal is present.
    // Setup GCLK1 to the DPLL0 Reference freq. of 32768 Hz
    // Setup GCLK1 to drive peripheral channel 1
    // Setup DPLL0 to 120MHz
    // Setup GCLK0 to 120MHz
    // Setup GCLK1 to drive peripheral channel 2
    // Setup DPLL1 to 48Mhz
    // Setup GCLK2 to 48MHz for USB and SERCOM
    // Setup GCLK3 to 48MHz for USB and SERCOM alt source
    // Setup GCLK4 to 1MHz and route to TC0/TC1

    // Setup GCLK0 for 48MHz as default state to keep the MCU running during config change.
    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL0) {
    }

    if (full_config) {
        #if MICROPY_HW_XOSC32K
        // OSCILLATOR CONTROL
        // Setup XOSC32K
        OSC32KCTRL->XOSC32K.bit.CGM = 01;
        OSC32KCTRL->XOSC32K.bit.XTALEN = 1; // 0: Generator 1: Crystal
        OSC32KCTRL->XOSC32K.bit.EN32K = 1;
        OSC32KCTRL->XOSC32K.bit.ONDEMAND = 1;
        OSC32KCTRL->XOSC32K.bit.RUNSTDBY = 1;
        OSC32KCTRL->XOSC32K.bit.STARTUP = 0;
        OSC32KCTRL->XOSC32K.bit.ENABLE = 1;
        OSC32KCTRL->CFDCTRL.bit.CFDPRESC = 0;
        OSC32KCTRL->CFDCTRL.bit.SWBACK = 0;
        OSC32KCTRL->CFDCTRL.bit.CFDEN = 0;
        OSC32KCTRL->EVCTRL.bit.CFDEO = 0;
        // make sure osc32kcrtl is ready
        while (OSC32KCTRL->STATUS.bit.XOSC32KRDY == 0) {
        }

        // Setup GCLK1 for 32kHz crystal
        GCLK->GENCTRL[1].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K;
        while (GCLK->SYNCBUSY.bit.GENCTRL1) {
        }

        #else
        // Set GCLK1 to DPLL0_REF_FREQ as defined in mpconfigboard.h (e.g. 32768 Hz)
        // GCLK->GENCTRL[1].reg = (48000000 / DPLL0_REF_FREQ) << GCLK_GENCTRL_DIV_Pos | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;

        // Setup GCLK1 for 32kHz ULP
        GCLK->GENCTRL[1].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K;
        while (GCLK->SYNCBUSY.bit.GENCTRL1) {
        }
        #endif // MICROPY_HW_CRYSTAL
    
        // Peripheral channel 1 is driven by GCLK1 and it feeds DPLL0
        GCLK->PCHCTRL[1].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN;
    }

    // Setup DPLL0 for 120 MHz
    // first: disable DPLL0 in case it is running
    OSCCTRL->Dpll[0].DPLLCTRLA.bit.ENABLE = 0;
    while (OSCCTRL->Dpll[0].DPLLSYNCBUSY.bit.ENABLE == 1) {
    }
    // Now configure the registers
    OSCCTRL->Dpll[0].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_DIV(1) | OSCCTRL_DPLLCTRLB_LBYPASS |
        OSCCTRL_DPLLCTRLB_REFCLK(0) | OSCCTRL_DPLLCTRLB_WUF;

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

    if (full_config) {

        // Peripheral channel 2 is driven by GCLK1 and it feeds DPLL1
        GCLK->PCHCTRL[2].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN;

        // Setup DPLL1 for PERIPHERAL_FREQ (48 MHz)
        // first: disable DPLL1 in case it is running
        OSCCTRL->Dpll[1].DPLLCTRLA.bit.ENABLE = 0;
        while (OSCCTRL->Dpll[1].DPLLSYNCBUSY.bit.ENABLE == 1) {
        }
        // Now configure the registers
        OSCCTRL->Dpll[1].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_DIV(1) | OSCCTRL_DPLLCTRLB_LBYPASS |
            OSCCTRL_DPLLCTRLB_REFCLK(0) | OSCCTRL_DPLLCTRLB_WUF;

        div = PERIPHERAL_FREQ / DPLLx_REF_FREQ;
        frac = (PERIPHERAL_FREQ - div * DPLLx_REF_FREQ) / (DPLLx_REF_FREQ / 32);
        OSCCTRL->Dpll[1].DPLLRATIO.reg = (frac << 16) + div - 1;
        // enable it again
        OSCCTRL->Dpll[1].DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE | OSCCTRL_DPLLCTRLA_RUNSTDBY;

        // Per errata 2.13.1
        while (!(OSCCTRL->Dpll[1].DPLLSTATUS.bit.CLKRDY == 1)) {
        }

        // Setup GCLK2 for DPLL1 output (48 or 48-200MHz)
        GCLK->GENCTRL[2].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DPLL1;
        while (GCLK->SYNCBUSY.bit.GENCTRL2) {
        }

        // Setup GCLK3 for 48MHz, Used alternatively for USART. USB
        GCLK->GENCTRL[3].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
        while (GCLK->SYNCBUSY.bit.GENCTRL3) {
        }

        // Setup GCLK4 for 2MHz, Used for TC0/1 counter
        GCLK->GENCTRL[4].reg = 24 << GCLK_GENCTRL_DIV_Pos | GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DPLL1;
        while (GCLK->SYNCBUSY.bit.GENCTRL4) {
        }
    }
    #endif // defined(MCU_SAMD51)
}


void samd_init(void) {
    init_clocks(cpu_freq, true);
    SysTick_Config(cpu_freq / 1000);
    init_us_counter();
    machine_uart_init();
    usb_init();
}
