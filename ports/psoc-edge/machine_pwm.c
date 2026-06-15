/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "cy_gpio.h"
#include "cy_sysclk.h"
#include "cy_tcpwm_pwm.h"
#include "cycfg_peripherals.h"
#include "cycfg_peripheral_clocks.h"
#include "mphalport.h"
#include "machine_pin.h"
#include "machine_tcpwm.h"

typedef enum {
    VALUE_NOT_SET = -1,
    DUTY_NOT_SET = 0,
    DUTY_U16,
    DUTY_NS
} duty_type_t;

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;                     /**< MicroPython base object */
    cy_stc_tcpwm_pwm_config_t pwm_obj;      /**< PDL PWM configuration struct */
    const machine_pin_obj_t *pin;           /**< Resolved GPIO pin object for PWM output */
    const machine_pin_af_obj_t *pin_af;      /**< TCPWM alternate function selected for the output pin */
    uint32_t counter_num;                   /**< TCPWM0 counter index assigned to this PWM instance */
    en_clk_dst_t pclk_dst;                  /**< PCLK clock destination for this counter's clock assignment */
    uint32_t frequency;                     /**< PWM output frequency in Hz */
    duty_type_t duty_type;                  /**< Indicates whether duty is set as DUTY_U16 or DUTY_NS */
    mp_int_t duty;                          /**< Duty cycle value: 0-65535 if DUTY_U16, nanoseconds if DUTY_NS */
    bool invert;                            /**< If true, inverts the PWM output signal polarity */
} machine_pwm_obj_t;

// Sentinel value meaning "no trigger"; masked with 0x3U when assigned to 2-bit inputMode fields.
#define CYBSP_PWM_LED_CTRL_INPUT_DISABLED 0x7U

// TCPWM base clock: PCLK 100 MHz divided to 1 MHz for usable duty resolution at practical
// frequencies. Divider register value 99 = divisor 100: 100 MHz / 100 = 1 MHz.
// At 1 MHz a 32-bit counter gives period0 = 1,000,000 at 1 Hz and 1,000 at 1 kHz.
// Maximum PWM output frequency = PWM_TCPWM_CLK_HZ / 2 = 500 kHz (period0 = 2).
#define PWM_TCPWM_CLK_HZ        1000000UL
#define PWM_TCPWM_CLK_DIV_VAL   99U

#define pwm_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

/* Unit conversion macros */
 #define pwm_duty_cycle_ns_to_u16(duty_ns, freq)        ((int)(((float)(duty_ns * freq) / (float)1000000000) * (float)65536) - 1)
 #define pwm_duty_cycle_u16_to_ns(duty_u16, freq)       ((int)(((float)(duty_u16 + 1) / (float)65536) * ((float)1000000000 / (float)freq)))
 #define pwm_duty_cycle_u16_to_percent(duty_u16)        ((float)(duty_u16 + 1) * 100 / (float)65536)
 #define pwm_duty_cycle_ns_to_percent(duty_ns, freq)    ((float)((duty_ns) * 100) / (float)(1000000000 / freq))
 #define pwm_duty_cycle_percent_to_compare(duty_percent, period) ((int)(((float)duty_percent / (float)100) * (float)period))
 #define pwm_freq_to_period_us(freq)                    ((uint32_t)(1000000 / freq))
 #define pwm_period_ns_to_us(period_ns)                 ((uint32_t)(period_ns / 1000))
 #define pwm_duty_cycle_u16_to_compare(duty_u16, period) ((int)(((float)(duty_u16 + 1) / (float)65536) * (float)period))

static machine_pwm_obj_t *pwm_obj[MICROPY_PY_MACHINE_PWM_MAX_OBJS] = { NULL };
static bool pwm_clk_configured = false; // true after the shared divider is programmed to PWM_TCPWM_CLK_HZ

static const machine_pin_af_obj_t *pwm_pin_af_find(const machine_pin_obj_t *pin) {
    for (uint8_t i = 0; i < pin->af_num; i++) {
        const machine_pin_af_obj_t *af = &pin->af[i];
        if (af->signal == MACHINE_PIN_AF_SIGNAL_TCPWM_LINE) {
            return af;
        }
    }

    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin %q does not support PWM output"), pin->name);
}

// Search the instance pool for an existing PWM object on the given port/pin; returns NULL if not found.
static inline machine_pwm_obj_t *pwm_obj_find_by_pin(uint8_t port, uint8_t pin) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_PWM_MAX_OBJS; i++) {
        if (pwm_obj[i] != NULL && pwm_obj[i]->pin->port == port && pwm_obj[i]->pin->pin == pin) {
            return pwm_obj[i];
        }
    }
    return NULL;
}

// Allocate and return a new PWM object from the instance pool; returns NULL if the pool is full.
static inline machine_pwm_obj_t *pwm_obj_alloc(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_PWM_MAX_OBJS; i++) {
        if (pwm_obj[i] == NULL) {
            pwm_obj[i] = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
            return pwm_obj[i];
        }
    }
    return NULL;
}

// Release a PWM object slot back to the instance pool.
static inline void pwm_obj_free(machine_pwm_obj_t *pwm_obj_ptr) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_PWM_MAX_OBJS; i++) {
        if (pwm_obj[i] == pwm_obj_ptr) {
            pwm_obj[i] = NULL;
            return;
        }
    }
}

// Raise ValueError if duty_ns exceeds the period length for the given frequency.
static void pwm_duty_ns_assert(mp_int_t duty_ns, uint32_t freq) {
    if (duty_ns > (int)(1000000000 / freq)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM duty in ns is larger than the period %d ns"), (int)(1000000000 / freq));
    }
}

// Configure the GPIO pin to route the TCPWM output via HSIOM.
static void pwm_pin_config(machine_pwm_obj_t *self) {
    GPIO_PRT_Type *port = Cy_GPIO_PortToAddr(self->pin->port);
    Cy_GPIO_SetHSIOM(port, self->pin->pin, self->pin_af->idx);
    Cy_GPIO_SetDrivemode(port, self->pin->pin, CY_GPIO_DM_STRONG_IN_OFF);
}

// Restore the GPIO pin to plain GPIO mode (Hi-Z, no peripheral routing)
static void pwm_pin_restore(const machine_pin_obj_t *pin) {
    GPIO_PRT_Type *port = Cy_GPIO_PortToAddr(pin->port);
    Cy_GPIO_SetHSIOM(port, pin->pin, HSIOM_SEL_GPIO);
    Cy_GPIO_SetDrivemode(port, pin->pin, CY_GPIO_DM_HIGHZ);
}

// Reconfigure and restart the TCPWM counter using the current pwm_obj config struct.
static void pwm_restart(machine_pwm_obj_t *self) {
    Cy_TCPWM_PWM_Disable(TCPWM0, self->counter_num);
    cy_en_tcpwm_status_t result = Cy_TCPWM_PWM_Init(TCPWM0, self->counter_num, &self->pwm_obj);
    pwm_assert_raise_val("PWM init failed with return code %lx !", result);
    Cy_TCPWM_PWM_Enable(TCPWM0, self->counter_num);
    Cy_TCPWM_TriggerReloadOrIndex_Single(TCPWM0, self->counter_num);
}

// Compute period and compare register values from the stored frequency/duty and write to the PDL config struct.
static void pwm_config(machine_pwm_obj_t *self) {
    if (self->frequency == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("PWM frequency must be greater than 0"));
    }
    self->pwm_obj.period0 = PWM_TCPWM_CLK_HZ / self->frequency;
    self->pwm_obj.period1 = PWM_TCPWM_CLK_HZ / self->frequency;

    if (self->duty_type == DUTY_U16) {
        self->pwm_obj.compare0 = pwm_duty_cycle_u16_to_compare(self->duty, self->pwm_obj.period0);
        self->pwm_obj.compare1 = pwm_duty_cycle_u16_to_compare(self->duty, self->pwm_obj.period1);
    } else if (self->duty_type == DUTY_NS) {
        int duty_u16 = pwm_duty_cycle_ns_to_u16(self->duty, self->frequency);
        self->pwm_obj.compare0 = pwm_duty_cycle_u16_to_compare(duty_u16, self->pwm_obj.period0);
        self->pwm_obj.compare1 = pwm_duty_cycle_u16_to_compare(duty_u16, self->pwm_obj.period1);
    }

    self->pwm_obj.invertPWMOut = self->invert;
}

// Parse freq/duty_u16/duty_ns/invert args, configure the hardware, and start the PWM counter.
static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_invert};

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq,     MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_invert,   MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_freq].u_int <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("PWM frequency must be greater than 0"));
    }
    if ((uint32_t)args[ARG_freq].u_int > PWM_TCPWM_CLK_HZ / 2) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("PWM frequency must not exceed %lu Hz"), (unsigned long)(PWM_TCPWM_CLK_HZ / 2));
    }

    if (args[ARG_duty_u16].u_int != VALUE_NOT_SET &&
        args[ARG_duty_ns].u_int != VALUE_NOT_SET) {
        mp_raise_ValueError(MP_ERROR_TEXT("PWM duty should be specified only in one format"));
    }

    self->frequency = (uint32_t)args[ARG_freq].u_int;

    if (args[ARG_duty_u16].u_int != VALUE_NOT_SET) {
        self->duty = args[ARG_duty_u16].u_int > 65535 ? 65535 : args[ARG_duty_u16].u_int;
        self->duty_type = DUTY_U16;
    } else if (args[ARG_duty_ns].u_int != VALUE_NOT_SET) {
        pwm_duty_ns_assert(args[ARG_duty_ns].u_int, self->frequency);
        self->duty = args[ARG_duty_ns].u_int;
        self->duty_type = DUTY_NS;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("PWM duty should be specified in either ns or u16"));
    }

    self->invert = args[ARG_invert].u_bool;

    /* Apply frequency and duty cycle to the PWM config struct */
    pwm_config(self);

    /* Route the TCPWM output to the configured GPIO pin */
    pwm_pin_config(self);

    /* Program the shared 16-bit divider to PWM_TCPWM_CLK_HZ on the first PWM init, then
     * connect it to this counter's PCLK input. All channels share divider CYBSP_PWM_LED_CTRL_CLK_DIV_NUM
     * so the reprogramming only needs to happen once. */
    if (!pwm_clk_configured) {
        Cy_SysClk_PeriPclkDisableDivider((en_clk_dst_t)CYBSP_PWM_LED_CTRL_CLK_DIV_GRP_NUM,
            CY_SYSCLK_DIV_16_BIT, CYBSP_PWM_LED_CTRL_CLK_DIV_NUM);
        Cy_SysClk_PeriPclkSetDivider((en_clk_dst_t)CYBSP_PWM_LED_CTRL_CLK_DIV_GRP_NUM,
            CY_SYSCLK_DIV_16_BIT, CYBSP_PWM_LED_CTRL_CLK_DIV_NUM, PWM_TCPWM_CLK_DIV_VAL);
        Cy_SysClk_PeriPclkEnableDivider((en_clk_dst_t)CYBSP_PWM_LED_CTRL_CLK_DIV_GRP_NUM,
            CY_SYSCLK_DIV_16_BIT, CYBSP_PWM_LED_CTRL_CLK_DIV_NUM);
        pwm_clk_configured = true;
    }
    Cy_SysClk_PeriPclkAssignDivider(self->pclk_dst, CY_SYSCLK_DIV_16_BIT, CYBSP_PWM_LED_CTRL_CLK_DIV_NUM);

    cy_en_tcpwm_status_t result = Cy_TCPWM_PWM_Init(TCPWM0,
        self->counter_num, &self->pwm_obj);
    pwm_assert_raise_val("PWM init failed with return code %lx !", result);

    /* Enable the TCPWM block */
    Cy_TCPWM_PWM_Enable(TCPWM0, self->counter_num);

    /* Start the PWM */
    Cy_TCPWM_TriggerReloadOrIndex_Single(TCPWM0, self->counter_num);
}

// Print a human-readable representation: pin name, frequency, and duty cycle percentage.
static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(Pin.cpu.%q, freq=%u, duty=%.2f%%)",
        self->pin->name,
        self->frequency,
        self->duty_type == DUTY_U16 ? pwm_duty_cycle_u16_to_percent(self->duty) : pwm_duty_cycle_ns_to_percent(self->duty, self->frequency));
}

// Constructor: resolve the pin, allocate an instance, apply the default PDL config, then call init_helper.
static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {

    // Check number of arguments: dest is required
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Resolve dest to a pin object (accepts Pin object, board name string, or CPU name string)
    const machine_pin_obj_t *pin = machine_pin_get_pin_obj(args[0]);

    // Reject if a PWM instance already exists for this pin
    if (pwm_obj_find_by_pin(pin->port, pin->pin) != NULL) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("PWM instance for Pin %q already exists, call deinit() first"), pin->name);
    }

    const machine_pin_af_obj_t *pin_af = pwm_pin_af_find(pin);
    uint32_t counter_num = pin_af->unit;
    en_clk_dst_t pclk_dst = machine_tcpwm_counter_pclk(counter_num);

    // Allocate a new instance from the PWM object pool.
    machine_pwm_obj_t *self = pwm_obj_alloc();
    if (self == NULL) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM: maximum number of instances (%d) reached"), MICROPY_PY_MACHINE_PWM_MAX_OBJS);
    }

    self->pin = pin;
    self->pin_af = pin_af;
    self->counter_num = counter_num;
    self->pclk_dst = pclk_dst;

    // Default config parameters for the PWM object.
    self->pwm_obj.pwmMode = CY_TCPWM_PWM_MODE_PWM;
    self->pwm_obj.clockPrescaler = CY_TCPWM_PWM_PRESCALER_DIVBY_1;
    self->pwm_obj.pwmAlignment = CY_TCPWM_PWM_LEFT_ALIGN;
    self->pwm_obj.deadTimeClocks = 0U;
    self->pwm_obj.runMode = CY_TCPWM_PWM_CONTINUOUS;
    self->pwm_obj.enablePeriodSwap = false;
    self->pwm_obj.enableCompareSwap = false;
    self->pwm_obj.interruptSources = 0U;
    self->pwm_obj.invertPWMOutN = CY_TCPWM_PWM_INVERT_ENABLE;
    self->pwm_obj.killMode = CY_TCPWM_PWM_ASYNC_KILL;
    self->pwm_obj.swapInputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U;
    self->pwm_obj.swapInput = CY_TCPWM_INPUT_0;
    self->pwm_obj.reloadInputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U;
    self->pwm_obj.reloadInput = CY_TCPWM_INPUT_0;
    self->pwm_obj.startInputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U;
    self->pwm_obj.startInput = CY_TCPWM_INPUT_0;
    self->pwm_obj.killInputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U;
    self->pwm_obj.killInput = CY_TCPWM_INPUT_0;
    self->pwm_obj.countInputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U;
    self->pwm_obj.countInput = CY_TCPWM_INPUT_1;
    self->pwm_obj.swapOverflowUnderflow = false;
    self->pwm_obj.immediateKill = false;
    self->pwm_obj.tapsEnabled = CY_TCPWM_INPUT_0;
    self->pwm_obj.compare2 = CY_TCPWM_GRP_CNT_CC0_DEFAULT;
    self->pwm_obj.compare3 = CY_TCPWM_GRP_CNT_CC0_BUFF_DEFAULT;
    self->pwm_obj.enableCompare1Swap = false;
    self->pwm_obj.compare0MatchUp = true;
    self->pwm_obj.compare0MatchDown = false;
    self->pwm_obj.compare1MatchUp = true;
    self->pwm_obj.compare1MatchDown = false;
    self->pwm_obj.kill1InputMode = CYBSP_PWM_LED_CTRL_INPUT_DISABLED & 0x3U;
    self->pwm_obj.kill1Input = CY_TCPWM_INPUT_0;
    self->pwm_obj.pwmOnDisable = CY_TCPWM_PWM_OUTPUT_HIGHZ;
    self->pwm_obj.trigger0Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED;
    self->pwm_obj.trigger1Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED;
    self->pwm_obj.reloadLineSelect = false;
    self->pwm_obj.line_out_sel = CY_TCPWM_OUTPUT_PWM_SIGNAL;
    self->pwm_obj.linecompl_out_sel = CY_TCPWM_OUTPUT_INVERTED_PWM_SIGNAL;
    self->pwm_obj.line_out_sel_buff = CY_TCPWM_OUTPUT_PWM_SIGNAL;
    self->pwm_obj.linecompl_out_sel_buff = CY_TCPWM_OUTPUT_INVERTED_PWM_SIGNAL;
    self->pwm_obj.deadTimeClocks_linecompl_out = 0U;

    // Process the remaining parameters (skip dest at args[0]).
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    // If allocation or init raises, free the counter and pool slot so they can be reused.
    nlr_buf_t nl;
    if (nlr_push(&nl) == 0) {
        machine_tcpwm_counter_alloc(self->counter_num, MP_OBJ_FROM_PTR(self));
        mp_machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);
        nlr_pop();
    } else {
        machine_tcpwm_counter_free(self->counter_num, MP_OBJ_FROM_PTR(self));
        pwm_pin_restore(self->pin);
        pwm_obj_free(self);
        nlr_jump(nl.ret_val);
    }

    return MP_OBJ_FROM_PTR(self);
}

// Disable the TCPWM counter, restore the GPIO pin to Hi-Z, and free the instance slot.
static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    Cy_TCPWM_PWM_Disable(TCPWM0, self->counter_num);
    pwm_pin_restore(self->pin);
    machine_tcpwm_counter_free(self->counter_num, MP_OBJ_FROM_PTR(self));
    pwm_obj_free(self);
}

// Return the current PWM output frequency in Hz.
static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->frequency);
}

// Update the PWM output frequency; recalculates the period register.
static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    if (freq <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("PWM frequency must be greater than 0"));
    }
    if ((uint32_t)freq > PWM_TCPWM_CLK_HZ / 2) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("PWM frequency must not exceed %lu Hz"), (unsigned long)(PWM_TCPWM_CLK_HZ / 2));
    }
    if (self->duty_type == DUTY_NS) {
        pwm_duty_ns_assert(self->duty, (uint32_t)freq);
    }

    // set the frequency
    self->frequency = (uint32_t)freq;

    /* Apply frequency and duty cycle to the PWM config struct */
    pwm_config(self);

    // Reconfigure and restart the TCPWM counter with the new config.
    pwm_restart(self);
}

// Return the current duty cycle as a 16-bit value (0-65535).
static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    if (self->duty_type == DUTY_U16) {
        return MP_OBJ_NEW_SMALL_INT(self->duty);
    }
    // Convert stored duty_ns to u16
    return MP_OBJ_NEW_SMALL_INT(pwm_duty_cycle_ns_to_u16(self->duty, self->frequency));
}

// Set the duty cycle from a 16-bit value (0-65535).
static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    self->duty = duty_u16 > 65535 ? 65535 : duty_u16;
    self->duty_type = DUTY_U16;

    pwm_config(self);
    pwm_restart(self);
}

// Return the current on-time duty cycle in nanoseconds.
static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    if (self->duty_type == DUTY_NS) {
        return MP_OBJ_NEW_SMALL_INT(self->duty);
    }
    // Convert stored duty_u16 to nanoseconds
    return MP_OBJ_NEW_SMALL_INT(pwm_duty_cycle_u16_to_ns(self->duty, self->frequency));
}

// Set the duty cycle as an on-time in nanoseconds.
static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    pwm_duty_ns_assert(duty_ns, self->frequency);

    self->duty = duty_ns;
    self->duty_type = DUTY_NS;

    pwm_config(self);
    pwm_restart(self);
}
