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
#include "py/mphal.h"
#include "irq.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "rom_map.h"
#include "prcm.h"
#include "pybuart.h"
#include "pybpin.h"
#include "pybrtc.h"
#include "simplelink.h"
#include "modnetwork.h"
#include "modwlan.h"
#include "moduos.h"
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


#ifdef DEBUG
extern OsiTaskHandle    mpTaskHandle;
extern OsiTaskHandle    svTaskHandle;
extern OsiTaskHandle    xSimpleLinkSpawnTaskHndl;
#endif


/// \module machine - functions related to the SoC
///

/******************************************************************************/
// Micro Python bindings;

STATIC mp_obj_t machine_reset(void) {
    // disable wlan
    wlan_stop(SL_STOP_TIMEOUT_LONG);
    // reset the cpu and it's peripherals
    MAP_PRCMMCUReset(true);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

#ifdef DEBUG
STATIC mp_obj_t machine_info(uint n_args, const mp_obj_t *args) {
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
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);
#endif

STATIC mp_obj_t machine_freq(void) {
    mp_obj_t tuple[1] = {
       mp_obj_new_int(HAL_FCPU_HZ),
    };
    return mp_obj_new_tuple(1, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_freq_obj, machine_freq);

STATIC mp_obj_t machine_unique_id(void) {
    uint8_t mac[SL_BSSID_LENGTH];
    wlan_get_mac (mac);
    return mp_obj_new_bytes(mac, SL_BSSID_LENGTH);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

STATIC mp_obj_t machine_main(mp_obj_t main) {
    if (MP_OBJ_IS_STR(main)) {
        MP_STATE_PORT(machine_config_main) = main;
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_main_obj, machine_main);

STATIC mp_obj_t machine_idle(void) {
    __WFI();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_idle_obj, machine_idle);

STATIC mp_obj_t machine_sleep (void) {
    pyb_sleep_sleep();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_sleep_obj, machine_sleep);

STATIC mp_obj_t machine_deepsleep (void) {
    pyb_sleep_deepsleep();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_deepsleep_obj, machine_deepsleep);

STATIC mp_obj_t machine_reset_cause (void) {
    return mp_obj_new_int(pyb_sleep_get_reset_cause());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

STATIC mp_obj_t machine_wake_reason (void) {
    return mp_obj_new_int(pyb_sleep_get_wake_reason());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_wake_reason_obj, machine_wake_reason);

STATIC const mp_map_elem_t machine_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),            MP_OBJ_NEW_QSTR(MP_QSTR_umachine) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_reset),               (mp_obj_t)&machine_reset_obj },
#ifdef DEBUG
    { MP_OBJ_NEW_QSTR(MP_QSTR_info),                (mp_obj_t)&machine_info_obj },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_freq),                (mp_obj_t)&machine_freq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unique_id),           (mp_obj_t)&machine_unique_id_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_main),                (mp_obj_t)&machine_main_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rng),                 (mp_obj_t)&machine_rng_get_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_idle),                (mp_obj_t)&machine_idle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep),               (mp_obj_t)&machine_sleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deepsleep),           (mp_obj_t)&machine_deepsleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset_cause),         (mp_obj_t)&machine_reset_cause_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_wake_reason),         (mp_obj_t)&machine_wake_reason_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_disable_irq),         (mp_obj_t)&pyb_disable_irq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable_irq),          (mp_obj_t)&pyb_enable_irq_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC),                 (mp_obj_t)&pyb_rtc_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Pin),                 (mp_obj_t)&pin_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ADC),                 (mp_obj_t)&pyb_adc_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_I2C),                 (mp_obj_t)&pyb_i2c_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPI),                 (mp_obj_t)&pyb_spi_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_UART),                (mp_obj_t)&pyb_uart_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Timer),               (mp_obj_t)&pyb_timer_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WDT),                 (mp_obj_t)&pyb_wdt_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD),                  (mp_obj_t)&pyb_sd_type },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_IDLE),                MP_OBJ_NEW_SMALL_INT(PYB_PWR_MODE_ACTIVE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLEEP),               MP_OBJ_NEW_SMALL_INT(PYB_PWR_MODE_LPDS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_DEEPSLEEP),           MP_OBJ_NEW_SMALL_INT(PYB_PWR_MODE_HIBERNATE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POWER_ON),            MP_OBJ_NEW_SMALL_INT(PYB_SLP_PWRON_RESET) }, // legacy constant
    { MP_OBJ_NEW_QSTR(MP_QSTR_PWRON_RESET),         MP_OBJ_NEW_SMALL_INT(PYB_SLP_PWRON_RESET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HARD_RESET),          MP_OBJ_NEW_SMALL_INT(PYB_SLP_HARD_RESET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WDT_RESET),           MP_OBJ_NEW_SMALL_INT(PYB_SLP_WDT_RESET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_OBJ_NEW_SMALL_INT(PYB_SLP_HIB_RESET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOFT_RESET),          MP_OBJ_NEW_SMALL_INT(PYB_SLP_SOFT_RESET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WLAN_WAKE),           MP_OBJ_NEW_SMALL_INT(PYB_SLP_WAKED_BY_WLAN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PIN_WAKE),            MP_OBJ_NEW_SMALL_INT(PYB_SLP_WAKED_BY_GPIO) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC_WAKE),            MP_OBJ_NEW_SMALL_INT(PYB_SLP_WAKED_BY_RTC) },
};

STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t machine_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};
