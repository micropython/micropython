/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2023 Damien P. George
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

// This file is never compiled standalone, it's included directly from
// extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE.

#include <stdio.h>

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "rom_map.h"
#include "pybuart.h"
#include "pybpin.h"
#include "pybrtc.h"
#include "simplelink.h"
#include "modwlan.h"
#include "random.h"
#include "pybadc.h"
#include "pybi2c.h"
#include "pybsd.h"
#include "pybsleep.h"
#include "pybspi.h"
#include "pybtimer.h"

#ifdef DEBUG
#define MICROPY_PY_MACHINE_INFO_ENTRY { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&machine_info_obj) },
#else
#define MICROPY_PY_MACHINE_INFO_ENTRY
#endif

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    MICROPY_PY_MACHINE_INFO_ENTRY \
    { MP_ROM_QSTR(MP_QSTR_main),                MP_ROM_PTR(&machine_main_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_rng),                 MP_ROM_PTR(&machine_rng_get_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_sleep),               MP_ROM_PTR(&machine_lightsleep_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_wake_reason),         MP_ROM_PTR(&machine_wake_reason_obj) }, \
    \
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&pyb_rtc_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_ADC),                 MP_ROM_PTR(&pyb_adc_type) }, \
    { MP_ROM_QSTR(MP_QSTR_I2C),                 MP_ROM_PTR(&pyb_i2c_type) }, \
    { MP_ROM_QSTR(MP_QSTR_SPI),                 MP_ROM_PTR(&pyb_spi_type) }, \
    { MP_ROM_QSTR(MP_QSTR_UART),                MP_ROM_PTR(&pyb_uart_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&pyb_timer_type) }, \
    { MP_ROM_QSTR(MP_QSTR_SD),                  MP_ROM_PTR(&pyb_sd_type) }, \
    \
    /* class constants */ \
    { MP_ROM_QSTR(MP_QSTR_IDLE),                MP_ROM_INT(PYB_PWR_MODE_ACTIVE) }, \
    { MP_ROM_QSTR(MP_QSTR_SLEEP),               MP_ROM_INT(PYB_PWR_MODE_LPDS) }, \
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP),           MP_ROM_INT(PYB_PWR_MODE_HIBERNATE) }, \
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(PYB_SLP_PWRON_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),          MP_ROM_INT(PYB_SLP_HARD_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(PYB_SLP_WDT_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_ROM_INT(PYB_SLP_HIB_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(PYB_SLP_SOFT_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_WLAN_WAKE),           MP_ROM_INT(PYB_SLP_WAKED_BY_WLAN) }, \
    { MP_ROM_QSTR(MP_QSTR_PIN_WAKE),            MP_ROM_INT(PYB_SLP_WAKED_BY_GPIO) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC_WAKE),            MP_ROM_INT(PYB_SLP_WAKED_BY_RTC) }, \

#ifdef DEBUG
extern OsiTaskHandle    mpTaskHandle;
extern OsiTaskHandle    svTaskHandle;
extern OsiTaskHandle    xSimpleLinkSpawnTaskHndl;
#endif

/******************************************************************************/
// MicroPython bindings;

NORETURN static void mp_machine_reset(void) {
    // disable wlan
    wlan_stop(SL_STOP_TIMEOUT_LONG);
    // reset the cpu and it's peripherals
    MAP_PRCMMCUReset(true);
    for (;;) {
    }
}

#ifdef DEBUG
static mp_obj_t machine_info(uint n_args, const mp_obj_t *args) {
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
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);
#endif

static mp_obj_t mp_machine_get_freq(void) {
    return mp_obj_new_int(HAL_FCPU_HZ);
}

static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    mp_raise_NotImplementedError(NULL);
}

static mp_obj_t mp_machine_unique_id(void) {
    uint8_t mac[SL_BSSID_LENGTH];
    wlan_get_mac (mac);
    return mp_obj_new_bytes(mac, SL_BSSID_LENGTH);
}

static mp_obj_t machine_main(mp_obj_t main) {
    if (mp_obj_is_str(main)) {
        MP_STATE_PORT(machine_config_main) = main;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s) value"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_main_obj, machine_main);

static void mp_machine_idle(void) {
    __WFI();
}

static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    pyb_sleep_sleep();
}

NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    pyb_sleep_deepsleep();
    for (;;) {
    }
}

static mp_int_t mp_machine_reset_cause(void) {
    return pyb_sleep_get_reset_cause();
}

static mp_obj_t machine_wake_reason (void) {
    return mp_obj_new_int(pyb_sleep_get_wake_reason());
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_wake_reason_obj, machine_wake_reason);

MP_REGISTER_ROOT_POINTER(mp_obj_t machine_config_main);
