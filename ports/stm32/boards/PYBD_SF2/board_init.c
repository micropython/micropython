/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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

#include <string.h>
#include "py/mphal.h"
#include "storage.h"

#if defined(STM32F722xx) || defined(STM32F723xx) || defined(STM32F732xx) || defined(STM32F733xx)
#define OTP_ADDR (0x1ff079e0)
#else
#define OTP_ADDR (0x1ff0f3c0)
#endif
#define OTP ((pyb_otp_t*)OTP_ADDR)

typedef struct _pyb_otp_t {
    uint16_t series;
    uint16_t rev;
    uint8_t mac[6];
} pyb_otp_t;

void mboot_board_early_init(void) {
    // Enable 500mA on WBUS-DIP28
    mp_hal_pin_config(pyb_pin_W23, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
}

void board_early_init(void) {
    // Configure EN_3V3 as an output pin, but keep it turned off
    mp_hal_pin_config(pyb_pin_EN_3V3, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0);

    // Explicitly init SPI2 because it's not enabled as a block device
    spi_bdev_ioctl(&spi_bdev2, BDEV_IOCTL_INIT, (uint32_t)&spiflash2_config);
}

#if !BUILDING_MBOOT

void board_sleep(int value) {
    mp_spiflash_deepsleep(&spi_bdev.spiflash, value);
    mp_spiflash_deepsleep(&spi_bdev2.spiflash, value);
}

void mp_hal_get_mac(int idx, uint8_t buf[6]) {
    // Check if OTP region has a valid MAC address, and use it if it does
    if (OTP->series == 0x00d1 && OTP->mac[0] == 'H' && OTP->mac[1] == 'J' && OTP->mac[2] == '0') {
        #if __GNUC__ >= 11
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Warray-bounds"
        #pragma GCC diagnostic ignored "-Wstringop-overread"
        #endif
        memcpy(buf, OTP->mac, 6);
        #if __GNUC__ >= 11
        #pragma GCC diagnostic pop
        #endif
        buf[5] += idx;
        return;
    }

    // Generate a random locally administered MAC address (LAA)
    mp_hal_generate_laa_mac(idx, buf);
}

#endif
