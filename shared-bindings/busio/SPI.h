// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/runtime.h"
#include "supervisor/shared/async_flag.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/busio/SPI.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t busio_spi_type;

// Argument tables, also used by async_spi.SPI, which takes the same arguments.
extern const mp_arg_t busio_spi_make_new_args[4];      // clock, MOSI, MISO, half_duplex
extern const mp_arg_t busio_spi_configure_args[4];
extern const mp_arg_t busio_spi_write_args[3];
extern const mp_arg_t busio_spi_readinto_args[4];
extern const mp_arg_t busio_spi_write_readinto_args[6];

// Construct an underlying SPI object.
extern void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex);

extern void common_hal_busio_spi_deinit(busio_spi_obj_t *self);
extern bool common_hal_busio_spi_deinited(busio_spi_obj_t *self);

// Mark as deinit without deiniting. This is used by displayio after copying the
// object elsewhere and prevents the heap from deiniting the object.
extern void common_hal_busio_spi_mark_deinit(busio_spi_obj_t *self);

extern bool common_hal_busio_spi_configure(busio_spi_obj_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits);

extern bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self);
extern bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self);
extern void common_hal_busio_spi_unlock(busio_spi_obj_t *self);

// Writes out the given data.
extern bool common_hal_busio_spi_write(busio_spi_obj_t *self, const uint8_t *data, size_t len);

#if CIRCUITPY_BUSIO_SPI_ASYNC
// Start writing data and return, possibly before it has been sent. *done is set once it has been
// sent, at the latest by common_hal_busio_spi_end(). data must stay valid and unchanged, and
// the bus must not be used, until then.
extern void common_hal_busio_spi_write_start(busio_spi_obj_t *self, const uint8_t *data, size_t len,
    circuitpy_async_flag_t *done);
// Finish the transfer started by a common_hal_busio_spi_*_start(), waiting if needed. Returns
// at once if there is none.
extern void common_hal_busio_spi_end(busio_spi_obj_t *self);
#endif

// Reads in len bytes while outputting the byte write_value.
extern bool common_hal_busio_spi_read(busio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_value);

// Reads and write len bytes simultaneously.
extern bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len);

// Return actual SPI bus frequency.
uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self);

// Return SPI bus phase.
uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self);

// Return SPI bus polarity.
uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self);

// This is used by the supervisor to claim SPI devices indefinitely.
#if CIRCUITPY_BULK_RESET
extern void common_hal_busio_spi_never_reset(busio_spi_obj_t *self);
#endif

extern busio_spi_obj_t *validate_obj_is_spi_bus(mp_obj_t obj_in, qstr arg_name);

// Wait as long as needed for the lock. This is used by SD card access from USB.
// For most ports, busy-wait while running the background tasks.
MP_WEAK bool common_hal_busio_spi_wait_for_lock(busio_spi_obj_t *self, uint32_t timeout_ms);
