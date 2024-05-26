// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "Simmel"
#define MICROPY_HW_MCU_NAME         "nRF52833"

#define MICROPY_HW_LED_STATUS          (&pin_P0_06)

#if SPI_FLASH_FILESYSTEM
#define SPI_FLASH_MOSI_PIN &pin_P0_09
#define SPI_FLASH_MISO_PIN &pin_P1_04
#define SPI_FLASH_SCK_PIN &pin_P0_10
#define SPI_FLASH_CS_PIN &pin_P1_06
#endif

#define CIRCUITPY_INTERNAL_NVM_SIZE 0
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (76 * 1024)

#define BOOTLOADER_SIZE                 (0x4000)  // 12 kiB
#define CIRCUITPY_BLE_CONFIG_SIZE       (12 * 1024)

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_08)
#define DEFAULT_I2C_BUS_SDA         (&pin_P1_09)

// Reduce nRF SoftRadio memory usage
#define BLEIO_VS_UUID_COUNT 10
#define BLEIO_HVN_TX_QUEUE_SIZE 2
#define BLEIO_CENTRAL_ROLE_COUNT 2
#define BLEIO_PERIPH_ROLE_COUNT 2
#define BLEIO_TOTAL_CONNECTION_COUNT 2
#define BLEIO_ATTR_TAB_SIZE (BLE_GATTS_ATTR_TAB_SIZE_DEFAULT * 2)

#define SOFTDEVICE_RAM_SIZE (32 * 1024)

#define MICROPY_FATFS_EXFAT 0
