// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "STM32F412G_DISCO"
#define MICROPY_HW_MCU_NAME         "STM32F412xGS"

#define FLASH_SIZE                  (0x100000)
#define FLASH_PAGE_SIZE             (0x4000)

#define HSE_VALUE ((uint32_t)8000000)
#define LSE_VALUE ((uint32_t)32768)
#define BOARD_HSE_SOURCE (RCC_HSE_BYPASS) // ST boards use the STLink clock signal
#define BOARD_HAS_LOW_SPEED_CRYSTAL (1)
#define CPY_CLK_USB_USES_AUDIOPLL (1)

#define DEFAULT_I2C_BUS_SCL (&pin_PB10)
#define DEFAULT_I2C_BUS_SDA (&pin_PB09)
