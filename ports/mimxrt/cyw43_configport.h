/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Damien P. George
 * Copyright (c) 2022 Jim Mussared
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
 */
#ifndef MICROPY_INCLUDED_MIMXRT_CYW43_CONFIGPORT_H
#define MICROPY_INCLUDED_MIMXRT_CYW43_CONFIGPORT_H

// The board-level config will be included here, so it can set some CYW43 values.
#include "extmod/mpbthci.h"
#include "extmod/cyw43_config_common.h"
#include "sdio.h"

#define CYW43_USE_SPI                   (0)
#define CYW43_ENABLE_BLUETOOTH_OVER_UART (1)
#define CYW43_LWIP                      (1)
#define CYW43_USE_STATS                 (0)

#ifndef CYW43_CHIPSET_FIRMWARE_INCLUDE_FILE
#define CYW43_CHIPSET_FIRMWARE_INCLUDE_FILE "lib/cyw43-driver/firmware/w4343WA1_7_45_98_50_combined.h"
#endif

#ifndef CYW43_WIFI_NVRAM_INCLUDE_FILE
#define CYW43_WIFI_NVRAM_INCLUDE_FILE   "lib/cyw43-driver/firmware/wifi_nvram_1dx.h"
#endif

#ifndef CYW43_BT_FIRMWARE_INCLUDE_FILE
#define CYW43_BT_FIRMWARE_INCLUDE_FILE  "lib/cyw43-driver/firmware/cyw43_btfw_4343A1.h"
#endif

#ifdef MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY
#define CYW43_BT_UART_BAUDRATE_ACTIVE_USE MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY
#endif

#ifdef MICROPY_HW_BLE_UART_BAUDRATE_DOWNLOAD_FIRMWARE
#define CYW43_BT_UART_BAUDRATE_DOWNLOAD_FIRMWARE MICROPY_HW_BLE_UART_BAUDRATE_DOWNLOAD_FIRMWARE
#endif

#define CYW43_CLEAR_SDIO_INT            (1)

#define CYW43_SDPCM_SEND_COMMON_WAIT    __WFI();
#define CYW43_DO_IOCTL_WAIT             __WFI();

#define cyw43_hal_uart_set_baudrate     mp_bluetooth_hci_uart_set_baudrate
#define cyw43_hal_uart_write            mp_bluetooth_hci_uart_write
#define cyw43_hal_uart_readchar         mp_bluetooth_hci_uart_readchar

#define cyw43_bluetooth_controller_init     mp_bluetooth_hci_controller_init
#define cyw43_bluetooth_controller_deinit   mp_bluetooth_hci_controller_deinit
#define cyw43_bluetooth_controller_woken    mp_bluetooth_hci_controller_woken
#define cyw43_bluetooth_controller_wakeup   mp_bluetooth_hci_controller_wakeup
#define cyw43_bluetooth_controller_sleep_maybe mp_bluetooth_hci_controller_sleep_maybe

#define CYW43_PIN_WL_REG_ON             MICROPY_HW_WL_REG_ON
#define CYW43_PIN_WL_HOST_WAKE          MICROPY_HW_WL_HOST_WAKE
#define CYW43_PIN_WL_SDIO_1             MICROPY_HW_SDIO_D1

#define CYW43_PIN_BT_REG_ON             MICROPY_HW_BT_REG_ON
#ifdef MICROPY_HW_BT_HOST_WAKE
#define CYW43_PIN_BT_HOST_WAKE          MICROPY_HW_BT_HOST_WAKE
#endif
#ifdef MICROPY_HW_BT_DEV_WAKE
#define CYW43_PIN_BT_DEV_WAKE           MICROPY_HW_BT_DEV_WAKE
#endif
#ifdef MICROPY_HW_BT_CTS
#define CYW43_PIN_BT_CTS                MICROPY_HW_BT_CTS
#endif

#if MICROPY_HW_ENABLE_RF_SWITCH
#define CYW43_PIN_RFSW_VDD              pin_WL_RFSW_VDD
#endif

#undef cyw43_hal_pin_config // mp_hal_pin_config not yet implemented on this port

static inline void cyw43_hal_pin_config(cyw43_hal_pin_obj_t pin, uint8_t mode, uint8_t pull, uint8_t alt) {
    machine_pin_set_mode(pin, mode);
}

static inline void cyw43_hal_pin_config_irq_falling(cyw43_hal_pin_obj_t pin, int enable) {
    if (enable) {
        machine_pin_config(pin, PIN_MODE_IT_FALLING, PIN_PULL_UP_100K, 0, 0, PIN_AF_MODE_ALT5);
    }
}

static inline void cyw43_sdio_init(void) {
    sdio_init(NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 14, 0));
}

static inline void cyw43_sdio_reinit(void) {
    sdio_reenable();
}

static inline void cyw43_sdio_deinit(void) {
    sdio_deinit();
}

static inline void cyw43_sdio_set_irq(bool enable) {
    sdio_enable_irq(enable);
}

static inline void cyw43_sdio_enable_high_speed_4bit(void) {
    sdio_enable_high_speed_4bit();
}

static inline int cyw43_sdio_transfer(uint32_t cmd, uint32_t arg, uint32_t *resp) {
    return sdio_transfer(cmd, arg, resp);
}

static inline int cyw43_sdio_transfer_cmd53(bool write, uint32_t block_size, uint32_t arg, size_t len, uint8_t *buf) {
    return sdio_transfer_cmd53(write, block_size, arg, len, buf);
}

#endif // MICROPY_INCLUDED_MIMXRT_CYW43_CONFIGPORT_H
