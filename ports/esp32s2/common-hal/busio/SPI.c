/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#include "shared-bindings/busio/SPI.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "boards/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "supervisor/shared/rgb_led_status.h"

static bool spi_never_reset[SOC_SPI_PERIPH_NUM];

void spi_reset(void) {
    for (spi_host_device_t host_id = SPI2_HOST; host_id < SOC_SPI_PERIPH_NUM; host_id++) {
        if (spi_never_reset[host_id]) {
            continue;
        }
        spi_bus_free(host_id);
    }
}

// This is copied in from the ESP-IDF because it is static.
// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
static bool bus_uses_iomux_pins(spi_host_device_t host, const spi_bus_config_t* bus_config)
{
    if (bus_config->sclk_io_num>=0 &&
        bus_config->sclk_io_num != spi_periph_signal[host].spiclk_iomux_pin) return false;
    if (bus_config->quadwp_io_num>=0 &&
        bus_config->quadwp_io_num != spi_periph_signal[host].spiwp_iomux_pin) return false;
    if (bus_config->quadhd_io_num>=0 &&
        bus_config->quadhd_io_num != spi_periph_signal[host].spihd_iomux_pin) return false;
    if (bus_config->mosi_io_num >= 0 &&
        bus_config->mosi_io_num != spi_periph_signal[host].spid_iomux_pin) return false;
    if (bus_config->miso_io_num>=0 &&
        bus_config->miso_io_num != spi_periph_signal[host].spiq_iomux_pin) return false;

    return true;
}

// End copied code.

static bool _spi_bus_free(spi_host_device_t host_id) {
    return spi_bus_get_attr(host_id) == NULL;
}

static void spi_interrupt_handler(void *arg) {

}

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
        const mcu_pin_obj_t * clock, const mcu_pin_obj_t * mosi,
        const mcu_pin_obj_t * miso) {
    spi_bus_config_t bus_config;
    bus_config.mosi_io_num = mosi != NULL ? mosi->number : -1;
    bus_config.miso_io_num = miso != NULL ? miso->number : -1;
    bus_config.sclk_io_num = clock != NULL ? clock->number : -1;
    bus_config.quadwp_io_num = -1;
    bus_config.quadhd_io_num = -1;
    bus_config.max_transfer_sz = 0; // Uses the default
    bus_config.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_SCLK |
                       (mosi != NULL ? SPICOMMON_BUSFLAG_MOSI : 0) |
                       (miso != NULL ? SPICOMMON_BUSFLAG_MISO : 0);
    bus_config.intr_flags = 0;

    // RAM and Flash is often on SPI1 and is unsupported by the IDF so use it as
    // a flag value.
    spi_host_device_t host_id = SPI1_HOST;
    self->connected_through_gpio = true;
    // Try and save SPI2 for pins that are on the IOMUX
    if (bus_uses_iomux_pins(SPI2_HOST, &bus_config) && _spi_bus_free(SPI2_HOST)) {
        host_id = SPI2_HOST;
        self->connected_through_gpio = false;
    } else if (_spi_bus_free(SPI3_HOST)) {
        host_id = SPI3_HOST;
    } else if (_spi_bus_free(SPI2_HOST)) {
        host_id = SPI2_HOST;
    }
    if (host_id == SPI1_HOST) {
        mp_raise_ValueError(translate("All SPI peripherals are in use"));
    }

    esp_err_t result = spi_bus_initialize(host_id, &bus_config, 0 /* dma channel */);
    if (result == ESP_ERR_NO_MEM) {
        mp_raise_msg(&mp_type_MemoryError, translate("ESP-IDF memory allocation failed"));
    } else if (result == ESP_ERR_INVALID_ARG) {
        mp_raise_ValueError(translate("Invalid pins"));
    }
    spi_bus_lock_dev_config_t config = { .flags = 0 };
    result = spi_bus_lock_register_dev(spi_bus_get_attr(host_id)->lock,
                                       &config,
                                       &self->lock);
    if (result == ESP_ERR_NO_MEM) {
        mp_raise_msg(&mp_type_MemoryError, translate("ESP-IDF memory allocation failed"));
    }


    result = esp_intr_alloc(spicommon_irqsource_for_host(host_id),
                            bus_config.intr_flags | ESP_INTR_FLAG_INTRDISABLED,
                            spi_interrupt_handler, self, &self->interrupt);
    if (result == ESP_ERR_NO_MEM) {
        mp_raise_msg(&mp_type_MemoryError, translate("ESP-IDF memory allocation failed"));
    }

    spi_hal_context_t* hal = &self->hal_context;
    hal->hw = NULL; // Set by spi_hal_init
    hal->dmadesc_tx = NULL;
    hal->dmadesc_rx = NULL;
    hal->dmadesc_n = 0;

    // We don't use native CS.
    hal->cs_setup = 0;
    hal->cs_hold = 0;
    hal->cs_pin_id = -1;
    hal->timing_conf = &self->timing_conf;

    hal->sio = 1;
    hal->half_duplex = 0;
    hal->tx_lsbfirst = 0;
    hal->rx_lsbfirst = 0;
    hal->dma_enabled = 0;
    hal->no_compensate = 1;
    // Ignore CS bits

    // We don't use cmd, addr or dummy bits.
    hal->cmd = 0;
    hal->cmd_bits = 0;
    hal->addr_bits = 0;
    hal->dummy_bits = 0;
    hal->addr = 0;

    hal->io_mode = SPI_LL_IO_MODE_NORMAL;

    spi_hal_init(hal, host_id);
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    spi_never_reset[self->host_id] = true;

    never_reset_pin(self->clock_pin);
    never_reset_pin(self->MOSI_pin);
    never_reset_pin(self->MISO_pin);
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->clock_pin == NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }
    spi_never_reset[self->host_id] = false;
    spi_bus_free(self->host_id);

    common_hal_reset_pin(self->clock_pin);
    common_hal_reset_pin(self->MOSI_pin);
    common_hal_reset_pin(self->MISO_pin);
    self->clock_pin = NULL;
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
        uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    if (baudrate == self->target_frequency &&
        polarity == self->polarity &&
        phase == self->phase &&
        bits == self->bits) {
        return true;
    }
    self->hal_context.mode = polarity << 1 | phase;
    self->polarity = polarity;
    self->phase = phase;
    self->bits = bits;
    self->target_frequency = baudrate;
    esp_err_t result =  spi_hal_get_clock_conf(&self->hal_context,
                                               self->target_frequency,
                                               128 /* duty_cycle */,
                                               self->connected_through_gpio,
                                               0 /* input_delay_ns */,
                                               &self->real_frequency,
                                               &self->timing_conf);
    if (result != ESP_OK) {
        return false;
    }

    spi_hal_setup_device(&self->hal_context);
    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    // If our lock has already been taken then return false because someone else
    // may already grabbed it in our call stack.
    if (self->has_lock) {
        return false;
    }
    // Wait to grab the lock from another task.
    esp_err_t result = spi_bus_lock_acquire_start(self->lock, portMAX_DELAY);
    self->has_lock = result == ESP_OK;
    return self->has_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    spi_bus_lock_acquire_end(self->lock);
    self->has_lock = false;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
        const uint8_t *data, size_t len) {
    return common_hal_busio_spi_transfer(self, data, NULL, len);
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
        uint8_t *data, size_t len, uint8_t write_value) {
    return common_hal_busio_spi_transfer(self, NULL, data, len);
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len) {
    if (len == 0) {
        return true;
    }

    spi_hal_context_t* hal = &self->hal_context;
    hal->tx_bitlen = len * self->bits;
    hal->rx_bitlen = len * self->bits;
    hal->send_buffer = (uint8_t*) data_out;
    hal->rcv_buffer = data_in;

    spi_hal_setup_trans(hal);
    spi_hal_prepare_data(hal);
    spi_hal_user_start(hal);
    if (len >= SOC_SPI_MAXIMUM_BUFFER_SIZE && false) {
        // Set up the interrupt and wait on the lock.
    } else {
        while (!spi_hal_usr_is_done(hal)) {
            RUN_BACKGROUND_TASKS;
        }
    }

    return true;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t* self) {
    return self->real_frequency;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t* self) {
    return self->phase;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t* self) {
    return self->polarity;
}
