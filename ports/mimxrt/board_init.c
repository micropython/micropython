/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Based on tinyusb/hw/bsp/teensy_40/teensy40.c
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018, hathach (tinyusb.org)
 * Copyright (c) 2020, Jim Mussared
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

#include "py/runtime.h"

#include "tusb.h"

#include "fsl_device_registers.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "fsl_clock.h"
#include "fsl_lpuart.h"

#include CLOCK_CONFIG_H
#include "modmachine.h"
#include "irq.h"

const uint8_t dcd_data[] = { 0x00 };

void usb_phy0_init(uint8_t d_cal, uint8_t txcal45dp, uint8_t txcal45dn);

void board_init(void) {
    // Clean and enable cache
    SCB_CleanDCache();
    SCB_EnableDCache();
    SCB_EnableICache();
    // Init clock
    BOARD_BootClockRUN();
    #if !defined(MIMXRT117x_SERIES)
    CLOCK_SetMode(kCLOCK_ModeRun);
    #endif
    SystemCoreClockUpdate();

    // Enable IOCON clock
    CLOCK_EnableClock(kCLOCK_Iomuxc);

    // SDRAM
    #if MICROPY_HW_SDRAM_AVAIL
    mimxrt_sdram_init();
    #endif

    // 1ms tick timer
    SysTick_Config(SystemCoreClock / 1000);
    NVIC_SetPriority(SysTick_IRQn, IRQ_PRI_SYSTICK);

    // USB0
    usb_phy0_init(0b0111, 0b0110, 0b0110);  // Configure nominal values for D_CAL and TXCAL45DP/DN

    // ADC
    machine_adc_init();

    // SDCard
    #if MICROPY_PY_MACHINE_SDCARD
    machine_sdcard_init0();
    #endif

    #if MICROPY_PY_MACHINE_I2S
    machine_i2s_init0();
    #endif
    // RTC
    machine_rtc_start();

    // OCRAM wait states (discarded, but code kept)
    #if 0
    MECC1->PIPE_ECC_EN =
        MECC_PIPE_ECC_EN_READ_DATA_WAIT_EN(1) |
        MECC_PIPE_ECC_EN_READ_ADDR_PIPE_EN(1) |
        MECC_PIPE_ECC_EN_WRITE_DATA_PIPE_EN(1) |
        MECC_PIPE_ECC_EN_WRITE_ADDR_PIPE_EN(1);

    MECC2->PIPE_ECC_EN =
        MECC_PIPE_ECC_EN_READ_DATA_WAIT_EN(1) |
        MECC_PIPE_ECC_EN_READ_ADDR_PIPE_EN(1) |
        MECC_PIPE_ECC_EN_WRITE_DATA_PIPE_EN(1) |
        MECC_PIPE_ECC_EN_WRITE_ADDR_PIPE_EN(1);

    FLEXRAM->FLEXRAM_CTRL =
        FLEXRAM_FLEXRAM_CTRL_OCRAM_RDATA_WAIT_EN(1) |
        FLEXRAM_FLEXRAM_CTRL_OCRAM_RADDR_PIPELINE_EN(1) |
        FLEXRAM_FLEXRAM_CTRL_OCRAM_WRDATA_PIPELINE_EN(1) |
        FLEXRAM_FLEXRAM_CTRL_OCRAM_WRADDR_PIPELINE_EN(1);
    #endif
}

void usb_phy0_init(uint8_t d_cal, uint8_t txcal45dp, uint8_t txcal45dn) {
    #ifdef USBPHY1
    USBPHY_Type *usb_phy = USBPHY1;
    #else
    USBPHY_Type *usb_phy = USBPHY;
    #endif

    CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, BOARD_XTAL0_CLK_HZ);
    CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, BOARD_XTAL0_CLK_HZ);

    #if defined(MIMXRT117x_SERIES)
    usb_phy->TRIM_OVERRIDE_EN = USBPHY_TRIM_OVERRIDE_EN_TRIM_DIV_SEL_OVERRIDE(1) |
        USBPHY_TRIM_OVERRIDE_EN_TRIM_ENV_TAIL_ADJ_VD_OVERRIDE(1) |
        USBPHY_TRIM_OVERRIDE_EN_TRIM_TX_D_CAL_OVERRIDE(1) |
        USBPHY_TRIM_OVERRIDE_EN_TRIM_TX_CAL45DP_OVERRIDE(1) |
        USBPHY_TRIM_OVERRIDE_EN_TRIM_TX_CAL45DN_OVERRIDE(1);  // Enable override for D_CAL and TXCAL45DP/DN
    #endif
    usb_phy->PWD = 0U;  // Set all bits in PWD register to normal operation
    usb_phy->TX = ((usb_phy->TX & (~(USBPHY_TX_D_CAL_MASK | USBPHY_TX_TXCAL45DM_MASK | USBPHY_TX_TXCAL45DP_MASK))) |
        (USBPHY_TX_D_CAL(d_cal) | USBPHY_TX_TXCAL45DP(txcal45dp) | USBPHY_TX_TXCAL45DM(txcal45dn)));  // Configure values for D_CAL and TXCAL45DP/DN
}

void USB_OTG1_IRQHandler(void) {
    tud_int_handler(0);
    tud_task();
    __SEV();
}

void USB_OTG2_IRQHandler(void) {
    tud_int_handler(1);
    tud_task();
    __SEV();
}
