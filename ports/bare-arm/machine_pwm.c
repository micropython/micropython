/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
 */

// PWM implementation for bare-arm port (STM32F4)

#include "py/runtime.h"
#include "py/mphal.h"

#define TIM2_BASE 0x40000000
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
    volatile uint32_t reserved;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
} TIM_TypeDef;

#define TIM2 ((TIM_TypeDef *)TIM2_BASE)

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    uint8_t channel;  // 1-4
    uint16_t freq;
    uint16_t duty;
} machine_pwm_obj_t;

static mp_obj_t machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 3, false);

    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, type);
    self->channel = mp_obj_get_int(args[0]);
    self->freq = (n_args >= 2) ? mp_obj_get_int(args[1]) : 1000;
    self->duty = (n_args >= 3) ? mp_obj_get_int(args[2]) : 512;

    // Enable TIM2 clock
    RCC_APB1ENR |= (1 << 0);

    // Configure timer for PWM
    TIM2->PSC = 83;  // 84MHz / 84 = 1MHz
    TIM2->ARR = 1000000 / self->freq;

    // Configure channel for PWM mode 1
    uint8_t shift = ((self->channel - 1) % 2) * 8;
    if (self->channel <= 2) {
        TIM2->CCMR1 &= ~(0xFF << shift);
        TIM2->CCMR1 |= (0x68 << shift);  // PWM mode 1
    } else {
        TIM2->CCMR2 &= ~(0xFF << shift);
        TIM2->CCMR2 |= (0x68 << shift);
    }

    // Enable channel output
    TIM2->CCER |= (1 << ((self->channel - 1) * 4));

    // Set duty cycle
    volatile uint32_t *ccr = &TIM2->CCR1 + (self->channel - 1);
    *ccr = (TIM2->ARR * self->duty) / 1023;

    // Enable timer
    TIM2->CR1 |= (1 << 0);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_pwm_duty(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(self->duty);
    } else {
        self->duty = mp_obj_get_int(args[1]);
        volatile uint32_t *ccr = &TIM2->CCR1 + (self->channel - 1);
        *ccr = (TIM2->ARR * self->duty) / 1023;
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_duty_obj, 1, 2, machine_pwm_duty);

static const mp_rom_map_elem_t machine_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&machine_pwm_duty_obj) },
};
static MP_DEFINE_CONST_DICT(machine_pwm_locals_dict, machine_pwm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pwm_type,
    MP_QSTR_PWM,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pwm_make_new,
    locals_dict, &machine_pwm_locals_dict
);
