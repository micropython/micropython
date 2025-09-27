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
#include "hardware/ticks.h"

#if PICO_RP2040
// The RTC clock frequency is 48MHz divided by power of 2 (to ensure an integer
// division ratio will be used in the clocks block).  A divisor of 1024 generates
// an RTC clock tick of 46875Hz.  This frequency is relatively close to the
// customary 32 or 32.768kHz 'slow clock' crystals and provides good timing resolution.
#define RTC_CLOCK_FREQ_HZ       (USB_CLK_KHZ * KHZ / 1024)
#endif

static void start_all_ticks(void) {
    uint32_t cycles = clock_get_hz(clk_ref) / MHZ;
    // Note RP2040 has a single tick generator in the watchdog which serves
    // watchdog, system timer and M0+ SysTick; The tick generator is clocked from clk_ref
    // but is now adapted by the hardware_ticks library for compatibility with RP2350
    // npte: hardware_ticks library now provides an adapter for RP2040

    for (int i = 0; i < (int)TICK_COUNT; ++i) {
        tick_start((tick_gen_num_t)i, cycles);
    }
}

// Wrap the SDK's clocks_init() function to save code size
void __wrap_runtime_init_clocks(void) {
    runtime_init_clocks_optional_usb(true);
}

// Copy of runtime_init_clocks() from pico-sdk, with USB
// PLL and clock init made optional (for light sleep wakeup).
void runtime_init_clocks_optional_usb(bool init_usb) {
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
    pll_init(pll_sys, PLL_COMMON_REFDIV, PLL_SYS_VCO_FREQ_HZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
    if (init_usb) {
        pll_init(pll_usb, PLL_COMMON_REFDIV, PLL_USB_VCO_FREQ_HZ, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2);
    }
    /// \end::pll_init[]

    // Configure clocks

    // todo amy, what is this N1,2,4 meant to mean?
    // RP2040 CLK_REF = XOSC (usually) 12MHz / 1 = 12MHz
    // RP2350 CLK_REF = XOSC (XOSC_MHZ) / N (1,2,4) = 12MHz

    // clk_ref aux select is 0 because:
    //
    // - RP2040: no aux mux on clk_ref, so this field is don't-care.
    //
    // - RP2350: there is an aux mux, but we are selecting one of the
    //   non-aux inputs to the glitchless mux, so the aux select doesn't
    //   matter. The value of 0 here happens to be the sys PLL.

    clock_configure(clk_ref,
        CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC,
        0,             // No aux mux
        XOSC_HZ,
        XOSC_HZ);

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

    #if HAS_RP2040_RTC
    // CLK RTC = PLL USB 48MHz / 1024 = 46875Hz
    clock_configure(clk_rtc,
        0,             // No GLMUX
        CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
        USB_CLK_KHZ * KHZ,
        RTC_CLOCK_FREQ_HZ);
    #endif

    // CLK PERI = clk_sys. Used as reference clock for UART and SPI serial.
    clock_configure(clk_peri,
        0,
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
        SYS_CLK_KHZ * KHZ,
        SYS_CLK_KHZ * KHZ);

    #if PICO_RP2350
    // CLK_HSTX = clk_sys. Transmit bit clock for the HSTX peripheral.
    clock_configure(clk_hstx,
        0,
        CLOCKS_CLK_HSTX_CTRL_AUXSRC_VALUE_CLK_SYS,
        SYS_CLK_KHZ * KHZ,
        SYS_CLK_KHZ * KHZ);
    #endif

    // Finally, all clocks are configured so start the ticks
    // The ticks use clk_ref so now that is configured we can start them
    start_all_ticks();
}
