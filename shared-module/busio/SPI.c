// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/busio/SPI.h"

#include "py/runtime.h"
#include "supervisor/background_callback.h"
#include "shared/runtime/interrupt_char.h"

// False when interrupted (Ctrl-C); the operation is then still running.
static bool wait(circuitpy_async_flag_t *done) {
    while (!CIRCUITPY_ASYNC_FLAG_IS_SET(done)) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            return false;
        }
    }
    return true;
}

#if CIRCUITPY_BUSIO_SPI_ASYNC
// Finish a write_start(). No background tasks: the caller holds the bus inside a transaction,
// and one of them may want it.
static void end_sent(busio_spi_obj_t *self) {
    if (self->sent == NULL) {
        return;
    }
    while (!CIRCUITPY_ASYNC_FLAG_IS_SET(self->sent)) {
    }
    common_hal_async_spi_spi_transfer_finish(&self->async_spi, self->sent_context, self->sent);
    self->sent = NULL;
}
#endif

void common_hal_busio_spi_construct(busio_spi_obj_t *self, const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso, bool half_duplex) {
    if (half_duplex) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_half_duplex);
    }
    common_hal_async_spi_spi_construct(&self->async_spi, clock, mosi, miso);
    self->sent = NULL;
}

#if CIRCUITPY_BULK_RESET
void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    common_hal_async_spi_spi_never_reset(&self->async_spi);
}
#endif

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return common_hal_async_spi_spi_deinited(&self->async_spi);
}

void common_hal_busio_spi_mark_deinit(busio_spi_obj_t *self) {
    common_hal_async_spi_spi_mark_deinit(&self->async_spi);
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    common_hal_async_spi_spi_deinit(&self->async_spi);
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self, uint32_t baudrate, uint8_t polarity,
    uint8_t phase, uint8_t bits) {
    common_hal_async_spi_spi_configure(&self->async_spi, baudrate, polarity, phase, bits);
    return true;
}

// The same lock as async_spi's, so C code and async code share the bus the same way.
bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return false;
    }
    return circuitpy_async_lock_try(common_hal_async_spi_spi_get_lock(&self->async_spi));
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return circuitpy_async_lock_held(common_hal_async_spi_spi_get_lock(&self->async_spi));
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    circuitpy_async_lock_release(common_hal_async_spi_spi_get_lock(&self->async_spi));
}

// Finish the transfer started with self->done.
static bool finish(busio_spi_obj_t *self, void *context) {
    if (!wait(&self->done)) {
        common_hal_async_spi_spi_transfer_cancel(&self->async_spi, context, &self->done);
        mp_handle_pending(true);  // raises the KeyboardInterrupt
        return false;
    }
    return common_hal_async_spi_spi_transfer_finish(&self->async_spi, context, &self->done);
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self, const uint8_t *data, size_t len) {
    return finish(self, common_hal_async_spi_spi_write_start(&self->async_spi, data, len, &self->done));
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self, uint8_t *data, size_t len,
    uint8_t write_value) {
    return finish(self, common_hal_async_spi_spi_readinto_start(&self->async_spi, data, len, write_value,
        &self->done));
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, const uint8_t *data_out,
    uint8_t *data_in, size_t len) {
    return finish(self, common_hal_async_spi_spi_write_readinto_start(&self->async_spi, data_out, data_in,
        len, &self->done));
}

#if CIRCUITPY_BUSIO_SPI_ASYNC
void common_hal_busio_spi_write_start(busio_spi_obj_t *self, const uint8_t *data, size_t len,
    circuitpy_async_flag_t *done) {
    self->sent_context = common_hal_async_spi_spi_write_start(&self->async_spi, data, len, done);
    self->sent = done;
}

void common_hal_busio_spi_end(busio_spi_obj_t *self) {
    end_sent(self);
}
#endif

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return common_hal_async_spi_spi_get_frequency(&self->async_spi);
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return common_hal_async_spi_spi_get_phase(&self->async_spi);
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return common_hal_async_spi_spi_get_polarity(&self->async_spi);
}
