// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/max3421e/Max3421E.h"

#include <stdint.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/background_callback.h"
#include "supervisor/usb.h"

#include "tusb.h"

max3421e_max3421e_obj_t *_active;

// Use CP's background callbacks to run the "interrupt" handler. The GPIO ISR
// stack is too small to do the SPI transactions that the interrupt handler does.
static background_callback_t tuh_callback;

void common_hal_max3421e_max3421e_construct(max3421e_max3421e_obj_t *self,
    busio_spi_obj_t *spi, const mcu_pin_obj_t *chip_select,
    const mcu_pin_obj_t *irq, uint32_t baudrate) {

    #if CIRCUITPY_USB_HOST
    if (tuh_inited()) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_usb_host);
    }
    #endif

    if (_active != NULL && !common_hal_max3421e_max3421e_deinited(_active)) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_max3421e);
        return;
    }

    common_hal_digitalio_digitalinout_construct(&self->chip_select, chip_select);
    common_hal_digitalio_digitalinout_switch_to_output(&self->chip_select, true, DRIVE_MODE_PUSH_PULL);

    common_hal_digitalio_digitalinout_construct(&self->irq, irq);

    common_hal_max3421e_max3421e_init_irq(self);

    self->bus = spi;
    self->baudrate = baudrate;
    _active = self;

    tuh_configure(CIRCUITPY_USB_MAX3421_INSTANCE, 0, NULL);
    tuh_init(CIRCUITPY_USB_MAX3421_INSTANCE);
}

bool common_hal_max3421e_max3421e_deinited(max3421e_max3421e_obj_t *self) {
    return self->bus == NULL;
}

void common_hal_max3421e_max3421e_deinit(max3421e_max3421e_obj_t *self) {
    if (common_hal_max3421e_max3421e_deinited(self)) {
        return;
    }
    common_hal_max3421e_max3421e_deinit_irq(self);

    tuh_deinit(CIRCUITPY_USB_MAX3421_INSTANCE);

    common_hal_digitalio_digitalinout_deinit(&self->chip_select);
    common_hal_digitalio_digitalinout_deinit(&self->irq);
    self->bus = NULL;

    if (_active == self) {
        _active = NULL;
    }
}

// TinyUSB uses the frame number from the host interface to measure time but it
// isn't updated for the Max3421E in an interrupt. Instead, use CP time keeping
// and run the interrupt handler as needed. Leave it in the background queue
// anyway. Don't run background tasks because this function is used by
// tuh_task() which is run as a background task.
#if CFG_TUSB_OS == OPT_OS_NONE
void osal_task_delay(uint32_t msec) {
    uint32_t end_time = common_hal_time_monotonic_ms() + msec;
    while (common_hal_time_monotonic_ms() < end_time) {
        if (tuh_callback.prev != NULL) {
            tuh_int_handler(CIRCUITPY_USB_MAX3421_INSTANCE, false);
        }
    }
}
#endif

static void tuh_interrupt_callback(void *data) {
    max3421e_max3421e_obj_t *self = (max3421e_max3421e_obj_t *)data;
    // Check that the stack is still going. This callback may have been delayed
    // enough that it was deinit in the meantime.
    if (tuh_inited()) {
        // Try and lock the spi bus. If we can't, then we may be in the middle of a transaction (it
        // calls RUN_BACKGROUND_TASKS.)
        if (!common_hal_busio_spi_try_lock(self->bus)) {
            // Come back to us.
            background_callback_add(&tuh_callback, tuh_interrupt_callback, (void *)self);

            return;
        }
        // Unlock the bus so the interrupt handler can use it.
        common_hal_busio_spi_unlock(self->bus);
        tuh_int_handler(CIRCUITPY_USB_MAX3421_INSTANCE, false);
        common_hal_max3421e_max3421e_irq_enabled(self, true);
        usb_background_schedule();
    }
}

// Ports must call this from their interrupt handler.
void max3421e_interrupt_handler(max3421e_max3421e_obj_t *arg) {
    max3421e_max3421e_obj_t *self = (max3421e_max3421e_obj_t *)arg;
    // Schedule the CP background callback.
    background_callback_add(&tuh_callback, tuh_interrupt_callback, (void *)self);
    common_hal_max3421e_max3421e_irq_enabled(self, false);
}

// API to control MAX3421 SPI CS
void tuh_max3421_spi_cs_api(uint8_t rhport, bool active) {
    max3421e_max3421e_obj_t *self = _active;
    // The SPI bus may be deinit before us so check for that.
    if (common_hal_busio_spi_deinited(self->bus)) {
        self->bus_locked = false;
        return;
    }
    if (active) {
        assert(self->bus_locked == false);
        if (!common_hal_busio_spi_try_lock(self->bus)) {
            return;
        }
        self->bus_locked = true;
        common_hal_busio_spi_configure(self->bus, self->baudrate, 0, 0, 8);
        common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    } else if (self->bus_locked) {
        common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
        common_hal_busio_spi_unlock(self->bus);
        self->bus_locked = false;
    }
}

// API to transfer data with MAX3421 SPI
// Either tx_buf or rx_buf can be NULL, which means transfer is write or read only
extern bool tuh_max3421_spi_xfer_api(uint8_t rhport, uint8_t const *tx_buf, uint8_t *rx_buf, size_t xfer_bytes) {
    max3421e_max3421e_obj_t *self = _active;
    if (!self->bus_locked || common_hal_busio_spi_deinited(self->bus)) {
        return false;
    }
    if (tx_buf == NULL) {
        return common_hal_busio_spi_read(self->bus, rx_buf, xfer_bytes, 0xff);
    }
    if (rx_buf == NULL) {
        return common_hal_busio_spi_write(self->bus, tx_buf, xfer_bytes);
    }
    return common_hal_busio_spi_transfer(self->bus, tx_buf, rx_buf, xfer_bytes);
}

// API to enable/disable MAX3421 INTR pin interrupt
void tuh_max3421_int_api(uint8_t rhport, bool enabled) {
    max3421e_max3421e_obj_t *self = _active;
    // Always turn off the interrupt if the SPI bus is deinit.
    if (common_hal_busio_spi_deinited(self->bus)) {
        enabled = false;
    }
    common_hal_max3421e_max3421e_irq_enabled(self, enabled);
}
