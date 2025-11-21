/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
 */

// ADC implementation for bare-arm port (STM32F4)

#include "py/runtime.h"
#include "py/mphal.h"

#define ADC1_BASE 0x40012000
#define RCC_APB2ENR (*(volatile uint32_t *)0x40023844)

typedef struct {
    volatile uint32_t SR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMPR1;
    volatile uint32_t SMPR2;
    volatile uint32_t JOFR[4];
    volatile uint32_t HTR;
    volatile uint32_t LTR;
    volatile uint32_t SQR1;
    volatile uint32_t SQR2;
    volatile uint32_t SQR3;
    volatile uint32_t JSQR;
    volatile uint32_t JDR[4];
    volatile uint32_t DR;
} ADC_TypeDef;

#define ADC1 ((ADC_TypeDef *)ADC1_BASE)

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    uint8_t channel;
} machine_adc_obj_t;

static mp_obj_t machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    machine_adc_obj_t *self = mp_obj_malloc(machine_adc_obj_t, type);
    self->channel = mp_obj_get_int(args[0]);

    // Enable ADC1 clock
    RCC_APB2ENR |= (1 << 8);

    // Configure ADC
    ADC1->CR2 = (1 << 0);  // ADON
    ADC1->SQR1 = 0;  // 1 conversion
    ADC1->SQR3 = self->channel;  // Channel

    // Set sampling time (84 cycles for channel)
    if (self->channel < 10) {
        ADC1->SMPR2 |= (3 << (self->channel * 3));
    } else {
        ADC1->SMPR1 |= (3 << ((self->channel - 10) * 3));
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_adc_read(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Start conversion
    ADC1->CR2 |= (1 << 30);  // SWSTART

    // Wait for conversion
    while (!(ADC1->SR & (1 << 1)));  // EOC

    return MP_OBJ_NEW_SMALL_INT(ADC1->DR & 0xFFF);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_obj, machine_adc_read);

static const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&machine_adc_read_obj) },
};
static MP_DEFINE_CONST_DICT(machine_adc_locals_dict, machine_adc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_adc_type,
    MP_QSTR_ADC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_adc_make_new,
    locals_dict, &machine_adc_locals_dict
);
