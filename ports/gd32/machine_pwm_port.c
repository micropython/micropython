// machine_pwm_port.c - machine.PWM for GD32F4xx (timer output compare)
// Included by extmod/machine_pwm.c via MICROPY_PY_MACHINE_PWM_INCLUDEFILE.

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "gd32f4xx.h"
#include "pin_defs.h"

// ---- timer tables ----
typedef struct {
    uint32_t timer;
    uint32_t rcu;
    uint8_t  is_advanced;   // TIMER0/7 have complementary outputs
} pwm_timer_info_t;

static const pwm_timer_info_t pwm_timers[] = {
    { TIMER0, RCU_TIMER0, 1 },
    { TIMER1, RCU_TIMER1, 0 },
    { TIMER2, RCU_TIMER2, 0 },
    { TIMER3, RCU_TIMER3, 0 },
    { TIMER4, RCU_TIMER4, 0 },
    { TIMER7, RCU_TIMER7, 1 },
    { TIMER8, RCU_TIMER8, 0 },
    { TIMER9, RCU_TIMER9, 0 },
    { TIMER10, RCU_TIMER10, 0 },
    { TIMER11, RCU_TIMER11, 0 },
    { TIMER12, RCU_TIMER12, 0 },
    { TIMER13, RCU_TIMER13, 0 },
};

#define NUM_PWM_TIMERS MP_ARRAY_SIZE(pwm_timers)

// ---- pin -> (timer, channel, AF) mapping ----
// Only the most common GD32F4 PWM pins are mapped.
typedef struct {
    uint8_t pin_enc;
    uint8_t timer_idx;
    uint8_t channel;     // 0..3
    uint8_t af;
} pwm_pin_map_t;

static const pwm_pin_map_t pwm_pin_map[] = {
    // Pin encodings are (port << 4) | pin_num, so 0x00 = PA0, 0x18 = PB8, etc.
    // Alternate functions and channels below follow the GD32F4xx datasheet
    // "Alternate function mapping" table (AF1/AF2/AF3/AF9 per peripheral).

    // ---- TIMER0 (advanced) - AF1 ----
    { 0x00, 0, 0, 1 },   // PA0  TIMER0_CH0
    { 0x01, 0, 1, 1 },   // PA1  TIMER0_CH1
    { 0x02, 0, 2, 1 },   // PA2  TIMER0_CH2
    { 0x03, 0, 3, 1 },   // PA3  TIMER0_CH3
    { 0x08, 0, 0, 1 },   // PA8  TIMER0_CH0
    { 0x09, 0, 1, 1 },   // PA9  TIMER0_CH1
    { 0x0a, 0, 2, 1 },   // PA10 TIMER0_CH2
    { 0x0b, 0, 3, 1 },   // PA11 TIMER0_CH3

    // ---- TIMER1 - AF1 ----
    { 0x05, 1, 0, 1 },   // PA5  TIMER1_CH0
    { 0x0f, 1, 0, 1 },   // PA15 TIMER1_CH0
    { 0x18, 1, 0, 1 },   // PB8  TIMER1_CH0
    { 0x13, 1, 1, 1 },   // PB3  TIMER1_CH1
    { 0x19, 1, 1, 1 },   // PB9  TIMER1_CH1
    { 0x1a, 1, 2, 1 },   // PB10 TIMER1_CH2
    { 0x12, 1, 3, 1 },   // PB2  TIMER1_CH3
    { 0x1b, 1, 3, 1 },   // PB11 TIMER1_CH3

    // ---- TIMER2 - AF2 ----
    { 0x06, 2, 0, 2 },   // PA6  TIMER2_CH0
    { 0x14, 2, 0, 2 },   // PB4  TIMER2_CH0
    { 0x26, 2, 0, 2 },   // PC6  TIMER2_CH0
    { 0x07, 2, 1, 2 },   // PA7  TIMER2_CH1
    { 0x15, 2, 1, 2 },   // PB5  TIMER2_CH1
    { 0x27, 2, 1, 2 },   // PC7  TIMER2_CH1
    { 0x10, 2, 2, 2 },   // PB0  TIMER2_CH2
    { 0x28, 2, 2, 2 },   // PC8  TIMER2_CH2
    { 0x11, 2, 3, 2 },   // PB1  TIMER2_CH3
    { 0x29, 2, 3, 2 },   // PC9  TIMER2_CH3

    // ---- TIMER3 - AF2 ----
    { 0x16, 3, 0, 2 },   // PB6  TIMER3_CH0
    { 0x3c, 3, 0, 2 },   // PD12 TIMER3_CH0
    { 0x17, 3, 1, 2 },   // PB7  TIMER3_CH1
    { 0x3d, 3, 1, 2 },   // PD13 TIMER3_CH1
    { 0x3e, 3, 2, 2 },   // PD14 TIMER3_CH2
    { 0x3f, 3, 3, 2 },   // PD15 TIMER3_CH3

    // ---- TIMER7 (advanced) - AF3 ----
    { 0x26, 5, 0, 3 },   // PC6  TIMER7_CH0
    { 0x27, 5, 1, 3 },   // PC7  TIMER7_CH1
    { 0x28, 5, 2, 3 },   // PC8  TIMER7_CH2
    { 0x29, 5, 3, 3 },   // PC9  TIMER7_CH3

    // ---- TIMER8 - AF3 ----
    { 0x02, 6, 0, 3 },   // PA2  TIMER8_CH0
    { 0x03, 6, 1, 3 },   // PA3  TIMER8_CH1

    // ---- TIMER9 - AF3 ----
    { 0x18, 7, 0, 3 },   // PB8  TIMER9_CH0

    // ---- TIMER10 - AF3 ----
    { 0x19, 8, 0, 3 },   // PB9  TIMER10_CH0

    // ---- TIMER11 - AF9 ----
    { 0x1e, 9, 0, 9 },   // PB14 TIMER11_CH0
    { 0x1f, 9, 1, 9 },   // PB15 TIMER11_CH1

    // ---- TIMER12 - AF9 ----
    { 0x06, 10, 0, 9 },  // PA6  TIMER12_CH0

    // ---- TIMER13 - AF9 ----
    { 0x07, 11, 0, 9 },  // PA7  TIMER13_CH0
};

#define NUM_PWM_PINS MP_ARRAY_SIZE(pwm_pin_map)

// ---- object ----
typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    uint8_t  timer_idx;
    uint8_t  channel;
    bool     is_static;
    bool     invert;
    uint32_t period;       // ARR value
    uint32_t freq;
} machine_pwm_obj_t;

static machine_pwm_obj_t machine_pwm_obj[NUM_PWM_TIMERS * 4];
static uint8_t machine_pwm_used = 0;

static const pwm_pin_map_t *find_pwm_pin(uint8_t pin_enc) {
    for (size_t i = 0; i < NUM_PWM_PINS; i++) {
        if (pwm_pin_map[i].pin_enc == pin_enc) {
            return &pwm_pin_map[i];
        }
    }
    return NULL;
}

static uint32_t timer_channel_of(uint8_t ch) {
    switch (ch) {
        case 0: return TIMER_CH_0;
        case 1: return TIMER_CH_1;
        case 2: return TIMER_CH_2;
        default: return TIMER_CH_3;
    }
}

static uint32_t timer_clk_hz(uint8_t timer_idx) {
    // TIMER0/7 (APB2) vs TIMER1..13 (APB1). GD32 timer clocks are x2 the
    // APB frequency whenever the APB prescaler is not 1.
    if (timer_idx == 0 || timer_idx == 5) {   // TIMER0, TIMER7
        return rcu_clock_freq_get(CK_APB2) * 2;
    }
    return rcu_clock_freq_get(CK_APB1) * 2;
}

static void pwm_apply_freq(machine_pwm_obj_t *self, uint32_t freq) {
    const pwm_timer_info_t *info = &pwm_timers[self->timer_idx];
    uint32_t clk = timer_clk_hz(self->timer_idx);

    if (freq == 0) {
        freq = 1;
    }

    // Choose a prescaler so that period fits in 16 bits with reasonable
    // duty resolution.
    uint32_t psc = clk / (freq * 0x10000u);
    if (psc > 0xffff) {
        psc = 0xffff;
    }
    uint32_t period = (clk / (psc + 1)) / freq;
    if (period == 0) {
        period = 1;
    }
    if (period > 0xffff) {
        period = 0xffff;
    }

    self->period = period;
    self->freq = freq;

    timer_parameter_struct tp;
    timer_struct_para_init(&tp);
    tp.prescaler = (uint16_t)psc;
    tp.alignedmode = TIMER_COUNTER_EDGE;
    tp.counterdirection = TIMER_COUNTER_UP;
    tp.period = period;
    tp.clockdivision = TIMER_CKDIV_DIV1;
    tp.repetitioncounter = 0;
    timer_init(info->timer, &tp);

    // output compare config
    timer_oc_parameter_struct oc;
    timer_channel_output_struct_para_init(&oc);
    oc.outputstate = TIMER_CCX_ENABLE;
    oc.outputnstate = TIMER_CCXN_DISABLE;
    oc.ocpolarity = self->invert ? TIMER_OC_POLARITY_LOW : TIMER_OC_POLARITY_HIGH;
    oc.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    oc.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    oc.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    uint32_t ch = timer_channel_of(self->channel);
    timer_channel_output_config(info->timer, ch, &oc);
    timer_channel_output_mode_config(info->timer, ch, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(info->timer, ch, TIMER_OC_SHADOW_ENABLE);

    if (info->is_advanced) {
        timer_primary_output_config(info->timer, ENABLE);
    }
    timer_auto_reload_shadow_enable(info->timer);
    timer_enable(info->timer);
}

static void pwm_set_duty_u16(machine_pwm_obj_t *self, uint32_t duty) {
    const pwm_timer_info_t *info = &pwm_timers[self->timer_idx];
    if (duty > 0xffff) {
        duty = 0xffff;
    }
    uint32_t pulse = (uint32_t)(((uint64_t)duty * self->period) / 0xffffu);
    timer_channel_output_pulse_value_config(info->timer,
        timer_channel_of(self->channel), pulse);
}

// ---- extmod hooks ----
static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(timer=%u, channel=%u, freq=%u)",
        (unsigned)self->timer_idx, (unsigned)self->channel, (unsigned)self->freq);
}

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args,
                                       const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_invert };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_invert,   MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->invert = args[ARG_invert].u_bool;

    // configure the pin for AF
    const pwm_pin_map_t *map = NULL;
    for (size_t i = 0; i < NUM_PWM_PINS; i++) {
        if (pwm_pin_map[i].timer_idx == self->timer_idx &&
            pwm_pin_map[i].channel == self->channel) {
            map = &pwm_pin_map[i];
            break;
        }
    }

    pwm_apply_freq(self, (uint32_t)args[ARG_freq].u_int);

    if (map) {
        uint8_t port = PIN_PORT(map->pin_enc);
        uint32_t mask = pin_hal_mask(PIN_NUM(map->pin_enc));
        rcu_periph_clock_enable(pin_port_rcu(port));
        gpio_af_set(pin_port_base(port), map->af, mask);
        gpio_mode_set(pin_port_base(port), GPIO_MODE_AF, GPIO_PUPD_NONE, mask);
        gpio_output_options_set(pin_port_base(port), GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, mask);
    }

    if (args[ARG_duty_u16].u_int > 0) {
        pwm_set_duty_u16(self, (uint32_t)args[ARG_duty_u16].u_int);
    } else if (args[ARG_duty_ns].u_int > 0) {
        uint64_t ns = (uint64_t)args[ARG_duty_ns].u_int;
        uint64_t period_ns = 1000000000ull / self->freq;
        uint32_t duty16 = (uint32_t)((ns * 0xffffull) / period_ns);
        pwm_set_duty_u16(self, duty16);
    }
}

static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args,
                                        size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    mp_int_t pin_enc = mp_obj_get_int(args[0]);
    const pwm_pin_map_t *map = find_pwm_pin((uint8_t)pin_enc);
    if (map == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("pin has no PWM capability"));
    }
    if (machine_pwm_used >= MP_ARRAY_SIZE(machine_pwm_obj)) {
        mp_raise_ValueError(MP_ERROR_TEXT("too many PWM objects"));
    }

    machine_pwm_obj_t *self = &machine_pwm_obj[machine_pwm_used++];
    self->base.type = type;
    self->timer_idx = map->timer_idx;
    self->channel = map->channel;
    self->is_static = false;
    self->invert = false;

    const pwm_timer_info_t *info = &pwm_timers[self->timer_idx];
    rcu_periph_clock_enable(info->rcu);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    const pwm_timer_info_t *info = &pwm_timers[self->timer_idx];
    timer_channel_output_state_config(info->timer,
        timer_channel_of(self->channel), TIMER_CCX_DISABLE);
    timer_disable(info->timer);
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->freq);
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    pwm_apply_freq(self, (uint32_t)freq);
}

// GD32 standard peripheral library has no pulse-value getter, so read the
// CCV register directly.
static uint32_t timer_channel_compare_value(uint32_t timer, uint8_t ch) {
    switch (ch) {
        case 0: return (uint32_t)TIMER_CH0CV(timer);
        case 1: return (uint32_t)TIMER_CH1CV(timer);
        case 2: return (uint32_t)TIMER_CH2CV(timer);
        default: return (uint32_t)TIMER_CH3CV(timer);
    }
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    const pwm_timer_info_t *info = &pwm_timers[self->timer_idx];
    uint32_t pulse = timer_channel_compare_value(info->timer, self->channel);
    uint32_t duty = self->period ? (uint32_t)(((uint64_t)pulse * 0xffffu) / self->period) : 0;
    return MP_OBJ_NEW_SMALL_INT(duty);
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    pwm_set_duty_u16(self, (uint32_t)duty_u16);
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    uint32_t duty16 = (uint32_t)mp_obj_get_int(mp_machine_pwm_duty_get_u16(self));
    uint64_t period_ns = 1000000000ull / self->freq;
    return MP_OBJ_NEW_SMALL_INT((mp_int_t)((duty16 * period_ns) / 0xffffu));
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    uint64_t period_ns = 1000000000ull / self->freq;
    uint32_t duty16 = (uint32_t)(((uint64_t)duty_ns * 0xffffu) / period_ns);
    pwm_set_duty_u16(self, duty16);
}
