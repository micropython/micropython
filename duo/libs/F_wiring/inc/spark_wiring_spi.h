/**
 ******************************************************************************
 * @file    spark_wiring_spi.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    13-March-2013
 * @brief   Header for spark_wiring_spi.c module
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.
  Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#ifndef __SPARK_WIRING_SPI_H
#define __SPARK_WIRING_SPI_H

#include "spark_wiring.h"
#include "spark_wiring_platform.h"
#include "spi_hal.h"

typedef void (*wiring_spi_dma_transfercomplete_callback_t)(void);

enum FrequencyScale
{
    HZ = 1,
    KHZ = HZ*1000,
    MHZ = KHZ*1000,
    SPI_CLK_SYSTEM = 0,         // represents the system clock speed
    SPI_CLK_ARDUINO = 16*MHZ,
    SPI_CLK_CORE = 72*MHZ,
    SPI_CLK_PHOTON = 60*MHZ
};

class SPIClass {
private:
  HAL_SPI_Interface _spi;

  /**
   * Set the divider reference clock.
   * The default is the system clock.
   */
  unsigned dividerReference;

public:
  SPIClass(HAL_SPI_Interface spi);
  virtual ~SPIClass() {};

  void begin();
  void begin(uint16_t);
  void end();

  void setBitOrder(uint8_t);
  void setDataMode(uint8_t);

  /**
   * Sets the clock speed that the divider is relative to. This does not change
   * the assigned clock speed until the next call to {@link #setClockDivider}
   * @param value   The clock speed reference value
   * @param scale   The clock speed reference scalar
   *
   * E.g.
   *
   * setClockDividerReference(ARDUINO);
   * setClockDividerReference(16, MHZ);
   *
   * @see #setClockDivider
   */
  void setClockDividerReference(unsigned value, unsigned scale=HZ);

  /**
   * Sets the clock speed as a divider relative to the clock divider reference.
   * @param divider SPI_CLOCK_DIVx where x is a power of 2 from 2 to 256.
   */
  void setClockDivider(uint8_t divider);

  /**
   * Sets the absolute clock speed. This will select the clock divider that is no greater than
   * {@code value*scale}.
   * @param value
   * @param scale
   * @return the actual clock speed set.
   */
  unsigned setClockSpeed(unsigned value, unsigned scale=HZ);


   /*
    * Test method to compute the divider needed to attain a given clock frequency.
    */
  static void computeClockDivider(unsigned reference, unsigned targetSpeed, uint8_t& divider, unsigned& clock);

  byte transfer(byte _data);
  void transfer(void* tx_buffer, void* rx_buffer, size_t length, wiring_spi_dma_transfercomplete_callback_t user_callback);

  void attachInterrupt();
  void detachInterrupt();

  bool isEnabled(void);
};

#ifndef SPARK_WIRING_NO_SPI

extern SPIClass SPI;

#if Wiring_SPI1
#ifdef SPI1
#undef SPI1
#endif  // SPI1

extern SPIClass SPI1;

#endif  // Wiring_SPI1

#if Wiring_SPI2
#ifdef SPI2
#undef SPI2
#endif  // SPI2

extern SPIClass SPI2;

#endif  // Wiring_SPI2

#endif  // SPARK_WIRING_NO_SPI

#endif
