/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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

#include "py/builtin.h"
#include "py/runtime.h"

#if MICROPY_PY_MACHINE

#include "extmod/modmachine.h"
#include "shared/runtime/pyexec.h"

#if MICROPY_PY_MACHINE_DHT_READINTO
#include "drivers/dht/dht.h"
#endif

#if !MICROPY_PY_SYS_EXIT
#error MICROPY_PY_MACHINE requires MICROPY_PY_SYS_EXIT
#endif

// The port must provide implementations of these low-level machine functions.

static void mp_machine_idle(void);

#if MICROPY_PY_MACHINE_BOOTLOADER
MP_NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args);
#endif

#if MICROPY_PY_MACHINE_RESET
MP_NORETURN static void mp_machine_reset(void);
static mp_int_t mp_machine_reset_cause(void);
#endif

#if MICROPY_PY_MACHINE_BARE_METAL_FUNCS
static mp_obj_t mp_machine_unique_id(void);
static mp_obj_t mp_machine_get_freq(void);
static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args);
static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args);
MP_NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args);
#endif

// The port can provide additional machine-module implementation in this file.
#ifdef MICROPY_PY_MACHINE_INCLUDEFILE
#include MICROPY_PY_MACHINE_INCLUDEFILE
#endif

#if MICROPY_PY_MACHINE_BOOTLOADER
MP_NORETURN mp_obj_t machine_bootloader(size_t n_args, const mp_obj_t *args) {
    mp_machine_bootloader(n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_bootloader_obj, 0, 1, machine_bootloader);
#endif

static mp_obj_t machine_idle(void) {
    mp_machine_idle();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_idle_obj, machine_idle);

#if MICROPY_PY_MACHINE_RESET

MP_NORETURN static mp_obj_t machine_reset(void) {
    mp_machine_reset();
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

static mp_obj_t machine_reset_cause(void) {
    return MP_OBJ_NEW_SMALL_INT(mp_machine_reset_cause());
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

#endif

#if MICROPY_PY_MACHINE_BARE_METAL_FUNCS

static mp_obj_t machine_unique_id(void) {
    return mp_machine_unique_id();
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

static mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_machine_get_freq();
    } else {
        mp_machine_set_freq(n_args, args);
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, MICROPY_PY_MACHINE_FREQ_NUM_ARGS_MAX, machine_freq);

static mp_obj_t machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    mp_machine_lightsleep(n_args, args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lightsleep_obj, 0, 1, machine_lightsleep);

MP_NORETURN static mp_obj_t machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    mp_machine_deepsleep(n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_deepsleep_obj, 0, 1, machine_deepsleep);

#endif

#if MICROPY_PY_MACHINE_DISABLE_IRQ_ENABLE_IRQ

static mp_obj_t machine_disable_irq(void) {
    uint32_t state = MICROPY_BEGIN_ATOMIC_SECTION();
    return mp_obj_new_int(state);
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_disable_irq_obj, machine_disable_irq);

static mp_obj_t machine_enable_irq(mp_obj_t state_in) {
    uint32_t state = mp_obj_get_int(state_in);
    MICROPY_END_ATOMIC_SECTION(state);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_enable_irq_obj, machine_enable_irq);

#endif

static const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_machine) },

    // Memory access objects.
    #if MICROPY_PY_MACHINE_MEMX
    { MP_ROM_QSTR(MP_QSTR_mem8), MP_ROM_PTR(&machine_mem8_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem16), MP_ROM_PTR(&machine_mem16_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem32), MP_ROM_PTR(&machine_mem32_obj) },
    #endif

    // Miscellaneous functions.
    #if MICROPY_PY_MACHINE_BARE_METAL_FUNCS
    { MP_ROM_QSTR(MP_QSTR_unique_id), MP_ROM_PTR(&machine_unique_id_obj) },
    #endif

    // Reset related functions.
    { MP_ROM_QSTR(MP_QSTR_soft_reset), MP_ROM_PTR(&mp_sys_exit_obj) },
    #if MICROPY_PY_MACHINE_BOOTLOADER
    { MP_ROM_QSTR(MP_QSTR_bootloader), MP_ROM_PTR(&machine_bootloader_obj) },
    #endif
    #if MICROPY_PY_MACHINE_RESET
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_cause), MP_ROM_PTR(&machine_reset_cause_obj) },
    #endif

    // Power related functions.
    { MP_ROM_QSTR(MP_QSTR_idle), MP_ROM_PTR(&machine_idle_obj) },
    #if MICROPY_PY_MACHINE_BARE_METAL_FUNCS
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_lightsleep), MP_ROM_PTR(&machine_lightsleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_deepsleep), MP_ROM_PTR(&machine_deepsleep_obj) },
    #endif

    // Interrupt related functions.
    #if MICROPY_PY_MACHINE_DISABLE_IRQ_ENABLE_IRQ
    { MP_ROM_QSTR(MP_QSTR_disable_irq), MP_ROM_PTR(&machine_disable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_irq), MP_ROM_PTR(&machine_enable_irq_obj) },
    #endif

    // Functions for bit protocols.
    #if MICROPY_PY_MACHINE_BITSTREAM
    { MP_ROM_QSTR(MP_QSTR_bitstream), MP_ROM_PTR(&machine_bitstream_obj) },
    #endif
    #if MICROPY_PY_MACHINE_DHT_READINTO
    { MP_ROM_QSTR(MP_QSTR_dht_readinto), MP_ROM_PTR(&dht_readinto_obj) },
    #endif
    #if MICROPY_PY_MACHINE_PULSE
    { MP_ROM_QSTR(MP_QSTR_time_pulse_us), MP_ROM_PTR(&machine_time_pulse_us_obj) },
    #endif

    // Classes for PinBase and Signal.
    #if MICROPY_PY_MACHINE_PIN_BASE
    { MP_ROM_QSTR(MP_QSTR_PinBase), MP_ROM_PTR(&machine_pinbase_type) },
    #endif
    #if MICROPY_PY_MACHINE_SIGNAL
    { MP_ROM_QSTR(MP_QSTR_Signal), MP_ROM_PTR(&machine_signal_type) },
    #endif

    // Classes for software bus protocols.
    #if MICROPY_PY_MACHINE_SOFTI2C
    { MP_ROM_QSTR(MP_QSTR_SoftI2C), MP_ROM_PTR(&mp_machine_soft_i2c_type) },
    #endif
    #if MICROPY_PY_MACHINE_SOFTSPI
    { MP_ROM_QSTR(MP_QSTR_SoftSPI), MP_ROM_PTR(&mp_machine_soft_spi_type) },
    #endif

    // Classes for hardware peripherals.
    #if MICROPY_PY_MACHINE_ADC
    { MP_ROM_QSTR(MP_QSTR_ADC), MP_ROM_PTR(&machine_adc_type) },
    #endif
    #if MICROPY_PY_MACHINE_ADC_BLOCK
    { MP_ROM_QSTR(MP_QSTR_ADCBlock), MP_ROM_PTR(&machine_adc_block_type) },
    #endif
    #if MICROPY_PY_MACHINE_DAC
    { MP_ROM_QSTR(MP_QSTR_DAC), MP_ROM_PTR(&machine_dac_type) },
    #endif
    #if MICROPY_PY_MACHINE_I2C
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&machine_i2c_type) },
    #endif
    #if MICROPY_PY_MACHINE_I2C_TARGET
    { MP_ROM_QSTR(MP_QSTR_I2CTarget), MP_ROM_PTR(&machine_i2c_target_type) },
    #endif
    #if MICROPY_PY_MACHINE_I2S
    { MP_ROM_QSTR(MP_QSTR_I2S), MP_ROM_PTR(&machine_i2s_type) },
    #endif
    #if MICROPY_PY_MACHINE_PWM
    { MP_ROM_QSTR(MP_QSTR_PWM), MP_ROM_PTR(&machine_pwm_type) },
    #endif
    #if MICROPY_PY_MACHINE_SPI
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&machine_spi_type) },
    #endif
    #if MICROPY_PY_MACHINE_UART
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&machine_uart_type) },
    #endif
    #if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
    { MP_ROM_QSTR(MP_QSTR_USBDevice), MP_ROM_PTR(&machine_usb_device_type) },
    #endif
    #if MICROPY_PY_MACHINE_WDT
    { MP_ROM_QSTR(MP_QSTR_WDT), MP_ROM_PTR(&machine_wdt_type) },
    #endif

    // A port can add extra entries to the module by defining the following macro.
    #ifdef MICROPY_PY_MACHINE_EXTRA_GLOBALS
    MICROPY_PY_MACHINE_EXTRA_GLOBALS
    #endif
};
static MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&machine_module_globals,
};

MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_machine, mp_module_machine);

#endif // MICROPY_PY_MACHINE
