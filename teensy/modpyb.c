/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#include <mk20dx128.h>
#include "Arduino.h"

#include "py/obj.h"
#include "py/gc.h"
#include "py/mphal.h"

#include "lib/utils/pyexec.h"

#include "gccollect.h"
#include "irq.h"
#include "systick.h"
#include "led.h"
#include "pin.h"
#include "timer.h"
#include "extint.h"
#include "usrsw.h"
#include "rng.h"
//#include "rtc.h"
//#include "i2c.h"
//#include "spi.h"
#include "uart.h"
#include "adc.h"
#include "storage.h"
#include "sdcard.h"
#include "accel.h"
#include "servo.h"
#include "dac.h"
#include "usb.h"
#include "portmodules.h"

/// \module pyb - functions related to the pyboard
///
/// The `pyb` module contains specific functions related to the pyboard.

/// \function bootloader()
/// Activate the bootloader without BOOT* pins.
STATIC mp_obj_t pyb_bootloader(void) {
    printf("bootloader command not current supported\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_bootloader_obj, pyb_bootloader);

/// \function info([dump_alloc_table])
/// Print out lots of information about the board.
STATIC mp_obj_t pyb_info(uint n_args, const mp_obj_t *args) {
    // get and print unique id; 96 bits
    {
        byte *id = (byte*)0x40048058;
        printf("ID=%02x%02x%02x%02x:%02x%02x%02x%02x:%02x%02x%02x%02x\n", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7], id[8], id[9], id[10], id[11]);
    }

    // get and print clock speeds
    printf("CPU=%u\nBUS=%u\nMEM=%u\n", F_CPU, F_BUS, F_MEM);

    // to print info about memory
    {
        printf("_etext=%p\n", &_etext);
        printf("_sidata=%p\n", &_sidata);
        printf("_sdata=%p\n", &_sdata);
        printf("_edata=%p\n", &_edata);
        printf("_sbss=%p\n", &_sbss);
        printf("_ebss=%p\n", &_ebss);
        printf("_estack=%p\n", &_estack);
        printf("_ram_start=%p\n", &_ram_start);
        printf("_heap_start=%p\n", &_heap_start);
        printf("_heap_end=%p\n", &_heap_end);
        printf("_ram_end=%p\n", &_ram_end);
    }

    // qstr info
    {
        uint n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        printf("qstr:\n  n_pool=%u\n  n_qstr=%u\n  n_str_data_bytes=%u\n  n_total_bytes=%u\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    }

    // GC info
    {
        gc_info_t info;
        gc_info(&info);
        printf("GC:\n");
        printf("  " UINT_FMT " total\n", info.total);
        printf("  " UINT_FMT " : " UINT_FMT "\n", info.used, info.free);
        printf("  1=" UINT_FMT " 2=" UINT_FMT " m=" UINT_FMT "\n", info.num_1block, info.num_2block, info.max_block);
    }

    if (n_args == 1) {
        // arg given means dump gc allocation table
        gc_dump_alloc_table();
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_info_obj, 0, 1, pyb_info);

/// \function unique_id()
/// Returns a string of 12 bytes (96 bits), which is the unique ID for the MCU.
STATIC mp_obj_t pyb_unique_id(void) {
    byte *id = (byte*)0x40048058;
    return mp_obj_new_bytes(id, 12);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_unique_id_obj, pyb_unique_id);

/// \function freq()
/// Return a tuple of clock frequencies: (SYSCLK, HCLK, PCLK1, PCLK2).
// TODO should also be able to set frequency via this function
STATIC mp_obj_t pyb_freq(void) {
    mp_obj_t tuple[3] = {
       mp_obj_new_int(F_CPU),
       mp_obj_new_int(F_BUS),
       mp_obj_new_int(F_MEM),
    };
    return mp_obj_new_tuple(3, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_freq_obj, pyb_freq);

/// \function sync()
/// Sync all file systems.
STATIC mp_obj_t pyb_sync(void) {
    printf("sync not currently implemented\n");
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
    return MP_OBJ_NEW_SMALL_INT(mp_hal_ticks_ms());
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
    uint32_t currMillis = mp_hal_ticks_ms();
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
    return MP_OBJ_NEW_SMALL_INT(micros());
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
    uint32_t currMicros = micros();
    return MP_OBJ_NEW_SMALL_INT((currMicros - startMicros) & 0x3fffffff);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_elapsed_micros_obj, pyb_elapsed_micros);

/// \function delay(ms)
/// Delay for the given number of milliseconds.
STATIC mp_obj_t pyb_delay(mp_obj_t ms_in) {
    mp_int_t ms = mp_obj_get_int(ms_in);
    if (ms >= 0) {
        mp_hal_delay_ms(ms);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_delay_obj, pyb_delay);

/// \function udelay(us)
/// Delay for the given number of microseconds.
STATIC mp_obj_t pyb_udelay(mp_obj_t usec_in) {
    mp_int_t usec = mp_obj_get_int(usec_in);
    delayMicroseconds(usec);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_udelay_obj, pyb_udelay);

STATIC mp_obj_t pyb_stop(void) {
    printf("stop not currently implemented\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_stop_obj, pyb_stop);

STATIC mp_obj_t pyb_standby(void) {
    printf("standby not currently implemented\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_standby_obj, pyb_standby);

/// \function have_cdc()
/// Return True if USB is connected as a serial device, False otherwise.
STATIC mp_obj_t pyb_have_cdc(void ) {
    return mp_obj_new_bool(usb_vcp_is_connected());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_have_cdc_obj, pyb_have_cdc);

/// \function hid((buttons, x, y, z))
/// Takes a 4-tuple (or list) and sends it to the USB host (the PC) to
/// signal a HID mouse-motion event.
STATIC mp_obj_t pyb_hid_send_report(mp_obj_t arg) {
#if 1
    printf("hid_send_report not currently implemented\n");
#else
    mp_obj_t *items;
    mp_obj_get_array_fixed_n(arg, 4, &items);
    uint8_t data[4];
    data[0] = mp_obj_get_int(items[0]);
    data[1] = mp_obj_get_int(items[1]);
    data[2] = mp_obj_get_int(items[2]);
    data[3] = mp_obj_get_int(items[3]);
    usb_hid_send_report(data);
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_hid_send_report_obj, pyb_hid_send_report);

MP_DECLARE_CONST_FUN_OBJ_1(pyb_source_dir_obj); // defined in main.c
MP_DECLARE_CONST_FUN_OBJ_1(pyb_main_obj); // defined in main.c
MP_DECLARE_CONST_FUN_OBJ_1(pyb_usb_mode_obj); // defined in main.c

STATIC const mp_map_elem_t pyb_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pyb) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_bootloader), (mp_obj_t)&pyb_bootloader_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_info), (mp_obj_t)&pyb_info_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unique_id), (mp_obj_t)&pyb_unique_id_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_freq), (mp_obj_t)&pyb_freq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_repl_info), (mp_obj_t)&pyb_set_repl_info_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_wfi), (mp_obj_t)&pyb_wfi_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable_irq), (mp_obj_t)&pyb_disable_irq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable_irq), (mp_obj_t)&pyb_enable_irq_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_stop), (mp_obj_t)&pyb_stop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_standby), (mp_obj_t)&pyb_standby_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_source_dir), (mp_obj_t)&pyb_source_dir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_main), (mp_obj_t)&pyb_main_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_usb_mode), (mp_obj_t)&pyb_usb_mode_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_have_cdc), (mp_obj_t)&pyb_have_cdc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hid), (mp_obj_t)&pyb_hid_send_report_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_millis), (mp_obj_t)&pyb_millis_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_elapsed_millis), (mp_obj_t)&pyb_elapsed_millis_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_micros), (mp_obj_t)&pyb_micros_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_elapsed_micros), (mp_obj_t)&pyb_elapsed_micros_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_delay), (mp_obj_t)&pyb_delay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_udelay), (mp_obj_t)&pyb_udelay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sync), (mp_obj_t)&pyb_sync_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_Timer), (mp_obj_t)&pyb_timer_type },

//#if MICROPY_HW_ENABLE_RNG
//    { MP_OBJ_NEW_QSTR(MP_QSTR_rng), (mp_obj_t)&pyb_rng_get_obj },
//#endif

//#if MICROPY_HW_ENABLE_RTC
//    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC), (mp_obj_t)&pyb_rtc_type },
//#endif

    { MP_OBJ_NEW_QSTR(MP_QSTR_Pin), (mp_obj_t)&pin_type },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_ExtInt), (mp_obj_t)&extint_type },

#if MICROPY_HW_ENABLE_SERVO
    { MP_OBJ_NEW_QSTR(MP_QSTR_pwm), (mp_obj_t)&pyb_pwm_set_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_servo), (mp_obj_t)&pyb_servo_set_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Servo), (mp_obj_t)&pyb_servo_type },
#endif

#if MICROPY_HW_HAS_SWITCH
    { MP_OBJ_NEW_QSTR(MP_QSTR_Switch), (mp_obj_t)&pyb_switch_type },
#endif

//#if MICROPY_HW_HAS_SDCARD
//    { MP_OBJ_NEW_QSTR(MP_QSTR_SD), (mp_obj_t)&pyb_sdcard_obj },
//#endif

    { MP_OBJ_NEW_QSTR(MP_QSTR_LED), (mp_obj_t)&pyb_led_type },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_I2C), (mp_obj_t)&pyb_i2c_type },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_SPI), (mp_obj_t)&pyb_spi_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_UART), (mp_obj_t)&pyb_uart_type },

//    { MP_OBJ_NEW_QSTR(MP_QSTR_ADC), (mp_obj_t)&pyb_adc_type },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_ADCAll), (mp_obj_t)&pyb_adc_all_type },

//#if MICROPY_HW_ENABLE_DAC
//    { MP_OBJ_NEW_QSTR(MP_QSTR_DAC), (mp_obj_t)&pyb_dac_type },
//#endif

//#if MICROPY_HW_HAS_MMA7660
//    { MP_OBJ_NEW_QSTR(MP_QSTR_Accel), (mp_obj_t)&pyb_accel_type },
//#endif
};

STATIC MP_DEFINE_CONST_DICT(pyb_module_globals, pyb_module_globals_table);

const mp_obj_module_t pyb_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&pyb_module_globals,
};
