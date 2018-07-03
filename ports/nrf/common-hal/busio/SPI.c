/*
 * SPI Master library for nRF5x.
 * Copyright (c) 2015 Arduino LLC
 * Copyright (c) 2016 Sandeep Mistry All right reserved.
 * Copyright (c) 2017 hathach
 * Copyright (c) 2018 Artur Pacholec
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "shared-bindings/busio/SPI.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "nrfx_spim.h"
#include "nrf_gpio.h"

#if NRFX_SPIM3_ENABLED
    #define INST_NO 3
#else
    #define INST_NO 2
#endif

// Convert frequency to clock-speed-dependent value
static nrf_spim_frequency_t baudrate_to_spim_frequency(const uint32_t baudrate) {
    if (baudrate <= 125000)
      return NRF_SPIM_FREQ_125K;

    if (baudrate <= 250000)
        return NRF_SPIM_FREQ_250K;

    if (baudrate <= 500000)
        return NRF_SPIM_FREQ_500K;

    if (baudrate <= 1000000)
        return NRF_SPIM_FREQ_1M;

    if (baudrate <= 2000000)
        return NRF_SPIM_FREQ_2M;

    if (baudrate <= 4000000)
        return NRF_SPIM_FREQ_4M;

    if (baudrate <= 8000000)
        return NRF_SPIM_FREQ_8M;

#ifdef SPIM_FREQUENCY_FREQUENCY_M16
    if (baudrate <= 16000000)
        return NRF_SPIM_FREQ_16M;
#endif

#ifdef SPIM_FREQUENCY_FREQUENCY_M32
    return NRF_SPIM_FREQ_32M;
#else
    return NRF_SPIM_FREQ_8M;
#endif
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self, const mcu_pin_obj_t * clock, const mcu_pin_obj_t * mosi, const mcu_pin_obj_t * miso) {
    const nrfx_spim_t instance = NRFX_SPIM_INSTANCE(INST_NO);
    self->spim = instance;

    nrfx_spim_config_t config = NRFX_SPIM_DEFAULT_CONFIG;
    config.frequency = NRF_SPIM_FREQ_8M;

    config.sck_pin = NRF_GPIO_PIN_MAP(clock->port, clock->pin);

    if (mosi != (mcu_pin_obj_t*)&mp_const_none_obj)
        config.mosi_pin = NRF_GPIO_PIN_MAP(mosi->port, mosi->pin);

    if (miso != (mcu_pin_obj_t*)&mp_const_none_obj)
        config.miso_pin = NRF_GPIO_PIN_MAP(miso->port, miso->pin);

    nrfx_err_t err = nrfx_spim_init(&self->spim, &config, NULL, NULL);

    // A soft reset doesn't uninit the driver so we might end up with a invalid state
    if (err == NRFX_ERROR_INVALID_STATE) {
        nrfx_spim_uninit(&self->spim);
        err = nrfx_spim_init(&self->spim, &config, NULL, NULL);
    }

    if (err != NRFX_SUCCESS)
        mp_raise_OSError(MP_EIO);

    self->inited = true;
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return !self->inited;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self))
        return;

    nrfx_spim_uninit(&self->spim);

    self->inited = false;
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
  // nrf52 does not support 16 bit
  if (bits != 8)
      return false;

  nrf_spim_frequency_set(self->spim.p_reg, baudrate_to_spim_frequency(baudrate));

  nrf_spim_mode_t mode = NRF_SPIM_MODE_0;
  if (polarity) {
      mode = (phase) ? NRF_SPIM_MODE_3 : NRF_SPIM_MODE_2;
  } else {
      mode = (phase) ? NRF_SPIM_MODE_1 : NRF_SPIM_MODE_0;
  }

  nrf_spim_configure(self->spim.p_reg, mode, NRF_SPIM_BIT_ORDER_MSB_FIRST);

  return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    bool grabbed_lock = false;
//    CRITICAL_SECTION_ENTER()
//        if (!self->has_lock) {
            grabbed_lock = true;
            self->has_lock = true;
//        }
//    CRITICAL_SECTION_LEAVE();
    return grabbed_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self, const uint8_t *data, size_t len) {
    if (len == 0)
        return true;

    const nrfx_spim_xfer_desc_t xfer = NRFX_SPIM_XFER_TX(data, len);
    const nrfx_err_t err = nrfx_spim_xfer(&self->spim, &xfer, 0);

    return (err == NRFX_SUCCESS);
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_value) {
    if (len == 0)
        return true;

    const nrfx_spim_xfer_desc_t xfer = NRFX_SPIM_XFER_RX(data, len);
    const nrfx_err_t err = nrfx_spim_xfer(&self->spim, &xfer, 0);

    return (err == NRFX_SUCCESS);
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, uint8_t *data_out, uint8_t *data_in, size_t len) {
    if (len == 0)
        return true;

    const nrfx_spim_xfer_desc_t xfer = NRFX_SPIM_SINGLE_XFER(data_out, len, data_in, len);
    const nrfx_err_t err = nrfx_spim_xfer(&self->spim, &xfer, 0);

    return (err == NRFX_SUCCESS);
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t* self) {
    switch (self->spim.p_reg->FREQUENCY) {
    case NRF_SPIM_FREQ_125K:
        return 125000;
    case NRF_SPIM_FREQ_250K:
        return 250000;
    case NRF_SPIM_FREQ_500K:
        return 500000;
    case NRF_SPIM_FREQ_1M:
        return 1000000;
    case NRF_SPIM_FREQ_2M:
        return 2000000;
    case NRF_SPIM_FREQ_4M:
        return 4000000;
    case NRF_SPIM_FREQ_8M:
        return 8000000;
#ifdef SPIM_FREQUENCY_FREQUENCY_M16
    case NRF_SPIM_FREQ_16M:
        return 16000000;
#endif
#ifdef SPIM_FREQUENCY_FREQUENCY_M32
    case NRF_SPIM_FREQ_32M:
        return 32000000;
#endif
    default:
        return 0;
    }
}
