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
 */

#include <string.h>
#include "py/mphal.h"
#include "storage.h"
#include "ulpi.h"

void NICLAV_board_startup(void) {
}

void NICLAV_board_pmic_enable(int);

void NICLAV_board_early_init(void) {
    HAL_InitTick(0);

    // Enable oscillator pin
    // This is enabled in the bootloader anyway.
    NICLAV_board_osc_enable(true);

    // Re/Enable PMIC rails.
    NICLAV_board_pmic_enable(true);

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

    #if MICROPY_HW_USB_HS_ULPI3320
    // Make sure UPLI is Not in low-power mode.
    ulpi_leave_low_power();
    #endif
}

void NICLAV_board_enter_bootloader(void) {
    RTC_HandleTypeDef RTCHandle;
    RTCHandle.Instance = RTC;
    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR0, 0xDF59);
    NVIC_SystemReset();
}

void NICLAV_board_osc_enable(int enable) {
    mp_hal_pin_config(pyb_pin_OSCEN, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_UP, 0);
    mp_hal_pin_config_speed(pyb_pin_OSCEN, MP_HAL_PIN_SPEED_LOW);
    mp_hal_pin_write(pyb_pin_OSCEN, enable);
}

void NICLAV_board_pmic_enable(int enable) {
    __HAL_RCC_GPIOF_CLK_ENABLE();

    // Configure PMIC I2C GPIOs
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

    // Configure PMIC I2C
    I2C_HandleTypeDef i2c;
    i2c.Instance = I2C2;
    i2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2c.Init.Timing = 0x20D09DE7;
    i2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    i2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
    i2c.Init.OwnAddress1 = 0xFE;
    i2c.Init.OwnAddress2 = 0xFE;
    i2c.Init.OwnAddress2Masks = 0;

    __HAL_RCC_I2C2_CLK_ENABLE();
    __HAL_RCC_I2C2_FORCE_RESET();
    __HAL_RCC_I2C2_RELEASE_RESET();
    HAL_I2C_Init(&i2c);

    // LDO1 2V8
    HAL_I2C_Master_Transmit(&i2c, 0x08 << 1, (uint8_t [2]) {0x4D, (enable) ? 0x01 : 0x00}, 2, 1000);
    // LDO2 1V8 CAM
    HAL_I2C_Master_Transmit(&i2c, 0x08 << 1, (uint8_t [2]) {0x50, (enable) ? 0x01 : 0x00}, 2, 1000);
    // LDO3 1V8
    HAL_I2C_Master_Transmit(&i2c, 0x08 << 1, (uint8_t [2]) {0x53, (enable) ? 0x01 : 0x00}, 2, 1000);
    // SW2  VDDIO_EXT
    HAL_I2C_Master_Transmit(&i2c, 0x08 << 1, (uint8_t [2]) {0x3b, (enable) ? 0x81 : 0x80}, 2, 1000);
    // SW3  3V3 (Leaving 3.3 on lowers ULPI current).
    // HAL_I2C_Master_Transmit(&i2c, 0x08<<1, (uint8_t [2]) {0x41, (enable) ? 0x81 : 0x80}, 2, 1000);
    // SW1  1V8 (Main supply is Never turned off)
    // HAL_I2C_Master_Transmit(&i2c, 0x08<<1, (uint8_t [2]) {0x35, (enable) ? 0x81 : 0x80}, 2, 1000);

    HAL_I2C_DeInit(&i2c);
    __HAL_RCC_I2C2_FORCE_RESET();
    __HAL_RCC_I2C2_RELEASE_RESET();
    __HAL_RCC_I2C2_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_0);
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_1);
    __HAL_RCC_GPIOF_CLK_DISABLE();
}

static __attribute__((naked, noreturn)) void cm4_enter_standby(void) {
    // Clear and mask D1 EXTIs.
    EXTI_D1->PR1 = 0x3fffff;
    EXTI_D1->IMR1 &= ~(0xFFFFu); // 16 lines

    // Clear and mask D2 EXTIs.
    EXTI_D2->IMR1 &= ~(0xFFFFu); // 16 lines
    EXTI_D2->PR1 = 0x3fffff;

    EXTI->D3PMR1 = 0;///0x0238FFFFu;
    EXTI->D3PMR2 = 0;///0x003F020Cu;
    EXTI->D3PMR3 = 0;///0x01000000u;

    // Set D2/D2 PDDS bits.
    PWR->CPUCR |= (PWR_CPUCR_PDDS_D2 | PWR_CPUCR_PDDS_D3);
    PWR->CPU2CR |= (PWR_CPU2CR_PDDS_D2 | PWR_CPU2CR_PDDS_D3);
    CLEAR_BIT(PWR->CPU2CR, PWR_CPU2CR_RUN_D3);

    // Set SLEEPDEEP bit.
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // Enter CPU stop mode.
    __WFI();
}

void NICLAV_board_low_power(int mode) {
    switch (mode) {
        case 0:     // Leave stop mode.
            ulpi_leave_low_power();
            break;
        case 1:     // Enter stop mode.
            ulpi_enter_low_power();
            break;
        case 2:     // Enter standby mode.
            ulpi_enter_low_power();
            break;
    }

    #if (MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE == 0)
    // Enable QSPI deepsleep for modes 1 and 2
    mp_spiflash_deepsleep(&spi_bdev.spiflash, (mode != 0));
    #endif

    if (mode == 2) {
        if (RCC->GCR & RCC_GCR_BOOT_C2) {
            // If CM4 is booted, it should be running a firmware
            // that supports low-power mode entry via HSEM.
            __HAL_RCC_HSEM_CLK_ENABLE();
            HAL_HSEM_FastTake(0);
            HAL_HSEM_Release(0, 0);
        } else {
            // If CM4 is not booted, rig an ISR vector, with a reset
            // handler that points to standby function and boot it.
            volatile extern char _cm4_ram_start[1024];

            uint32_t *cm4_ram = (uint32_t *)_cm4_ram_start;
            cm4_ram[0] = (uint32_t)(_cm4_ram_start + 1024);
            cm4_ram[1] = ((uint32_t)&cm4_enter_standby) | 1;
            SCB_CleanDCache_by_Addr((uint32_t *)_cm4_ram_start, 8);

            HAL_PWREx_HoldCore(PWR_CORE_CPU2);
            HAL_SYSCFG_CM4BootAddConfig(SYSCFG_BOOT_ADDR0, (uint32_t)_cm4_ram_start);
            HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);
        }

        // Wait for the CM4 to enter stop mode.
        HAL_Delay(100);

        // Disable all power rails, except core voltage.
        NICLAV_board_pmic_enable(false);

        // Reset all buses, peripherals, GPIO clocks etc..
        RCC->AHB1RSTR = 0x0A00C023U;
        RCC->AHB2RSTR = 0x00000271U;
        RCC->AHB3RSTR = 0x00015031U;
        RCC->AHB4RSTR = 0x0328077FU; // LSE GPIO port is masked.

        RCC->APB1LRSTR = 0xE8FFC3FFU;
        RCC->APB1HRSTR = 0x00000136U;
        RCC->APB2RSTR = 0x31D73033U;
        RCC->APB3RSTR = 0x00000018U;
        RCC->APB4RSTR = 0x0020DEAAU;
    }
}
