// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/busio/SPI.h"
#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-module/displayio/Group.h"

#define CIRCUITPY_USB_MAX3421_INSTANCE 2

typedef struct {
    mp_obj_base_t base;
    busio_spi_obj_t *bus;
    digitalio_digitalinout_obj_t chip_select;
    digitalio_digitalinout_obj_t irq;
    uint32_t baudrate;
    bool bus_locked;
} max3421e_max3421e_obj_t;

// Ports need to implement these two functions in order to support pin interrupts.

// Setup irq on self->irq to call tuh_int_handler(rhport, true) on falling edge
void common_hal_max3421e_max3421e_init_irq(max3421e_max3421e_obj_t *self);

// Deinit the irq
void common_hal_max3421e_max3421e_deinit_irq(max3421e_max3421e_obj_t *self);

// Enable or disable the irq interrupt.
void common_hal_max3421e_max3421e_irq_enabled(max3421e_max3421e_obj_t *self, bool enabled);

// Queue up the actual interrupt handler for when the SPI bus is free.
void max3421e_interrupt_handler(max3421e_max3421e_obj_t *self);
