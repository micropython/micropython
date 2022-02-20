/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#include "extmod/machine_mem.h"
#include "samd_soc.h"
#include "modmachine.h"

// ASF 4
#include "hal_flash.h"
#include "hal_init.h"
#include "hpl_gclk_base.h"
#include "hpl_pm_base.h"

#if defined(MCU_SAMD21)
#define DBL_TAP_ADDR ((volatile uint32_t *)(0x20000000 + 32 * 1024 - 4))
#elif defined(MCU_SAMD51)
#define DBL_TAP_ADDR ((volatile uint32_t *)(0x20000000 + 192 * 1024 - 4))
#endif
#define DBL_TAP_MAGIC_LOADER 0xf01669ef
#define DBL_TAP_MAGIC_RESET 0xf02669ef

MP_DEFINE_CONST_FUN_OBJ_0(machine_uart_init_obj, machine_uart_init);
MP_DEFINE_CONST_FUN_OBJ_0(machine_uart_deinit_obj, machine_uart_deinit);

STATIC mp_obj_t machine_reset(void) {
    *DBL_TAP_ADDR = DBL_TAP_MAGIC_RESET;
    NVIC_SystemReset();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

STATIC mp_obj_t machine_bootloader(void) {
    *DBL_TAP_ADDR = DBL_TAP_MAGIC_LOADER;
    NVIC_SystemReset();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_bootloader_obj, machine_bootloader);

STATIC mp_obj_t machine_freq(void) {
    return MP_OBJ_NEW_SMALL_INT(CPU_FREQ);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_freq_obj, machine_freq);

STATIC mp_obj_t machine_unique_id(void) {
    // Each device has a unique 128-bit serial number which is a concatenation of four 32-bit
    // words contained at the following addresses. The uniqueness of the serial number is
    // guaranteed only when using all 128 bits.
    // Atmel SAM D21E / SAM D21G / SAM D21J
    // SMART ARM-Based Microcontroller
    // DATASHEET
    // 9.6 (SAMD51) or 9.3.3 (or 10.3.3 depending on which manual)(SAMD21) Serial Number
    //
    // EXAMPLE (SAMD21)
    // ----------------
    // OpenOCD:
    // Word0:
    // > at91samd21g18.cpu mdw 0x0080A00C 1
    // 0x0080a00c: 6e27f15f
    // Words 1-3:
    // > at91samd21g18.cpu mdw 0x0080A040 3
    // 0x0080a040: 50534b54 332e3120 ff091645
    //
    // MicroPython (this code and same order as shown in Arduino IDE)
    // >>> ubinascii.hexlify(machine.unique_id())
    // b'6e27f15f50534b54332e3120ff091645'

    #if defined(MCU_SAMD21)
    uint32_t *id_addresses[4] = {(uint32_t *)0x0080A00C, (uint32_t *)0x0080A040,
                                 (uint32_t *)0x0080A044, (uint32_t *)0x0080A048};
    #elif defined(MCU_SAMD51)
    uint32_t *id_addresses[4] = {(uint32_t *)0x008061FC, (uint32_t *)0x00806010,
                                 (uint32_t *)0x00806014, (uint32_t *)0x00806018};
    #endif
    uint8_t raw_id[16];

    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 4; k++) {
            // 'Reverse' the read bytes into a 32 bit word (Consistent with Arduino)
            raw_id[4 * i + k] = (*(id_addresses[i]) >> (24 - k * 8)) & 0xff;
        }
    }

    return mp_obj_new_bytes((byte *)&raw_id, sizeof(raw_id));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_umachine) },
    { MP_ROM_QSTR(MP_QSTR_reset),               MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_bootloader),          MP_ROM_PTR(&machine_bootloader_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq),                MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem8),                MP_ROM_PTR(&machine_mem8_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem16),               MP_ROM_PTR(&machine_mem16_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem32),               MP_ROM_PTR(&machine_mem32_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id),           MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_uart_init),           MP_ROM_PTR(&machine_uart_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_uart_deinit),         MP_ROM_PTR(&machine_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) },
    { MP_ROM_QSTR(MP_QSTR_LED),                 MP_ROM_PTR(&machine_led_type) },
};
STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&machine_module_globals,
};
