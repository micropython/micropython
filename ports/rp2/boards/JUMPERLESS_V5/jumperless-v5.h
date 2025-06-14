/*
 * Copyright (c) 2024 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// -----------------------------------------------------
// NOTE: THIS HEADER IS ALSO INCLUDED BY ASSEMBLER SO
//       SHOULD ONLY CONSIST OF PREPROCESSOR DIRECTIVES
// -----------------------------------------------------
// Board definition for the Jumperless v5
//
// This header may be included by other board headers as "boards/jumperless_v5.h"

// pico_cmake_set PICO_PLATFORM=rp2350

#ifndef _BOARDS_JUMPERLESS_V5_H
#define _BOARDS_JUMPERLESS_V5_H

// On some samples, the xosc can take longer to stabilize than is usual
#ifndef PICO_XOSC_STARTUP_DELAY_MULTIPLIER
#define PICO_XOSC_STARTUP_DELAY_MULTIPLIER 64
#endif

// For board detection
#define JUMPERLESS_V5

// --- RP2350 VARIANT ---
#define PICO_RP2350A 0 // 1 for RP2350A, 0 for RP2350B

// // --- BOARD SPECIFIC ---
// TODO: see https://github.com/raspberrypi/pico-sdk/pull/2363/files as an example

// --- UART ---
#ifndef PICO_DEFAULT_UART
#define PICO_DEFAULT_UART 0
#endif
#ifndef PICO_DEFAULT_UART_TX_PIN
#define PICO_DEFAULT_UART_TX_PIN 0
#endif
#ifndef PICO_DEFAULT_UART_RX_PIN
#define PICO_DEFAULT_UART_RX_PIN 1
#endif

// --- I2C ---
#ifndef PICO_DEFAULT_I2C
#define PICO_DEFAULT_I2C 1
#endif
#ifndef PICO_DEFAULT_I2C_SDA_PIN
#define PICO_DEFAULT_I2C_SDA_PIN 4
#endif
#ifndef PICO_DEFAULT_I2C_SCL_PIN
#define PICO_DEFAULT_I2C_SCL_PIN 5
#endif

// --- SPI ---
#ifndef PICO_DEFAULT_SPI
#define PICO_DEFAULT_SPI 0
#endif
#ifndef PICO_DEFAULT_SPI_SCK_PIN
#define PICO_DEFAULT_SPI_SCK_PIN 22
#endif
#ifndef PICO_DEFAULT_SPI_TX_PIN
#define PICO_DEFAULT_SPI_TX_PIN 23
#endif
#ifndef PICO_DEFAULT_SPI_RX_PIN
#define PICO_DEFAULT_SPI_RX_PIN 20
#endif
#ifndef PICO_DEFAULT_SPI_CSN_PIN
#define PICO_DEFAULT_SPI_CSN_PIN 21
#endif

// --- FLASH ---

#define PICO_BOOT_STAGE2_CHOOSE_W25Q080 1

#ifndef PICO_FLASH_SPI_CLKDIV
#define PICO_FLASH_SPI_CLKDIV 2
#endif

// pico_cmake_set_default PICO_FLASH_SIZE_BYTES = (128 * 1024 * 1024)
#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (128 * 1024 * 1024)
#endif

// pico_cmake_set_default PICO_RP2350_A2_SUPPORTED = 1
#ifndef PICO_RP2350_A2_SUPPORTED
#define PICO_RP2350_A2_SUPPORTED 1
#endif

#endif // _BOARDS_JUMPERLESS_V5_H
