/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2023 Damien P. George
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

// This file is never compiled standalone, it's included directly from
// extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE.

#include "py/mphal.h"
#include "mp_usbd.h"
#include "modmachine.h"
#include "uart.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "hardware/structs/rosc.h"
#include "hardware/structs/scb.h"
#include "hardware/structs/syscfg.h"
#include "hardware/watchdog.h"
#include "hardware/xosc.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "pico/unique_id.h"
#if MICROPY_PY_NETWORK_CYW43
#include "lib/cyw43-driver/src/cyw43.h"
#endif

#define RP2_RESET_PWRON (1)
#define RP2_RESET_WDT (3)

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&machine_rtc_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) }, \
    \
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(RP2_RESET_PWRON) }, \
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(RP2_RESET_WDT) }, \

static mp_obj_t mp_machine_unique_id(void) {
    pico_unique_board_id_t id;
    pico_get_unique_board_id(&id);
    return mp_obj_new_bytes(id.id, sizeof(id.id));
}

NORETURN static void mp_machine_reset(void) {
    watchdog_reboot(0, SRAM_END, 0);
    for (;;) {
        __wfi();
    }
}

static mp_int_t mp_machine_reset_cause(void) {
    int reset_cause;
    if (watchdog_caused_reboot()) {
        reset_cause = RP2_RESET_WDT;
    } else {
        reset_cause = RP2_RESET_PWRON;
    }
    return reset_cause;
}

NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args) {
    MICROPY_BOARD_ENTER_BOOTLOADER(n_args, args);
    rosc_hw->ctrl = ROSC_CTRL_ENABLE_VALUE_ENABLE << ROSC_CTRL_ENABLE_LSB;
    reset_usb_boot(0, 0);
    for (;;) {
    }
}

static mp_obj_t mp_machine_get_freq(void) {
    return MP_OBJ_NEW_SMALL_INT(mp_hal_get_cpu_freq());
}

static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    mp_int_t freq = mp_obj_get_int(args[0]);
    if (!set_sys_clock_khz(freq / 1000, false)) {
        mp_raise_ValueError(MP_ERROR_TEXT("cannot change frequency"));
    }
    #if MICROPY_HW_ENABLE_UART_REPL
    setup_default_uart();
    mp_uart_init();
    #endif
}

static void mp_machine_idle(void) {
    __wfe();
}

static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    mp_int_t delay_ms = 0;
    bool use_timer_alarm = false;

    if (n_args == 1) {
        delay_ms = mp_obj_get_int(args[0]);
        if (delay_ms <= 1) {
            // Sleep is too small, just use standard delay.
            mp_hal_delay_ms(delay_ms);
            return;
        }
        use_timer_alarm = delay_ms < (1ULL << 32) / 1000;
        if (use_timer_alarm) {
            // Use timer alarm to wake.
        } else {
            // TODO: Use RTC alarm to wake.
            mp_raise_ValueError(MP_ERROR_TEXT("sleep too long"));
        }
    }

    const uint32_t xosc_hz = XOSC_MHZ * 1000000;

    uint32_t my_interrupts = MICROPY_BEGIN_ATOMIC_SECTION();
    #if MICROPY_PY_NETWORK_CYW43
    if (cyw43_has_pending && cyw43_poll != NULL) {
        MICROPY_END_ATOMIC_SECTION(my_interrupts);
        return;
    }
    #endif

    #if MICROPY_HW_ENABLE_USBDEV
    // Only disable the USB clock if a USB host has not configured the device
    bool disable_usb = !tud_mounted();
    #else
    bool disable_usb = true;
    #endif
    if (disable_usb) {
        clock_stop(clk_usb);
    }

    clock_stop(clk_adc);

    // CLK_REF = XOSC
    clock_configure(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, xosc_hz, xosc_hz);

    // CLK_SYS = CLK_REF
    clock_configure(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF, 0, xosc_hz, xosc_hz);

    // CLK_RTC = XOSC / 256
    clock_configure(clk_rtc, 0, CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_XOSC_CLKSRC, xosc_hz, xosc_hz / 256);

    // CLK_PERI = CLK_SYS
    clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS, xosc_hz, xosc_hz);

    // Disable PLLs.
    pll_deinit(pll_sys);
    if (disable_usb) {
        pll_deinit(pll_usb);
    }

    // Disable ROSC.
    rosc_hw->ctrl = ROSC_CTRL_ENABLE_VALUE_DISABLE << ROSC_CTRL_ENABLE_LSB;

    if (n_args == 0) {
        #if MICROPY_PY_NETWORK_CYW43
        gpio_set_dormant_irq_enabled(CYW43_PIN_WL_HOST_WAKE, GPIO_IRQ_LEVEL_HIGH, true);
        #endif
        xosc_dormant();
    } else {
        uint32_t sleep_en0 = clocks_hw->sleep_en0;
        uint32_t sleep_en1 = clocks_hw->sleep_en1;
        bool timer3_enabled = irq_is_enabled(3);

        clocks_hw->sleep_en0 = CLOCKS_SLEEP_EN0_CLK_RTC_RTC_BITS;
        if (use_timer_alarm) {
            // Make sure ALARM3/IRQ3 is enabled on _this_ core
            timer_hw->inte |= 1 << 3;
            if (!timer3_enabled) {
                irq_set_enabled(3, true);
            }
            // Use timer alarm to wake.
            clocks_hw->sleep_en1 = CLOCKS_SLEEP_EN1_CLK_SYS_TIMER_BITS;
            timer_hw->alarm[3] = timer_hw->timerawl + delay_ms * 1000;
        } else {
            // TODO: Use RTC alarm to wake.
            clocks_hw->sleep_en1 = 0;
        }

        if (!disable_usb) {
            clocks_hw->sleep_en0 |= CLOCKS_SLEEP_EN0_CLK_SYS_PLL_USB_BITS;
            clocks_hw->sleep_en1 |= CLOCKS_SLEEP_EN1_CLK_USB_USBCTRL_BITS;
        }

        scb_hw->scr |= M0PLUS_SCR_SLEEPDEEP_BITS;
        __wfi();
        scb_hw->scr &= ~M0PLUS_SCR_SLEEPDEEP_BITS;
        if (!timer3_enabled) {
            irq_set_enabled(3, false);
        }
        clocks_hw->sleep_en0 = sleep_en0;
        clocks_hw->sleep_en1 = sleep_en1;
    }

    // Enable ROSC.
    rosc_hw->ctrl = ROSC_CTRL_ENABLE_VALUE_ENABLE << ROSC_CTRL_ENABLE_LSB;

    // Bring back all clocks.
    clocks_init();
    MICROPY_END_ATOMIC_SECTION(my_interrupts);
}

NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    mp_machine_lightsleep(n_args, args);
    mp_machine_reset();
}
