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
#include "tusb.h"

// "MP" macros defined in "boards/$(BOARD)/mpconfigboard.h"
mp_obj_t machine_uart_init(void) {
    // Firstly, assign alternate function SERCOM PADs to GPIO pins.
    PORT->Group[MP_PIN_GRP].PINCFG[MP_TX_PIN].bit.PMUXEN = 1; // Enable
    PORT->Group[MP_PIN_GRP].PINCFG[MP_RX_PIN].bit.PMUXEN = 1; // Enable
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
    GCLK->PCHCTRL[MP_SERCOM_GCLK_ID_x_CORE].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;
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

    // Baud rate is clock dependant.
    #if CPU_FREQ == 8000000
    uint32_t baud = 50437; // 115200 baud; 65536*(1 - 16 * 115200/8e6)
    #elif CPU_FREQ == 48000000
    uint32_t baud = 63019; // 115200 baud; 65536*(1 - 16 * 115200/48e6)
    #elif CPU_FREQ == 120000000
    uint32_t baud = 64529; // 115200 baud; 65536*(1 - 16 * 115200/120e6)
    #endif
    USARTx->USART.BAUD.bit.BAUD = baud; // Set Baud
    USARTx->USART.CTRLA.bit.ENABLE = 1; // Enable the peripheral
    // Wait for the Registers to update.
    while (USARTx->USART.SYNCBUSY.bit.ENABLE) {
    }

    return mp_const_none;
}

// Disconnect SERCOM from GPIO pins. (Can't SWRST, as that will totally kill USART).
mp_obj_t machine_uart_deinit(void) {
    // Reset
    printf("Disabling the Alt-Funct, releasing the USART pins for GPIO... \n");
    PORT->Group[MP_PIN_GRP].PINCFG[MP_TX_PIN].bit.PMUXEN = 0; // Disable
    PORT->Group[MP_PIN_GRP].PINCFG[MP_RX_PIN].bit.PMUXEN = 0; // Disable

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
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK1;
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

void samd_init(void) {
    #if defined(MCU_SAMD21)

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

    GCLK->GENDIV.reg = GCLK_GENDIV_ID(0) | GCLK_GENDIV_DIV(1);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(0);
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }

    #elif defined(MCU_SAMD51)

    GCLK->GENCTRL[1].reg = 1 << GCLK_GENCTRL_DIV_Pos | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL;
    while (GCLK->SYNCBUSY.bit.GENCTRL1) {
    }

    #endif

    SysTick_Config(CPU_FREQ / 1000);
    machine_uart_init();
    usb_init();
}
