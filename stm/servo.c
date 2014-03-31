#include <stdio.h>
#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "servo.h"

// PWM
// TIM2 and TIM5 have CH1, CH2, CH3, CH4 on PA0-PA3 respectively
// they are both 32-bit counters
// 16-bit prescaler
// TIM2_CH3 also on PB10 (used below)
void servo_init(void) {
    // TIM2 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // for PB10
    /*
    // GPIOB Configuration: TIM2_CH3 (PB10)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Connect TIM2 pins to AF1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_TIM2);
    */

    // for PA0, PA1, PA2, PA3
    {
        // GPIOA Configuration: TIM2_CH0, TIM2_CH1 (PA0, PA1)
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        // Connect TIM2 pins to AF1
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM2);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM2);
    }

    // Compute the prescaler value so TIM2 runs at 100kHz
    uint16_t PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 100000) - 1;

    // Time base configuration
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 2000; // timer cycles at 50Hz
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // PWM Mode configuration
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 150; // units of 10us
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure); // channel 1
    TIM_OC2Init(TIM2, &TIM_OCInitStructure); // channel 2
    TIM_OC3Init(TIM2, &TIM_OCInitStructure); // channel 3
    TIM_OC4Init(TIM2, &TIM_OCInitStructure); // channel 4

    // ?
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable); // channel 1
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); // channel 2
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable); // channel 3
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); // channel 4

    // ?
    TIM_ARRPreloadConfig(TIM2, ENABLE);

    // TIM2 enable counter
    TIM_Cmd(TIM2, ENABLE);
}

/******************************************************************************/
/* Micro Python bindings                                                      */

STATIC mp_obj_t pyb_servo_set(mp_obj_t port, mp_obj_t value) {
    int p = mp_obj_get_int(port);
    int v = mp_obj_get_int(value);
    if (v < 50) { v = 50; }
    if (v > 250) { v = 250; }
    switch (p) {
        case 1: TIM2->CCR1 = v; break;
        case 2: TIM2->CCR2 = v; break;
        case 3: TIM2->CCR3 = v; break;
        case 4: TIM2->CCR4 = v; break;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_servo_set_obj, pyb_servo_set);

STATIC mp_obj_t pyb_pwm_set(mp_obj_t period, mp_obj_t pulse) {
    int pe = mp_obj_get_int(period);
    int pu = mp_obj_get_int(pulse);
    TIM2->ARR = pe;
    TIM2->CCR3 = pu;
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_pwm_set_obj, pyb_pwm_set);

typedef struct _pyb_servo_obj_t {
    mp_obj_base_t base;
    uint servo_id;
} pyb_servo_obj_t;

STATIC void servo_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_servo_obj_t *self = self_in;
    print(env, "<Servo %lu>", self->servo_id);
}

STATIC mp_obj_t servo_obj_angle(mp_obj_t self_in, mp_obj_t angle) {
    pyb_servo_obj_t *self = self_in;
#if MICROPY_ENABLE_FLOAT
    machine_int_t v = 152 + 85.0 * mp_obj_get_float(angle) / 90.0;
#else
    machine_int_t v = 152 + 85 * mp_obj_get_int(angle) / 90;
#endif
    if (v < 65) { v = 65; }
    if (v > 210) { v = 210; }
    switch (self->servo_id) {
        case 1: TIM2->CCR1 = v; break;
        case 2: TIM2->CCR2 = v; break;
        case 3: TIM2->CCR3 = v; break;
        case 4: TIM2->CCR4 = v; break;
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(servo_obj_angle_obj, servo_obj_angle);

STATIC const mp_map_elem_t servo_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_angle), (mp_obj_t)&servo_obj_angle_obj },
};

STATIC MP_DEFINE_CONST_DICT(servo_locals_dict, servo_locals_dict_table);

STATIC const mp_obj_type_t servo_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_Servo,
    .print = servo_obj_print,
    .locals_dict = (mp_obj_t)&servo_locals_dict,
};

STATIC mp_obj_t pyb_Servo(mp_obj_t servo_id) {
    pyb_servo_obj_t *o = m_new_obj(pyb_servo_obj_t);
    o->base.type = &servo_obj_type;
    o->servo_id = mp_obj_get_int(servo_id);
    return o;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_Servo_obj, pyb_Servo);
