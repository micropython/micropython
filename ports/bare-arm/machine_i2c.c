/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
 */

// I2C implementation for bare-arm port (STM32F4)

#include "py/runtime.h"
#include "py/mphal.h"

#define I2C1_BASE 0x40005400
#define I2C2_BASE 0x40005800

#define RCC_APB1ENR (*(volatile uint32_t *)0x40023840)

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
} I2C_TypeDef;

#define I2C1 ((I2C_TypeDef *)I2C1_BASE)
#define I2C2 ((I2C_TypeDef *)I2C2_BASE)

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    I2C_TypeDef *i2c;
    uint8_t i2c_id;
} machine_i2c_obj_t;

static mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    machine_i2c_obj_t *self = mp_obj_malloc(machine_i2c_obj_t, type);
    self->i2c_id = mp_obj_get_int(args[0]);
    self->i2c = (self->i2c_id == 1) ? I2C1 : I2C2;

    // Enable I2C clock
    RCC_APB1ENR |= (self->i2c_id == 1) ? (1 << 21) : (1 << 22);

    // Configure I2C (100kHz standard mode, 42MHz APB1)
    self->i2c->CR1 = 0;
    self->i2c->CR2 = 42;  // APB1 freq in MHz
    self->i2c->CCR = 210; // 100kHz
    self->i2c->TRISE = 43;
    self->i2c->CR1 = (1 << 0);  // PE

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_i2c_scan(mp_obj_t self_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t list = mp_obj_new_list(0, NULL);

    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        // Send START
        self->i2c->CR1 |= (1 << 8);
        while (!(self->i2c->SR1 & (1 << 0)));  // Wait SB

        // Send address
        self->i2c->DR = (addr << 1);
        uint32_t timeout = 10000;
        while (timeout-- && !(self->i2c->SR1 & ((1 << 1) | (1 << 10))));

        if (self->i2c->SR1 & (1 << 1)) {  // ADDR set, device responded
            (void)self->i2c->SR2;  // Clear ADDR
            mp_obj_list_append(list, MP_OBJ_NEW_SMALL_INT(addr));
        }

        // Send STOP
        self->i2c->CR1 |= (1 << 9);
    }

    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_scan_obj, machine_i2c_scan);

static const mp_rom_map_elem_t machine_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&machine_i2c_scan_obj) },
};
static MP_DEFINE_CONST_DICT(machine_i2c_locals_dict, machine_i2c_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_make_new,
    locals_dict, &machine_i2c_locals_dict
);
