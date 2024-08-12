// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "lib/tinyusb/src/device/usbd.h"
#include "supervisor/background_callback.h"
#include "supervisor/usb.h"
#include "src/rp2_common/hardware_irq/include/hardware/irq.h"
#include "pico/platform.h"
#include "src/rp2040/hardware_regs/include/hardware/regs/intctrl.h"

void init_usb_hardware(void) {
}

static void _usb_irq_wrapper(void) {
    usb_irq_handler(0);
}

void post_usb_init(void) {
    irq_add_shared_handler(USBCTRL_IRQ, _usb_irq_wrapper,
        PICO_SHARED_IRQ_HANDLER_LOWEST_ORDER_PRIORITY);

    // There is a small window where the USB interrupt may be handled by the
    // pico-sdk instead of CircuitPython. If that is the case, then we'll have
    // USB events to process that we didn't queue up a background task for. So,
    // queue one up here even if we might not have anything to do.
    usb_background_schedule();
}
