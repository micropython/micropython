// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/async_spi/SPI.h"

#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "supervisor/port.h"
#include "bindings/zephyr_kernel/__init__.h"

#include <errno.h>
#include <iobroker/iobroker.h>

// Settings take the other config slot: drivers see a change by the pointer.
static void apply_config(async_spi_spi_obj_t *self, uint32_t frequency, uint16_t operation) {
    struct spi_config *current = &self->config[self->active_config];
    if (current->frequency == frequency && current->operation == operation) {
        return;
    }
    self->active_config = 1 - self->active_config;
    self->config[self->active_config].frequency = frequency;
    self->config[self->active_config].operation = operation;
}

static void transfer_done(const struct device *dev, int result, void *userdata) {
    ARG_UNUSED(dev);
    async_spi_spi_obj_t *self = userdata;
    self->result = result;
    CIRCUITPY_ASYNC_FLAG_SET(self->done);
    port_wake_main_task_from_isr();
}

// Zephyr cannot stop a transfer, so cancel and deinit wait for it. No background tasks: this
// also runs from a finalizer during GC.
static void wait_done(async_spi_spi_obj_t *self) {
    if (self->done == NULL) {
        return;
    }
    while (!CIRCUITPY_ASYNC_FLAG_IS_SET(self->done)) {
        port_task_sleep_ms(1);
    }
    self->done = NULL;
}

// A fixed devicetree device, as for board.SPI(): already initialized, never released.
void common_hal_async_spi_spi_construct_from_device(async_spi_spi_obj_t *self,
    const struct device *dev) {
    circuitpy_async_lock_init(&self->lock);
    self->spi_device = dev;
    self->dynamic = false;
    self->clock = NULL;
    self->mosi = NULL;
    self->miso = NULL;
    self->active_config = 0;
    for (size_t i = 0; i < 2; i++) {
        self->config[i].frequency = 100000;
        self->config[i].operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_LINES_SINGLE;
    }
    self->done = NULL;
    self->fill = NULL;
    self->fill_len = 0;
    self->fill_from_port = false;
}

static void free_fill(async_spi_spi_obj_t *self) {
    if (self->fill_from_port) {
        port_free(self->fill);
    }
    self->fill = NULL;
    self->fill_len = 0;
    self->fill_from_port = false;
}

void common_hal_async_spi_spi_construct(async_spi_spi_obj_t *self, const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso) {
    const struct device *dev = NULL;
    int ret = iobroker_spi_allocate(clock->package_pin,
        mosi != NULL ? mosi->package_pin : IOBROKER_NO_PIN,
        miso != NULL ? miso->package_pin : IOBROKER_NO_PIN, &dev);
    if (ret < 0) {
        if (ret == -ENODEV) {
            mp_raise_ValueError(MP_ERROR_TEXT("All SPI peripherals are in use"));
        }
        if (ret == -EBUSY) {
            mp_raise_ValueError(MP_ERROR_TEXT("Internal resource(s) in use"));
        }
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Use device tree to define %q devices"), MP_QSTR_SPI);
    }

    common_hal_async_spi_spi_construct_from_device(self, dev);
    self->dynamic = true;
    self->clock = clock;
    self->mosi = mosi;
    self->miso = miso;

    // A routed device is initialized now; a fixed one already was (-EALREADY).
    int init_ret = device_init(dev);
    if (init_ret < 0 && init_ret != -EALREADY) {
        common_hal_async_spi_spi_deinit(self);
        raise_zephyr_error(init_ret);
    }
}

bool common_hal_async_spi_spi_deinited(async_spi_spi_obj_t *self) {
    return self->spi_device == NULL;
}

void common_hal_async_spi_spi_deinit(async_spi_spi_obj_t *self) {
    if (common_hal_async_spi_spi_deinited(self)) {
        return;
    }
    circuitpy_async_lock_deinit(&self->lock);
    wait_done(self);
    free_fill(self);
    // A fixed devicetree device (board.SPI()) stays usable.
    if (!self->dynamic) {
        return;
    }
    (void)iobroker_release(self->spi_device);
    self->spi_device = NULL;
    self->clock = NULL;
    self->mosi = NULL;
    self->miso = NULL;
}

void common_hal_async_spi_spi_never_reset(async_spi_spi_obj_t *self) {
}

void common_hal_async_spi_spi_mark_deinit(async_spi_spi_obj_t *self) {
    if (self->dynamic) {
        self->spi_device = NULL;
    }
}

static uint16_t operation_of(uint8_t polarity, uint8_t phase, uint8_t bits) {
    uint16_t operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(bits) | SPI_LINES_SINGLE;
    if (polarity) {
        operation |= SPI_MODE_CPOL;
    }
    if (phase) {
        operation |= SPI_MODE_CPHA;
    }
    return operation;
}

// The caller holds the lock, so no transfer is running.
void common_hal_async_spi_spi_configure(async_spi_spi_obj_t *self, uint32_t baudrate,
    uint8_t polarity, uint8_t phase, uint8_t bits) {
    apply_config(self, baudrate, operation_of(polarity, phase, bits));
}

circuitpy_async_lock_obj_t *common_hal_async_spi_spi_get_lock(async_spi_spi_obj_t *self) {
    return &self->lock;
}

uint8_t common_hal_async_spi_spi_get_polarity(async_spi_spi_obj_t *self) {
    return (self->config[self->active_config].operation & SPI_MODE_CPOL) != 0;
}

uint8_t common_hal_async_spi_spi_get_phase(async_spi_spi_obj_t *self) {
    return (self->config[self->active_config].operation & SPI_MODE_CPHA) != 0;
}

uint32_t common_hal_async_spi_spi_get_frequency(async_spi_spi_obj_t *self) {
    return self->config[self->active_config].frequency;
}

// Start a transfer; done is set from the driver's callback, or at once for an empty one.
static void start(async_spi_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in,
    size_t len, circuitpy_async_flag_t *done) {
    wait_done(self);
    CIRCUITPY_ASYNC_FLAG_INIT(done);
    self->done = done;
    self->result = 0;
    if (len == 0) {
        CIRCUITPY_ASYNC_FLAG_SET(done);
        return;
    }
    self->tx_buf.buf = (void *)data_out;
    self->tx_buf.len = len;
    self->tx.buffers = &self->tx_buf;
    self->tx.count = data_out != NULL ? 1 : 0;
    self->rx_buf.buf = data_in;
    self->rx_buf.len = len;
    self->rx.buffers = &self->rx_buf;
    self->rx.count = data_in != NULL ? 1 : 0;

    int ret = -ENOTSUP;
    if (DEVICE_API_GET(spi, self->spi_device)->transceive_async != NULL) {
        ret = spi_transceive_cb(self->spi_device, &self->config[self->active_config],
            self->tx.count ? &self->tx : NULL, self->rx.count ? &self->rx : NULL,
            transfer_done, self);
    }
    if (ret < 0) {
        // Reported by transfer_end() or transfer_finish(); nothing raises here, as a
        // blocking busio call may run from a background callback.
        self->result = ret;
        CIRCUITPY_ASYNC_FLAG_SET(done);
    }
}

void *common_hal_async_spi_spi_write_start(async_spi_spi_obj_t *self, const uint8_t *data,
    size_t len, circuitpy_async_flag_t *done) {
    start(self, data, NULL, len, done);
    return NULL;
}

void *common_hal_async_spi_spi_readinto_start(async_spi_spi_obj_t *self, uint8_t *data,
    size_t len, uint8_t write_value, circuitpy_async_flag_t *done) {
    // Without a TX buffer a driver sends its own idle value (0xFF on nRF), so the read sends
    // write_value from a buffer, kept for the next read. The previous transfer must be over
    // before it is touched.
    wait_done(self);
    uint8_t *fill = NULL;
    if (len != 0) {
        if (self->fill_len < len) {
            free_fill(self);
            if (gc_alloc_possible()) {
                self->fill = m_malloc(len);
            } else {
                self->fill = port_malloc(len, false);
                self->fill_from_port = true;
            }
            if (self->fill == NULL) {
                self->fill_from_port = false;
                self->result = -ENOMEM;
                CIRCUITPY_ASYNC_FLAG_INIT(done);
                CIRCUITPY_ASYNC_FLAG_SET(done);
                self->done = done;
                return NULL;
            }
            self->fill_len = len;
        }
        fill = self->fill;
        memset(fill, write_value, len);
    }
    start(self, fill, data, len, done);
    return NULL;
}

void *common_hal_async_spi_spi_write_readinto_start(async_spi_spi_obj_t *self,
    const uint8_t *data_out, uint8_t *data_in, size_t len, circuitpy_async_flag_t *done) {
    start(self, data_out, data_in, len, done);
    return NULL;
}

// Called once done is set.
bool common_hal_async_spi_spi_transfer_finish(async_spi_spi_obj_t *self, void *context,
    circuitpy_async_flag_t *done) {
    if (self->done != done) {
        return true;
    }
    self->done = NULL;
    return self->result == 0;
}

mp_obj_t common_hal_async_spi_spi_transfer_end(async_spi_spi_obj_t *self, void *context,
    circuitpy_async_flag_t *done) {
    if (!common_hal_async_spi_spi_transfer_finish(self, context, done)) {
        raise_zephyr_error(self->result);
    }
    return mp_const_none;
}

void common_hal_async_spi_spi_transfer_cancel(async_spi_spi_obj_t *self, void *context,
    circuitpy_async_flag_t *done) {
    if (self->done == done) {
        wait_done(self);
    }
}
