// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/runtime.h"
#include "common-hal/async_spi/SPI.h"
#include "common-hal/microcontroller/Pin.h"
#include "supervisor/shared/async_flag.h"

extern const mp_obj_type_t async_spi_spi_type;

void common_hal_async_spi_spi_construct(async_spi_spi_obj_t *self, const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso);
bool common_hal_async_spi_spi_deinited(async_spi_spi_obj_t *self);
// Stops a running transfer.
void common_hal_async_spi_spi_deinit(async_spi_spi_obj_t *self);
// For busio.SPI built on this implementation (shared-module/busio/SPI.c).
void common_hal_async_spi_spi_never_reset(async_spi_spi_obj_t *self);
void common_hal_async_spi_spi_mark_deinit(async_spi_spi_obj_t *self);
uint32_t common_hal_async_spi_spi_get_frequency(async_spi_spi_obj_t *self);
circuitpy_async_lock_obj_t *common_hal_async_spi_spi_get_lock(async_spi_spi_obj_t *self);
uint8_t common_hal_async_spi_spi_get_polarity(async_spi_spi_obj_t *self);
uint8_t common_hal_async_spi_spi_get_phase(async_spi_spi_obj_t *self);

// Each start returns a context for state of that one operation, which end() and cancel() get
// back; NULL when the object holds the state.

// Applies the settings. The caller holds the lock, so no transfer is running.
void common_hal_async_spi_spi_configure(async_spi_spi_obj_t *self, uint32_t baudrate,
    uint8_t polarity, uint8_t phase, uint8_t bits);

// Each transfer start sets done once the transfer has finished, possibly from an interrupt. The
// buffers must stay valid until done is set. transfer_end() and transfer_cancel() act only on
// the transfer started with done.
void *common_hal_async_spi_spi_write_start(async_spi_spi_obj_t *self, const uint8_t *data,
    size_t len, circuitpy_async_flag_t *done);
void *common_hal_async_spi_spi_readinto_start(async_spi_spi_obj_t *self, uint8_t *data,
    size_t len, uint8_t write_value, circuitpy_async_flag_t *done);
void *common_hal_async_spi_spi_write_readinto_start(async_spi_spi_obj_t *self,
    const uint8_t *data_out, uint8_t *data_in, size_t len, circuitpy_async_flag_t *done);
// Finishes the transfer started with done, once done is set; false if the driver reported an
// error. For busio.SPI, which must not raise from a background callback.
bool common_hal_async_spi_spi_transfer_finish(async_spi_spi_obj_t *self, void *context,
    circuitpy_async_flag_t *done);
// The same for an await: raises on an error.
mp_obj_t common_hal_async_spi_spi_transfer_end(async_spi_spi_obj_t *self, void *context,
    circuitpy_async_flag_t *done);
void common_hal_async_spi_spi_transfer_cancel(async_spi_spi_obj_t *self, void *context,
    circuitpy_async_flag_t *done);
