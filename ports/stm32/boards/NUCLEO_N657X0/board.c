/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 Damien P. George
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

#include "py/mphal.h"
#include "boardctrl.h"
#include "xspi.h"

// Values for OTP fuses for VDDIO3, to select low voltage mode (<2.5V).
// See RM0486, Section 5, Table 18.
#define BSEC_HW_CONFIG_ID       (124U)
#define BSEC_HWS_HSLV_VDDIO3    (1U << 15)

static void board_config_vdd(void) {
    // TODO: move some of the below code to a common location for all N6 boards?

    // Enable PWR, BSEC and SYSCFG clocks.
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR);
    LL_APB4_GRP2_EnableClock(LL_APB4_GRP2_PERIPH_BSEC);
    LL_APB4_GRP2_EnableClock(LL_APB4_GRP2_PERIPH_SYSCFG);

    // Program high speed IO optimization fuses if they aren't already set.
    uint32_t fuse;
    BSEC_HandleTypeDef hbsec = { .Instance = BSEC };
    const uint32_t mask = BSEC_HWS_HSLV_VDDIO3;
    if (HAL_BSEC_OTP_Read(&hbsec, BSEC_HW_CONFIG_ID, &fuse) != HAL_OK) {
        fuse = 0;
    } else if ((fuse & mask) != mask) {
        // Program the fuse, and read back the set value.
        if (HAL_BSEC_OTP_Program(&hbsec, BSEC_HW_CONFIG_ID, fuse | mask, HAL_BSEC_NORMAL_PROG) != HAL_OK) {
            fuse = 0;
        } else if (HAL_BSEC_OTP_Read(&hbsec, BSEC_HW_CONFIG_ID, &fuse) != HAL_OK) {
            fuse = 0;
        }
    }

    // Enable Vdd ADC, needed for the ADC to work.
    LL_PWR_EnableVddADC();

    // Configure VDDIO2.
    LL_PWR_EnableVddIO2();
    LL_PWR_SetVddIO2VoltageRange(LL_PWR_VDDIO_VOLTAGE_RANGE_3V3);
    SYSCFG->VDDIO2CCCR |= SYSCFG_VDDIO2CCCR_EN; // enable IO compensation

    // Configure VDDIO3.  Only enable 1.8V mode if the fuse is set.
    LL_PWR_EnableVddIO3();
    if (fuse & BSEC_HWS_HSLV_VDDIO3) {
        LL_PWR_SetVddIO3VoltageRange(LL_PWR_VDDIO_VOLTAGE_RANGE_1V8);
    }
    SYSCFG->VDDIO3CCCR |= SYSCFG_VDDIO3CCCR_EN; // enable IO compensation

    // Configure VDDIO4.
    LL_PWR_EnableVddIO4();
    LL_PWR_SetVddIO4VoltageRange(LL_PWR_VDDIO_VOLTAGE_RANGE_3V3);
    SYSCFG->VDDIO4CCCR |= SYSCFG_VDDIO4CCCR_EN; // enable IO compensation

    // Enable VDD for ADC and USB.
    LL_PWR_EnableVddADC();
    LL_PWR_EnableVddUSB();
}

void mboot_board_early_init(void) {
    board_config_vdd();
    xspi_init();
}

void board_early_init(void) {
    #if !MICROPY_HW_RUNS_FROM_EXT_FLASH
    // Firmware runs directly from SRAM, so configure VDD and enable XSPI flash.
    board_config_vdd();
    xspi_init();
    #endif
}

void board_leave_standby(void) {
    // TODO: move some of the below code to a common location for all N6 boards?

    // Enable PWR, BSEC and SYSCFG clocks.
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR);
    LL_APB4_GRP2_EnableClock(LL_APB4_GRP2_PERIPH_BSEC);
    LL_APB4_GRP2_EnableClock(LL_APB4_GRP2_PERIPH_SYSCFG);

    // Configure VDDIO2.
    LL_PWR_EnableVddIO2();
    LL_PWR_SetVddIO2VoltageRange(LL_PWR_VDDIO_VOLTAGE_RANGE_3V3);
    SYSCFG->VDDIO2CCCR |= SYSCFG_VDDIO2CCCR_EN; // enable IO compensation

    // Configure VDDIO3 (1.8V mode selection is retained).
    LL_PWR_EnableVddIO3();
    SYSCFG->VDDIO3CCCR |= SYSCFG_VDDIO3CCCR_EN; // enable IO compensation

    // Configure VDDIO4.
    LL_PWR_EnableVddIO4();
    LL_PWR_SetVddIO4VoltageRange(LL_PWR_VDDIO_VOLTAGE_RANGE_3V3);
    SYSCFG->VDDIO4CCCR |= SYSCFG_VDDIO4CCCR_EN; // enable IO compensation

    // Enable VDD for ADC and USB.
    LL_PWR_EnableVddADC();
    LL_PWR_EnableVddUSB();
}
