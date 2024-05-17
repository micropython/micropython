// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
