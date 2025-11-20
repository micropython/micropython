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

// Machine module for minimal port (STM32F4-based)

#include <stdint.h>
#include "py/runtime.h"
#include "py/mphal.h"

// STM32F4 GPIO registers - same as bare-arm port
#define RCC_BASE      0x40023800
#define GPIOA_BASE    0x40020000
#define GPIOB_BASE    0x40020400
#define GPIOC_BASE    0x40020800
#define GPIOD_BASE    0x40020C00

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

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB3RSTR;
    uint32_t RESERVED0;
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    uint32_t RESERVED1[2];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB3ENR;
    uint32_t RESERVED2;
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
} RCC_TypeDef;

#define RCC ((RCC_TypeDef *)RCC_BASE)

// ========== Pin Object ==========
typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t port;  // 0=A, 1=B, 2=C, 3=D
    uint8_t pin;   // 0-15
    uint8_t mode;  // 0=IN, 1=OUT
    uint8_t pull;  // 0=NONE, 1=UP, 2=DOWN
} machine_pin_obj_t;

STATIC GPIO_TypeDef *get_gpio(uint8_t port) {
    switch (port) {
        case 0: return (GPIO_TypeDef *)GPIOA_BASE;
        case 1: return (GPIO_TypeDef *)GPIOB_BASE;
        case 2: return (GPIO_TypeDef *)GPIOC_BASE;
        case 3: return (GPIO_TypeDef *)GPIOD_BASE;
        default: return (GPIO_TypeDef *)GPIOA_BASE;
    }
}

STATIC void enable_gpio_clock(uint8_t port) {
    RCC->AHB1ENR |= (1 << port);
}

STATIC mp_obj_t machine_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 3, false);

    machine_pin_obj_t *self = mp_obj_malloc(machine_pin_obj_t, type);

    mp_int_t pin_num = mp_obj_get_int(args[0]);
    self->port = pin_num / 16;
    self->pin = pin_num % 16;
    self->mode = (n_args >= 2) ? mp_obj_get_int(args[1]) : 0;
    self->pull = (n_args >= 3) ? mp_obj_get_int(args[2]) : 0;

    enable_gpio_clock(self->port);

    GPIO_TypeDef *gpio = get_gpio(self->port);

    // Configure mode
    gpio->MODER &= ~(3 << (self->pin * 2));
    if (self->mode == 1) {
        gpio->MODER |= (1 << (self->pin * 2));  // Output
    }

    // Configure pull-up/down
    gpio->PUPDR &= ~(3 << (self->pin * 2));
    if (self->pull == 1) {
        gpio->PUPDR |= (1 << (self->pin * 2));  // Pull-up
    } else if (self->pull == 2) {
        gpio->PUPDR |= (2 << (self->pin * 2));  // Pull-down
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    GPIO_TypeDef *gpio = get_gpio(self->port);

    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT((gpio->IDR >> self->pin) & 1);
    } else {
        if (mp_obj_is_true(args[1])) {
            gpio->BSRR = (1 << self->pin);
        } else {
            gpio->BSRR = (1 << (self->pin + 16));
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

STATIC mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    GPIO_TypeDef *gpio = get_gpio(self->port);
    gpio->BSRR = (1 << self->pin);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

STATIC mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    GPIO_TypeDef *gpio = get_gpio(self->port);
    gpio->BSRR = (1 << (self->pin + 16));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },

    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_PULL_NONE), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(2) },
};
STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pin_make_new,
    locals_dict, &machine_pin_locals_dict
);

// ========== Machine Module Functions ==========

STATIC mp_obj_t machine_reset(void) {
    // Cortex-M4 NVIC reset
    #define SCB_AIRCR   (*(volatile uint32_t *)0xE000ED0C)
    SCB_AIRCR = 0x05FA0004;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

STATIC mp_obj_t machine_freq(void) {
    return MP_OBJ_NEW_SMALL_INT(168000000);  // 168 MHz for STM32F405
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_freq_obj, machine_freq);

STATIC mp_obj_t machine_unique_id(void) {
    // STM32 unique device ID at 0x1FFF7A10
    uint32_t *id = (uint32_t *)0x1FFF7A10;
    byte uid[12];
    for (int i = 0; i < 3; i++) {
        uint32_t val = id[i];
        uid[i * 4 + 0] = val & 0xFF;
        uid[i * 4 + 1] = (val >> 8) & 0xFF;
        uid[i * 4 + 2] = (val >> 16) & 0xFF;
        uid[i * 4 + 3] = (val >> 24) & 0xFF;
    }
    return mp_obj_new_bytes(uid, sizeof(uid));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

STATIC mp_obj_t machine_idle(void) {
    __WFI();  // Wait for interrupt
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_idle_obj, machine_idle);

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_machine) },

    // Classes
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) },

    // Functions
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id), MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_idle), MP_ROM_PTR(&machine_idle_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&machine_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_machine, mp_module_machine);
