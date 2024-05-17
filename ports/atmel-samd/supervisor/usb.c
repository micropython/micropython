// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "atmel_start_pins.h"
#include "hpl/pm/hpl_pm_base.h"
#include "hpl/gclk/hpl_gclk_base.h"
#include "hal_gpio.h"
#include "lib/tinyusb/src/device/usbd.h"
#include "supervisor/background_callback.h"
#include "supervisor/usb.h"

void init_usb_hardware(void) {
    #ifdef SAMD21
    _pm_enable_bus_clock(PM_BUS_APBB, USB);
    _pm_enable_bus_clock(PM_BUS_AHB, USB);
    _gclk_enable_channel(USB_GCLK_ID, GCLK_CLKCTRL_GEN_GCLK0_Val);
    #endif

    #ifdef SAM_D5X_E5X
    hri_gclk_write_PCHCTRL_reg(GCLK, USB_GCLK_ID, GCLK_PCHCTRL_GEN_GCLK1_Val | GCLK_PCHCTRL_CHEN);
    hri_mclk_set_AHBMASK_USB_bit(MCLK);
    hri_mclk_set_APBBMASK_USB_bit(MCLK);
    #endif

    gpio_set_pin_direction(PIN_PA24, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(PIN_PA24, false);
    gpio_set_pin_pull_mode(PIN_PA24, GPIO_PULL_OFF);
    gpio_set_pin_direction(PIN_PA25, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(PIN_PA25, false);
    gpio_set_pin_pull_mode(PIN_PA25, GPIO_PULL_OFF);
    #ifdef SAMD21
    gpio_set_pin_function(PIN_PA24, PINMUX_PA24G_USB_DM);
    gpio_set_pin_function(PIN_PA25, PINMUX_PA25G_USB_DP);
    #endif
    #ifdef SAM_D5X_E5X
    gpio_set_pin_function(PIN_PA24, PINMUX_PA24H_USB_DM);
    gpio_set_pin_function(PIN_PA25, PINMUX_PA25H_USB_DP);
    #endif
}

#ifdef SAMD21
void USB_Handler(void) {
    usb_irq_handler(0);
}
#endif

#ifdef SAM_D5X_E5X
// These are different subsets of USB interrupts, *NOT* different USB peripherals.
void USB_0_Handler(void) {
    usb_irq_handler(0);
}

void USB_1_Handler(void) {
    usb_irq_handler(0);
}

void USB_2_Handler(void) {
    usb_irq_handler(0);
}

void USB_3_Handler(void) {
    usb_irq_handler(0);
}
#endif
