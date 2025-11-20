/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
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
#include "py/mphal.h"
#include "extmod/modmachine.h"

// External declarations for machine module classes
extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_uart_type;
extern const mp_obj_type_t machine_i2c_type;
extern const mp_obj_type_t machine_spi_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_timer_type;

// machine.reset()
STATIC mp_obj_t machine_reset(void) {
    // Perform software reset via MIPS assembly
    __asm__ volatile("syscall");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

// machine.soft_reset()
STATIC mp_obj_t machine_soft_reset(void) {
    pyexec_system_exit = PYEXEC_FORCED_EXIT;
    mp_raise_type(&mp_type_SystemExit);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_soft_reset_obj, machine_soft_reset);

// machine.reset_cause()
STATIC mp_obj_t machine_reset_cause(void) {
    // TODO: Read RCON register to determine reset cause
    // For now return MACHINE_PWR_ON_RESET
    return MP_OBJ_NEW_SMALL_INT(1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

// machine.freq([sysclk, pbclk])
STATIC mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // Return current frequency (80 MHz default for PIC32MX)
        return MP_OBJ_NEW_SMALL_INT(80000000);
    } else {
        // Setting frequency not supported, would require PLL reconfiguration
        mp_raise_NotImplementedError(MP_ERROR_TEXT("freq set not supported"));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 2, machine_freq);

// machine.unique_id()
STATIC mp_obj_t machine_unique_id(void) {
    // PIC32 has unique device ID in DEVID register
    // For now return dummy ID
    byte id[8] = {0x50, 0x49, 0x43, 0x33, 0x32, 0x00, 0x00, 0x01};
    return mp_obj_new_bytes(id, sizeof(id));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

// machine.idle()
STATIC mp_obj_t machine_idle(void) {
    // Enter IDLE mode (CPU stops, peripherals continue)
    __asm__ volatile("wait");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_idle_obj, machine_idle);

// machine.sleep()
STATIC mp_obj_t machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    // Enter Sleep mode
    __asm__ volatile("wait");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lightsleep_obj, 0, 1, machine_lightsleep);

// machine.deepsleep()
STATIC mp_obj_t machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    // TODO: Configure deep sleep mode and wake sources
    mp_raise_NotImplementedError(MP_ERROR_TEXT("deepsleep not yet implemented"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_deepsleep_obj, 0, 1, machine_deepsleep);

// machine.disable_irq()
STATIC mp_obj_t machine_disable_irq(void) {
    uint32_t state = MACHINE_DISABLE_IRQ();
    return mp_obj_new_int(state);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_disable_irq_obj, machine_disable_irq);

// machine.enable_irq(state)
STATIC mp_obj_t machine_enable_irq(mp_obj_t state_in) {
    uint32_t state = mp_obj_get_int(state_in);
    MACHINE_ENABLE_IRQ(state);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_enable_irq_obj, machine_enable_irq);

// machine.time_pulse_us(pin, pulse_level, timeout_us)
STATIC mp_obj_t machine_time_pulse_us(size_t n_args, const mp_obj_t *args) {
    // TODO: Implement time_pulse_us using Timer peripheral
    mp_raise_NotImplementedError(MP_ERROR_TEXT("time_pulse_us not yet implemented"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_time_pulse_us_obj, 2, 3, machine_time_pulse_us);

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_umachine) },

    // Reset causes
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET), MP_ROM_INT(5) },

    // Wake reasons
    { MP_ROM_QSTR(MP_QSTR_WLAN_WAKE), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_PIN_WAKE), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_RTC_WAKE), MP_ROM_INT(4) },

    // Classes
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&machine_uart_type) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&machine_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&machine_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_PWM), MP_ROM_PTR(&machine_pwm_type) },
    { MP_ROM_QSTR(MP_QSTR_ADC), MP_ROM_PTR(&machine_adc_type) },
    { MP_ROM_QSTR(MP_QSTR_Timer), MP_ROM_PTR(&machine_timer_type) },

    // Functions
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_soft_reset), MP_ROM_PTR(&machine_soft_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_cause), MP_ROM_PTR(&machine_reset_cause_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id), MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_idle), MP_ROM_PTR(&machine_idle_obj) },
    { MP_ROM_QSTR(MP_QSTR_lightsleep), MP_ROM_PTR(&machine_lightsleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_deepsleep), MP_ROM_PTR(&machine_deepsleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_irq), MP_ROM_PTR(&machine_disable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_irq), MP_ROM_PTR(&machine_enable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_time_pulse_us), MP_ROM_PTR(&machine_time_pulse_us_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&machine_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_umachine, mp_module_machine);
