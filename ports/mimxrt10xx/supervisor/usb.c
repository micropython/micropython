// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include "fsl_clock.h"
#include "tusb.h"

#include "supervisor/linker.h"
#include "supervisor/usb.h"

#include "imx_usb.h"

void init_usb_instance(mp_int_t instance) {
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
