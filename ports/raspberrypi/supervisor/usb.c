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

void post_usb_init(void) {
    irq_set_enabled(USBCTRL_IRQ, false);

    irq_handler_t usb_handler = irq_get_exclusive_handler(USBCTRL_IRQ);
    if (usb_handler) {
        irq_remove_handler(USBCTRL_IRQ, usb_handler);
    }
    irq_set_exclusive_handler(USBCTRL_IRQ, usb_irq_handler);

    irq_set_enabled(USBCTRL_IRQ, true);
}
