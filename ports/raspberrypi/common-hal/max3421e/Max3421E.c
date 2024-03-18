/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
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

#include "shared-module/max3421e/Max3421E.h"

#include "lib/tinyusb/src/host/usbh.h"
#include "shared-bindings/busio/SPI.h"
#include "supervisor/usb.h"

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"

static max3421e_max3421e_obj_t *active_max = NULL;

static void _interrupt_wrapper(void) {
    if (active_max == NULL) {
        return;
    }
    size_t pin_number = active_max->irq.pin->number;
    if ((gpio_get_irq_event_mask(pin_number) & GPIO_IRQ_LEVEL_LOW) == 0) {
        return;
    }
    gpio_acknowledge_irq(pin_number, GPIO_IRQ_LEVEL_LOW);
    max3421e_interrupt_handler(active_max);
}

// Setup irq on self->irq to call tuh_int_handler(rhport, true) on falling edge
void common_hal_max3421e_max3421e_init_irq(max3421e_max3421e_obj_t *self) {
    size_t pin_number = self->irq.pin->number;
    active_max = self;
    gpio_add_raw_irq_handler(pin_number, _interrupt_wrapper);
    irq_set_enabled(IO_IRQ_BANK0, true);
    gpio_set_irq_enabled(pin_number, GPIO_IRQ_LEVEL_LOW, true);
}

void common_hal_max3421e_max3421e_deinit_irq(max3421e_max3421e_obj_t *self) {
    size_t pin_number = self->irq.pin->number;
    gpio_set_irq_enabled(pin_number, GPIO_IRQ_LEVEL_LOW, false);
    irq_set_enabled(IO_IRQ_BANK0, false);
    gpio_acknowledge_irq(pin_number, GPIO_IRQ_LEVEL_LOW);
    gpio_remove_raw_irq_handler(pin_number, _interrupt_wrapper);
    active_max = NULL;
}

// Enable or disable the irq interrupt.
void common_hal_max3421e_max3421e_irq_enabled(max3421e_max3421e_obj_t *self, bool enabled) {
    size_t pin_number = self->irq.pin->number;
    gpio_set_irq_enabled(pin_number, GPIO_IRQ_LEVEL_LOW, enabled);
}
