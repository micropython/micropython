/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
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

#include <stdio.h>

#include "modmachine.h"

#include "py/gc.h"
#include "py/mphal.h"
#include "extmod/machine_mem.h"

#include "gccollect.h"
#include "irq.h"
#include "pin.h"
#include "timer.h"
#include "uart.h"

#include "core_pins.h"

bool machine_soft_reset = false;

// The RCM_SRS0 register is only 8 bits wide, so we create other values with bits
// in the second byte

#define MACHINE_SOFT_RESET  0x00000100

// machine.info([dump_alloc_table])
// Print out lots of information about the board.
STATIC mp_obj_t machine_info(uint n_args, const mp_obj_t *args) {
    // get and print unique id; 96 bits
    {
        byte *id = (byte*)MP_HAL_UNIQUE_ID_ADDRESS;
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

#if 0
    // free space on flash
    {
        DWORD nclst;
        FATFS *fatfs;
        f_getfree("/flash", &nclst, &fatfs);
        printf("LFS free: %u bytes\n", (uint)(nclst * fatfs->csize * 512));
    }
#endif

    if (n_args == 1) {
        // arg given means dump gc allocation table
        gc_dump_alloc_table();
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);

// Returns a string of 12 bytes (96 bits), which is the unique ID for the MCU.
STATIC mp_obj_t machine_unique_id(void) {
    byte *id = (byte*)MP_HAL_UNIQUE_ID_ADDRESS;
    return mp_obj_new_bytes(id, 12);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

// Resets the pyboard in a manner similar to pushing the external RESET button.
STATIC mp_obj_t machine_reset(void) {
    // teensy doesn't seem to be setup to #include core_cm4.h which is where
    // NVIC_SystemReset is defined. So we just copy the body and needed constants
    // here.

    __asm__ volatile ("dsb");                   /* Ensure all outstanding memory accesses included
                                                   buffered write are completed before reset */
#define SCB_AIRCR_VECTKEY_Pos       16
#define SCB_AIRCR_PRIGROUP_Pos      8
#define SCB_AIRCR_PRIGROUP_Msk      (7UL << SCB_AIRCR_PRIGROUP_Pos)
#define SCB_AIRCR_SYSRESETREQ_Pos   2
#define SCB_AIRCR_SYSRESETREQ_Msk   (1UL << SCB_AIRCR_SYSRESETREQ_Pos)

    SCB_AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos)     |
                  (SCB_AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                   SCB_AIRCR_SYSRESETREQ_Msk);  /* Keep priority group unchanged */
    __asm__ volatile ("dsb");                   /* Ensure completion of memory access */
    while(1);                                   /* wait until reset */

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

// Activate the bootloader without BOOT* pins.
STATIC NORETURN mp_obj_t machine_bootloader(void) {
    _reboot_Teensyduino_();
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_bootloader_obj, machine_bootloader);

/// \function freq()
/// Return a tuple of clock frequencies: (SYSCLK, HCLK, PCLK1, PCLK2).
// TODO should also be able to set frequency via this function
STATIC mp_obj_t machine_freq(void) {
    mp_obj_t tuple[3] = {
       mp_obj_new_int(F_CPU),
       mp_obj_new_int(F_BUS),
       mp_obj_new_int(F_MEM),
    };
    return mp_obj_new_tuple(3, tuple);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_freq_obj, machine_freq);

STATIC mp_obj_t machine_sleep(void) {
    printf("sleep not currently implemented\n");
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_sleep_obj, machine_sleep);

STATIC mp_obj_t machine_deepsleep(void) {
    printf("deepsleep not currently implemented\n");
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_deepsleep_obj, machine_deepsleep);

STATIC mp_obj_t machine_reset_cause(void) {
    if (machine_soft_reset) {
        return MP_OBJ_NEW_SMALL_INT(MACHINE_SOFT_RESET);
    }
    if (RCM_SRS0 & RCM_SRS0_POR) {
        return MP_OBJ_NEW_SMALL_INT(RCM_SRS0_POR);
    }
    if (RCM_SRS0 & RCM_SRS0_PIN) {
        return MP_OBJ_NEW_SMALL_INT(RCM_SRS0_PIN);
    }
    if (RCM_SRS0 & RCM_SRS0_WDOG) {
        return MP_OBJ_NEW_SMALL_INT(RCM_SRS0_WDOG);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

STATIC mp_obj_t machine_wake_reason(void) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_wake_reason_obj, machine_wake_reason);

STATIC const mp_map_elem_t machine_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),            MP_OBJ_NEW_QSTR(MP_QSTR_umachine) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_info),                (mp_obj_t)&machine_info_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unique_id),           (mp_obj_t)&machine_unique_id_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset),               (mp_obj_t)&machine_reset_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bootloader),          (mp_obj_t)&machine_bootloader_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_freq),                (mp_obj_t)&machine_freq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_idle),                (mp_obj_t)&pyb_wfi_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep),               (mp_obj_t)&machine_sleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deepsleep),           (mp_obj_t)&machine_deepsleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset_cause),         (mp_obj_t)&machine_reset_cause_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_wake_reason),         (mp_obj_t)&machine_wake_reason_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_disable_irq),         (mp_obj_t)&pyb_disable_irq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable_irq),          (mp_obj_t)&pyb_enable_irq_obj },

    { MP_ROM_QSTR(MP_QSTR_mem8),                    (mp_obj_t)&machine_mem8_obj },
    { MP_ROM_QSTR(MP_QSTR_mem16),                   (mp_obj_t)&machine_mem16_obj },
    { MP_ROM_QSTR(MP_QSTR_mem32),                   (mp_obj_t)&machine_mem32_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_Pin),                 (mp_obj_t)&pin_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Timer),               (mp_obj_t)&pyb_timer_type },

#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC),                 (mp_obj_t)&pyb_rtc_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ADC),                 (mp_obj_t)&pyb_adc_type },
#endif
#if 0
    // TODO: Per new API, both types below, if called with 1 arg (ID), should still
    // initialize master mode on the peripheral.
    { MP_OBJ_NEW_QSTR(MP_QSTR_I2C),                 (mp_obj_t)&machine_i2c_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPI),                 (mp_obj_t)&pyb_spi_type },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_UART),                (mp_obj_t)&pyb_uart_type },
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_WDT),                 (mp_obj_t)&pyb_wdt_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HeartBeat),           (mp_obj_t)&pyb_heartbeat_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD),                  (mp_obj_t)&pyb_sd_type },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_IDLE),                MP_OBJ_NEW_SMALL_INT(PYB_PWR_MODE_ACTIVE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLEEP),               MP_OBJ_NEW_SMALL_INT(PYB_PWR_MODE_LPDS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_DEEPSLEEP),           MP_OBJ_NEW_SMALL_INT(PYB_PWR_MODE_HIBERNATE) },
#endif

    { MP_OBJ_NEW_QSTR(MP_QSTR_POWER_ON),            MP_OBJ_NEW_SMALL_INT(RCM_SRS0_POR) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HARD_RESET),          MP_OBJ_NEW_SMALL_INT(RCM_SRS0_PIN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WDT_RESET),           MP_OBJ_NEW_SMALL_INT(RCM_SRS0_WDOG) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOFT_RESET),          MP_OBJ_NEW_SMALL_INT(MACHINE_SOFT_RESET) },
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_OBJ_NEW_SMALL_INT(PYB_SLP_HIB_RESET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WLAN_WAKE),           MP_OBJ_NEW_SMALL_INT(PYB_SLP_WAKED_BY_WLAN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PIN_WAKE),            MP_OBJ_NEW_SMALL_INT(PYB_SLP_WAKED_BY_GPIO) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC_WAKE),            MP_OBJ_NEW_SMALL_INT(PYB_SLP_WAKED_BY_RTC) },
#endif
};


STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t machine_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};
