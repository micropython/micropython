/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>
#include "supervisor/port.h"
#include "boards/board.h"

#include "nrfx/hal/nrf_power.h"
#include "nrfx/drivers/include/nrfx_power.h"

#include "nrf/cache.h"
#include "nrf/clocks.h"
#include "nrf/power.h"
#include "nrf/timers.h"

#include "shared-module/gamepad/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "common-hal/bleio/__init__.h"
#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"
#include "common-hal/pulseio/PWMOut.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/rtc/RTC.h"
#include "tick.h"

#include "shared-bindings/rtc/__init__.h"

static void power_warning_handler(void) {
    reset_into_safe_mode(BROWNOUT);
}

safe_mode_t port_init(void) {
    nrf_peripherals_clocks_init();

    // If GPIO voltage is set wrong in UICR, this will fix it, and
    // will also do a reset to make the change take effect.
    nrf_peripherals_power_init();

    nrfx_power_pofwarn_config_t power_failure_config;
    power_failure_config.handler = power_warning_handler;
    power_failure_config.thr = NRF_POWER_POFTHR_V27;
    #if NRF_POWER_HAS_VDDH
    power_failure_config.thrvddh = NRF_POWER_POFTHRVDDH_V27;
    #endif
    nrfx_power_pof_init(&power_failure_config);
    nrfx_power_pof_enable(&power_failure_config);

    nrf_peripherals_enable_cache();

    // Configure millisecond timer initialization.
    tick_init();

    #if CIRCUITPY_RTC
    rtc_init();
    #endif

    // Will do usb_init() if chip supports USB.
    board_init();

    return NO_SAFE_MODE;
}

void reset_port(void) {
#ifdef CIRCUITPY_GAMEPAD_TICKS
    gamepad_reset();
#endif

    i2c_reset();
    spi_reset();
    uart_reset();

#if CIRCUITPY_PULSEIO
    pwmout_reset();
    pulseout_reset();
    pulsein_reset();
#endif

    timers_reset();

#if CIRCUITPY_RTC
    rtc_reset();
#endif

#if CIRCUITPY_BLEIO
    bleio_reset();
#endif

    reset_all_pins();
}

void reset_to_bootloader(void) {
    enum { DFU_MAGIC_SERIAL = 0x4e };

    NRF_POWER->GPREGRET = DFU_MAGIC_SERIAL;
    reset_cpu();
}

void reset_cpu(void) {
    NVIC_SystemReset();
}

extern uint32_t _ebss;
// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
    _ebss = value;
}

uint32_t port_get_saved_word(void) {
    return _ebss;
}

void HardFault_Handler(void) {
    reset_into_safe_mode(HARD_CRASH);
    while (true) {
        asm("nop;");
    }
}
