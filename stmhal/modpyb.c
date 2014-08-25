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

#include "stm32f4xx_hal.h"

#include "mpconfig.h"
#include "misc.h"
#include "nlr.h"
#include "qstr.h"
#include "obj.h"
#include "gc.h"
#include "gccollect.h"
#include "irq.h"
#include "systick.h"
#include "pyexec.h"
#include "led.h"
#include "pin.h"
#include "timer.h"
#include "extint.h"
#include "usrsw.h"
#include "rng.h"
#include "rtc.h"
#include "i2c.h"
#include "spi.h"
#include "uart.h"
#include "adc.h"
#include "storage.h"
#include "sdcard.h"
#include "accel.h"
#include "servo.h"
#include "dac.h"
#include "lcd.h"
#include "usb.h"
#include "pybstdio.h"
#include "ff.h"
#include "portmodules.h"

/// \module pyb - functions related to the pyboard
///
/// The `pyb` module contains specific functions related to the pyboard.

/// \function bootloader()
/// Activate the bootloader without BOOT* pins.
STATIC NORETURN mp_obj_t pyb_bootloader(void) {
    pyb_usb_dev_stop();
    storage_flush();

    HAL_RCC_DeInit();
    HAL_DeInit();

    __HAL_REMAPMEMORY_SYSTEMFLASH();

    // arm-none-eabi-gcc 4.9.0 does not correctly inline this
    // MSP function, so we write it out explicitly here.
    //__set_MSP(*((uint32_t*) 0x00000000));
    __ASM volatile ("movs r3, #0\nldr r3, [r3, #0]\nMSR msp, r3\n" : : : "r3", "sp");

    ((void (*)(void)) *((uint32_t*) 0x00000004))();

    while (1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_bootloader_obj, pyb_bootloader);

/// \function info([dump_alloc_table])
/// Print out lots of information about the board.
STATIC mp_obj_t pyb_info(uint n_args, const mp_obj_t *args) {
    // get and print unique id; 96 bits
    {
        byte *id = (byte*)0x1fff7a10;
        printf("ID=%02x%02x%02x%02x:%02x%02x%02x%02x:%02x%02x%02x%02x\n", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7], id[8], id[9], id[10], id[11]);
    }

    // get and print clock speeds
    // SYSCLK=168MHz, HCLK=168MHz, PCLK1=42MHz, PCLK2=84MHz
    {
        printf("S=%lu\nH=%lu\nP1=%lu\nP2=%lu\n",
               HAL_RCC_GetSysClockFreq(),
               HAL_RCC_GetHCLKFreq(),
               HAL_RCC_GetPCLK1Freq(),
               HAL_RCC_GetPCLK2Freq());
    }

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

    // free space on flash
    {
        DWORD nclst;
        FATFS *fatfs;
        f_getfree("/flash", &nclst, &fatfs);
        printf("LFS free: %u bytes\n", (uint)(nclst * fatfs->csize * 512));
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
    byte *id = (byte*)0x1fff7a10;
    return mp_obj_new_bytes(id, 12);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_unique_id_obj, pyb_unique_id);

/// \function freq()
/// Return a tuple of clock frequencies: (SYSCLK, HCLK, PCLK1, PCLK2).
// TODO should also be able to set frequency via this function
STATIC mp_obj_t pyb_freq(void) {
    mp_obj_t tuple[4] = {
       mp_obj_new_int(HAL_RCC_GetSysClockFreq()),
       mp_obj_new_int(HAL_RCC_GetHCLKFreq()),
       mp_obj_new_int(HAL_RCC_GetPCLK1Freq()),
       mp_obj_new_int(HAL_RCC_GetPCLK2Freq()),
    };
    return mp_obj_new_tuple(4, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_freq_obj, pyb_freq);

/// \function sync()
/// Sync all file systems.
STATIC mp_obj_t pyb_sync(void) {
    storage_flush();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_sync_obj, pyb_sync);

/// \function millis()
/// Returns the number of milliseconds since the board was last reset.
///
/// Note that this may return a negative number. This allows you to always
/// do:
///     start = pyb.millis()
///     ...do some operation...
///     elapsed = pyb.millis() - start
///
/// and as long as the time of your operation is less than 24 days, you'll
/// always get the right answer and not have to worry about whether pyb.millis()
/// wraps around.
STATIC mp_obj_t pyb_millis(void) {
    // We want to "cast" the 32 bit unsigned into a small-int.  This means
    // copying the MSB down 1 bit (extending the sign down), which is
    // equivalent to just using the MP_OBJ_NEW_SMALL_INT macro.
    return MP_OBJ_NEW_SMALL_INT(HAL_GetTick());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_millis_obj, pyb_millis);

/// \function micros()
/// Returns the number of microseconds since the board was last reset.
///
/// Note that this may return a negative number. This allows you to always
/// do:
///     start = pyb.micros()
///     ...do some operation...
///     elapsed = pyb.micros() - start
///
/// and as long as the time of your operation is less than 35 minutes, you'll
/// always get the right answer and not have to worry about whether pyb.micros()
/// wraps around.
STATIC mp_obj_t pyb_micros(void) {
    // We want to "cast" the 32 bit unsigned into a small-int.  This means
    // copying the MSB down 1 bit (extending the sign down), which is
    // equivalent to just using the MP_OBJ_NEW_SMALL_INT macro.
    return MP_OBJ_NEW_SMALL_INT(sys_tick_get_microseconds());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_micros_obj, pyb_micros);

/// \function delay(ms)
/// Delay for the given number of milliseconds.
STATIC mp_obj_t pyb_delay(mp_obj_t ms_in) {
    mp_int_t ms = mp_obj_get_int(ms_in);
    if (ms >= 0) {
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
        uint32_t count = 0;
        const uint32_t utime = (168 * usec / 4);
        while (++count <= utime) {
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_udelay_obj, pyb_udelay);

#if 0
STATIC void SYSCLKConfig_STOP(void) {
    /* After wake-up from STOP reconfigure the system clock */
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* Wait till HSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET) {
    }

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08) {
    }
}
#endif

STATIC mp_obj_t pyb_stop(void) {
#if 0
    PWR_EnterSTANDBYMode();
    //PWR_FlashPowerDownCmd(ENABLE); don't know what the logic is with this

    /* Enter Stop Mode */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    /* Configures system clock after wake-up from STOP: enable HSE, PLL and select
     *        PLL as system clock source (HSE and PLL are disabled in STOP mode) */
    SYSCLKConfig_STOP();

    //PWR_FlashPowerDownCmd(DISABLE);
#endif
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_stop_obj, pyb_stop);

STATIC mp_obj_t pyb_standby(void) {
#if 0
    PWR_EnterSTANDBYMode();
#endif
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_standby_obj, pyb_standby);

/// \function have_cdc()
/// Return True if USB is connected as a serial device, False otherwise.
STATIC mp_obj_t pyb_have_cdc(void ) {
    return MP_BOOL(usb_vcp_is_connected());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_have_cdc_obj, pyb_have_cdc);

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
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "need a UART object"));
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_repl_uart_obj, 0, 1, pyb_repl_uart);

/// \function hid((buttons, x, y, z))
/// Takes a 4-tuple (or list) and sends it to the USB host (the PC) to
/// signal a HID mouse-motion event.
STATIC mp_obj_t pyb_hid_send_report(mp_obj_t arg) {
    mp_obj_t *items;
    mp_obj_get_array_fixed_n(arg, 4, &items);
    uint8_t data[4];
    data[0] = mp_obj_get_int(items[0]);
    data[1] = mp_obj_get_int(items[1]);
    data[2] = mp_obj_get_int(items[2]);
    data[3] = mp_obj_get_int(items[3]);
    usb_hid_send_report(data);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_hid_send_report_obj, pyb_hid_send_report);

MP_DECLARE_CONST_FUN_OBJ(pyb_main_obj); // defined in main.c
MP_DECLARE_CONST_FUN_OBJ(pyb_usb_mode_obj); // defined in main.c

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
    { MP_OBJ_NEW_QSTR(MP_QSTR_main), (mp_obj_t)&pyb_main_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_usb_mode), (mp_obj_t)&pyb_usb_mode_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_have_cdc), (mp_obj_t)&pyb_have_cdc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_repl_uart), (mp_obj_t)&pyb_repl_uart_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hid), (mp_obj_t)&pyb_hid_send_report_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_USB_VCP), (mp_obj_t)&pyb_usb_vcp_type },

    { MP_OBJ_NEW_QSTR(MP_QSTR_millis), (mp_obj_t)&pyb_millis_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_micros), (mp_obj_t)&pyb_micros_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_delay), (mp_obj_t)&pyb_delay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_udelay), (mp_obj_t)&pyb_udelay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sync), (mp_obj_t)&pyb_sync_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_Timer), (mp_obj_t)&pyb_timer_type },

#if MICROPY_HW_ENABLE_RNG
    { MP_OBJ_NEW_QSTR(MP_QSTR_rng), (mp_obj_t)&pyb_rng_get_obj },
#endif

#if MICROPY_HW_ENABLE_RTC
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC), (mp_obj_t)&pyb_rtc_type },
#endif

    { MP_OBJ_NEW_QSTR(MP_QSTR_Pin), (mp_obj_t)&pin_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ExtInt), (mp_obj_t)&extint_type },

#if MICROPY_HW_ENABLE_SERVO
    { MP_OBJ_NEW_QSTR(MP_QSTR_pwm), (mp_obj_t)&pyb_pwm_set_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_servo), (mp_obj_t)&pyb_servo_set_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Servo), (mp_obj_t)&pyb_servo_type },
#endif

#if MICROPY_HW_HAS_SWITCH
    { MP_OBJ_NEW_QSTR(MP_QSTR_Switch), (mp_obj_t)&pyb_switch_type },
#endif

#if MICROPY_HW_HAS_SDCARD
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD), (mp_obj_t)&pyb_sdcard_obj },
#endif

    { MP_OBJ_NEW_QSTR(MP_QSTR_LED), (mp_obj_t)&pyb_led_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_I2C), (mp_obj_t)&pyb_i2c_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPI), (mp_obj_t)&pyb_spi_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_UART), (mp_obj_t)&pyb_uart_type },

    { MP_OBJ_NEW_QSTR(MP_QSTR_ADC), (mp_obj_t)&pyb_adc_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ADCAll), (mp_obj_t)&pyb_adc_all_type },

#if MICROPY_HW_ENABLE_DAC
    { MP_OBJ_NEW_QSTR(MP_QSTR_DAC), (mp_obj_t)&pyb_dac_type },
#endif

#if MICROPY_HW_HAS_MMA7660
    { MP_OBJ_NEW_QSTR(MP_QSTR_Accel), (mp_obj_t)&pyb_accel_type },
#endif

#if MICROPY_HW_HAS_LCD
    { MP_OBJ_NEW_QSTR(MP_QSTR_LCD), (mp_obj_t)&pyb_lcd_type },
#endif
};

STATIC const mp_obj_dict_t pyb_module_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = MP_ARRAY_SIZE(pyb_module_globals_table),
        .alloc = MP_ARRAY_SIZE(pyb_module_globals_table),
        .table = (mp_map_elem_t*)pyb_module_globals_table,
    },
};

const mp_obj_module_t pyb_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_pyb,
    .globals = (mp_obj_dict_t*)&pyb_module_globals,
};
