/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "lib/tinyusb/src/device/usbd.h"
#include "supervisor/background_callback.h"
#include "supervisor/usb.h"
#include "src/rp2_common/hardware_irq/include/hardware/irq.h"
#include "src/rp2_common/pico_platform/include/pico/platform.h"
#include "src/rp2040/hardware_regs/include/hardware/regs/intctrl.h"

void init_usb_hardware(void) {
}

STATIC void _usb_irq_wrapper(void) {
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
