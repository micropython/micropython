/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Arduino SA
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
#include "sdram.h"

#define BOOTLOADER_MAGIC    (0xDF59)

void OPTA_board_startup(void) {
}

void OPTA_board_early_init(void) {
    HAL_InitTick(TICK_INT_PRIORITY);

    // Enable oscillator pin
    // This is enabled in the bootloader anyway.
    OPTA_board_osc_enable(true);

    #if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
    // The Arduino/mbed bootloader uses the MPU to protect sector 1
    // which is used for the flash filesystem. The following code
    // resets and disables all MPU regions configured in the bootloader.
    HAL_MPU_Disable();
    MPU_Region_InitTypeDef MPU_InitStruct;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

    for (int i = MPU_REGION_NUMBER0; i < MPU_REGION_NUMBER15; i++) {
        MPU_InitStruct.Number = i;
        MPU_InitStruct.Enable = MPU_REGION_DISABLE;
        HAL_MPU_ConfigRegion(&MPU_InitStruct);
    }
    #endif

    // Reset ETH Phy
    mp_hal_pin_config(pyb_pin_ETH_RST, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_UP, 0);
    mp_hal_pin_config_speed(pyb_pin_ETH_RST, MP_HAL_PIN_SPEED_LOW);
    mp_hal_pin_write(pyb_pin_ETH_RST, 0);
    HAL_Delay(100);
    mp_hal_pin_write(pyb_pin_ETH_RST, 1);
}

void OPTA_board_enter_bootloader(void) {
    RTC_HandleTypeDef RTCHandle;
    RTCHandle.Instance = RTC;
    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR0, BOOTLOADER_MAGIC);
    NVIC_SystemReset();
}

void OPTA_board_osc_enable(int enable) {
    mp_hal_pin_config(pyb_pin_OSCEN, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_UP, 0);
    mp_hal_pin_config_speed(pyb_pin_OSCEN, MP_HAL_PIN_SPEED_LOW);
    mp_hal_pin_write(pyb_pin_OSCEN, enable);
}

void OPTA_board_low_power(int mode) {

    #if !MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
    // Enable QSPI deepsleep for modes 1 and 2
    mp_spiflash_deepsleep(&spi_bdev.spiflash, (mode != 0));
    #endif

    #if defined(M4_APP_ADDR)
    // Signal Cortex-M4 to go to Standby mode.
    if (mode == 2) {
        __HAL_RCC_HSEM_CLK_ENABLE();
        HAL_HSEM_FastTake(0);
        HAL_HSEM_Release(0, 0);
        __HAL_RCC_HSEM_CLK_DISABLE();
        HAL_Delay(100);
    }
    #endif
}
