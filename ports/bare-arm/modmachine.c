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

// Machine module for bare-arm port (STM32F4 hardware access)

#include <stdint.h>
#include "py/runtime.h"
#include "py/mphal.h"

// Forward declarations for all peripheral types
extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_uart_type;
extern const mp_obj_type_t machine_i2c_type;
extern const mp_obj_type_t machine_spi_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_timer_type;

// STM32F405 register definitions
#define RCC_BASE      0x40023800
#define GPIOA_BASE    0x40020000
#define GPIOB_BASE    0x40020400
#define GPIOC_BASE    0x40020800
#define GPIOD_BASE    0x40020C00

#define RCC_AHB1ENR   (*(volatile uint32_t *)(RCC_BASE + 0x30))

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_TypeDef;

#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD ((GPIO_TypeDef *)GPIOD_BASE)

// ========== Pin Object ==========
typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t port;  // 0=A, 1=B, 2=C, 3=D
    uint8_t pin;   // 0-15
    uint8_t mode;  // 0=IN, 1=OUT
    uint8_t pull;  // 0=NONE, 1=UP, 2=DOWN
} machine_pin_obj_t;

static GPIO_TypeDef *get_gpio_port(uint8_t port) {
    switch (port) {
        case 0: return GPIOA;
        case 1: return GPIOB;
        case 2: return GPIOC;
        case 3: return GPIOD;
        default: return GPIOA;
    }
}

static void gpio_clock_enable(uint8_t port) {
    RCC_AHB1ENR |= (1 << port);
}

static mp_obj_t machine_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 3, false);

    machine_pin_obj_t *self = mp_obj_malloc(machine_pin_obj_t, type);

    // Parse pin number (0-63: port*16 + pin)
    mp_int_t pin_num = mp_obj_get_int(args[0]);
    self->port = pin_num / 16;
    self->pin = pin_num % 16;
    self->mode = 0;  // INPUT
    self->pull = 0;  // NONE

    // Enable GPIO clock
    gpio_clock_enable(self->port);

    GPIO_TypeDef *gpio = get_gpio_port(self->port);

    // Parse mode if provided
    if (n_args >= 2) {
        mp_int_t mode = mp_obj_get_int(args[1]);
        self->mode = mode;

        // Configure mode
        uint32_t moder = gpio->MODER;
        moder &= ~(3 << (self->pin * 2));
        moder |= (mode << (self->pin * 2));
        gpio->MODER = moder;
    }

    // Parse pull if provided
    if (n_args >= 3) {
        mp_int_t pull = mp_obj_get_int(args[2]);
        self->pull = pull;

        uint32_t pupdr = gpio->PUPDR;
        pupdr &= ~(3 << (self->pin * 2));
        pupdr |= (pull << (self->pin * 2));
        gpio->PUPDR = pupdr;
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    GPIO_TypeDef *gpio = get_gpio_port(self->port);

    if (n_args == 1) {
        // Get value
        return MP_OBJ_NEW_SMALL_INT((gpio->IDR >> self->pin) & 1);
    } else {
        // Set value
        mp_int_t value = mp_obj_get_int(args[1]);
        if (value) {
            gpio->BSRR = (1 << self->pin);  // Set
        } else {
            gpio->BSRR = (1 << (self->pin + 16));  // Reset
        }
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    GPIO_TypeDef *gpio = get_gpio_port(self->port);
    gpio->BSRR = (1 << self->pin);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    GPIO_TypeDef *gpio = get_gpio_port(self->port);
    gpio->BSRR = (1 << (self->pin + 16));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },

    // Mode constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(1) },

    // Pull constants
    { MP_ROM_QSTR(MP_QSTR_PULL_NONE), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(2) },
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pin_make_new,
    locals_dict, &machine_pin_locals_dict
);

// ========== Machine Module ==========

static mp_obj_t machine_reset(void) {
    // Trigger system reset via NVIC
    #define SCB_AIRCR ((volatile uint32_t *)0xE000ED0C)
    *SCB_AIRCR = 0x05FA0004;  // SYSRESETREQ
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

static mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    // STM32F405 default: 168 MHz
    if (n_args == 0) {
        return MP_OBJ_NEW_SMALL_INT(168000000);
    }
    // Setting frequency not supported in bare-metal
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 1, machine_freq);

static mp_obj_t machine_unique_id(void) {
    // STM32 unique ID at 0x1FFF7A10 (96 bits)
    uint8_t *id = (uint8_t *)0x1FFF7A10;
    return mp_obj_new_bytes(id, 12);
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

static mp_obj_t machine_idle(void) {
    __asm__ volatile ("wfi");  // Wait For Interrupt
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_idle_obj, machine_idle);

static const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_machine) },

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
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id), MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_idle), MP_ROM_PTR(&machine_idle_obj) },
};
static MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&machine_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_machine, mp_module_machine);
