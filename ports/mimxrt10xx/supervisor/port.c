/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2017 Artur Pacholec
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

#include "boards/board.h"
#include "supervisor/port.h"

#include "fsl_device_registers.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pulseio/PWMOut.h"
#include "common-hal/rtc/RTC.h"

#include "reset.h"
#include "tick.h"

#include "tusb.h"

#if CIRCUITPY_GAMEPAD
#include "shared-module/gamepad/__init__.h"
#endif
#if CIRCUITPY_GAMEPADSHIFT
#include "shared-module/gamepadshift/__init__.h"
#endif
#include "shared-module/_pew/PewPew.h"

#include "clocks.h"

#include "fsl_gpio.h"
#include "fsl_lpuart.h"

void mpu_init(void)
{
	ARM_MPU_Disable();

	SCB_EnableDCache();
	SCB_EnableICache();
}

safe_mode_t port_init(void) {
    mpu_init();
    clocks_init();

    // Configure millisecond timer initialization.
    tick_init();

#if CIRCUITPY_RTC
    rtc_init();
#endif

    // Reset everything into a known state before board_init.
    reset_port();

    // Init the board last so everything else is ready
    board_init();

    if (board_requests_safe_mode()) {
        return USER_SAFE_MODE;
    }

    return NO_SAFE_MODE;
}

void reset_port(void) {
    //reset_sercoms();

#if CIRCUITPY_AUDIOIO
    audio_dma_reset();
    audioout_reset();
#endif
#if CIRCUITPY_AUDIOBUSIO
    i2sout_reset();
    //pdmin_reset();
#endif

#if CIRCUITPY_TOUCHIO && CIRCUITPY_TOUCHIO_USE_NATIVE
    touchin_reset();
#endif

//    eic_reset();

#if CIRCUITPY_PULSEIO
    pulseout_reset();
    pwmout_reset();
#endif

#if CIRCUITPY_RTC
    rtc_reset();
#endif

#if CIRCUITPY_GAMEPAD
    gamepad_reset();
#endif
#if CIRCUITPY_GAMEPADSHIFT
    gamepadshift_reset();
#endif
#if CIRCUITPY_PEW
    pew_reset();
#endif

    //reset_event_system();

    reset_all_pins();
}

void reset_to_bootloader(void) {
    _bootloader_dbl_tap = DBL_TAP_MAGIC;
    reset();
}

void reset_cpu(void) {
    reset();
}

extern uint32_t _heap_start, _estack;
uint32_t *port_stack_get_limit(void) {
    return &_heap_start;
}

uint32_t *port_stack_get_top(void) {
    return &_estack;
}

extern uint32_t __bss_end__;
// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
    __bss_end__ = value;
}

uint32_t port_get_saved_word(void) {
    return __bss_end__;
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
__attribute__((used)) void HardFault_Handler(void)
{
    reset_into_safe_mode(HARD_CRASH);
    while (true) {
        asm("nop;");
    }
}
