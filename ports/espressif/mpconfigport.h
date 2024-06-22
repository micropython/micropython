// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2015 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Enable for debugging.
// #define CIRCUITPY_VERBOSE_BLE               (1)

#define MICROPY_NLR_THUMB                   (0)

#define MICROPY_USE_INTERNAL_PRINTF         (0)
#define MICROPY_PY_SYS_PLATFORM             "Espressif"

#define CIRCUITPY_DIGITALIO_HAVE_INPUT_ONLY (1)

#include "py/circuitpy_mpconfig.h"

#define MICROPY_NLR_SETJMP                  (1)
#define CIRCUITPY_DEFAULT_STACK_SIZE        0x6000

// Nearly all boards have this because it is used to enter the ROM bootloader.
#ifndef CIRCUITPY_BOOT_BUTTON
  #if defined(CONFIG_IDF_TARGET_ESP32C2) || defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32H2)
    #define CIRCUITPY_BOOT_BUTTON (&pin_GPIO9)
  #elif !defined(CONFIG_IDF_TARGET_ESP32)
    #define CIRCUITPY_BOOT_BUTTON (&pin_GPIO0)
  #endif
#endif

#define CIRCUITPY_INTERNAL_NVM_START_ADDR (0x9000)

// 20kB is statically allocated to nvs, but when overwriting an existing
// item, it's temporarily necessary to store both the old and new copies.
// Additionally, there is some overhad for the names and values of items
// in nvs, and alignment to 4kB flash erase boundaries may give better
// performance characteristics (h/t @tannewt). This implies we should select an
// 8kB size for CircuitPython'ns NVM.
#ifndef CIRCUITPY_INTERNAL_NVM_SIZE
#define CIRCUITPY_INTERNAL_NVM_SIZE (8 * 1024)
#endif

// Define to (1) in mpconfigboard.h if the board has a defined I2C port that
// lacks pull up resistors (Espressif's HMI Devkit), and the internal pull-up
// resistors will be enabled for all busio.I2C objects. This is only to
// compensate for design decisions that are out of the control of the authors
// of CircuitPython and is not an endorsement of running without appropriate
// external pull up resistors.
#ifndef CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP
#define CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP (0)
#endif

// Protect the background queue with a lock because both cores may modify it.
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
extern portMUX_TYPE background_task_mutex;
#define CALLBACK_CRITICAL_BEGIN (taskENTER_CRITICAL(&background_task_mutex))
#define CALLBACK_CRITICAL_END (taskEXIT_CRITICAL(&background_task_mutex))

// 20 dBm is the default and the highest max tx power.
// Allow a different value to be specified for boards that have trouble with using the maximum power.
#ifndef CIRCUITPY_WIFI_DEFAULT_TX_POWER
#define CIRCUITPY_WIFI_DEFAULT_TX_POWER (20)
#endif
