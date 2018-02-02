/*
 * SPI Master library for nRF5x.
 * Copyright (c) 2015 Arduino LLC
 * Copyright (c) 2016 Sandeep Mistry All right reserved.
 * Copyright (c) 2017 hathach
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

#include "nrf.h"
#include "pins.h"

// Convert frequency to clock-speed-dependent value. Return 0 if out of range.
static uint32_t baudrate_to_reg(const uint32_t baudrate) {
    uint32_t value;

    if (baudrate <= 125000) {
      value = SPI_FREQUENCY_FREQUENCY_K125;
    } else if (baudrate <= 250000) {
      value = SPI_FREQUENCY_FREQUENCY_K250;
    } else if (baudrate <= 500000) {
      value = SPI_FREQUENCY_FREQUENCY_K500;
    } else if (baudrate <= 1000000) {
      value = SPI_FREQUENCY_FREQUENCY_M1;
    } else if (baudrate <= 2000000) {
      value = SPI_FREQUENCY_FREQUENCY_M2;
    } else if (baudrate <= 4000000) {
      value = SPI_FREQUENCY_FREQUENCY_M4;
    } else {
      value = SPI_FREQUENCY_FREQUENCY_M8;
    }

    return value;
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self, const mcu_pin_obj_t * clock, const mcu_pin_obj_t * mosi, const mcu_pin_obj_t * miso) {

  // 1 for I2C, 0 for SPI
  self->spi = NRF_SPI0;

  self->spi->PSELSCK  = clock->pin;
  self->spi->PSELMOSI = mosi->pin;
  self->spi->PSELMISO = miso->pin;


#if NRF52840_XXAA
    self->spi->PSELSCK  |= (clock->port << SPI_PSEL_SCK_PORT_Pos);
    self->spi->PSELMOSI |= (mosi->port << SPI_PSEL_MOSI_PORT_Pos);
    self->spi->PSELMISO |= (miso->port << SPI_PSEL_MISO_PORT_Pos);
#endif
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
  return self->spi == NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }

#ifdef NRF52840_XXAA
    self->spi->PSEL.SCK  = SPI_PSEL_SCK_CONNECT_Disconnected;
    self->spi->PSEL.MOSI = SPI_PSEL_MOSI_CONNECT_Disconnected;
    self->spi->PSEL.MISO = SPI_PSEL_MISO_CONNECT_Disconnected;
#else
    self->spi->PSELSCK  = SPI_PSEL_SCK_PSELSCK_Disconnected;
    self->spi->PSELMOSI = SPI_PSEL_MOSI_PSELMOSI_Disconnected;
    self->spi->PSELMISO = SPI_PSEL_MISO_PSELMISO_Disconnected;
#endif
//    reset_pin(self->clock_pin);
//    reset_pin(self->MOSI_pin);
//    reset_pin(self->MISO_pin);

    self->spi = NULL;
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
  // nrf52 does not support 16 bit
  if ( bits != 8 ) return false;

  self->spi->ENABLE = (SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos);

  uint32_t config = (SPI_CONFIG_ORDER_MsbFirst << SPI_CONFIG_ORDER_Pos);

  config |= ((polarity ? SPI_CONFIG_CPOL_ActiveLow : SPI_CONFIG_CPOL_ActiveHigh) << SPI_CONFIG_CPOL_Pos);
  config |= ((phase    ? SPI_CONFIG_CPHA_Trailing  : SPI_CONFIG_CPHA_Leading   ) << SPI_CONFIG_CPHA_Pos);

  self->spi->CONFIG    = config;
  self->spi->FREQUENCY = baudrate_to_reg(baudrate);

  self->spi->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);

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
  if (len == 0) {
    return true;
  }

  while (len)
  {
    self->spi->TXD = *data;

    while(!self->spi->EVENTS_READY);

    (void) self->spi->RXD;
    data++;
    len--;

    self->spi->EVENTS_READY = 0x0UL;
  }

  return true;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_value) {
  if (len == 0) {
    return true;
  }

  while (len)
  {
    self->spi->TXD = write_value;

    while(!self->spi->EVENTS_READY);

    *data = self->spi->RXD;

    data++;
    len--;

    self->spi->EVENTS_READY = 0x0UL;
  }

  return true;
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, uint8_t *data_out, uint8_t *data_in, size_t len) {
  if (len == 0) {
    return true;
  }

  while (len)
  {
    self->spi->TXD = *data_out;

    while(!self->spi->EVENTS_READY);

    *data_in = self->spi->RXD;

    data_out++;
    data_in++;
    len--;

    self->spi->EVENTS_READY = 0x0UL;
  }

  return true;
}
  
uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t* self) {
    switch (self->spi->FREQUENCY) {
    case SPI_FREQUENCY_FREQUENCY_K125:
        return 125000;
    case SPI_FREQUENCY_FREQUENCY_K250:
        return 250000;
    case SPI_FREQUENCY_FREQUENCY_K500:
        return 500000;
    case SPI_FREQUENCY_FREQUENCY_M1:
        return 1000000;
    case SPI_FREQUENCY_FREQUENCY_M2:
        return 2000000;
    case SPI_FREQUENCY_FREQUENCY_M4:
        return 4000000;
    case SPI_FREQUENCY_FREQUENCY_M8:
        return 8000000;
    default:
        return 0;
    }
}
