/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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
 *
 * ESP-Hosted WiFi HAL.
 */

#ifndef MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_HAL_H
#define MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_HAL_H

#ifndef ESP_HOSTED_DEBUG
#define ESP_HOSTED_DEBUG    (0)
#endif

#if ESP_HOSTED_DEBUG
#define PROTOBUF_C_UNPACK_ERROR(...)    error_printf(__VA_ARGS__);
#endif

#define ANSI_C_RED          "\x1B[31m"
#define ANSI_C_GREEN        "\x1B[32m"
#define ANSI_C_YELLOW       "\x1B[33m"
#define ANSI_C_BLUE         "\x1B[34m"
#define ANSI_C_MAGENTA      "\x1B[35m"
#define ANSI_C_CYAN         "\x1B[36m"
#define ANSI_C_WHITE        "\x1B[37m"
#define ANSI_C_DEFAULT      "\x1B[0m"

#if ESP_HOSTED_DEBUG
#define do_printf(...)      mp_printf(&mp_plat_print, __VA_ARGS__)
#else
#define do_printf(...)
#endif

// Logging macros.
#define debug_printf(...)   do_printf(ANSI_C_BLUE); do_printf(__VA_ARGS__); do_printf(ANSI_C_DEFAULT);
#define info_printf(...)    do_printf(ANSI_C_GREEN); do_printf(__VA_ARGS__); do_printf(ANSI_C_DEFAULT);
#define warn_printf(...)    do_printf(ANSI_C_YELLOW); do_printf(__VA_ARGS__); do_printf(ANSI_C_DEFAULT);
#define error_printf(...)   do_printf(ANSI_C_RED); do_printf(__VA_ARGS__); do_printf(ANSI_C_DEFAULT);
#define crit_printf(...)    do_printf(ANSI_C_MAGENTA); do_printf(__VA_ARGS__); do_printf(ANSI_C_DEFAULT);

typedef enum {
    ESP_HOSTED_MODE_BT,
    ESP_HOSTED_MODE_WIFI,
} esp_hosted_mode_t;

// HAL functions to be implemented by ports.
// Note A default machine-based implementation is provided in esp_hosted_hal.c.
int esp_hosted_hal_init(uint32_t mode);
int esp_hosted_hal_deinit(void);
void esp_hosted_hal_irq_enable(bool enable);
bool esp_hosted_hal_data_ready(void);
int esp_hosted_hal_atomic_enter(void);
int esp_hosted_hal_atomic_exit(void);
int esp_hosted_hal_spi_transfer(const uint8_t *tx_buf, uint8_t *rx_buf, uint32_t size);

// Memory management functions.
// Note alloc/free need to match the Protobuf allocator signature.
void *esp_hosted_hal_alloc(void *user, size_t size);
void esp_hosted_hal_free(void *user, void *ptr);
void *esp_hosted_hal_calloc(size_t nmemb, size_t size);
void *esp_hosted_hal_realloc(void *ptr, size_t size);
#endif // MICROPY_INCLUDED_DRIVERS_ESP_HOSTED_HAL_H
