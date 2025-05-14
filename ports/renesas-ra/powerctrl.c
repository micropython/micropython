/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
 * Copyright (c) 2022 Renesas Electronics Corporation
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

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "hal_data.h"
#include "rtc.h"
#include "powerctrl.h"

#if 0

// Sleep Mode
// At power on, by default sleep is set as the low-power mode. Sleep
// mode is the most convenient low-power mode available, as it does
// not require any special configuration (other than configuring and
// enabling a suitable interrupt or event to wake the MCU from sleep)
// to return to normal program-execution mode.
// The states of the SRAM, the processor registers, and the hardware
// peripherals are all maintained in sleep mode, and the time needed
// to enter and wake from sleep is minimal. Any interrupt causes the
// MCU device to wake from sleep mode, including the Systick interrupt
// used by the RTOS scheduler.

// Software Standby Mode
// In software-standby mode, the CPU, as well as most of the on-chip
// peripheral functions and all of the internal oscillators, are
// stopped.
// The contents of the CPU internal registers and SRAM data, the states
// of on-chip peripheral functions, and I/O Ports are all retained.
// Software-standby mode allows significant reduction in power
// consumption, because most of the oscillators are stopped in this mode.
// Like sleep mode, standby mode requires an interrupt or event be
// configured and enabled to wake up.

// Snooze Mode
// Snooze mode can be used with some MCU peripherals to execute basic
// tasks while keeping the MCU in a low-power state.
// Many core peripherals and all clocks can be selected to run during
// Snooze, allowing for more flexible low-power configuration than
// Software Standby mode. To enable Snooze, select "Software Standby
// mode with Snooze mode enabled" for the "Low Power Mode" configuration
// option.
// Snooze mode settings (including entry/exit sources) are available
// under "Standby Options".

// Deep Software Standby Mode
// Deep Software Standby Mode is only available on some MCU devices.
// The MCU always wakes from Deep Software Standby Mode by going
// through reset, either by the negation of the reset pin or by one of
// the wakeup sources configurable in the "Deep Standby Options"
// configuration group.

#endif

#if defined(USE_FSP_LPM)

// LPM_MODE_SLEEP:              Sleep mode
// LPM_MODE_STANDBY:            Software Standby mode
// LPM_MODE_STANDBY_SNOOZE:     Software Standby mode with Snooze mode enabled
// LPM_MODE_DEEP:               Deep Software Standby mode

lpm_instance_ctrl_t g_lpm_sleep_ctrl;

const lpm_cfg_t g_lpm_sleep_cfg = {
    .low_power_mode = LPM_MODE_SLEEP,
    .snooze_cancel_sources = LPM_SNOOZE_CANCEL_SOURCE_NONE,
    .standby_wake_sources = (lpm_standby_wake_source_t)0,
    .snooze_request_source = LPM_SNOOZE_REQUEST_RTC_PERIOD,
    .snooze_end_sources = (lpm_snooze_end_t)0,
    .dtc_state_in_snooze = LPM_SNOOZE_DTC_DISABLE,
    #if BSP_FEATURE_LPM_HAS_SBYCR_OPE
    .output_port_enable = 0,
    #endif
    #if BSP_FEATURE_LPM_HAS_DEEP_STANDBY
    .io_port_state = 0,
    .power_supply_state = 0,
    .deep_standby_cancel_source = (lpm_deep_standby_cancel_source_t)0,
    .deep_standby_cancel_edge = (lpm_deep_standby_cancel_edge_t)0,
    #endif
    .p_extend = NULL,
};

const lpm_instance_t g_lpm_sleep = {
    .p_api = &g_lpm_on_lpm,
    .p_ctrl = &g_lpm_sleep_ctrl,
    .p_cfg = &g_lpm_sleep_cfg
};

lpm_instance_ctrl_t g_lpm_deep_ctrl;

const lpm_cfg_t g_lpm_deep_cfg = {
    .low_power_mode = LPM_MODE_DEEP,
    .snooze_cancel_sources =
        LPM_SNOOZE_CANCEL_SOURCE_NONE,
    .standby_wake_sources =
        (lpm_standby_wake_source_t)0,
    .snooze_request_source =
        LPM_SNOOZE_REQUEST_RXD0_FALLING,
    .snooze_end_sources =
        (lpm_snooze_end_t)0,
    .dtc_state_in_snooze =
        LPM_SNOOZE_DTC_DISABLE,
    #if BSP_FEATURE_LPM_HAS_SBYCR_OPE
    .output_port_enable = LPM_OUTPUT_PORT_ENABLE_RETAIN,
    #endif
    #if BSP_FEATURE_LPM_HAS_DEEP_STANDBY
    .io_port_state = LPM_IO_PORT_NO_CHANGE,
    .power_supply_state = LPM_POWER_SUPPLY_DEEPCUT0,
    .deep_standby_cancel_source = LPM_DEEP_STANDBY_CANCEL_SOURCE_RTC_INTERVAL | LPM_DEEP_STANDBY_CANCEL_SOURCE_RTC_ALARM | (lpm_deep_standby_cancel_source_t)0,
    .deep_standby_cancel_edge = (lpm_deep_standby_cancel_edge_t)0,
    #endif
    .p_extend = NULL,
};

const lpm_instance_t g_lpm_deep = {
    .p_api = &g_lpm_on_lpm,
    .p_ctrl = &g_lpm_deep_ctrl,
    .p_cfg = &g_lpm_deep_cfg
};

lpm_instance_ctrl_t g_lpm_standby_ctrl;

const lpm_cfg_t g_lpm_standby_cfg = {
    .low_power_mode = LPM_MODE_STANDBY,
    .snooze_cancel_sources =
        LPM_SNOOZE_CANCEL_SOURCE_NONE,
    .standby_wake_sources =
        LPM_STANDBY_WAKE_SOURCE_RTCALM | LPM_STANDBY_WAKE_SOURCE_RTCPRD | (lpm_standby_wake_source_t)0,
    .snooze_request_source = LPM_SNOOZE_REQUEST_RXD0_FALLING,
    .snooze_end_sources = (lpm_snooze_end_t)0,
    .dtc_state_in_snooze = LPM_SNOOZE_DTC_DISABLE,
    #if BSP_FEATURE_LPM_HAS_SBYCR_OPE
    .output_port_enable = LPM_OUTPUT_PORT_ENABLE_RETAIN,
    #endif
    #if BSP_FEATURE_LPM_HAS_DEEP_STANDBY
    .io_port_state = LPM_IO_PORT_NO_CHANGE,
    .power_supply_state = LPM_POWER_SUPPLY_DEEPCUT0,
    .deep_standby_cancel_source = LPM_DEEP_STANDBY_CANCEL_SOURCE_RTC_INTERVAL | (lpm_deep_standby_cancel_source_t)0,
    .deep_standby_cancel_edge = (lpm_deep_standby_cancel_edge_t)0,
    #endif
    .p_extend = NULL,
};

const lpm_instance_t g_lpm_standby = {
    .p_api = &g_lpm_on_lpm,
    .p_ctrl = &g_lpm_standby_ctrl,
    .p_cfg = &g_lpm_standby_cfg
};

#endif

MP_NORETURN void powerctrl_mcu_reset(void) {
    #if BSP_TZ_SECURE_BUILD
    R_BSP_NonSecureEnter();
    #else
    NVIC_SystemReset();
    #endif
    while (1) {
        ;
    }
}

MP_NORETURN void powerctrl_enter_bootloader(uint32_t r0, uint32_t bl_addr) {
    while (1) {
        ;
    }
}

// static __attribute__((naked)) void branch_to_bootloader(uint32_t r0, uint32_t
// bl_addr) {
// }

void powerctrl_check_enter_bootloader(void) {
}

void powerctrl_enter_sleep_mode(void) {
    // start trandition to RA MCU sleep mode
    #if defined(USE_FSP_LPM)
    fsp_err_t err;
    err = R_LPM_Open(&g_lpm_sleep_ctrl, &g_lpm_sleep_cfg);
    if (err != FSP_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Can't enter sleep mode"));
    }
    err = R_LPM_LowPowerModeEnter(&g_lpm_sleep_ctrl);
    if (err != FSP_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Can't enter sleep mode"));
    }
    #endif
}

void powerctrl_enter_stop_mode(void) {
    // Disable IRQs so that the IRQ that wakes the device from stop mode is not
    // executed until after the clocks are reconfigured
    uint32_t irq_state = disable_irq();

    #if defined(MICROPY_BOARD_ENTER_STOP)
    MICROPY_BOARD_ENTER_STOP
    #endif

    // start trandition to RA MCU sleep mode
    #if defined(USE_FSP_LPM)
    fsp_err_t err;
    err = R_LPM_Open(&g_lpm_standby_ctrl, &g_lpm_standby_cfg);
    if (err != FSP_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Can't enter stop mode"));
    }
    err = R_LPM_LowPowerModeEnter(&g_lpm_standby_ctrl);
    if (err != FSP_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Can't enter stop mode"));
    }
    #endif

    // reconfigure the system clock after waking up

    #if defined(MICROPY_BOARD_LEAVE_STOP)
    MICROPY_BOARD_LEAVE_STOP
    #endif

    // Enable IRQs now that all clocks are reconfigured
    enable_irq(irq_state);
}

MP_NORETURN void powerctrl_enter_standby_mode(void) {
    rtc_init_finalise();

    #if defined(MICROPY_BOARD_ENTER_STANDBY)
    MICROPY_BOARD_ENTER_STANDBY
    #endif

    // start trandition to RA MCU deep software standby mode via software standby mode
    #if defined(USE_FSP_LPM)

    fsp_err_t err;
    err = R_LPM_Open(&g_lpm_deep_ctrl, &g_lpm_deep_cfg);
    /* Handle any errors. This function should be defined by the user. */
    if (err != FSP_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Can't enter stop mode"));
    }
    /* Check the Deep Software Standby Reset Flag. */
    if (1U == R_SYSTEM->RSTSR0_b.DPSRSTF) {
        /* Clear the IOKEEP bit to allow I/O Port use. */
        err = R_LPM_IoKeepClear(&g_lpm_deep_ctrl);
        if (err != FSP_SUCCESS) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Can't enter stop mode"));
        }
    }
    /* Add user code here. */
    /* Reconfigure the module to set the IOKEEP bit before entering deep software standby. */
    err = R_LPM_LowPowerReconfigure(&g_lpm_deep_ctrl, &g_lpm_deep_cfg);
    if (err != FSP_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Can't enter stop mode"));
    }
    err = R_LPM_LowPowerModeEnter(&g_lpm_deep_ctrl);
    /* Code after R_LPM_LowPowerModeEnter when using Deep Software Standby never be executed.
     * Deep software standby exits by resetting the MCU. */
    if (err != FSP_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Can't enter stop mode"));
    }

    #endif

    // We need to clear the PWR wake-up-flag before entering standby, since
    // the flag may have been set by a previous wake-up event.  Furthermore,
    // we need to disable the wake-up sources while clearing this flag, so
    // that if a source is active it does actually wake the device.
    // See section 5.3.7 of RM0090.

    // save RTC interrupts
    // disable register write protection
    // disable RTC interrupts
    // clear RTC wake-up flags

    // enable previously-enabled RTC interrupts
    // enable register write protection

    // enter standby mode

    // we never return; MCU is reset on exit from standby

    powerctrl_mcu_reset();
}
