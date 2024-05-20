// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 flom84
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME "NUCLEO F446RE"
#define MICROPY_HW_MCU_NAME "STM32F446xx"

#define FLASH_SIZE (0x80000u)     // 512K
#define FLASH_PAGE_SIZE (0x4000u) // 16K

#define HSE_VALUE ((uint32_t)8000000u)
#define BOARD_HSE_SOURCE (RCC_HSE_ON)
// The schematic has a 32k crystal that isn't fitted. Uncommented the line below if you add it.
// #define BOARD_HAS_LOW_SPEED_CRYSTAL (1)
// #define LSE_VALUE  ((uint32_t)32000U)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)

// USART3 + USB FTDI
// #define CIRCUITPY_CONSOLE_UART_TX (&pin_PC10)
// #define CIRCUITPY_CONSOLE_UART_RX (&pin_PC11)

// USART2 + ST link
// #define CIRCUITPY_CONSOLE_UART_TX (&pin_PA02)
// #define CIRCUITPY_CONSOLE_UART_RX (&pin_PA03)

// Status LEDs
#define MICROPY_HW_LED_STATUS (&pin_PA05)

#define MICROPY_FATFS_EXFAT 0

#define BOARD_NO_VBUS_SENSE (1)
