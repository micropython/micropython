/***************************************************************************//**
 * @file mpconfigboard.h
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "build-explorerkit_xg24_brd2703a/pin_functions.h"

// Micropython setup
#define MICROPY_HW_BOARD_NAME "SiLabs xG24 Explorer Kit"
#define MICROPY_HW_MCU_NAME EFR32_SERIES_LOWER

#define HSE_VALUE ((uint32_t)8000000)
#define BOARD_HSE_SOURCE (RCC_HSE_BYPASS)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)

#define MICROPY_HW_LED_STATUS (&pin_PA4)

#define DEFAULT_I2C_BUS_SDA   (&pin_PC5)
#define DEFAULT_I2C_BUS_SCL   (&pin_PC4)
#define DEFAULT_I2C_PERIPHERAL I2C0

#define DEFAULT_SPI_BUS_SCK   (&pin_PC1)
#define DEFAULT_SPI_BUS_MOSI  (&pin_PC3)
#define DEFAULT_SPI_BUS_MISO  (&pin_PC2)
#define DEFAULT_SPI_BUS_SS    (&pin_PC0)

#define NVM_BYTEARRAY_BUFFER_SIZE (512)
#define CIRCUITPY_INTERNAL_NVM_SIZE (512)
#undef MICROPY_USE_INTERNAL_PRINTF
#define MICROPY_USE_INTERNAL_PRINTF (0)
