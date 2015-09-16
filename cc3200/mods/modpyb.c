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

#include <stdint.h>
#include "std.h"

#include "py/mpstate.h"
#include "py/runtime.h"
#include MICROPY_HAL_H
#include "irq.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "rom_map.h"
#include "prcm.h"
#include "pyexec.h"
#include "ff.h"
#include "diskio.h"
#include "sflash_diskio.h"
#include "pybuart.h"
#include "pybpin.h"
#include "pybrtc.h"
#include "mpsystick.h"
#include "simplelink.h"
#include "modnetwork.h"
#include "modwlan.h"
#include "moduos.h"
#include "telnet.h"
#include "FreeRTOS.h"
#include "portable.h"
#include "task.h"
#include "mpexception.h"
#include "random.h"
#include "pybadc.h"
#include "pybi2c.h"
#include "pybsd.h"
#include "pybwdt.h"
#include "pybsleep.h"
#include "pybspi.h"
#include "pybtimer.h"
#include "utils.h"
#include "gccollect.h"
#include "mperror.h"
#include "genhdr/mpversion.h"


#ifdef DEBUG
extern OsiTaskHandle    mpTaskHandle;
extern OsiTaskHandle    svTaskHandle;
extern OsiTaskHandle    xSimpleLinkSpawnTaskHndl;
#endif


/// \module pyb - functions related to the pyboard
///
/// The `pyb` module contains specific functions related to the pyboard.

/// \function reset()
/// Resets the pyboard in a manner similar to pushing the external
/// reset button.
STATIC mp_obj_t pyb_reset(void) {
    // disable wlan
    wlan_stop(SL_STOP_TIMEOUT_LONG);
    // reset the cpu and it's peripherals
    MAP_PRCMMCUReset(true);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_reset_obj, pyb_reset);

#ifdef DEBUG
/// \function info([dump_alloc_table])
/// Print out some run time info which is helpful during development.
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

/// \function freq()
/// Returns the CPU frequency: (F_CPU).
STATIC mp_obj_t pyb_freq(void) {
    mp_obj_t tuple[1] = {
       mp_obj_new_int(HAL_FCPU_HZ),
    };
    return mp_obj_new_tuple(1, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_freq_obj, pyb_freq);

/// \function unique_id()
/// Returns a string of 6 bytes (48 bits), which is the unique ID for the MCU.
STATIC mp_obj_t pyb_unique_id(void) {
    uint8_t mac[SL_BSSID_LENGTH];
    wlan_get_mac (mac);
    return mp_obj_new_bytes(mac, SL_BSSID_LENGTH);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_unique_id_obj, pyb_unique_id);

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

MP_DECLARE_CONST_FUN_OBJ(pyb_main_obj); // defined in main.c

STATIC const mp_map_elem_t pyb_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),            MP_OBJ_NEW_QSTR(MP_QSTR_pyb) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_reset),               (mp_obj_t)&pyb_reset_obj },
#ifdef DEBUG
    { MP_OBJ_NEW_QSTR(MP_QSTR_info),                (mp_obj_t)&pyb_info_obj },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_freq),                (mp_obj_t)&pyb_freq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unique_id),           (mp_obj_t)&pyb_unique_id_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_repl_uart),           (mp_obj_t)&pyb_repl_uart_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_disable_irq),         (mp_obj_t)&pyb_disable_irq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable_irq),          (mp_obj_t)&pyb_enable_irq_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_main),                (mp_obj_t)&pyb_main_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rng),                 (mp_obj_t)&pyb_rng_get_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC),                 (mp_obj_t)&pyb_rtc_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Pin),                 (mp_obj_t)&pin_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ADC),                 (mp_obj_t)&pyb_adc_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_I2C),                 (mp_obj_t)&pyb_i2c_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPI),                 (mp_obj_t)&pyb_spi_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_UART),                (mp_obj_t)&pyb_uart_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Timer),               (mp_obj_t)&pyb_timer_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WDT),                 (mp_obj_t)&pyb_wdt_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Sleep),               (mp_obj_t)&pyb_sleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HeartBeat),           (mp_obj_t)&pyb_heartbeat_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD),                  (mp_obj_t)&pyb_sd_type },
};

STATIC MP_DEFINE_CONST_DICT(pyb_module_globals, pyb_module_globals_table);

const mp_obj_module_t pyb_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_pyb,
    .globals = (mp_obj_dict_t*)&pyb_module_globals,
};
