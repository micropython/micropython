// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "stm32f411ce-blackpill"
#define MICROPY_HW_MCU_NAME         "STM32F411CE"

#define FLASH_SIZE                  (0x80000)
#define FLASH_PAGE_SIZE             (0x4000)

#define HSE_VALUE ((uint32_t)25000000)
#define BOARD_NO_VBUS_SENSE (1)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)

// On-board flash
// #define SPI_FLASH_MOSI_PIN          (&pin_PA07)
// #define SPI_FLASH_MISO_PIN          (&pin_PA06)
// #define SPI_FLASH_SCK_PIN           (&pin_PA05)
// #define SPI_FLASH_CS_PIN            (&pin_PA04)

#define DEFAULT_I2C_BUS_SCL (&pin_PB06)
#define DEFAULT_I2C_BUS_SDA (&pin_PB07)

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x2000 - 0xC000)
