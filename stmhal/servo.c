#include <stdio.h>

#include "stm32f4xx_hal.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "runtime.h"
#include "servo.h"

// this servo driver uses hardware PWM to drive servos on PA0, PA1, PA2, PA3 = X1, X2, X3, X4
// TIM2 and TIM5 have CH1, CH2, CH3, CH4 on PA0-PA3 respectively
// they are both 32-bit counters with 16-bit prescaler
// we use TIM2

#define PYB_SERVO_NUM (4)

typedef struct _pyb_servo_obj_t {
    mp_obj_base_t base;
    uint16_t servo_id;
    uint16_t time_left;
    int16_t pulse_accum;
    uint16_t pulse_cur;
    uint16_t pulse_dest;
} pyb_servo_obj_t;

STATIC const mp_obj_type_t servo_obj_type;

STATIC pyb_servo_obj_t pyb_servo_obj[PYB_SERVO_NUM];

TIM_HandleTypeDef TIM2_Handle;

void servo_init(void) {
    // TIM2 clock enable
    __TIM2_CLK_ENABLE();

    // set up and enable interrupt
    HAL_NVIC_SetPriority(TIM2_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    // PWM clock configuration
    TIM2_Handle.Instance = TIM2;
    TIM2_Handle.Init.Period = 2000; // timer cycles at 50Hz
    TIM2_Handle.Init.Prescaler = ((SystemCoreClock / 2) / 100000) - 1; // timer runs at 100kHz
    TIM2_Handle.Init.ClockDivision = 0;
    TIM2_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init(&TIM2_Handle);

    // reset servo objects
    for (int i = 0; i < PYB_SERVO_NUM; i++) {
        pyb_servo_obj[i].base.type = &servo_obj_type;
        pyb_servo_obj[i].servo_id = i + 1;
        pyb_servo_obj[i].time_left = 0;
        pyb_servo_obj[i].pulse_cur = 150; // units of 10us
        pyb_servo_obj[i].pulse_dest = 0;
    }
}

void servo_timer_irq_callback(void) {
    bool need_it = false;
    for (int i = 0; i < PYB_SERVO_NUM; i++) {
        pyb_servo_obj_t *s = &pyb_servo_obj[i];
        if (s->pulse_cur != s->pulse_dest) {
            if (s->time_left <= 1) {
                s->pulse_cur = s->pulse_dest;
                s->time_left = 0;
            } else {
                s->pulse_accum += s->pulse_dest - s->pulse_cur;
                s->pulse_cur += s->pulse_accum / s->time_left;
                s->pulse_accum %= s->time_left;
                s->time_left--;
                need_it = true;
            }
            switch (s->servo_id) {
                case 1: TIM2->CCR1 = s->pulse_cur; break;
                case 2: TIM2->CCR2 = s->pulse_cur; break;
                case 3: TIM2->CCR3 = s->pulse_cur; break;
                case 4: TIM2->CCR4 = s->pulse_cur; break;
            }
        }
    }
    if (need_it) {
        __HAL_TIM_ENABLE_IT(&TIM2_Handle, TIM_IT_UPDATE);
    } else {
        __HAL_TIM_DISABLE_IT(&TIM2_Handle, TIM_IT_UPDATE);
    }
}

STATIC void servo_init_channel(pyb_servo_obj_t *s) {
    uint32_t pin;
    uint32_t channel;
    switch (s->servo_id) {
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
    oc_init.Pulse = s->pulse_cur; // units of 10us
    oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_init.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&TIM2_Handle, &oc_init, channel);

    // start PWM
    HAL_TIM_PWM_Start(&TIM2_Handle, channel);
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

STATIC void pyb_servo_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_servo_obj_t *self = self_in;
    print(env, "<Servo %lu at %lu>", self->servo_id, self->pulse_cur);
}

STATIC mp_obj_t pyb_servo_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    rt_check_nargs(n_args, 1, 1, n_kw, false);

    // get servo number
    machine_int_t servo_id = mp_obj_get_int(args[0]) - 1;

    // check servo number
    if (!(0 <= servo_id && servo_id < PYB_SERVO_NUM)) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Servo %d does not exist", servo_id));
    }

    // get and init servo object
    pyb_servo_obj_t *s = &pyb_servo_obj[servo_id];
    s->pulse_dest = s->pulse_cur;
    s->time_left = 0;
    servo_init_channel(s);

    return s;
}

STATIC mp_obj_t pyb_servo_angle(uint n_args, const mp_obj_t *args) {
    pyb_servo_obj_t *self = args[0];
    if (n_args == 1) {
        // get angle
        return mp_obj_new_int((self->pulse_cur - 152) * 90 / 85);
    } else {
#if MICROPY_ENABLE_FLOAT
        machine_int_t v = 152 + 85.0 * mp_obj_get_float(args[1]) / 90.0;
#else
        machine_int_t v = 152 + 85 * mp_obj_get_int(args[1]) / 90;
#endif
        if (v < 65) { v = 65; }
        if (v > 210) { v = 210; }
        self->pulse_dest = v;
        if (n_args == 2) {
            // set angle immediately
            self->time_left = 0;
        } else {
            // set angle over a given time (given in milli seconds)
            self->time_left = mp_obj_get_int(args[2]) / 20;
            self->pulse_accum = 0;
        }
        servo_timer_irq_callback();
        return mp_const_none;
    }
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_servo_angle_obj, 1, 3, pyb_servo_angle);

STATIC const mp_map_elem_t pyb_servo_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_angle), (mp_obj_t)&pyb_servo_angle_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_servo_locals_dict, pyb_servo_locals_dict_table);

const mp_obj_type_t pyb_servo_type = {
    { &mp_type_type },
    .name = MP_QSTR_Servo,
    .print = pyb_servo_print,
    .make_new = pyb_servo_make_new,
    .locals_dict = (mp_obj_t)&pyb_servo_locals_dict,
};
