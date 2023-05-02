/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

#include "fsl_clock.h"
#include "tusb.h"

#include "supervisor/linker.h"
#include "supervisor/usb.h"

STATIC void init_usb_instance(mp_int_t instance) {
    if (instance < 0) {
        return;
    }
    USBPHY_Type *usb_phy;
    #ifdef USBPHY2
    if (instance == 0) {
        usb_phy = USBPHY1;
    #else
    (void)instance;
    usb_phy = USBPHY;
        #endif
        CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, 480000000U);

    #ifdef USBPHY2
    } else if (instance == 1) {
        CLOCK_EnableUsbhs1PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs1Clock(kCLOCK_Usb480M, 480000000U);
        usb_phy = USBPHY2;
    } else {
        // Unsupported instance
        return;
    }
    #endif

        // Enable PHY support for Low speed device + LS via FS Hub
        usb_phy->CTRL |= USBPHY_CTRL_SET_ENUTMILEVEL2_MASK | USBPHY_CTRL_SET_ENUTMILEVEL3_MASK;

        // Enable all power for normal operation
        usb_phy->PWD = 0;

        // TX Timing
        uint32_t phytx = usb_phy->TX;
        phytx &= ~(USBPHY_TX_D_CAL_MASK | USBPHY_TX_TXCAL45DM_MASK | USBPHY_TX_TXCAL45DP_MASK);
        phytx |= USBPHY_TX_D_CAL(0x0C) | USBPHY_TX_TXCAL45DP(0x06) | USBPHY_TX_TXCAL45DM(0x06);
        usb_phy->TX = phytx;
    }

    void init_usb_hardware(void) {
        init_usb_instance(CIRCUITPY_USB_DEVICE_INSTANCE);
        // We can't dynamically start the USB Host port at the moment, so do it
        // up front.
        init_usb_instance(CIRCUITPY_USB_HOST_INSTANCE);
    }

// Provide the prototypes for the interrupt handlers. The iMX RT SDK doesn't.
// The SDK only links to them from assembly.
    #ifdef MIMXRT1042_SERIES
    void USB_OTG_IRQHandler(void);
    void PLACE_IN_ITCM(USB_OTG_IRQHandler)(void) {
    #else
    void USB_OTG1_IRQHandler(void);
    void PLACE_IN_ITCM(USB_OTG1_IRQHandler)(void) {
        #endif
        usb_irq_handler(0);
    }

    #ifdef USBPHY2
    void USB_OTG2_IRQHandler(void);
    void PLACE_IN_ITCM(USB_OTG2_IRQHandler)(void) {
        usb_irq_handler(1);
    }
    #endif
