#include <stdio.h>

#include "stm32f4xx_hal.h"

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "servo.h"

// this servo driver uses hardware PWM to drive servos on PA0, PA1, PA2, PA3 = X1, X2, X3, X4
// TIM2 and TIM5 have CH1, CH2, CH3, CH4 on PA0-PA3 respectively
// they are both 32-bit counters with 16-bit prescaler
// we use TIM2

STATIC TIM_HandleTypeDef servo_TimHandle;

void servo_init(void) {
    // TIM2 clock enable
    __TIM2_CLK_ENABLE();

    // PWM clock configuration
    servo_TimHandle.Instance = TIM2;
    servo_TimHandle.Init.Period = 2000; // timer cycles at 50Hz
    servo_TimHandle.Init.Prescaler = ((SystemCoreClock / 2) / 100000) - 1; // timer runs at 100kHz
    servo_TimHandle.Init.ClockDivision = 0;
    servo_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init(&servo_TimHandle);
}

STATIC void servo_init_channel(int channel_in) {
    uint32_t pin;
    uint32_t channel;
    switch (channel_in) {
        case 1: pin = GPIO_PIN_0; channel = TIM_CHANNEL_1; break;
        case 2: pin = GPIO_PIN_1; channel = TIM_CHANNEL_2; break;
        case 3: pin = GPIO_PIN_2; channel = TIM_CHANNEL_3; break;
        case 4: pin = GPIO_PIN_3; channel = TIM_CHANNEL_4; break;
        default: return;
    }

    // GPIO configuration
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = pin;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PWM mode configuration
    TIM_OC_InitTypeDef oc_init;
    oc_init.OCMode = TIM_OCMODE_PWM1;
    oc_init.Pulse = 150; // units of 10us
    oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_init.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&servo_TimHandle, &oc_init, channel);

    // start PWM
    HAL_TIM_PWM_Start(&servo_TimHandle, channel);
}

/******************************************************************************/
// Micro Python bindings

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

STATIC const mp_method_t servo_methods[] = {
    { "angle", &servo_obj_angle_obj },
    { NULL, NULL },
};

STATIC const mp_obj_type_t servo_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_Servo,
    .print = servo_obj_print,
    .methods = servo_methods,
};

STATIC mp_obj_t pyb_Servo(mp_obj_t servo_id) {
    pyb_servo_obj_t *o = m_new_obj(pyb_servo_obj_t);
    o->base.type = &servo_obj_type;
    o->servo_id = mp_obj_get_int(servo_id);
    servo_init_channel(o->servo_id);
    return o;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_Servo_obj, pyb_Servo);
