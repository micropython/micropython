/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pico.h"
#include "clocks_extra.h"
#include "hardware/regs/clocks.h"
#include "hardware/platform_defs.h"
#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"

#define RTC_CLOCK_FREQ_HZ       (USB_CLK_KHZ * KHZ / 1024)

// Wrap the SDK's clocks_init() function to save code size
void __wrap_clocks_init(void) {
    clocks_init_optional_usb(true);
}

// Copy of clocks_init() from pico-sdk, with USB
// PLL and clock init made optional (for light sleep wakeup).
void clocks_init_optional_usb(bool init_usb) {
    // Start tick in watchdog, the argument is in 'cycles per microsecond' i.e. MHz
    watchdog_start_tick(XOSC_KHZ / KHZ);

    // Modification: removed FPGA check here

    // Disable resus that may be enabled from previous software
    clocks_hw->resus.ctrl = 0;

    // Enable the xosc
    xosc_init();

    // Before we touch PLLs, switch sys and ref cleanly away from their aux sources.
    hw_clear_bits(&clocks_hw->clk[clk_sys].ctrl, CLOCKS_CLK_SYS_CTRL_SRC_BITS);
    while (clocks_hw->clk[clk_sys].selected != 0x1) {
        tight_loop_contents();
    }
    hw_clear_bits(&clocks_hw->clk[clk_ref].ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);
    while (clocks_hw->clk[clk_ref].selected != 0x1) {
        tight_loop_contents();
    }

    /// \tag::pll_init[]
    pll_init(pll_sys, PLL_COMMON_REFDIV, PLL_SYS_VCO_FREQ_KHZ * KHZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
    if (init_usb) {
        pll_init(pll_usb, PLL_COMMON_REFDIV, PLL_USB_VCO_FREQ_KHZ * KHZ, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2);
    }
    /// \end::pll_init[]

    // Configure clocks
    // CLK_REF = XOSC (usually) 12MHz / 1 = 12MHz
    clock_configure(clk_ref,
        CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC,
        0,             // No aux mux
        XOSC_KHZ * KHZ,
        XOSC_KHZ * KHZ);

    /// \tag::configure_clk_sys[]
    // CLK SYS = PLL SYS (usually) 125MHz / 1 = 125MHz
    clock_configure(clk_sys,
        CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
        CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
        SYS_CLK_KHZ * KHZ,
        SYS_CLK_KHZ * KHZ);
    /// \end::configure_clk_sys[]

    if (init_usb) {
        // CLK USB = PLL USB 48MHz / 1 = 48MHz
        clock_configure(clk_usb,
            0, // No GLMUX
            CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
            USB_CLK_KHZ * KHZ,
            USB_CLK_KHZ * KHZ);
    }

    // CLK ADC = PLL USB 48MHZ / 1 = 48MHz
    clock_configure(clk_adc,
        0,             // No GLMUX
        CLOCKS_CLK_ADC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
        USB_CLK_KHZ * KHZ,
        USB_CLK_KHZ * KHZ);

    // CLK RTC = PLL USB 48MHz / 1024 = 46875Hz
    clock_configure(clk_rtc,
        0,             // No GLMUX
        CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
        USB_CLK_KHZ * KHZ,
        RTC_CLOCK_FREQ_HZ);

    // CLK PERI = clk_sys. Used as reference clock for Peripherals. No dividers so just select and enable
    // Normally choose clk_sys or clk_usb
    clock_configure(clk_peri,
        0,
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
        SYS_CLK_KHZ * KHZ,
        SYS_CLK_KHZ * KHZ);
}
