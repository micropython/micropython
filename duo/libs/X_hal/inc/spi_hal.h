/**
 ******************************************************************************
 * @file    spi_hal.h
 * @author  Satish Nair, Brett Walach
 * @version V1.0.0
 * @date    12-Sept-2014
 * @brief
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_HAL_H
#define __SPI_HAL_H

/* Includes ------------------------------------------------------------------*/
#include "pinmap_hal.h"

/* Exported types ------------------------------------------------------------*/
typedef enum HAL_SPI_Interface {
    HAL_SPI_INTERFACE1 = 0,    //maps to SPI1 (pins: A3, A4, A5)
    HAL_SPI_INTERFACE2 = 1     //maps to SPI3 (pins: D4, D3, D2)
#if PLATFORM_ID == 10 // Electron
   ,HAL_SPI_INTERFACE3 = 2     //maps to SPI3 (pins: C3, C2, C1)
#endif
} HAL_SPI_Interface;

typedef void (*HAL_SPI_DMA_UserCallback)(void);

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
#define SPI_MODE0               0x00
#define SPI_MODE1               0x01
#define SPI_MODE2               0x02
#define SPI_MODE3               0x03

#define SPI_CLOCK_DIV2          0x00
#define SPI_CLOCK_DIV4          0x08
#define SPI_CLOCK_DIV8          0x10
#define SPI_CLOCK_DIV16         0x18
#define SPI_CLOCK_DIV32         0x20
#define SPI_CLOCK_DIV64         0x28
#define SPI_CLOCK_DIV128        0x30
#define SPI_CLOCK_DIV256        0x38

#define SPI_DEFAULT_SS          ((uint16_t)(-1))

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hal_spi_info_t {
    uint16_t size;

    uint32_t system_clock;      // the clock speed that is divided when setting a divider

} hal_spi_info_t;

void HAL_SPI_Init(HAL_SPI_Interface spi);
void HAL_SPI_Begin(HAL_SPI_Interface spi, uint16_t pin);
void HAL_SPI_End(HAL_SPI_Interface spi);
void HAL_SPI_Set_Bit_Order(HAL_SPI_Interface spi, uint8_t order);
void HAL_SPI_Set_Data_Mode(HAL_SPI_Interface spi, uint8_t mode);
void HAL_SPI_Set_Clock_Divider(HAL_SPI_Interface spi, uint8_t rate);
uint16_t HAL_SPI_Send_Receive_Data(HAL_SPI_Interface spi, uint16_t data);
void HAL_SPI_DMA_Transfer(HAL_SPI_Interface spi, void* tx_buffer, void* rx_buffer, uint32_t length, HAL_SPI_DMA_UserCallback userCallback);
bool HAL_SPI_Is_Enabled_Old();
bool HAL_SPI_Is_Enabled(HAL_SPI_Interface spi);
void HAL_SPI_Info(HAL_SPI_Interface spi, hal_spi_info_t* info, void* reserved);

#ifdef __cplusplus
}
#endif

#endif  /* __SPI_HAL_H */
