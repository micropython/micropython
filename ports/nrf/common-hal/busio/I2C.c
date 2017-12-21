/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Sandeep Mistry All right reserved.
 * Copyright (c) 2017 hathach
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

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "pins.h"
#include "nrf.h"

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self, const mcu_pin_obj_t* scl, const mcu_pin_obj_t* sda, uint32_t frequency) {
    if (scl->pin == sda->pin) {
        mp_raise_ValueError("Invalid pins");
    }

    NRF_GPIO->PIN_CNF[scl->pin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                                    | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                                    | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                                    | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                                    | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

    NRF_GPIO->PIN_CNF[sda->pin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                                    | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                                    | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                                    | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                                    | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

    // 1 for I2C, 0 for SPI
    self->twi = NRF_TWIM1;

    if ( frequency < 100000 ) {
      self->twi->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K100;
    }else if ( frequency < 250000 ) {
      self->twi->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K250;
    }else {
      self->twi->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K400;
    }

    self->twi->ENABLE = (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);

    self->twi->PSEL.SCL = scl->pin;
    self->twi->PSEL.SDA = sda->pin;

}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
  return self->twi->ENABLE == 0;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }

    uint8_t scl_pin = self->twi->PSEL.SCL;
    uint8_t sda_pin = self->twi->PSEL.SDA;

    self->twi->ENABLE   = (TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos);
    self->twi->PSEL.SCL = (TWIM_PSEL_SCL_CONNECT_Disconnected << TWIM_PSEL_SCL_CONNECT_Pos);
    self->twi->PSEL.SDA = (TWIM_PSEL_SDA_CONNECT_Disconnected << TWIM_PSEL_SDA_CONNECT_Pos);

    reset_pin(scl_pin);
    reset_pin(sda_pin);
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
  // Write no data when just probing
  return 0 == common_hal_busio_i2c_write(self, addr, NULL, 0, true);
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    bool grabbed_lock = false;
//    CRITICAL_SECTION_ENTER()
        if (!self->has_lock) {
            grabbed_lock = true;
            self->has_lock = true;
        }
//    CRITICAL_SECTION_LEAVE();
    return grabbed_lock;
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
}

uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr, const uint8_t *data, size_t len, bool stopBit) {
  NRF_TWIM_Type* twi = self->twi;

  twi->ADDRESS       = addr;
  twi->TASKS_RESUME  = 1;

  twi->TXD.PTR       = (uint32_t) data;
  twi->TXD.MAXCNT    = len;

  twi->TASKS_STARTTX = 1;

  // Wait for TX started
  while(!twi->EVENTS_TXSTARTED && !twi->EVENTS_ERROR) {}
  twi->EVENTS_TXSTARTED = 0;

  // Wait for TX complete
  if ( len )
  {
    while(!twi->EVENTS_LASTTX && !twi->EVENTS_ERROR) {}
    twi->EVENTS_LASTTX = 0x0UL;
  }

  if (stopBit || twi->EVENTS_ERROR)
  {
    twi->TASKS_STOP = 0x1UL;
    while(!twi->EVENTS_STOPPED);
    twi->EVENTS_STOPPED = 0x0UL;
  }
  else
  {
    twi->TASKS_SUSPEND = 0x1UL;
    while(!twi->EVENTS_SUSPENDED);
    twi->EVENTS_SUSPENDED = 0x0UL;
  }

  if (twi->EVENTS_ERROR)
  {
    twi->EVENTS_ERROR = 0x0UL;
    uint32_t error = twi->ERRORSRC;
    twi->ERRORSRC = error;

    return error;
  }

  return 0;
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr, uint8_t *data, size_t len) {
  NRF_TWIM_Type* twi = self->twi;

  if(len == 0) return 0;
  bool stopBit = true; // should be a parameter

  twi->ADDRESS       = addr;
  twi->TASKS_RESUME  = 0x1UL;

  twi->RXD.PTR       = (uint32_t) data;
  twi->RXD.MAXCNT    = len;

  twi->TASKS_STARTRX = 0x1UL;

  while(!twi->EVENTS_RXSTARTED && !twi->EVENTS_ERROR);
  twi->EVENTS_RXSTARTED = 0x0UL;

  while(!twi->EVENTS_LASTRX && !twi->EVENTS_ERROR);
  twi->EVENTS_LASTRX = 0x0UL;

  if (stopBit || twi->EVENTS_ERROR)
  {
    twi->TASKS_STOP = 0x1UL;
    while(!twi->EVENTS_STOPPED);
    twi->EVENTS_STOPPED = 0x0UL;
  }
  else
  {
    twi->TASKS_SUSPEND = 0x1UL;
    while(!twi->EVENTS_SUSPENDED);
    twi->EVENTS_SUSPENDED = 0x0UL;
  }

  if (twi->EVENTS_ERROR)
  {
    twi->EVENTS_ERROR = 0x0UL;
    uint32_t error = twi->ERRORSRC;
    twi->ERRORSRC = error;

    return error;
  }

  // number of byte read
//  (void) _p_twim->RXD.AMOUNT;

  return 0;
}
