/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <stdio.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/runtime/pyexec.h"
#include "stm32_it.h"
#include "irq.h"
#include "led.h"
#include "timer.h"
#include "extint.h"
#include "usrsw.h"
#include "rng.h"
#include "rtc.h"
#include "i2c.h"
#include "spi.h"
#include "uart.h"
#include "pyb_can.h"
#include "adc.h"
#include "storage.h"
#include "sdcard.h"
#include "accel.h"
#include "servo.h"
#include "dac.h"
#include "lcd.h"
#include "usb.h"
#include "portmodules.h"
#include "modmachine.h"
#include "extmod/modmachine.h"
#include "extmod/modnetwork.h"
#include "extmod/vfs.h"
#include "extmod/modtime.h"

#if MICROPY_PY_PYB

static mp_obj_t pyb_fault_debug(mp_obj_t value) {
    pyb_hard_fault_debug = mp_obj_is_true(value);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(pyb_fault_debug_obj, pyb_fault_debug);

static mp_obj_t pyb_idle(void) {
    __WFI();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(pyb_idle_obj, pyb_idle);

#if MICROPY_PY_PYB_LEGACY

// Returns the number of milliseconds which have elapsed since `start`.
// This function takes care of counter wrap and always returns a positive number.
static mp_obj_t pyb_elapsed_millis(mp_obj_t start) {
    uint32_t startMillis = mp_obj_get_int(start);
    uint32_t currMillis = mp_hal_ticks_ms();
    return MP_OBJ_NEW_SMALL_INT((currMillis - startMillis) & 0x3fffffff);
}
static MP_DEFINE_CONST_FUN_OBJ_1(pyb_elapsed_millis_obj, pyb_elapsed_millis);

// Returns the number of microseconds which have elapsed since `start`.
// This function takes care of counter wrap and always returns a positive number.
static mp_obj_t pyb_elapsed_micros(mp_obj_t start) {
    uint32_t startMicros = mp_obj_get_int(start);
    uint32_t currMicros = mp_hal_ticks_us();
    return MP_OBJ_NEW_SMALL_INT((currMicros - startMicros) & 0x3fffffff);
}
static MP_DEFINE_CONST_FUN_OBJ_1(pyb_elapsed_micros_obj, pyb_elapsed_micros);

#endif

MP_DECLARE_CONST_FUN_OBJ_KW(pyb_main_obj); // defined in main.c

// Get or set the UART object that the REPL is repeated on.
// This is a legacy function, use of os.dupterm is preferred.
static mp_obj_t pyb_repl_uart(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        if (MP_STATE_PORT(pyb_stdio_uart) == NULL) {
            return mp_const_none;
        } else {
            return MP_OBJ_FROM_PTR(MP_STATE_PORT(pyb_stdio_uart));
        }
    } else {
        if (args[0] == mp_const_none) {
            if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
                uart_attach_to_repl(MP_STATE_PORT(pyb_stdio_uart), false);
                MP_STATE_PORT(pyb_stdio_uart) = NULL;
            }
        } else if (mp_obj_get_type(args[0]) == &machine_uart_type) {
            MP_STATE_PORT(pyb_stdio_uart) = MP_OBJ_TO_PTR(args[0]);
            uart_attach_to_repl(MP_STATE_PORT(pyb_stdio_uart), true);
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("need a UART object"));
        }
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_repl_uart_obj, 0, 1, pyb_repl_uart);

#if MICROPY_PY_NETWORK
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mod_network_country_obj);
#else
// Provide a no-op version of pyb.country for backwards compatibility on
// boards that don't support networking.
static mp_obj_t pyb_country(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    (void)args;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_network_country_obj, 0, 1, pyb_country);
#endif

static const mp_rom_map_elem_t pyb_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pyb) },

    { MP_ROM_QSTR(MP_QSTR_fault_debug), MP_ROM_PTR(&pyb_fault_debug_obj) },

    #if MICROPY_PY_PYB_LEGACY
    { MP_ROM_QSTR(MP_QSTR_bootloader), MP_ROM_PTR(&machine_bootloader_obj) },
    { MP_ROM_QSTR(MP_QSTR_hard_reset), MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&machine_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id), MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_freq_obj) },
    #endif
    #if MICROPY_REPL_INFO
    { MP_ROM_QSTR(MP_QSTR_repl_info), MP_ROM_PTR(&pyb_set_repl_info_obj) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_wfi), MP_ROM_PTR(&pyb_idle_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_irq), MP_ROM_PTR(&machine_disable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_irq), MP_ROM_PTR(&machine_enable_irq_obj) },
    #if IRQ_ENABLE_STATS
    { MP_ROM_QSTR(MP_QSTR_irq_stats), MP_ROM_PTR(&pyb_irq_stats_obj) },
    #endif

    #if MICROPY_PY_PYB_LEGACY
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&machine_lightsleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_standby), MP_ROM_PTR(&machine_deepsleep_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_main), MP_ROM_PTR(&pyb_main_obj) },
    { MP_ROM_QSTR(MP_QSTR_repl_uart), MP_ROM_PTR(&pyb_repl_uart_obj) },

    // Deprecated (use network.country instead).
    { MP_ROM_QSTR(MP_QSTR_country), MP_ROM_PTR(&mod_network_country_obj) },

    #if MICROPY_HW_ENABLE_USB
    { MP_ROM_QSTR(MP_QSTR_usb_mode), MP_ROM_PTR(&pyb_usb_mode_obj) },
    #if MICROPY_HW_USB_HID
    { MP_ROM_QSTR(MP_QSTR_hid_mouse), MP_ROM_PTR(&pyb_usb_hid_mouse_obj) },
    { MP_ROM_QSTR(MP_QSTR_hid_keyboard), MP_ROM_PTR(&pyb_usb_hid_keyboard_obj) },
    { MP_ROM_QSTR(MP_QSTR_USB_HID), MP_ROM_PTR(&pyb_usb_hid_type) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_USB_VCP), MP_ROM_PTR(&pyb_usb_vcp_type) },
    #if MICROPY_PY_PYB_LEGACY
    // these 2 are deprecated; use USB_VCP.isconnected and USB_HID.send instead
    { MP_ROM_QSTR(MP_QSTR_have_cdc), MP_ROM_PTR(&pyb_have_cdc_obj) },
    #if MICROPY_HW_USB_HID
    { MP_ROM_QSTR(MP_QSTR_hid), MP_ROM_PTR(&pyb_hid_send_report_obj) },
    #endif
    #endif
    #endif

    #if MICROPY_PY_PYB_LEGACY
    { MP_ROM_QSTR(MP_QSTR_millis), MP_ROM_PTR(&mp_time_ticks_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_elapsed_millis), MP_ROM_PTR(&pyb_elapsed_millis_obj) },
    { MP_ROM_QSTR(MP_QSTR_micros), MP_ROM_PTR(&mp_time_ticks_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_elapsed_micros), MP_ROM_PTR(&pyb_elapsed_micros_obj) },
    { MP_ROM_QSTR(MP_QSTR_delay), MP_ROM_PTR(&mp_time_sleep_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_udelay), MP_ROM_PTR(&mp_time_sleep_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_sync), MP_ROM_PTR(&mp_os_sync_obj) },
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&mp_vfs_mount_obj) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_Timer), MP_ROM_PTR(&pyb_timer_type) },

    #if MICROPY_HW_ENABLE_RNG
    { MP_ROM_QSTR(MP_QSTR_rng), MP_ROM_PTR(&pyb_rng_get_obj) },
    #endif

    #if MICROPY_HW_ENABLE_RTC
    { MP_ROM_QSTR(MP_QSTR_RTC), MP_ROM_PTR(&pyb_rtc_type) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&pin_type) },
    { MP_ROM_QSTR(MP_QSTR_ExtInt), MP_ROM_PTR(&extint_type) },

    #if MICROPY_HW_ENABLE_SERVO
    { MP_ROM_QSTR(MP_QSTR_pwm), MP_ROM_PTR(&pyb_pwm_set_obj) },
    { MP_ROM_QSTR(MP_QSTR_servo), MP_ROM_PTR(&pyb_servo_set_obj) },
    { MP_ROM_QSTR(MP_QSTR_Servo), MP_ROM_PTR(&pyb_servo_type) },
    #endif

    #if MICROPY_HW_HAS_SWITCH
    { MP_ROM_QSTR(MP_QSTR_Switch), MP_ROM_PTR(&pyb_switch_type) },
    #endif

    #if MICROPY_HW_HAS_FLASH
    { MP_ROM_QSTR(MP_QSTR_Flash), MP_ROM_PTR(&pyb_flash_type) },
    #endif

    #if MICROPY_HW_ENABLE_SDCARD
    #if MICROPY_PY_PYB_LEGACY
    { MP_ROM_QSTR(MP_QSTR_SD), MP_ROM_PTR(&pyb_sdcard_obj) }, // now obsolete
    #endif
    { MP_ROM_QSTR(MP_QSTR_SDCard), MP_ROM_PTR(&pyb_sdcard_type) },
    #endif
    #if MICROPY_HW_ENABLE_MMCARD
    { MP_ROM_QSTR(MP_QSTR_MMCard), MP_ROM_PTR(&pyb_mmcard_type) },
    #endif

    #if defined(MICROPY_HW_LED1)
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pyb_led_type) },
    #endif
    #if MICROPY_PY_PYB_LEGACY && MICROPY_HW_ENABLE_HW_I2C
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&pyb_i2c_type) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&pyb_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&machine_uart_type) },
    #if MICROPY_HW_ENABLE_CAN
    { MP_ROM_QSTR(MP_QSTR_CAN), MP_ROM_PTR(&pyb_can_type) },
    #endif

    #if MICROPY_HW_ENABLE_ADC
    { MP_ROM_QSTR(MP_QSTR_ADC), MP_ROM_PTR(&pyb_adc_type) },
    { MP_ROM_QSTR(MP_QSTR_ADCAll), MP_ROM_PTR(&pyb_adc_all_type) },
    #endif

    #if MICROPY_HW_ENABLE_DAC
    { MP_ROM_QSTR(MP_QSTR_DAC), MP_ROM_PTR(&pyb_dac_type) },
    #endif

    #if MICROPY_HW_HAS_MMA7660 || MICROPY_HW_HAS_KXTJ3
    { MP_ROM_QSTR(MP_QSTR_Accel), MP_ROM_PTR(&pyb_accel_type) },
    #endif

    #if MICROPY_HW_HAS_LCD
    { MP_ROM_QSTR(MP_QSTR_LCD), MP_ROM_PTR(&pyb_lcd_type) },
    #endif
};

static MP_DEFINE_CONST_DICT(pyb_module_globals, pyb_module_globals_table);

const mp_obj_module_t pyb_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pyb_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_pyb, pyb_module);

#endif // MICROPY_PY_PYB
