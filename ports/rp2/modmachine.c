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
#include "rp2_psram.h"
#include "rp2_flash.h"
#include "clocks_extra.h"
#include "hardware/pll.h"
#include "hardware/structs/rosc.h"
#include "hardware/structs/scb.h"
#include "hardware/structs/syscfg.h"
#include "hardware/watchdog.h"
#include "hardware/xosc.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "pico/unique_id.h"
#include "pico/runtime_init.h"
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

MP_NORETURN static void mp_machine_reset(void) {
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

MP_NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args) {
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

    // If necessary, increase the flash divider before increasing the clock speed
    const int old_freq = clock_get_hz(clk_sys);
    rp2_flash_set_timing_for_freq(MAX(freq, old_freq));

    if (!set_sys_clock_khz(freq / 1000, false)) {
        mp_raise_ValueError(MP_ERROR_TEXT("cannot change frequency"));
    }
    if (n_args > 1) {
        mp_int_t freq_peri = mp_obj_get_int(args[1]);
        if (freq_peri != (USB_CLK_KHZ * KHZ)) {
            if (freq_peri == freq) {
                clock_configure(clk_peri,
                    0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                    freq,
                    freq);
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("peripheral freq must be 48_000_000 or the same as the MCU freq"));
            }
        }
    }

    // If clock speed was reduced, maybe we can reduce the flash divider
    if (freq < old_freq) {
        rp2_flash_set_timing_for_freq(freq);
    }

    #if MICROPY_HW_ENABLE_UART_REPL
    setup_default_uart();
    mp_uart_init();
    #endif
    #if MICROPY_HW_ENABLE_PSRAM
    psram_init(MICROPY_HW_PSRAM_CS_PIN);
    #endif
}

static void mp_machine_idle(void) {
    MICROPY_INTERNAL_WFE(1);
}

static void alarm_sleep_callback(uint alarm_id) {
}

// Set this to 1 to enable some debug of the interrupt that woke the device
#define DEBUG_LIGHTSLEEP 0

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

    uint32_t my_interrupts = MICROPY_BEGIN_ATOMIC_SECTION();
    #if MICROPY_PY_NETWORK_CYW43
    if (cyw43_poll_is_pending()) {
        MICROPY_END_ATOMIC_SECTION(my_interrupts);
        return;
    }
    #endif

    #if MICROPY_PY_THREAD
    static bool in_lightsleep;
    if (in_lightsleep) {
        // The other CPU is also in machine.lightsleep()
        MICROPY_END_ATOMIC_SECTION(my_interrupts);
        return;
    }
    in_lightsleep = true;
    #endif

    #if MICROPY_HW_ENABLE_USBDEV
    // Only disable the USB clock if a USB host has not configured the device
    // or if going to DORMANT mode.
    bool disable_usb = !(tud_mounted() && n_args > 0);
    #else
    bool disable_usb = true;
    #endif
    if (disable_usb) {
        clock_stop(clk_usb);
    }

    clock_stop(clk_adc);
    #if PICO_RP2350
    clock_stop(clk_hstx);
    #endif

    // CLK_REF = XOSC
    clock_configure(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ, XOSC_HZ);

    // CLK_SYS = CLK_REF
    clock_configure(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF, 0, XOSC_HZ, XOSC_HZ);

    // CLK_RTC = XOSC / 256
    #if PICO_RP2040
    clock_configure(clk_rtc, 0, CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_XOSC_CLKSRC, XOSC_HZ, XOSC_HZ / 256);
    #endif

    // CLK_PERI = CLK_SYS
    clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS, XOSC_HZ, XOSC_HZ);

    // Disable PLLs.
    pll_deinit(pll_sys);
    if (disable_usb) {
        pll_deinit(pll_usb);
    }

    // Disable ROSC.
    rosc_hw->ctrl = ROSC_CTRL_ENABLE_VALUE_DISABLE << ROSC_CTRL_ENABLE_LSB;

    #if DEBUG_LIGHTSLEEP
    #if PICO_RP2040
    uint32_t pending_intr = 0;
    #else
    uint32_t pending_intr[2] = { 0 };
    #endif
    #endif

    bool alarm_armed = false;
    if (n_args == 0) {
        #if MICROPY_PY_NETWORK_CYW43
        gpio_set_dormant_irq_enabled(CYW43_PIN_WL_HOST_WAKE, GPIO_IRQ_LEVEL_HIGH, true);
        #endif
        xosc_dormant();
    } else {
        uint32_t save_sleep_en0 = clocks_hw->sleep_en0;
        uint32_t save_sleep_en1 = clocks_hw->sleep_en1;
        if (use_timer_alarm) {
            // Use timer alarm to wake.
            clocks_hw->sleep_en0 = 0x0;
            #if PICO_RP2040
            clocks_hw->sleep_en1 = CLOCKS_SLEEP_EN1_CLK_SYS_TIMER_BITS;
            #elif PICO_RP2350
            clocks_hw->sleep_en1 = CLOCKS_SLEEP_EN1_CLK_REF_TICKS_BITS | CLOCKS_SLEEP_EN1_CLK_SYS_TIMER0_BITS;
            #else
            #error Unknown processor
            #endif
            hardware_alarm_claim(MICROPY_HW_LIGHTSLEEP_ALARM_NUM);
            hardware_alarm_set_callback(MICROPY_HW_LIGHTSLEEP_ALARM_NUM, alarm_sleep_callback);
            if (hardware_alarm_set_target(MICROPY_HW_LIGHTSLEEP_ALARM_NUM, make_timeout_time_ms(delay_ms)) == PICO_OK) {
                alarm_armed = true;
            }
        } else {
            // TODO: Use RTC alarm to wake.
            clocks_hw->sleep_en0 = 0x0;
            clocks_hw->sleep_en1 = 0x0;
        }

        if (!disable_usb) {
            clocks_hw->sleep_en0 |= CLOCKS_SLEEP_EN0_CLK_SYS_PLL_USB_BITS;
            #if PICO_RP2040
            clocks_hw->sleep_en1 |= CLOCKS_SLEEP_EN1_CLK_USB_USBCTRL_BITS;
            #elif PICO_RP2350
            clocks_hw->sleep_en1 |= CLOCKS_SLEEP_EN1_CLK_USB_BITS;
            #else
            #error Unknown processor
            #endif
        }

        #if PICO_ARM
        // Configure SLEEPDEEP bits on Cortex-M CPUs.
        #if PICO_RP2040
        scb_hw->scr |= M0PLUS_SCR_SLEEPDEEP_BITS;
        #elif PICO_RP2350
        scb_hw->scr |= M33_SCR_SLEEPDEEP_BITS;
        #else
        #error Unknown processor
        #endif
        #endif

        // Go into low-power mode.
        if (alarm_armed) {
            __wfi();

            #if DEBUG_LIGHTSLEEP
            #if PICO_RP2040
            pending_intr = nvic_hw->ispr;
            #else
            pending_intr[0] = nvic_hw->ispr[0];
            pending_intr[1] = nvic_hw->ispr[1];
            #endif
            #endif
        }
        clocks_hw->sleep_en0 = save_sleep_en0;
        clocks_hw->sleep_en1 = save_sleep_en1;
    }

    // Enable ROSC.
    rosc_hw->ctrl = ROSC_CTRL_ENABLE_VALUE_ENABLE << ROSC_CTRL_ENABLE_LSB;

    // Bring back all clocks.
    runtime_init_clocks_optional_usb(disable_usb);
    MICROPY_END_ATOMIC_SECTION(my_interrupts);

    // Re-sync mp_hal_time_ns() counter with aon timer.
    mp_hal_time_ns_set_from_rtc();

    // Note: This must be done after MICROPY_END_ATOMIC_SECTION
    if (use_timer_alarm) {
        if (alarm_armed) {
            hardware_alarm_cancel(MICROPY_HW_LIGHTSLEEP_ALARM_NUM);
        }
        hardware_alarm_set_callback(MICROPY_HW_LIGHTSLEEP_ALARM_NUM, NULL);
        hardware_alarm_unclaim(MICROPY_HW_LIGHTSLEEP_ALARM_NUM);

        #if DEBUG_LIGHTSLEEP
        // Check irq.h for the list of IRQ's
        // for rp2040 00000042: TIMER_IRQ_1 woke the device as expected
        //            00000020: USBCTRL_IRQ woke the device (probably early)
        // For rp2350 00000000:00000002: TIMER0_IRQ_1 woke the device as expected
        //            00000000:00004000: USBCTRL_IRQ woke the device (probably early)
        #if PICO_RP2040
        mp_printf(MP_PYTHON_PRINTER, "lightsleep: pending_intr=%08lx\n", pending_intr);
        #else
        mp_printf(MP_PYTHON_PRINTER, "lightsleep: pending_intr=%08lx:%08lx\n", pending_intr[1], pending_intr[0]);
        #endif
        #endif
    }

    #if MICROPY_PY_THREAD
    // Clearing the flag here is atomic, and we know we're the ones who set it
    // (higher up, inside the critical section)
    in_lightsleep = false;
    #endif
}

MP_NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    mp_machine_lightsleep(n_args, args);
    mp_machine_reset();
}
