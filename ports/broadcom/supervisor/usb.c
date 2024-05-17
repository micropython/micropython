// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "lib/tinyusb/src/device/usbd.h"
#include "py/runtime.h"
#include "supervisor/background_callback.h"
#include "supervisor/usb.h"

#include "peripherals/broadcom/interrupts.h"
#include "peripherals/broadcom/vcmailbox.h"

uint32_t SystemCoreClock = 700 * 1000 * 1000;

void USB_IRQHandler(void) {
    usb_irq_handler(0);
}

void init_usb_hardware(void) {
    vcmailbox_set_power_state(VCMAILBOX_DEVICE_USB_HCD, true);

    BP_SetPriority(USB_IRQn, 0x00);
    BP_ClearPendingIRQ(USB_IRQn);
    BP_EnableIRQ(USB_IRQn);
    BP_EnableIRQs();
}
