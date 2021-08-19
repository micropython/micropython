/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Armink (armink.ztl@gmail.com)
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

#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "shared/runtime/pyexec.h"
#include "extmod/machine_mem.h"
#include "extmod/machine_signal.h"
#include "extmod/machine_pulse.h"
#include "extmod/machine_i2c.h"
#include "extmod/machine_spi.h"
#include "modmachine.h"
#include "machine_uart.h"
#include "machine_adc.h"
#include "machine_pwm.h"
#include "machine_lcd.h"
#include "machine_rtc.h"
#include "machine_wdt.h"
#include "machine_timer.h"

#include <rthw.h>

#if MICROPY_PY_MACHINE

STATIC mp_obj_t machine_info(size_t n_args, const mp_obj_t *args) {
#ifdef RT_USING_FINSH
    extern long list_thread(void);
#endif
    // RT-Thread info
    {
        mp_printf(&mp_plat_print, "---------------------------------------------\n");
        mp_printf(&mp_plat_print, "RT-Thread\n");
        mp_printf(&mp_plat_print, "---------------------------------------------\n");

#ifdef RT_USING_FINSH
        extern void list_mem(void);
        extern void list_memheap(void);

#ifdef RT_USING_MEMHEAP_AS_HEAP
        list_memheap();
#else
        list_mem();
#endif

        list_thread();
#endif
        mp_printf(&mp_plat_print, "---------------------------------------------\n");
    }

    // qstr info
    {
        mp_uint_t n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        mp_printf(&mp_plat_print, "qstr:\n  n_pool=" UINT_FMT "\n  n_qstr=" UINT_FMT "\n  n_str_data_bytes=" UINT_FMT "\n  n_total_bytes=" UINT_FMT "\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    }
    mp_printf(&mp_plat_print, "---------------------------------------------\n");

    // GC info
    {
        gc_info_t info;
        gc_info(&info);
        mp_printf(&mp_plat_print, "GC:\n");
        mp_printf(&mp_plat_print, "  " UINT_FMT " total\n", info.total);
        mp_printf(&mp_plat_print, "  " UINT_FMT " : " UINT_FMT "\n", info.used, info.free);
        mp_printf(&mp_plat_print, "  1=" UINT_FMT " 2=" UINT_FMT " m=" UINT_FMT "\n", info.num_1block, info.num_2block, info.max_block);
    }

    // free space on flash
    {
        //TODO
    }

    if (n_args == 1) {
        // arg given means dump gc allocation table
        gc_dump_alloc_table();
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);

STATIC mp_obj_t machine_unique_id(void) {
    //TODO
    MP_RTT_NOT_IMPL_PRINT;
    return 0;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

STATIC mp_obj_t machine_reset(void) {
    //TODO
    MP_RTT_NOT_IMPL_PRINT;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

STATIC mp_obj_t machine_soft_reset(void) {
    pyexec_system_exit = PYEXEC_FORCED_EXIT;
    nlr_raise(mp_obj_new_exception(&mp_type_SystemExit));
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_soft_reset_obj, machine_soft_reset);

/*
* @param clkid - range 0~127 (e.g 0:SYSCLK 1:HCLK 2:PCLK1 etc)
*
* @return 0 - ok, -1 - no such clock
*/
RT_WEAK int mp_port_get_freq(int clkid, int *freq)
{
    return -1;
}

STATIC mp_obj_t machine_freq(void) {
    int i;
    mp_obj_list_t *ret_list = m_new(mp_obj_list_t, 1);
    mp_obj_list_init(ret_list, 0);
    int freq;

    for (i = 0; i < 128; i ++)
    {
        if (mp_port_get_freq(i, &freq) != 0)
            break;

        mp_obj_list_append(ret_list, mp_obj_new_int(freq));
    }

    return MP_OBJ_FROM_PTR(ret_list);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_freq_obj, machine_freq);

STATIC mp_obj_t pyb_wfi(void) {
    //TODO __WFI();
    MP_RTT_NOT_IMPL_PRINT;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_wfi_obj, pyb_wfi);

static rt_base_t int_lvl;
STATIC mp_obj_t pyb_disable_irq(void) {
    int_lvl = rt_hw_interrupt_disable();
    return mp_obj_new_bool(1);
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_disable_irq_obj, pyb_disable_irq);

STATIC mp_obj_t pyb_enable_irq(size_t n_args, const mp_obj_t *arg) {
    if (n_args == 0) {
        rt_hw_interrupt_enable(int_lvl);
    } else {
        if (mp_obj_is_true(arg[0])) {
            rt_hw_interrupt_enable(int_lvl);
        } else {
            int_lvl = rt_hw_interrupt_disable();
        }
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_enable_irq_obj, 0, 1, pyb_enable_irq);

STATIC mp_obj_t machine_sleep (void) {
    //TODO
    MP_RTT_NOT_IMPL_PRINT;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_sleep_obj, machine_sleep);

STATIC mp_obj_t machine_deepsleep (void) {
    //TODO
    MP_RTT_NOT_IMPL_PRINT;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_deepsleep_obj, machine_deepsleep);

STATIC mp_obj_t machine_reset_cause(void) {
    //TODO
    MP_RTT_NOT_IMPL_PRINT;
    return MP_OBJ_NEW_SMALL_INT(42);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_umachine) },
    { MP_ROM_QSTR(MP_QSTR_info),                MP_ROM_PTR(&machine_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id),           MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset),               MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_soft_reset),          MP_ROM_PTR(&machine_soft_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq),                MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_idle),                MP_ROM_PTR(&pyb_wfi_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep),               MP_ROM_PTR(&machine_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_deepsleep),           MP_ROM_PTR(&machine_deepsleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_cause),         MP_ROM_PTR(&machine_reset_cause_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_irq),         MP_ROM_PTR(&pyb_disable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_irq),          MP_ROM_PTR(&pyb_enable_irq_obj) },
//    { MP_ROM_QSTR(MP_QSTR_time_pulse_us),       MP_ROM_PTR(&machine_time_pulse_us_obj) },
#if MICROPY_PY_PIN
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) },
#endif
    { MP_ROM_QSTR(MP_QSTR_Signal),              MP_ROM_PTR(&machine_signal_type) },
#if MICROPY_PY_MACHINE_I2C
    { MP_ROM_QSTR(MP_QSTR_I2C),                 MP_ROM_PTR(&machine_i2c_type) },
#endif
#if MICROPY_PY_MACHINE_SPI
    { MP_ROM_QSTR(MP_QSTR_SPI),                 MP_ROM_PTR(&mp_machine_soft_spi_type) },
#endif
#if MICROPY_PY_MACHINE_UART
    { MP_ROM_QSTR(MP_QSTR_UART),                MP_ROM_PTR(&machine_uart_type) },
#endif
#if MICROPY_PY_MACHINE_RTC
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&machine_rtc_type) },
#endif
#if MICROPY_PY_MACHINE_LCD
    { MP_ROM_QSTR(MP_QSTR_LCD),                 MP_ROM_PTR(&machine_lcd_type ) },
#endif
#if MICROPY_PY_MACHINE_PWM
    { MP_ROM_QSTR(MP_QSTR_PWM),                 MP_ROM_PTR(&machine_pwm_type) },
#endif
#if MICROPY_PY_MACHINE_ADC
    { MP_ROM_QSTR(MP_QSTR_ADC),                 MP_ROM_PTR(&machine_adc_type) },
#endif
#if MICROPY_PY_MACHINE_WDT
    { MP_ROM_QSTR(MP_QSTR_WDT),                 MP_ROM_PTR(&machine_wdt_type) },
#endif
#if MICROPY_PY_MACHINE_TIMER
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};

#endif // MICROPY_PY_MACHINE
