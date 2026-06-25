/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2025 Infineon Technologies AG
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

// std includes

// mpy includes
#include "py/obj.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/runtime/pyexec.h"
#include "extmod/modnetwork.h"

// port-specific includes
#include "cybsp.h"
#include "cy_syslib.h"
#include "modmachine.h"
#include "modpsocedge.h"

enum clock_freq_type PLL0_freq = AUDIO_SYS_CLOCK_73_728_000_HZ;
enum clock_freq_type freq_peri;

#if MICROPY_PY_MACHINE

#if MICROPY_PY_MACHINE_SPI_TARGET
#define MICROPY_PY_MACHINE_SPITARGET_GLOBAL \
    { MP_ROM_QSTR(MP_QSTR_SPITarget), MP_ROM_PTR(&machine_spi_target_type) },
#else
#define MICROPY_PY_MACHINE_SPITARGET_GLOBAL
#endif

// Reset cause values — PWRON=0 matches the C zero-initialisation of
// reset_cause. SOFT must be non-zero so machine_deinit() can mark a
// soft reset unambiguously.
#define MACHINE_PWRON_RESET     (0)
#define MACHINE_HARD_RESET      (1)
#define MACHINE_WDT_RESET       (2)
#define MACHINE_DEEPSLEEP_RESET (3)
#define MACHINE_SOFT_RESET      (4)

static uint32_t reset_cause;

// Called before each MicroPython soft reset so the next call to
// machine.reset_cause() returns machine.SOFT_RESET.
void machine_deinit(void) {
    reset_cause = MACHINE_SOFT_RESET;
}

// machine.idle()
// This executies a wfi machine instruction which reduces power consumption
// of the MCU until an interrupt occurs, at which point execution continues.
static void mp_machine_idle(void) {
    __WFI(); // standard ARM instruction
}

#if MICROPY_PY_MACHINE_RESET

MP_NORETURN static void mp_machine_reset(void) {
    NVIC_SystemReset();
    for (;;) {
    }
}

static mp_int_t mp_machine_reset_cause(void) {
    if (reset_cause == MACHINE_SOFT_RESET) {
        return MACHINE_SOFT_RESET;
    }
    // RES_CAUSE registers are sticky; read directly without clearing so
    // repeated calls return a consistent value.
    uint32_t reason = Cy_SysLib_GetResetReason();
    if (reason & CY_SYSLIB_RESET_HWWDT) {
        return MACHINE_WDT_RESET;
    } else if (reason & CY_SYSLIB_RESET_DPSLP_FAULT) {
        return MACHINE_DEEPSLEEP_RESET;
    } else if (reason & (CY_SYSLIB_RESET_XRES | CY_SYSLIB_RESET_SOFT)) {
        // XRES: external reset pin. SOFT: NVIC_SystemReset() / machine.reset().
        return MACHINE_HARD_RESET;
    } else {
        return MACHINE_PWRON_RESET;
    }
}

#endif // MICROPY_PY_MACHINE_RESET
/* TODO: currently unused
static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    freq_peri = mp_obj_get_int(args[0]); // Assuming the enum values are used as integers
    switch (freq_peri) {
        case AUDIO_SYS_CLOCK_73_728_000_HZ:
            PLL0_freq = AUDIO_SYS_CLOCK_73_728_000_HZ;
            break;
        case AUDIO_SYS_CLOCK_169_344_000_HZ:
            PLL0_freq = AUDIO_SYS_CLOCK_169_344_000_HZ;
            break;
        default:
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid frequency type %lu"), freq_peri);
            break;
    }
}
*/

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    /* Modules */ \
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_PDM_PCM),             MP_ROM_PTR(&machine_pdm_pcm_type) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&machine_rtc_type) }, \
    { MP_ROM_QSTR(MP_QSTR_IPC),                 MP_ROM_PTR(&machine_ipc_type) }, \
    { MP_ROM_QSTR(MP_QSTR_UART),                MP_ROM_PTR(&machine_uart_type) }, \
    { MP_ROM_QSTR(MP_QSTR_PWM),                 MP_ROM_PTR(&machine_pwm_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) }, \
    { MP_ROM_QSTR(MP_QSTR_WDT),                 MP_ROM_PTR(&machine_wdt_type) }, \
    MICROPY_PY_MACHINE_SPITARGET_GLOBAL \
    /* Reset cause constants */ \
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(MACHINE_PWRON_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),          MP_ROM_INT(MACHINE_HARD_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(MACHINE_WDT_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_ROM_INT(MACHINE_DEEPSLEEP_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(MACHINE_SOFT_RESET) },

#endif // MICROPY_PY_MACHINE
