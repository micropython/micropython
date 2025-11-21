/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
 */

// Timer implementation for bare-arm port (STM32F4)

#include "py/runtime.h"
#include "py/mphal.h"

#define TIM3_BASE 0x40000400
#define RCC_APB1ENR (*(volatile uint32_t *)0x40023840)

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
} TIM_TypeDef;

#define TIM3 ((TIM_TypeDef *)TIM3_BASE)

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    uint32_t period_ms;
} machine_timer_obj_t;

static mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    machine_timer_obj_t *self = mp_obj_malloc(machine_timer_obj_t, type);
    self->period_ms = (n_args >= 1) ? mp_obj_get_int(args[0]) : 1000;

    // Enable TIM3 clock
    RCC_APB1ENR |= (1 << 1);

    // Configure timer
    TIM3->PSC = 83999;  // 84MHz / 84000 = 1kHz
    TIM3->ARR = self->period_ms;
    TIM3->CR1 = (1 << 0);  // CEN

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_timer_value(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(TIM3->CNT);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_value_obj, machine_timer_value);

static const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_timer_value_obj) },
};
static MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, machine_timer_make_new,
    locals_dict, &machine_timer_locals_dict
);
