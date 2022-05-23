/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Copyright (c) 2016 STMicroelectronics. All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 * USB3320 ULPI functions ported from stm32f7xx_lp_modes.c
 *
 * Source file: https://github.com/STMicroelectronics/STM32CubeF7/blob/v1.15.0/Projects/STM32746G-Discovery/Examples/PWR/PWR_CurrentConsumption/Src/stm32f7xx_lp_modes.c
 */

#include "py/mphal.h"
#include "py/mperrno.h"
#include "pin_static_af.h"
#include "ulpi.h"

#if MICROPY_HW_USB_HS_ULPI3320

#include STM32_HAL_H

#define USBULPI_PHYCR           ((uint32_t)(0x40040000 + 0x034))
#define USBULPI_D07             ((uint32_t)0x000000FF)
#define USBULPI_New             ((uint32_t)0x02000000)
#define USBULPI_RW              ((uint32_t)0x00400000)
#define USBULPI_S_BUSY          ((uint32_t)0x04000000)
#define USBULPI_S_DONE          ((uint32_t)0x08000000)
#define USBULPI_TIMEOUT_COUNT   (100)

#define USB_OTG_READ_REG32(reg)  (*(__IO uint32_t *)(reg))
#define USB_OTG_WRITE_REG32(reg, value) (*(__IO uint32_t *)(reg) = (value))

/**
  * @brief  Read CR value
  * @param  Addr the Address of the ULPI Register
  * @retval Returns value of PHY CR register
  */
static uint32_t USB_ULPI_Read(uint32_t Addr) {
    uint32_t val = 0;
    uint32_t timeout = USBULPI_TIMEOUT_COUNT;

    USB_OTG_WRITE_REG32(USBULPI_PHYCR, USBULPI_New | (Addr << 16));
    val = USB_OTG_READ_REG32(USBULPI_PHYCR);
    while (((val & USBULPI_S_DONE) == 0) && (timeout--)) {
        val = USB_OTG_READ_REG32(USBULPI_PHYCR);
    }
    val = USB_OTG_READ_REG32(USBULPI_PHYCR);
    return val & 0x000000ff;
}

/**
  * @brief  Write CR value
  * @param  Addr the Address of the ULPI Register
  * @param  Data Data to write
  * @retval Returns value of PHY CR register
  */
static uint32_t USB_ULPI_Write(uint32_t Addr, uint32_t Data) {
    uint32_t val;
    uint32_t timeout = USBULPI_TIMEOUT_COUNT;

    USB_OTG_WRITE_REG32(USBULPI_PHYCR, USBULPI_New | USBULPI_RW | (Addr << 16) | (Data & 0x000000ff));
    val = USB_OTG_READ_REG32(USBULPI_PHYCR);
    while (((val & USBULPI_S_DONE) == 0) && (timeout--)) {
        val = USB_OTG_READ_REG32(USBULPI_PHYCR);
    }

    val = USB_OTG_READ_REG32(USBULPI_PHYCR);
    return 0;
}

/**
  * @brief  This function configures the USB PHY to enter the low power mode
  * @param  None
  * @retval None
  */
int ulpi_enter_low_power(void) {
    uint32_t regval = 0;

    // Disable ULPI_CLK by accessing ULPI_PHY
    // Read Vendor ID : (Low, High) 0x24,0x04 for USB3300
    regval = USB_ULPI_Read(0x00);
    if (regval != 0x24) {
        return MP_EIO;
    }

    regval = USB_ULPI_Read(0x01);
    if (regval != 0x04) {
        return MP_EIO;
    }

    // Read Product ID
    regval = USB_ULPI_Read(0x02);
    if (regval != 0x07) {
        return MP_EIO;
    }

    regval = USB_ULPI_Read(0x03);
    if (regval != 0x00) {
        return MP_EIO;
    }

    // Write to scratch register the pattern 0x55
    USB_ULPI_Write(0x16, 0x55);
    // Read to scratch Register and check-it again the written Pattern
    regval = USB_ULPI_Read(0x16);
    if (regval != 0x55) {
        return MP_EIO;
    }

    // Write to scratch register the pattern 0xAA
    USB_ULPI_Write(0x16, 0xAA);
    // Read to scratch Register and check-it again the written Pattern
    regval = USB_ULPI_Read(0x16);
    if (regval != 0xAA) {
        return MP_EIO;
    }

    // Read InterfaceControl reg
    regval = USB_ULPI_Read(0x07);

    // Write InterfaceControl reg,to disable PullUp on stp,
    // to avoid USB_PHY wake up when MCU entering standby
    USB_ULPI_Write(0x07, regval | 0x80);

    // Read InterfaceControl reg
    regval = USB_ULPI_Read(0x07);
    if (regval != 0x80) {
        return MP_EIO;
    }

    // Read FunctionControl reg
    regval = USB_ULPI_Read(0x04);

    // Reg 0x40 has a different value if USB is disconnected
    if (regval != 0x40 && regval != 0x45) {
        return MP_EIO;
    }

    // Write FunctionControl reg,to put PHY into LowPower mode
    USB_ULPI_Write(0x04, regval & (~0x40));

    // Read FunctionControl reg again
    regval = USB_ULPI_Read(0x04);
    if (regval != 0x00) {
        return MP_EIO;
    }

    // Wait for the transceiver clock to stop.
    HAL_Delay(10);

    return 0;
}

/**
 * @brief  This function wakeup the USB PHY from the Low power mode
 * @param  None
 * @retval None
 */
int ulpi_leave_low_power(void) {
    // Configure STP as an output pin
    mp_hal_pin_config(MICROPY_HW_USB_HS_ULPI_STP, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_config_speed(MICROPY_HW_USB_HS_ULPI_STP, MP_HAL_PIN_SPEED_VERY_HIGH);

    // Configure DIR as an input pin
    mp_hal_pin_config(MICROPY_HW_USB_HS_ULPI_DIR, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_config_speed(MICROPY_HW_USB_HS_ULPI_DIR, MP_HAL_PIN_SPEED_VERY_HIGH);

    // Set STP pin high
    mp_hal_pin_high(MICROPY_HW_USB_HS_ULPI_STP);

    // Wait for DIR to go low
    for (uint32_t i = 0; i < 500; i++, HAL_Delay(1)) {
        if (mp_hal_pin_read(MICROPY_HW_USB_HS_ULPI_DIR) == 0) {
            break;
        }
    }

    // Save DIR pin state before switching to alternate function
    int dir_state = mp_hal_pin_read(MICROPY_HW_USB_HS_ULPI_DIR);

    // Revert STP/DIR to their alternate functions
    mp_hal_pin_config(MICROPY_HW_USB_HS_ULPI_STP, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, GPIO_AF10_OTG_HS);
    mp_hal_pin_config_speed(MICROPY_HW_USB_HS_ULPI_STP, MP_HAL_PIN_SPEED_VERY_HIGH);

    mp_hal_pin_config(MICROPY_HW_USB_HS_ULPI_DIR, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, GPIO_AF10_OTG_HS);
    mp_hal_pin_config_speed(MICROPY_HW_USB_HS_ULPI_DIR, MP_HAL_PIN_SPEED_VERY_HIGH);

    return (dir_state == 0) ? 0 : -1;
}

#endif // MICROPY_HW_USB_HS_ULPI3320
