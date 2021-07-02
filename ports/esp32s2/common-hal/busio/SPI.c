/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
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

#include <string.h>

#include "py/runtime.h"
#include "shared-bindings/busio/SPI.h"

#include "driver/spi_common_internal.h"

#define SPI_MAX_DMA_BITS (SPI_MAX_DMA_LEN * 8)

static bool spi_never_reset[SOC_SPI_PERIPH_NUM];
static spi_device_handle_t spi_handle[SOC_SPI_PERIPH_NUM];

static bool spi_bus_is_free(spi_host_device_t host_id) {
    return spi_bus_get_attr(host_id) == NULL;
}

void spi_reset(void) {
    for (spi_host_device_t host_id = SPI2_HOST; host_id < SOC_SPI_PERIPH_NUM; host_id++) {
        if (spi_never_reset[host_id]) {
            continue;
        }
        if (!spi_bus_is_free(host_id)) {
            spi_bus_remove_device(spi_handle[host_id]);
            spi_bus_free(host_id);
        }
    }
}

static void set_spi_config(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    const spi_device_interface_config_t device_config = {
        .clock_speed_hz = baudrate,
        .mode = phase | (polarity << 1),
        .spics_io_num = -1, // No CS pin
        .queue_size = 1,
        .pre_cb = NULL
    };
    esp_err_t result = spi_bus_add_device(self->host_id, &device_config, &spi_handle[self->host_id]);
    if (result != ESP_OK) {
        mp_raise_RuntimeError(translate("SPI configuration failed"));
    }
    self->baudrate = baudrate;
    self->polarity = polarity;
    self->phase = phase;
    self->bits = bits;
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso) {

    const spi_bus_config_t bus_config = {
        .mosi_io_num = mosi != NULL ? mosi->number : -1,
        .miso_io_num = miso != NULL ? miso->number : -1,
        .sclk_io_num = clock != NULL ? clock->number : -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    for (spi_host_device_t host_id = SPI2_HOST; host_id < SOC_SPI_PERIPH_NUM; host_id++) {
        if (spi_bus_is_free(host_id)) {
            self->host_id = host_id;
        }
    }

    if (self->host_id == 0) {
        mp_raise_ValueError(translate("All SPI peripherals are in use"));
    }

    esp_err_t result = spi_bus_initialize(self->host_id, &bus_config, self->host_id /* dma channel */);
    if (result == ESP_ERR_NO_MEM) {
        mp_raise_msg(&mp_type_MemoryError, translate("ESP-IDF memory allocation failed"));
    } else if (result == ESP_ERR_INVALID_ARG) {
        mp_raise_ValueError(translate("Invalid pins"));
    }

    set_spi_config(self, 250000, 0, 0, 8);

    self->MOSI = mosi;
    self->MISO = miso;
    self->clock = clock;

    if (mosi != NULL) {
        claim_pin(mosi);
    }
    if (miso != NULL) {
        claim_pin(miso);
    }
    claim_pin(clock);
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    spi_never_reset[self->host_id] = true;
    common_hal_never_reset_pin(self->clock);
    if (self->MOSI != NULL) {
        common_hal_never_reset_pin(self->MOSI);
    }
    if (self->MISO != NULL) {
        common_hal_never_reset_pin(self->MISO);
    }
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->clock == NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }

    spi_never_reset[self->host_id] = false;
    spi_bus_remove_device(spi_handle[self->host_id]);
    spi_bus_free(self->host_id);

    common_hal_reset_pin(self->MOSI);
    common_hal_reset_pin(self->MISO);
    common_hal_reset_pin(self->clock);
    self->clock = NULL;
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    if (baudrate == self->baudrate &&
        polarity == self->polarity &&
        phase == self->phase &&
        bits == self->bits) {
        return true;
    }
    spi_bus_remove_device(spi_handle[self->host_id]);
    set_spi_config(self, baudrate, polarity, phase, bits);
    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    bool grabbed_lock = false;
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    return grabbed_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
    const uint8_t *data, size_t len) {
    if (self->MOSI == NULL) {
        mp_raise_ValueError(translate("No MOSI Pin"));
    }
    return common_hal_busio_spi_transfer(self, data, NULL, len);
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len, uint8_t write_value) {
    if (self->MISO == NULL) {
        mp_raise_ValueError(translate("No MISO Pin"));
    }
    if (self->MOSI == NULL) {
        return common_hal_busio_spi_transfer(self, NULL, data, len);
    } else {
        memset(data, write_value, len);
        return common_hal_busio_spi_transfer(self, data, data, len);
    }
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self,
    const uint8_t *data_out, uint8_t *data_in, size_t len) {
    if (len == 0) {
        return true;
    }
    // Other than the read special case, stop transfers that don't have a pin/array match
    if (!self->MOSI && (data_out != data_in)) {
        mp_raise_ValueError(translate("No MOSI Pin"));
    }
    if (!self->MISO && data_in) {
        mp_raise_ValueError(translate("No MISO Pin"));
    }

    spi_transaction_t transaction = { 0 };

    // Round to nearest whole set of bits
    int bits_to_send = len * 8 / self->bits * self->bits;

    if (len <= 4) {
        if (data_out != NULL) {
            memcpy(&transaction.tx_data, data_out, len);
        }

        transaction.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
        transaction.length = bits_to_send;
        spi_device_transmit(spi_handle[self->host_id], &transaction);

        if (data_in != NULL) {
            memcpy(data_in, &transaction.rx_data, len);
        }
    } else {
        int offset = 0;
        int bits_remaining = bits_to_send;

        while (bits_remaining && !mp_hal_is_interrupted()) {
            memset(&transaction, 0, sizeof(transaction));

            transaction.length =
                bits_remaining > SPI_MAX_DMA_BITS ? SPI_MAX_DMA_BITS : bits_remaining;

            if (data_out != NULL) {
                transaction.tx_buffer = data_out + offset;
            }
            if (data_in != NULL) {
                transaction.rx_buffer = data_in + offset;
            }

            spi_device_transmit(spi_handle[self->host_id], &transaction);
            bits_remaining -= transaction.length;

            // doesn't need ceil(); loop ends when bits_remaining is 0
            offset += transaction.length / 8;

            RUN_BACKGROUND_TASKS;
        }
    }
    return true;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->baudrate;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->polarity;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->phase;
}
