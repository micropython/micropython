/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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

// Low-level support for USB host (see shared/tinyusb/mp_usbh.c), on the USB OTG
// controller given by MICROPY_HW_USB_HOST_RHPORT.

#include "py/mphal.h"

#if MICROPY_HW_USB_HOST

#include "irq.h"
#include "shared/tinyusb/mp_usbh.h"

// The device's TinyUSB root hub port, which the host mustn't share.
#if MICROPY_HW_USB_HS && !defined(STM32N6)
#define USB_DEVICE_RHPORT (1)
#elif MICROPY_HW_USB_HS || MICROPY_HW_USB_FS
#define USB_DEVICE_RHPORT (0)
#else
#define USB_DEVICE_RHPORT (-1)
#endif
#if MICROPY_HW_USB_HOST_RHPORT == USB_DEVICE_RHPORT
#error "USB host and device can't use the same controller"
#endif

#if defined(MICROPY_HW_USB_HOST_VBUS_PIN) && !defined(MICROPY_HW_USB_HOST_VBUS_ON)
#define MICROPY_HW_USB_HOST_VBUS_ON (1)
#endif

#if defined(STM32N6)

// Both controllers have an embedded high speed PHY on dedicated pins, clocked like
// USB1's (see powerctrlboot.c) from HSE/2, which must be 24MHz.
#if MICROPY_HW_USB_HOST_RHPORT == 0
#define USBH_IRQn USB1_OTG_HS_IRQn
#define USBH_IRQHandler USB1_OTG_HS_IRQHandler
#define USBH_PHYC USB1_HS_PHYC
#define USBH_OTG LL_AHB5_GRP1_PERIPH_OTG1
#define USBH_OTGPHY LL_AHB5_GRP1_PERIPH_OTGPHY1
#define USBH_PHY_CLKSOURCE LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC
#define USBH_PHY_CKREF_CLKSOURCE LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC
#else
#define USBH_IRQn USB2_OTG_HS_IRQn
#define USBH_IRQHandler USB2_OTG_HS_IRQHandler
#define USBH_PHYC USB2_HS_PHYC
#define USBH_OTG LL_AHB5_GRP1_PERIPH_OTG2
#define USBH_OTGPHY LL_AHB5_GRP1_PERIPH_OTGPHY2
#define USBH_PHY_CLKSOURCE LL_RCC_OTGPHY2_CLKSOURCE_HSE_DIV_2_OSC
#define USBH_PHY_CKREF_CLKSOURCE LL_RCC_OTGPHY2CKREF_CLKSOURCE_HSE_DIV_2_OSC
#endif

static void usbh_controller_init(void) {
    LL_RCC_HSE_SelectHSEDiv2AsDiv2Clock();
    LL_RCC_SetOTGPHYClockSource(USBH_PHY_CLKSOURCE);
    LL_RCC_SetOTGPHYCKREFClockSource(USBH_PHY_CKREF_CLKSOURCE);

    LL_AHB5_GRP1_ForceReset(USBH_OTG | USBH_OTGPHY);
    LL_AHB5_GRP1_ReleaseReset(USBH_OTG | USBH_OTGPHY);
    LL_AHB5_GRP1_EnableClock(USBH_OTG | USBH_OTGPHY);
    LL_AHB5_GRP1_EnableClockLowPower(USBH_OTG | USBH_OTGPHY);

    // Select the 24MHz reference clock, and let the PHY's PLL lock before TinyUSB
    // resets the controller.
    MODIFY_REG(USBH_PHYC->USBPHYC_CR, USB_USBPHYC_CR_FSEL, 2 << USB_USBPHYC_CR_FSEL_Pos);
    mp_hal_delay_ms(5);
}

static void usbh_controller_deinit(void) {
    LL_AHB5_GRP1_DisableClock(USBH_OTG | USBH_OTGPHY);
}

#elif defined(STM32F4) || defined(STM32F7) || defined(STM32H7) || defined(STM32L4)

// OTG_FS, or OTG_HS with its embedded full speed PHY.
#if MICROPY_HW_USB_HOST_RHPORT == 0

#if !defined(USB_OTG_FS) && !defined(USB2_OTG_FS)
#error "USB host needs an OTG_FS controller"
#endif
#define USBH_IRQn OTG_FS_IRQn
#define USBH_IRQHandler OTG_FS_IRQHandler
#define USBH_OTG_BASE USB_OTG_FS_PERIPH_BASE
#define USBH_PIN_DM pin_A11
#define USBH_PIN_DP pin_A12
#if defined(STM32H7)
#define USBH_PIN_AF GPIO_AF10_OTG1_FS
#define USBH_CLK_ENABLE() __HAL_RCC_USB2_OTG_FS_CLK_ENABLE()
#define USBH_CLK_DISABLE() __HAL_RCC_USB2_OTG_FS_CLK_DISABLE()
#define USBH_CLK_SLEEP() do { __HAL_RCC_USB2_OTG_FS_CLK_SLEEP_ENABLE(); __HAL_RCC_USB2_OTG_FS_ULPI_CLK_SLEEP_DISABLE(); } while (0)
#else
#define USBH_PIN_AF GPIO_AF10_OTG_FS
#define USBH_CLK_ENABLE() __HAL_RCC_USB_OTG_FS_CLK_ENABLE()
#define USBH_CLK_DISABLE() __HAL_RCC_USB_OTG_FS_CLK_DISABLE()
#define USBH_CLK_SLEEP() __HAL_RCC_USB_OTG_FS_CLK_SLEEP_ENABLE()
#endif

#else

#if defined(STM32L4) || defined(USB_HS_PHYC) || !(defined(USB_OTG_HS) || defined(USB1_OTG_HS)) || defined(STM32H7A3xx) || defined(STM32H7A3xxQ) || defined(STM32H7B3xx) || defined(STM32H7B3xxQ) || defined(STM32H723xx)
#error "USB host on OTG_HS is only supported with its full speed PHY on PB14/PB15"
#endif
#define USBH_IRQn OTG_HS_IRQn
#define USBH_IRQHandler OTG_HS_IRQHandler
#define USBH_OTG_BASE USB_OTG_HS_PERIPH_BASE
#define USBH_PIN_DM pin_B14
#define USBH_PIN_DP pin_B15
#if defined(STM32H7)
#define USBH_PIN_AF GPIO_AF12_OTG2_FS
#define USBH_CLK_ENABLE() __HAL_RCC_USB1_OTG_HS_CLK_ENABLE()
#define USBH_CLK_DISABLE() __HAL_RCC_USB1_OTG_HS_CLK_DISABLE()
#define USBH_CLK_SLEEP() do { __HAL_RCC_USB1_OTG_HS_CLK_SLEEP_ENABLE(); __HAL_RCC_USB1_OTG_HS_ULPI_CLK_SLEEP_DISABLE(); } while (0)
#else
#define USBH_PIN_AF GPIO_AF12_OTG_HS_FS
#define USBH_CLK_ENABLE() __HAL_RCC_USB_OTG_HS_CLK_ENABLE()
#define USBH_CLK_DISABLE() __HAL_RCC_USB_OTG_HS_CLK_DISABLE()
#define USBH_CLK_SLEEP() do { __HAL_RCC_USB_OTG_HS_CLK_SLEEP_ENABLE(); __HAL_RCC_USB_OTG_HS_ULPI_CLK_SLEEP_DISABLE(); } while (0)
#endif

#endif

static void usbh_controller_init(void) {
    mp_hal_pin_config(USBH_PIN_DM, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, USBH_PIN_AF);
    mp_hal_pin_config_speed(USBH_PIN_DM, GPIO_SPEED_FREQ_VERY_HIGH);
    mp_hal_pin_config(USBH_PIN_DP, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, USBH_PIN_AF);
    mp_hal_pin_config_speed(USBH_PIN_DP, GPIO_SPEED_FREQ_VERY_HIGH);

    #if defined(STM32L4)
    if (__HAL_RCC_PWR_IS_CLK_DISABLED()) {
        __HAL_RCC_PWR_CLK_ENABLE();
        HAL_PWREx_EnableVddUSB();
        __HAL_RCC_PWR_CLK_DISABLE();
    } else {
        HAL_PWREx_EnableVddUSB();
    }
    #endif

    // The clock must keep running in sleep, or WFI stops the USB.
    USBH_CLK_SLEEP();
    USBH_CLK_ENABLE();

    #if defined(STM32F4) || defined(STM32F7)
    // Like ST's HAL, don't sense VBUS in host mode.
    USB_OTG_GlobalTypeDef *otg = (USB_OTG_GlobalTypeDef *)USBH_OTG_BASE;
    #if defined(USB_OTG_GCCFG_VBDEN)
    otg->GCCFG &= ~USB_OTG_GCCFG_VBDEN;
    #else
    otg->GCCFG &= ~(USB_OTG_GCCFG_VBUSASEN | USB_OTG_GCCFG_VBUSBSEN);
    otg->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
    #endif
    #endif
}

static void usbh_controller_deinit(void) {
    USBH_CLK_DISABLE();
}

#else
#error "USB host isn't supported on this MCU"
#endif

void mp_usbh_ll_init(void) {
    usbh_controller_init();

    // TinyUSB enables the IRQ, but doesn't set its priority.
    NVIC_SetPriority(USBH_IRQn, MICROPY_HW_USB_HOST_RHPORT == 0 ? IRQ_PRI_OTG_FS : IRQ_PRI_OTG_HS);

    #if defined(MICROPY_HW_USB_HOST_VBUS_PIN)
    mp_hal_pin_output(MICROPY_HW_USB_HOST_VBUS_PIN);
    mp_hal_pin_write(MICROPY_HW_USB_HOST_VBUS_PIN, MICROPY_HW_USB_HOST_VBUS_ON);
    #endif
}

void mp_usbh_ll_deinit(void) {
    #if defined(MICROPY_HW_USB_HOST_VBUS_PIN)
    mp_hal_pin_write(MICROPY_HW_USB_HOST_VBUS_PIN, !MICROPY_HW_USB_HOST_VBUS_ON);
    #endif
    NVIC_DisableIRQ(USBH_IRQn);
    usbh_controller_deinit();
}

void USBH_IRQHandler(void) {
    IRQ_ENTER(USBH_IRQn);
    mp_usbh_int_handler(MICROPY_HW_USB_HOST_RHPORT);
    IRQ_EXIT(USBH_IRQn);
}

#endif // MICROPY_HW_USB_HOST
