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

#include "py/mphal.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "samd_soc.h"
#include "sam.h"
#include "tusb.h"

extern void machine_rtc_start(bool force);

static void usb_init(void) {
    // Init USB clock
    #if defined(MCU_SAMD21)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK5 | GCLK_CLKCTRL_ID_USB;
    PM->AHBMASK.bit.USB_ = 1;
    PM->APBBMASK.bit.USB_ = 1;
    uint8_t alt = 6; // alt G, USB
    #elif defined(MCU_SAMD51)
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK5;
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
}

// Initialize the µs counter on TC 0/1 or TC4/5
void init_us_counter(void) {
    #if defined(MCU_SAMD21)

    PM->APBCMASK.bit.TC3_ = 1; // Enable TC3 clock
    PM->APBCMASK.bit.TC4_ = 1; // Enable TC4 clock
    // Select multiplexer generic clock source and enable.
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK3 | GCLK_CLKCTRL_ID_TC4_TC5;
    // Wait while it updates synchronously.
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }

    // configure the timer
    TC4->COUNT32.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT32_Val;
    TC4->COUNT32.CTRLA.bit.RUNSTDBY = 1;
    TC4->COUNT32.CTRLA.bit.ENABLE = 1;
    while (TC4->COUNT32.STATUS.bit.SYNCBUSY) {
    }
    TC4->COUNT32.READREQ.reg = TC_READREQ_RREQ | TC_READREQ_RCONT | 0x10;
    while (TC4->COUNT32.STATUS.bit.SYNCBUSY) {
    }
    // Enable the IRQ
    TC4->COUNT32.INTENSET.reg = TC_INTENSET_OVF;
    NVIC_EnableIRQ(TC4_IRQn);

    #elif defined(MCU_SAMD51)

    MCLK->APBAMASK.bit.TC0_ = 1; // Enable TC0 clock
    MCLK->APBAMASK.bit.TC1_ = 1; // Enable TC1 clock
    // Peripheral channel 9 is driven by GCLK3, 8 MHz.
    GCLK->PCHCTRL[TC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK3 | GCLK_PCHCTRL_CHEN;
    while (GCLK->PCHCTRL[TC0_GCLK_ID].bit.CHEN == 0) {
    }

    // configure the timer
    TC0->COUNT32.CTRLA.bit.PRESCALER = 0;
    TC0->COUNT32.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT32_Val;
    TC0->COUNT32.CTRLA.bit.RUNSTDBY = 1;
    TC0->COUNT32.CTRLA.bit.ENABLE = 1;
    while (TC0->COUNT32.SYNCBUSY.bit.ENABLE) {
    }

    // Enable the IRQ
    TC0->COUNT32.INTENSET.reg = TC_INTENSET_OVF;
    NVIC_EnableIRQ(TC0_IRQn);
    #endif
}

void samd_init(void) {
    init_clocks(get_cpu_freq());
    init_us_counter();
    usb_init();
    #if defined(MCU_SAMD51)
    mp_hal_ticks_cpu_enable();
    #endif
    machine_rtc_start(false);
}

#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_I2C_TARGET || MICROPY_PY_MACHINE_SPI || MICROPY_PY_MACHINE_UART

Sercom *sercom_instance[] = SERCOM_INSTS;
MP_REGISTER_ROOT_POINTER(void *sercom_table[SERCOM_INST_NUM]);

// Common Sercom functions used by all Serial devices
void sercom_enable(Sercom *uart, int state) {
    uart->USART.CTRLA.bit.ENABLE = state; // Set the state on/off
    // Wait for the Registers to update.
    while (uart->USART.SYNCBUSY.bit.ENABLE) {
    }
}

void sercom_deinit_all(void) {
    for (int i = 0; i < SERCOM_INST_NUM; i++) {
        Sercom *uart = sercom_instance[i];
        uart->USART.INTENCLR.reg = 0xff;
        sercom_register_irq(i, NULL);
        sercom_enable(uart, 0);
        MP_STATE_PORT(sercom_table[i]) = NULL;
    }
}

#endif

void samd_get_unique_id(samd_unique_id_t *id) {
    // Atmel SAM D21E / SAM D21G / SAM D21J
    // SMART ARM-Based Microcontroller
    // DATASHEET
    // 9.6 (SAMD51) or 9.3.3 (or 10.3.3 depending on which manual)(SAMD21) Serial Number
    //
    // EXAMPLE (SAMD21)
    // ----------------
    // OpenOCD:
    // Word0:
    // > at91samd21g18.cpu mdw 0x0080A00C 1
    // 0x0080a00c: 6e27f15f
    // Words 1-3:
    // > at91samd21g18.cpu mdw 0x0080A040 3
    // 0x0080a040: 50534b54 332e3120 ff091645
    //
    // MicroPython (this code and same order as shown in Arduino IDE)
    // >>> binascii.hexlify(machine.unique_id())
    // b'6e27f15f50534b54332e3120ff091645'

    #if defined(MCU_SAMD21)
    uint32_t *id_addresses[4] = {(uint32_t *)0x0080A00C, (uint32_t *)0x0080A040,
                                 (uint32_t *)0x0080A044, (uint32_t *)0x0080A048};
    #elif defined(MCU_SAMD51)
    uint32_t *id_addresses[4] = {(uint32_t *)0x008061FC, (uint32_t *)0x00806010,
                                 (uint32_t *)0x00806014, (uint32_t *)0x00806018};
    #endif

    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 4; k++) {
            // 'Reverse' the read bytes into a 32 bit word (Consistent with Arduino)
            id->bytes[4 * i + k] = (*(id_addresses[i]) >> (24 - k * 8)) & 0xff;
        }
    }
}
