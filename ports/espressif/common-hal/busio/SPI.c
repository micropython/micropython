// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "freertos/projdefs.h"
#include "py/runtime.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "esp_private/spi_common_internal.h"

#define SPI_MAX_DMA_BITS (SPI_MAX_DMA_LEN * 8)
#define MAX_SPI_TRANSACTIONS 10

static spi_device_handle_t spi_handle[SOC_SPI_PERIPH_NUM];

static bool spi_bus_is_free(spi_host_device_t host_id) {
    return spi_bus_get_attr(host_id) == NULL;
}

// Add a throwaway device at the given target clock, ask the driver what
// frequency it would actually produce, then remove it. Returns the actual
// frequency in Hz, or -1 if a device could not be added at that target.
static int spi_probe_actual_freq(busio_spi_obj_t *self,
    spi_device_interface_config_t *device_config, int target_hz) {
    device_config->clock_speed_hz = target_hz;
    spi_device_handle_t handle;
    if (spi_bus_add_device(self->host_id, device_config, &handle) != ESP_OK) {
        return -1;
    }
    int freq_khz = 0;
    spi_device_get_actual_freq(handle, &freq_khz);
    spi_bus_remove_device(handle);
    return freq_khz * 1000;
}

static void set_spi_config(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    spi_device_interface_config_t device_config = {
        .clock_speed_hz = baudrate,
        .mode = phase | (polarity << 1),
        .spics_io_num = -1, // No CS pin
        .queue_size = MAX_SPI_TRANSACTIONS,
        .pre_cb = NULL
    };

    // The ESP-IDF driver rounds clock_speed_hz to the nearest frequency it can
    // produce, which may be HIGHER than requested. Treat baudrate as a ceiling:
    // probe candidate targets (each time adding a throwaway device, asking the
    // driver what it actually produced, and removing it) and keep the highest
    // target whose actual frequency does not exceed baudrate. We don't inspect
    // divisor internals -- those vary between Espressif chips -- only the
    // public add/measure/remove API.
    int target = baudrate;
    int actual = spi_probe_actual_freq(self, &device_config, target);
    if (actual < 0 || actual > (int)baudrate) {
        // Bisect for the highest target whose actual frequency is <= baudrate.
        // Stop refining once the interval is within 1 kHz, the resolution that
        // spi_device_get_actual_freq reports.
        int lo = 1;
        int hi = baudrate;
        while (hi - lo > 1000) {
            int mid = lo + (hi - lo) / 2;
            int mid_actual = spi_probe_actual_freq(self, &device_config, mid);
            if (mid_actual >= 0 && mid_actual <= (int)baudrate) {
                lo = mid;
            } else {
                hi = mid;
            }
        }
        target = lo;
    }

    device_config.clock_speed_hz = target;
    esp_err_t result = spi_bus_add_device(self->host_id, &device_config, &spi_handle[self->host_id]);
    if (result != ESP_OK) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("SPI configuration failed"));
    }

    // Report the frequency the driver actually settled on for the real device.
    int actual_khz = 0;
    spi_device_get_actual_freq(spi_handle[self->host_id], &actual_khz);
    self->baudrate = actual_khz * 1000;
    self->requested_baudrate = baudrate;
    self->polarity = polarity;
    self->phase = phase;
    self->bits = bits;
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex) {

    // Ensure the object starts in its deinit state.
    common_hal_busio_spi_mark_deinit(self);
    #if CIRCUITPY_BUSIO_SPI_ASYNC
    self->async_count = 0;
    #endif

    const spi_bus_config_t bus_config = {
        .mosi_io_num = mosi != NULL ? mosi->number : -1,
        .miso_io_num = miso != NULL ? miso->number : -1,
        .sclk_io_num = clock != NULL ? clock->number : -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    if (half_duplex) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_half_duplex);
    }

    for (spi_host_device_t host_id = SPI2_HOST; host_id < SOC_SPI_PERIPH_NUM; host_id++) {
        if (spi_bus_is_free(host_id)) {
            self->host_id = host_id;
        }
    }

    if (self->host_id == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("All SPI peripherals are in use"));
    }

    esp_err_t result = spi_bus_initialize(self->host_id, &bus_config, SPI_DMA_CH_AUTO);
    if (result == ESP_ERR_NO_MEM) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("ESP-IDF memory allocation failed"));
    } else if (result == ESP_ERR_INVALID_ARG) {
        raise_ValueError_invalid_pins();
    }

    self->mutex = xSemaphoreCreateMutex();
    if (self->mutex == NULL) {
        spi_bus_free(self->host_id);
        mp_raise_RuntimeError(MP_ERROR_TEXT("Unable to create lock"));
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

void common_hal_busio_spi_mark_deinit(busio_spi_obj_t *self) {
    self->clock = NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }

    // Wait for other users to finish. Ctrl-C ends the wait and leaves the bus up.
    while (!common_hal_busio_spi_try_lock(self)) {
        if (mp_hal_is_interrupted()) {
            return;
        }
        RUN_BACKGROUND_TASKS;
    }

    #if CIRCUITPY_BUSIO_SPI_ASYNC
    common_hal_busio_spi_end(self);
    #endif

    // Mark as deinit early in case we are used in an interrupt.
    common_hal_reset_pin(self->clock);
    common_hal_busio_spi_mark_deinit(self);

    spi_bus_remove_device(spi_handle[self->host_id]);
    spi_bus_free(self->host_id);

    // Release the mutex before we delete it. Otherwise FreeRTOS gets unhappy.
    xSemaphoreGive(self->mutex);
    vSemaphoreDelete(self->mutex);
    self->mutex = NULL;

    common_hal_reset_pin(self->MOSI);
    common_hal_reset_pin(self->MISO);
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    if (baudrate == self->requested_baudrate &&
        polarity == self->polarity &&
        phase == self->phase &&
        bits == self->bits) {
        return true;
    }
    #if CIRCUITPY_BUSIO_SPI_ASYNC
    common_hal_busio_spi_end(self);
    #endif
    spi_bus_remove_device(spi_handle[self->host_id]);
    set_spi_config(self, baudrate, polarity, phase, bits);
    return true;
}

// Wait as long as needed for the lock. This is used by SD card access from USB.
// Overrides the default busy-wait implementation in shared-bindings/busio/SPI.c
bool common_hal_busio_spi_wait_for_lock(busio_spi_obj_t *self, uint32_t timeout_ms) {
    if (common_hal_busio_spi_deinited(self)) {
        return false;
    }
    return xSemaphoreTake(self->mutex, pdMS_TO_TICKS(timeout_ms)) == pdTRUE;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    return common_hal_busio_spi_wait_for_lock(self, 0);
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return (self->mutex != NULL) && (xSemaphoreGetMutexHolder(self->mutex) == xTaskGetCurrentTaskHandle());
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    if (self->mutex != NULL) {
        xSemaphoreGive(self->mutex);
    }
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
    const uint8_t *data, size_t len) {
    if (self->MOSI == NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_mosi);
    }
    return common_hal_busio_spi_transfer(self, data, NULL, len);
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len, uint8_t write_value) {
    if (self->MISO == NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_miso);
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
    if (self->MOSI == NULL && data_out != NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_mosi);
    }
    if (self->MISO == NULL && data_in != NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_miso);
    }

    spi_transaction_t transactions[MAX_SPI_TRANSACTIONS];

    // Round to nearest whole set of bits
    int bits_to_send = len * 8 / self->bits * self->bits;

    if (len <= 4) {
        memset(&transactions[0], 0, sizeof(spi_transaction_t));
        if (data_out != NULL) {
            memcpy(&transactions[0].tx_data, data_out, len);
        }

        transactions[0].flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
        transactions[0].length = bits_to_send;
        esp_err_t result = spi_device_transmit(spi_handle[self->host_id], &transactions[0]);
        if (result != ESP_OK) {
            return false;
        }

        if (data_in != NULL) {
            memcpy(data_in, &transactions[0].rx_data, len);
        }
    } else {
        int offset = 0;
        int bits_remaining = bits_to_send;
        int cur_trans = 0;

        while (bits_remaining && !mp_hal_is_interrupted()) {

            cur_trans = 0;
            while (bits_remaining && (cur_trans != MAX_SPI_TRANSACTIONS)) {
                memset(&transactions[cur_trans], 0, sizeof(spi_transaction_t));

                transactions[cur_trans].length =
                    bits_remaining > SPI_MAX_DMA_BITS ? SPI_MAX_DMA_BITS : bits_remaining;

                if (data_out != NULL) {
                    transactions[cur_trans].tx_buffer = data_out + offset;
                }
                if (data_in != NULL) {
                    transactions[cur_trans].rx_buffer = data_in + offset;
                }

                bits_remaining -= transactions[cur_trans].length;

                // doesn't need ceil(); loop ends when bits_remaining is 0
                offset += transactions[cur_trans].length / 8;
                cur_trans++;
            }

            // Queueing fails if the driver can't allocate a DMA-capable bounce buffer. Wait only for
            // the transactions that were queued; waiting for any others would block forever.
            esp_err_t result = ESP_OK;
            int queued = 0;
            while (queued < cur_trans) {
                result = spi_device_queue_trans(spi_handle[self->host_id], &transactions[queued], portMAX_DELAY);
                if (result != ESP_OK) {
                    break;
                }
                queued++;
            }

            spi_transaction_t *rtrans;
            for (int x = 0; x < queued; x++) {
                RUN_BACKGROUND_TASKS;
                spi_device_get_trans_result(spi_handle[self->host_id], &rtrans, portMAX_DELAY);
            }
            if (result != ESP_OK) {
                return false;
            }
        }
    }
    return true;
}

#if CIRCUITPY_BUSIO_SPI_ASYNC
void common_hal_busio_spi_write_start(busio_spi_obj_t *self, const uint8_t *data, size_t len,
    circuitpy_async_flag_t *done) {
    common_hal_busio_spi_end(self);
    CIRCUITPY_ASYNC_FLAG_INIT(done);
    self->async_done = done;
    size_t chunks = (len + SPI_MAX_DMA_LEN - 1) / SPI_MAX_DMA_LEN;
    // Short writes, other word sizes and writes needing more transactions than we keep go the
    // normal way.
    if (len <= 4 || self->bits != 8 || chunks > MP_ARRAY_SIZE(self->async_trans)) {
        common_hal_busio_spi_write(self, data, len);
        CIRCUITPY_ASYNC_FLAG_SET(done);
        return;
    }
    size_t queued = 0;
    while (queued < chunks) {
        size_t offset = queued * SPI_MAX_DMA_LEN;
        spi_transaction_t *trans = &self->async_trans[queued];
        memset(trans, 0, sizeof(*trans));
        trans->length = MIN(len - offset, SPI_MAX_DMA_LEN) * 8;
        trans->tx_buffer = data + offset;
        if (spi_device_queue_trans(spi_handle[self->host_id], trans, portMAX_DELAY) != ESP_OK) {
            break;
        }
        queued++;
    }
    if (queued < chunks) {
        // Queueing fails if the driver can't allocate a DMA-capable bounce buffer. Finish the
        // chunks that were queued, then send the rest the normal way.
        spi_transaction_t *trans;
        for (size_t i = 0; i < queued; i++) {
            spi_device_get_trans_result(spi_handle[self->host_id], &trans, portMAX_DELAY);
        }
        size_t offset = queued * SPI_MAX_DMA_LEN;
        common_hal_busio_spi_write(self, data + offset, len - offset);
        CIRCUITPY_ASYNC_FLAG_SET(done);
        return;
    }
    self->async_count = chunks;
}

void common_hal_busio_spi_end(busio_spi_obj_t *self) {
    if (self->async_count == 0) {
        return;
    }
    // No background tasks here: the caller holds the bus, and one of them may want it.
    spi_transaction_t *trans;
    while (self->async_count > 0) {
        spi_device_get_trans_result(spi_handle[self->host_id], &trans, portMAX_DELAY);
        self->async_count--;
    }
    CIRCUITPY_ASYNC_FLAG_SET(self->async_done);
}
#endif

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->baudrate;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->polarity;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->phase;
}
