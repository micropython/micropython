/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include <string.h>
#include <stdlib.h>

#include "supervisor/background_callback.h"
#include "supervisor/board.h"
#include "supervisor/port.h"

#include "bindings/rp2pio/StateMachine.h"
#include "genhdr/mpversion.h"
#include "shared-bindings/audiopwmio/PWMAudioOut.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/countio/Counter.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/rtc/__init__.h"
#include "shared-bindings/pwmio/PWMOut.h"

#if CIRCUITPY_SSL
#include "common-hal/ssl/__init__.h"
#endif

#if CIRCUITPY_WIFI
#include "common-hal/wifi/__init__.h"
#endif

#include "common-hal/rtc/RTC.h"
#include "common-hal/busio/UART.h"

#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/stack.h"
#include "supervisor/shared/tick.h"

#include "src/rp2040/hardware_structs/include/hardware/structs/watchdog.h"
#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"
#include "src/rp2_common/hardware_uart/include/hardware/uart.h"
#include "src/rp2_common/hardware_sync/include/hardware/sync.h"
#include "src/rp2_common/hardware_timer/include/hardware/timer.h"
#if CIRCUITPY_CYW43
#include "py/mphal.h"
#include "pico/cyw43_arch.h"
#endif
#include "src/common/pico_time/include/pico/time.h"
#include "src/common/pico_binary_info/include/pico/binary_info.h"

#include "pico/bootrom.h"
#include "hardware/watchdog.h"

#include "supervisor/serial.h"

#include "tusb.h"
#include <cmsis_compiler.h>

extern volatile bool mp_msc_enabled;

STATIC void _tick_callback(uint alarm_num);

STATIC void _binary_info(void) {
    // Binary info readable with `picotool`.
    bi_decl(bi_program_name("CircuitPython"));
    bi_decl(bi_program_version_string(MICROPY_GIT_TAG));
    bi_decl(bi_program_build_date_string(MICROPY_BUILD_DATE));
    bi_decl(bi_program_url("https://circuitpython.org"));

    bi_decl(bi_program_build_attribute("BOARD=" CIRCUITPY_BOARD_ID));
    // TODO: Add build attribute for debug builds. Needs newer CircuitPython with CIRCUITPY_DEBUG.
}

extern uint32_t _ld_dtcm_bss_start;
extern uint32_t _ld_dtcm_bss_size;
extern uint32_t _ld_dtcm_data_destination;
extern uint32_t _ld_dtcm_data_size;
extern uint32_t _ld_dtcm_data_flash_copy;
extern uint32_t _ld_itcm_destination;
extern uint32_t _ld_itcm_size;
extern uint32_t _ld_itcm_flash_copy;

safe_mode_t port_init(void) {
    _binary_info();
    // Set brown out.

    // Copy all of the "tightly coupled memory" code and data to run from RAM.
    // This lets us use the 16k cache for dynamically used data and code.
    // We must do this before we try and call any of its code or load the data.
    for (uint32_t i = 0; i < ((size_t)&_ld_itcm_size) / 4; i++) {
        (&_ld_itcm_destination)[i] = (&_ld_itcm_flash_copy)[i];
        // Now zero it out to evict the line from the XIP cache. Without this,
        // it'll stay in the XIP cache anyway.
        (&_ld_itcm_flash_copy)[i] = 0x0;
    }

    // Copy all of the data to run from DTCM.
    for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_data_size) / 4; i++) {
        (&_ld_dtcm_data_destination)[i] = (&_ld_dtcm_data_flash_copy)[i];
        // Now zero it out to evict the line from the XIP cache. Without this,
        // it'll stay in the XIP cache anyway.
        (&_ld_dtcm_data_flash_copy)[i] = 0x0;
    }

    // Clear DTCM bss.
    for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_bss_size) / 4; i++) {
        (&_ld_dtcm_bss_start)[i] = 0;
    }

    #if CIRCUITPY_CYW43
    never_reset_pin_number(23);
    never_reset_pin_number(24);
    never_reset_pin_number(25);
    never_reset_pin_number(29);
    #endif

    // Reset everything into a known state before board_init.
    reset_port();

    // Initialize RTC
    common_hal_rtc_init();

    // For the tick.
    hardware_alarm_claim(0);
    hardware_alarm_set_callback(0, _tick_callback);

    // Check brownout.

    #if CIRCUITPY_CYW43
    // A small number of samples of pico w need an additional delay before
    // initializing the cyw43 chip. Delays inside cyw43_arch_init_with_country
    // are intended to meet the power on timing requirements, but apparently
    // are inadequate. We'll back off this long delay based on future testing.
    mp_hal_delay_ms(1000);
    // Change this as a placeholder as to how to init with country code.
    // Default country code is CYW43_COUNTRY_WORLDWIDE)
    if (cyw43_arch_init_with_country(PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE)) {
        serial_write("WiFi init failed\n");
    } else {
        cyw_ever_init = true;
    }
    #endif
    if (board_requests_safe_mode()) {
        return SAFE_MODE_USER;
    }

    return SAFE_MODE_NONE;
}

void reset_port(void) {
    #if CIRCUITPY_BUSIO
    reset_i2c();
    reset_spi();
    reset_uart();
    #endif

    #if CIRCUITPY_COUNTIO
    reset_countio();
    #endif

    #if CIRCUITPY_PWMIO
    pwmout_reset();
    #endif

    #if CIRCUITPY_RP2PIO
    reset_rp2pio_statemachine();
    #endif

    #if CIRCUITPY_RTC
    rtc_reset();
    #endif

    #if CIRCUITPY_AUDIOPWMIO
    audiopwmout_reset();
    #endif
    #if CIRCUITPY_AUDIOCORE
    audio_dma_reset();
    #endif

    #if CIRCUITPY_SSL
    ssl_reset();
    #endif

    #if CIRCUITPY_WIFI
    wifi_reset();
    #endif

    reset_all_pins();
}

void reset_to_bootloader(void) {
    reset_usb_boot(0, 0);
    while (true) {
    }
}

void reset_cpu(void) {
    watchdog_reboot(0, SRAM_END, 0);
    watchdog_start_tick(12);

    while (true) {
        __wfi();
    }
}

bool port_has_fixed_stack(void) {
    return false;
}

// From the linker script
extern uint32_t _ld_cp_dynamic_mem_start;
extern uint32_t _ld_cp_dynamic_mem_end;
uint32_t *port_stack_get_limit(void) {
    return &_ld_cp_dynamic_mem_start;
}

uint32_t *port_stack_get_top(void) {
    return &_ld_cp_dynamic_mem_end;
}

uint32_t *port_heap_get_bottom(void) {
    return port_stack_get_limit();
}

uint32_t *port_heap_get_top(void) {
    return port_stack_get_top();
}

void port_set_saved_word(uint32_t value) {
    // Store in a watchdog scratch register instead of RAM. 4-7 are used by the
    // sdk. 0 is used by alarm. 1-3 are free.
    watchdog_hw->scratch[1] = value;
}

uint32_t port_get_saved_word(void) {
    return watchdog_hw->scratch[1];
}

static volatile bool ticks_enabled;

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    uint64_t microseconds = time_us_64();
    return 1024 * (microseconds / 1000000) + (microseconds % 1000000) / 977;
}

STATIC void _tick_callback(uint alarm_num) {
    if (ticks_enabled) {
        supervisor_tick();
        hardware_alarm_set_target(0, delayed_by_us(get_absolute_time(), 977));
    }
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    ticks_enabled = true;
    hardware_alarm_set_target(0, delayed_by_us(get_absolute_time(), 977));
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    // One additional _tick_callback may occur, but it will just return
    // whenever !ticks_enabled. Cancel is not called just in case
    // it could nuke a timeout set by port_interrupt_after_ticks.
    ticks_enabled = false;
}

// This is called by sleep, we ignore it when our ticks are enabled because
// they'll wake us up earlier. If we don't, we'll mess up ticks by overwriting
// the next RTC wake up time.
void port_interrupt_after_ticks(uint32_t ticks) {
    if (!ticks_enabled) {
        hardware_alarm_set_target(0, delayed_by_us(get_absolute_time(), ticks * 977));
    }
}

void port_idle_until_interrupt(void) {
    common_hal_mcu_disable_interrupts();
    if (!background_callback_pending() && !tud_task_event_ready()) {
        __DSB();
        __WFI();
    }
    common_hal_mcu_enable_interrupts();
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
extern void HardFault_Handler(void); // provide a prototype to avoid a missing-prototypes diagnostic
__attribute__((used)) void __not_in_flash_func(HardFault_Handler)(void) {
    // Only safe mode from core 0 which is running CircuitPython. Core 1 faulting
    // should not be fatal to CP. (Fingers crossed.)
    if (get_core_num() == 0) {
        reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    }
    while (true) {
        asm ("nop;");
    }
}

void port_yield() {
    #if CIRCUITPY_CYW43
    cyw43_arch_poll();
    #endif
}

void port_boot_info(void) {
    #if CIRCUITPY_CYW43
    mp_printf(&mp_plat_print, "MAC");
    for (int i = 0; i < 6; i++) {
        mp_printf(&mp_plat_print, ":%02X", cyw43_state.mac[i]);
    }
    mp_printf(&mp_plat_print, "\n");
    #endif
}
