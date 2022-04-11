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

#include "clock_config.h"
#include "modmachine.h"


const uint8_t dcd_data[] = { 0x00 };

void board_init(void) {
    // Init clock
    BOARD_BootClockRUN();
    SystemCoreClockUpdate();

    // Enable IOCON clock
    CLOCK_EnableClock(kCLOCK_Iomuxc);

    // ------------- SDRAM ------------ //
    #ifdef MICROPY_HW_SDRAM_AVAIL
    mimxrt_sdram_init();
    #endif

    // 1ms tick timer
    SysTick_Config(SystemCoreClock / 1000);

    // ------------- USB0 ------------- //
    // Clock
    CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
    CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, 480000000U);

    #ifdef USBPHY1
    USBPHY_Type *usb_phy = USBPHY1;
    #else
    USBPHY_Type *usb_phy = USBPHY;
    #endif

    // Enable PHY support for Low speed device + LS via FS Hub
    usb_phy->CTRL |= USBPHY_CTRL_SET_ENUTMILEVEL2_MASK | USBPHY_CTRL_SET_ENUTMILEVEL3_MASK;

    // Enable all power for normal operation
    usb_phy->PWD = 0;

    // TX Timing
    uint32_t phytx = usb_phy->TX;
    phytx &= ~(USBPHY_TX_D_CAL_MASK | USBPHY_TX_TXCAL45DM_MASK | USBPHY_TX_TXCAL45DP_MASK);
    phytx |= USBPHY_TX_D_CAL(0x0C) | USBPHY_TX_TXCAL45DP(0x06) | USBPHY_TX_TXCAL45DM(0x06);
    usb_phy->TX = phytx;

    // USB1
    //  CLOCK_EnableUsbhs1PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
    //  CLOCK_EnableUsbhs1Clock(kCLOCK_Usb480M, 480000000U);

    // ADC
    machine_adc_init();

    // PIT
    machine_timer_init_PIT();

    // SDCard
    #if MICROPY_PY_MACHINE_SDCARD
    machine_sdcard_init0();
    #endif

    #if MICROPY_PY_MACHINE_I2S
    machine_i2s_init0();
    #endif
    // RTC
    machine_rtc_start();
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
