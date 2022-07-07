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
#include "extmod/machine_bitstream.h"
#include "extmod/machine_mem.h"
#include "extmod/machine_pulse.h"
#include "extmod/machine_i2c.h"
#include "extmod/machine_spi.h"
#include "drivers/dht/dht.h"
#include "modmachine.h"
#include "samd_soc.h"

// ASF 4
#include "hal_flash.h"
#include "hal_init.h"
#include "hpl_gclk_base.h"
#include "hpl_pm_base.h"

#if MICROPY_PY_MACHINE

#if defined(MCU_SAMD21)
#define DBL_TAP_ADDR ((volatile uint32_t *)(0x20000000 + 32 * 1024 - 4))
#elif defined(MCU_SAMD51)
#define DBL_TAP_ADDR ((volatile uint32_t *)(0x20000000 + 192 * 1024 - 4))
#endif
#define DBL_TAP_MAGIC_LOADER 0xf01669ef
#define DBL_TAP_MAGIC_RESET 0xf02669ef

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

STATIC mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return MP_OBJ_NEW_SMALL_INT(get_cpu_freq());
    } else {
        uint32_t freq = mp_obj_get_int(args[0]);
        if (freq >= 1000000 && freq <= MAX_CPU_FREQ) {
            set_cpu_freq(freq);
            SysTick_Config(get_cpu_freq() / 1000);
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 1, machine_freq);

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

STATIC mp_obj_t machine_idle(void) {
    MICROPY_EVENT_POLL_HOOK;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_idle_obj, machine_idle);

STATIC mp_obj_t machine_disable_irq(void) {
    uint32_t state = MICROPY_BEGIN_ATOMIC_SECTION();
    return mp_obj_new_int(state);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_disable_irq_obj, machine_disable_irq);

STATIC mp_obj_t machine_enable_irq(mp_obj_t state_in) {
    uint32_t state = mp_obj_get_int(state_in);
    MICROPY_END_ATOMIC_SECTION(state);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_enable_irq_obj, machine_enable_irq);

STATIC mp_obj_t machine_reset_cause(void) {
    #if defined(MCU_SAMD21)
    return MP_OBJ_NEW_SMALL_INT(PM->RCAUSE.reg);
    #elif defined(MCU_SAMD51)
    return MP_OBJ_NEW_SMALL_INT(RSTC->RCAUSE.reg);
    #else
    return MP_OBJ_NEW_SMALL_INT(0);
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_umachine) },
    { MP_ROM_QSTR(MP_QSTR_reset),               MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_bootloader),          MP_ROM_PTR(&machine_bootloader_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq),                MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem8),                MP_ROM_PTR(&machine_mem8_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem16),               MP_ROM_PTR(&machine_mem16_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem32),               MP_ROM_PTR(&machine_mem32_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id),           MP_ROM_PTR(&machine_unique_id_obj) },

    { MP_ROM_QSTR(MP_QSTR_ADC),                 MP_ROM_PTR(&machine_adc_type) },
    { MP_ROM_QSTR(MP_QSTR_DAC),                 MP_ROM_PTR(&machine_dac_type) },
    { MP_ROM_QSTR(MP_QSTR_LED),                 MP_ROM_PTR(&machine_led_type) },
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) },
    { MP_ROM_QSTR(MP_QSTR_PWM),                 MP_ROM_PTR(&machine_pwm_type) },
    { MP_ROM_QSTR(MP_QSTR_SoftI2C),             MP_ROM_PTR(&mp_machine_soft_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_I2C),                 MP_ROM_PTR(&machine_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_SoftSPI),             MP_ROM_PTR(&mp_machine_soft_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_SPI),                 MP_ROM_PTR(&machine_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) },
    { MP_ROM_QSTR(MP_QSTR_UART),                MP_ROM_PTR(&machine_uart_type) },
    { MP_ROM_QSTR(MP_QSTR_WDT),                 MP_ROM_PTR(&machine_wdt_type) },

    { MP_ROM_QSTR(MP_QSTR_idle),                MP_ROM_PTR(&machine_idle_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_irq),         MP_ROM_PTR(&machine_disable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_irq),          MP_ROM_PTR(&machine_enable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_cause),         MP_ROM_PTR(&machine_reset_cause_obj) },
    { MP_ROM_QSTR(MP_QSTR_time_pulse_us),       MP_ROM_PTR(&machine_time_pulse_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_bitstream),           MP_ROM_PTR(&machine_bitstream_obj) },
    #if MICROPY_PY_MACHINE_DHT_READINTO
    { MP_ROM_QSTR(MP_QSTR_dht_readinto),        MP_ROM_PTR(&dht_readinto_obj) },
    #endif

    // Class constants.
    // Use numerical constants instead of the symbolic names,
    // since the names differ between SAMD21 and SAMD51.
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(0x01) },
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),          MP_ROM_INT(0x10) },
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(0x20) },
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(0x40) },
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_ROM_INT(0x80) },
};
STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&machine_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_umachine, mp_module_machine);

#endif // MICROPY_PY_MACHINE
