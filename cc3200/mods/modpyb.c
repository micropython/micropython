/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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


#include <std.h>
#include <stdint.h>

#include "py/mpstate.h"
#include MICROPY_HAL_H
#include "irq.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "prcm.h"
#include "pyexec.h"
#include "pybuart.h"
#include "pybpin.h"
#include "pybrtc.h"
#include "pybsystick.h"
#include "simplelink.h"
#include "modwlan.h"
#include "telnet.h"
#include "ff.h"
#include "diskio.h"
#include "sflash_diskio.h"
#include "FreeRTOS.h"
#include "portable.h"
#include "task.h"
#include "mpexception.h"
#include "random.h"
#include "pybextint.h"
#include "pybadc.h"
#include "pybi2c.h"
#include "pybsd.h"
#include "pybwdt.h"
#include "utils.h"
#include "gccollect.h"
#include "mperror.h"


#ifdef DEBUG
extern OsiTaskHandle    mpTaskHandle;
extern OsiTaskHandle    svTaskHandle;
extern OsiTaskHandle    xSimpleLinkSpawnTaskHndl;
#endif

/// \module pyb - functions related to the pyboard
///
/// The `pyb` module contains specific functions related to the pyboard.

/// \function hard_reset()
/// Resets the pyboard in a manner similar to pushing the external RESET
/// button.
STATIC mp_obj_t pyb_hard_reset(void) {
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
    // disable wlan services
    wlan_stop_servers();
#endif
    wlan_stop();
    // perform a SoC reset
    PRCMSOCReset();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_hard_reset_obj, pyb_hard_reset);

#ifdef DEBUG
/// \function info([dump_alloc_table])
/// Print out some run time info which is helpful duirng development.
STATIC mp_obj_t pyb_info(uint n_args, const mp_obj_t *args) {
    // FreeRTOS info
    {
        printf("---------------------------------------------\n");
        printf("FreeRTOS\n");
        printf("---------------------------------------------\n");
        printf("Total heap: %u\n", configTOTAL_HEAP_SIZE);
        printf("Free heap: %u\n", xPortGetFreeHeapSize());
        printf("MpTask min free stack: %u\n", (unsigned int)uxTaskGetStackHighWaterMark((TaskHandle_t)mpTaskHandle));
        printf("ServersTask min free stack: %u\n", (unsigned int)uxTaskGetStackHighWaterMark((TaskHandle_t)svTaskHandle));
        printf("SlTask min free stack: %u\n", (unsigned int)uxTaskGetStackHighWaterMark(xSimpleLinkSpawnTaskHndl));
        printf("IdleTask min free stack: %u\n", (unsigned int)uxTaskGetStackHighWaterMark(xTaskGetIdleTaskHandle()));

        uint32_t *pstack = (uint32_t *)&_stack;
        while (*pstack == 0x55555555) {
            pstack++;
        }
        printf("MAIN min free stack: %u\n", pstack - ((uint32_t *)&_stack));
        printf("---------------------------------------------\n");
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_info_obj, 0, 1, pyb_info);
#endif

/// \function unique_id()
/// Returns a string of 6 bytes (48 bits), which is the unique MAC address of the SoC
STATIC mp_obj_t pyb_mac(void) {
    uint8_t mac[6];
    wlan_get_mac (mac);
    return mp_obj_new_bytes(mac, 6);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_mac_obj, pyb_mac);

/// \function freq()
/// Returns the CPU frequency: (F_CPU).
STATIC mp_obj_t pyb_freq(void) {
    return mp_obj_new_int(HAL_FCPU_HZ);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_freq_obj, pyb_freq);

/// \function sync()
/// Sync all file systems.
STATIC mp_obj_t pyb_sync(void) {
    sflash_disk_flush();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_sync_obj, pyb_sync);

/// \function millis()
/// Returns the number of milliseconds since the board was last reset.
///
/// The result is always a micropython smallint (31-bit signed number), so
/// after 2^30 milliseconds (about 12.4 days) this will start to return
/// negative numbers.
STATIC mp_obj_t pyb_millis(void) {
    // We want to "cast" the 32 bit unsigned into a small-int.  This means
    // copying the MSB down 1 bit (extending the sign down), which is
    // equivalent to just using the MP_OBJ_NEW_SMALL_INT macro.
    return MP_OBJ_NEW_SMALL_INT(HAL_GetTick());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_millis_obj, pyb_millis);

/// \function elapsed_millis(start)
/// Returns the number of milliseconds which have elapsed since `start`.
///
/// This function takes care of counter wrap, and always returns a positive
/// number. This means it can be used to measure periods upto about 12.4 days.
///
/// Example:
///     start = pyb.millis()
///     while pyb.elapsed_millis(start) < 1000:
///         # Perform some operation
STATIC mp_obj_t pyb_elapsed_millis(mp_obj_t start) {
    uint32_t startMillis = mp_obj_get_int(start);
    uint32_t currMillis = HAL_GetTick();
    return MP_OBJ_NEW_SMALL_INT((currMillis - startMillis) & 0x3fffffff);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_elapsed_millis_obj, pyb_elapsed_millis);

/// \function micros()
/// Returns the number of microseconds since the board was last reset.
///
/// The result is always a micropython smallint (31-bit signed number), so
/// after 2^30 microseconds (about 17.8 minutes) this will start to return
/// negative numbers.
STATIC mp_obj_t pyb_micros(void) {
    // We want to "cast" the 32 bit unsigned into a small-int.  This means
    // copying the MSB down 1 bit (extending the sign down), which is
    // equivalent to just using the MP_OBJ_NEW_SMALL_INT macro.
    return MP_OBJ_NEW_SMALL_INT(sys_tick_get_microseconds());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_micros_obj, pyb_micros);

/// \function elapsed_micros(start)
/// Returns the number of microseconds which have elapsed since `start`.
///
/// This function takes care of counter wrap, and always returns a positive
/// number. This means it can be used to measure periods upto about 17.8 minutes.
///
/// Example:
///     start = pyb.micros()
///     while pyb.elapsed_micros(start) < 1000:
///         # Perform some operation
STATIC mp_obj_t pyb_elapsed_micros(mp_obj_t start) {
    uint32_t startMicros = mp_obj_get_int(start);
    uint32_t currMicros = sys_tick_get_microseconds();
    return MP_OBJ_NEW_SMALL_INT((currMicros - startMicros) & 0x3fffffff);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_elapsed_micros_obj, pyb_elapsed_micros);

/// \function delay(ms)
/// Delay for the given number of milliseconds.
STATIC mp_obj_t pyb_delay(mp_obj_t ms_in) {
    mp_int_t ms = mp_obj_get_int(ms_in);
    if (ms > 0) {
        HAL_Delay(ms);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_delay_obj, pyb_delay);

/// \function udelay(us)
/// Delay for the given number of microseconds.
STATIC mp_obj_t pyb_udelay(mp_obj_t usec_in) {
    mp_int_t usec = mp_obj_get_int(usec_in);
    if (usec > 0) {
        UtilsDelay(UTILS_DELAY_US_TO_COUNT(usec));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_udelay_obj, pyb_udelay);

STATIC mp_obj_t pyb_stop(void) {
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_stop_obj, pyb_stop);

STATIC mp_obj_t pyb_standby(void) {
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_standby_obj, pyb_standby);

/// \function repl_uart(uart)
/// Get or set the UART object that the REPL is repeated on.
STATIC mp_obj_t pyb_repl_uart(uint n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        if (pyb_stdio_uart == NULL) {
            return mp_const_none;
        } else {
            return pyb_stdio_uart;
        }
    } else {
        if (args[0] == mp_const_none) {
            pyb_stdio_uart = NULL;
        } else if (mp_obj_get_type(args[0]) == &pyb_uart_type) {
            pyb_stdio_uart = args[0];
        } else {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, mpexception_num_type_invalid_arguments));
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_repl_uart_obj, 0, 1, pyb_repl_uart);

/// \function mkdisk('path')
/// Formats the selected drive, useful when the filesystem has been damaged beyond repair
STATIC mp_obj_t pyb_mkdisk(mp_obj_t path_o) {
    const char *path = mp_obj_str_get_str(path_o);
    if (FR_OK != f_mkfs(path, 1, 0)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_mkdisk_obj, pyb_mkdisk);

/// \function wdt_enable('msec')
/// Enabled the watchdog timer with msec timeout value
STATIC mp_obj_t pyb_enable_wdt(mp_obj_t msec_in) {
    mp_int_t msec = mp_obj_get_int(msec_in);
    pybwdt_ret_code_t ret;
    ret = pybwdt_enable (msec);
    if (ret == E_PYBWDT_IS_RUNNING) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
    }
    else if (ret == E_PYBWDT_INVALID_TIMEOUT) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_enable_wdt_obj, pyb_enable_wdt);

/// \function wdt_kick()
/// Kicks the watchdog timer
STATIC mp_obj_t pyb_kick_wdt(void) {
    pybwdt_kick ();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_kick_wdt_obj, pyb_kick_wdt);

/// \function enable_heartbeat()
/// Enables the heartbeat signal
STATIC mp_obj_t pyb_enable_heartbeat(void) {
    mperror_enable_heartbeat ();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_enable_heartbeat_obj, pyb_enable_heartbeat);

/// \function disable_heartbeat()
/// Disables the heartbeat signal
STATIC mp_obj_t pyb_disable_heartbeat(void) {
    mperror_disable_heartbeat ();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_disable_heartbeat_obj, pyb_disable_heartbeat);

MP_DECLARE_CONST_FUN_OBJ(pyb_main_obj); // defined in main.c

STATIC const mp_map_elem_t pyb_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),            MP_OBJ_NEW_QSTR(MP_QSTR_pyb) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_hard_reset),          (mp_obj_t)&pyb_hard_reset_obj },
#ifdef DEBUG
    { MP_OBJ_NEW_QSTR(MP_QSTR_info),                (mp_obj_t)&pyb_info_obj },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_mac),                 (mp_obj_t)&pyb_mac_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_freq),                (mp_obj_t)&pyb_freq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_repl_info),           (mp_obj_t)&pyb_set_repl_info_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_wfi),                 (mp_obj_t)&pyb_wfi_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable_irq),         (mp_obj_t)&pyb_disable_irq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable_irq),          (mp_obj_t)&pyb_enable_irq_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_stop),                (mp_obj_t)&pyb_stop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_standby),             (mp_obj_t)&pyb_standby_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_main),                (mp_obj_t)&pyb_main_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_repl_uart),           (mp_obj_t)&pyb_repl_uart_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_millis),              (mp_obj_t)&pyb_millis_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_elapsed_millis),      (mp_obj_t)&pyb_elapsed_millis_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_micros),              (mp_obj_t)&pyb_micros_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_elapsed_micros),      (mp_obj_t)&pyb_elapsed_micros_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_delay),               (mp_obj_t)&pyb_delay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_udelay),              (mp_obj_t)&pyb_udelay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sync),                (mp_obj_t)&pyb_sync_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mkdisk),              (mp_obj_t)&pyb_mkdisk_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable_wdt),          (mp_obj_t)&pyb_enable_wdt_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_kick_wdt),            (mp_obj_t)&pyb_kick_wdt_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable_heartbeat),    (mp_obj_t)&pyb_enable_heartbeat_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable_heartbeat),   (mp_obj_t)&pyb_disable_heartbeat_obj },

#if MICROPY_HW_ENABLE_RNG
    { MP_OBJ_NEW_QSTR(MP_QSTR_rng),                 (mp_obj_t)&pyb_rng_get_obj },
#endif

#if MICROPY_HW_ENABLE_RTC
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC),                 (mp_obj_t)&pyb_rtc_type },
#endif

    { MP_OBJ_NEW_QSTR(MP_QSTR_Pin),                 (mp_obj_t)&pin_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ExtInt),              (mp_obj_t)&extint_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ADC),                 (mp_obj_t)&pyb_adc_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_I2C),                 (mp_obj_t)&pyb_i2c_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_UART),                (mp_obj_t)&pyb_uart_type },

#if MICROPY_HW_HAS_SDCARD
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD),                  (mp_obj_t)&pyb_sd_type },
#endif
};

STATIC MP_DEFINE_CONST_DICT(pyb_module_globals, pyb_module_globals_table);

const mp_obj_module_t pyb_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_pyb,
    .globals = (mp_obj_dict_t*)&pyb_module_globals,
};
