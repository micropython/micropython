/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "samd_soc.h"
#include "tusb.h"

static void uart0_init(void) {
    #if defined(MCU_SAMD21)

    // SERCOM0, TX=PA06=PAD2, RX=PA07=PAD3, ALT-D
    PORT->Group[0].PMUX[3].reg = 0x33;
    PORT->Group[0].PINCFG[6].reg = 1;
    PORT->Group[0].PINCFG[7].reg = 1;

    PM->APBCMASK.bit.SERCOM0_ = 1;
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_SERCOM0_CORE;
    while (GCLK->STATUS.bit.SYNCBUSY) { }

    uint32_t rxpo = 3;
    uint32_t txpo = 1;

    #elif defined(MCU_SAMD51)

    // SERCOM3, TX=PA17=PAD0, RX=PA16=PAD1, ALT-D
    PORT->Group[0].PMUX[8].reg = 0x33;
    PORT->Group[0].PINCFG[16].reg = 1;
    PORT->Group[0].PINCFG[17].reg = 1;

    // Use Generator 0 which is already enabled and switched to DFLL @ 48MHz
    GCLK->PCHCTRL[SERCOM3_GCLK_ID_CORE].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;
    MCLK->APBBMASK.bit.SERCOM3_ = 1;

    uint32_t rxpo = 1;
    uint32_t txpo = 2;

    #endif

    while (USARTx->USART.SYNCBUSY.bit.SWRST) { }
    USARTx->USART.CTRLA.bit.SWRST = 1;
    while (USARTx->USART.SYNCBUSY.bit.SWRST) { }

    USARTx->USART.CTRLA.reg =
        SERCOM_USART_CTRLA_DORD
        | SERCOM_USART_CTRLA_RXPO(rxpo)
        | SERCOM_USART_CTRLA_TXPO(txpo)
        | SERCOM_USART_CTRLA_MODE(1)
        ;
    USARTx->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN;
    while (USARTx->USART.SYNCBUSY.bit.CTRLB) { }
    #if CPU_FREQ == 8000000
    uint32_t baud = 50437; // 115200 baud; 65536*(1 - 16 * 115200/8e6)
    #elif CPU_FREQ == 48000000
    uint32_t baud = 63019; // 115200 baud; 65536*(1 - 16 * 115200/48e6)
    #elif CPU_FREQ == 120000000
    uint32_t baud = 64529; // 115200 baud; 65536*(1 - 16 * 115200/120e6)
    #endif
    USARTx->USART.BAUD.bit.BAUD = baud;
    USARTx->USART.CTRLA.bit.ENABLE = 1;
    while (USARTx->USART.SYNCBUSY.bit.ENABLE) { }
}

static void usb_init(void) {
    // Init USB clock
    #if defined(MCU_SAMD21)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_USB;
    PM->AHBMASK.bit.USB_ = 1;
    PM->APBBMASK.bit.USB_ = 1;
    uint8_t alt = 6; // alt G, USB
    #elif defined(MCU_SAMD51)
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK1;
    while (GCLK->PCHCTRL[USB_GCLK_ID].bit.CHEN == 0) { }
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

void samd_init(void) {
    #if defined(MCU_SAMD21)

    NVMCTRL->CTRLB.bit.MANW = 1; // errata "Spurious Writes"
    NVMCTRL->CTRLB.bit.RWS = 1; // 1 read wait state for 48MHz

    // Enable DFLL48M
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {}
    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_CSTEP(1) | SYSCTRL_DFLLMUL_FSTEP(1)
        | SYSCTRL_DFLLMUL_MUL(48000);
    uint32_t coarse = (*((uint32_t*)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk)
        >> FUSES_DFLL48M_COARSE_CAL_Pos;
    if (coarse == 0x3f) {
        coarse = 0x1f;
    }
    uint32_t fine = 512;
    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(fine);
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_CCDIS | SYSCTRL_DFLLCTRL_USBCRM
        | SYSCTRL_DFLLCTRL_MODE | SYSCTRL_DFLLCTRL_ENABLE;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {}

    GCLK->GENDIV.reg = GCLK_GENDIV_ID(0) | GCLK_GENDIV_DIV(1);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(0);
    while (GCLK->STATUS.bit.SYNCBUSY) { }

    // Configure PA10 as output for LED
    PORT->Group[0].DIRSET.reg = 1 << 10;

    #elif defined(MCU_SAMD51)

    GCLK->GENCTRL[1].reg = 1 << GCLK_GENCTRL_DIV_Pos | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL1) { }

    // Configure PA22 as output for LED
    PORT->Group[0].DIRSET.reg = 1 << 22;

    #endif

    SysTick_Config(CPU_FREQ / 1000);
    uart0_init();
    usb_init();
}
