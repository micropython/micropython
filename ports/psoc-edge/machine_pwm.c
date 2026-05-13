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
#include "machine_pin.h"

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
    uint32_t counter_num;                   /**< TCPWM0 counter index assigned to this PWM instance */
    en_clk_dst_t pclk_dst;                 /**< PCLK clock destination for this counter's clock assignment */
    uint32_t frequency;                     /**< PWM output frequency in Hz */
    duty_type_t duty_type;                  /**< Indicates whether duty is set as DUTY_U16 or DUTY_NS */
    mp_int_t duty;                          /**< Duty cycle value: 0-65535 if DUTY_U16, nanoseconds if DUTY_NS */
    bool invert;                            /**< If true, inverts the PWM output signal polarity */
} machine_pwm_obj_t;

/** Input trigger mode value meaning "disabled" - passed to inputMode fields (masked with 0x3U to extract the 2-bit mode) */
#define CYBSP_PWM_LED_CTRL_INPUT_DISABLED 0x7U

// TCPWM clock = PCLK (100 MHz) / (divider+1) = 100,000,000 / 50,000 = 2000 Hz
#define CYBSP_PWM_LED_CTRL_CLK_HZ   2000UL

// Maps (port, pin) → TCPWM0 counter number for pins that support HSIOM_SEL_ACT_1 routing.
// Only pins with a unique counter number can generate an independent PWM frequency.
// Pins sharing counter 0 (e.g. P20_5, P17_0, P16_0) cannot have different frequencies simultaneously.
static const struct {
    uint8_t port;
    uint8_t pin;
    uint32_t counter_num;
    en_clk_dst_t pclk_dst;     // PCLK clock destination for Cy_SysClk_PeriPclkAssignDivider
} pwm_pin_map[] = {
    // {9,  1, 0, PCLK_TCPWM0_CLOCK_COUNTER_EN0},  // P9_1  - LINE0       (shares counter 0)
    // {9,  3, 0, PCLK_TCPWM0_CLOCK_COUNTER_EN0},  // P9_3  - LINE_COMPL0 (shares counter 0)
    // {14, 3, 0, PCLK_TCPWM0_CLOCK_COUNTER_EN0},  // P14_3 - LINE0       (shares counter 0)
    // {14, 4, 0, PCLK_TCPWM0_CLOCK_COUNTER_EN0},  // P14_4 - LINE_COMPL0 (shares counter 0)
    {16, 0, 0, PCLK_TCPWM0_CLOCK_COUNTER_EN0},  // P16_0 - LINE0  (counter 0)
    {16, 1, 1, PCLK_TCPWM0_CLOCK_COUNTER_EN1},  // P16_1 - LINE1  (counter 1)
    {16, 2, 2, PCLK_TCPWM0_CLOCK_COUNTER_EN2},  // P16_2 - LINE2  (counter 2)
    {16, 3, 3, PCLK_TCPWM0_CLOCK_COUNTER_EN3},  // P16_3 - LINE3  (counter 3)
    {16, 4, 4, PCLK_TCPWM0_CLOCK_COUNTER_EN4},  // P16_4 - LINE4  (counter 4)
    {16, 5, 5, PCLK_TCPWM0_CLOCK_COUNTER_EN5},  // P16_5 - LINE5  (counter 5)
    {16, 6, 6, PCLK_TCPWM0_CLOCK_COUNTER_EN6},  // P16_6 - LINE6  (counter 6)
    {16, 7, 7, PCLK_TCPWM0_CLOCK_COUNTER_EN7},  // P16_7 - LINE7  (counter 7)
    // {17, 0, 0, PCLK_TCPWM0_CLOCK_COUNTER_EN0},  // P17_0 - LINE_COMPL0 (shares counter 0)
    // {20, 5, 0, PCLK_TCPWM0_CLOCK_COUNTER_EN0},  // P20_5 - LINE_COMPL0 (shares counter 0)
};

// Look up the pin→counter mapping. Returns false if the pin is not PWM-capable.
static bool pwm_pin_get_counter(uint8_t port, uint8_t pin, uint32_t *counter_num, en_clk_dst_t *pclk_dst) {
    for (uint8_t i = 0; i < MP_ARRAY_SIZE(pwm_pin_map); i++) {
        if (pwm_pin_map[i].port == port && pwm_pin_map[i].pin == pin) {
            *counter_num = pwm_pin_map[i].counter_num;
            *pclk_dst = pwm_pin_map[i].pclk_dst;
            return true;
        }
    }
    return false;
}

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

static inline machine_pwm_obj_t *pwm_obj_find_by_pin(uint8_t port, uint8_t pin) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_PWM_MAX_OBJS; i++) {
        if (pwm_obj[i] != NULL && pwm_obj[i]->pin->port == port && pwm_obj[i]->pin->pin == pin) {
            return pwm_obj[i];
        }
    }
    return NULL;
}

static inline machine_pwm_obj_t *pwm_obj_alloc() {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_PWM_MAX_OBJS; i++) {
        if (pwm_obj[i] == NULL) {
            pwm_obj[i] = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
            return pwm_obj[i];
        }
    }
    return NULL;
}

static inline void pwm_obj_free(machine_pwm_obj_t *pwm_obj_ptr) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_PWM_MAX_OBJS; i++) {
        if (pwm_obj[i] == pwm_obj_ptr) {
            pwm_obj[i] = NULL;
        }
    }
}

static void pwm_duty_ns_assert(mp_int_t duty_ns, uint32_t freq) {
    if (duty_ns > (int)(1000000000 / freq)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM duty in ns is larger than the period %d ns"), (int)(1000000000 / freq));
    }
}

// Configure the GPIO pin to route the TCPWM output via HSIOM (HSIOM_SEL_ACT_1 = 9 for all TCPWM0 outputs)
static void pwm_pin_config(const machine_pin_obj_t *pin) {
    GPIO_PRT_Type *port = Cy_GPIO_PortToAddr(pin->port);
    Cy_GPIO_SetHSIOM(port, pin->pin, HSIOM_SEL_ACT_1);
    Cy_GPIO_SetDrivemode(port, pin->pin, CY_GPIO_DM_STRONG_IN_OFF);
}

// Restore the GPIO pin to plain GPIO mode (Hi-Z, no peripheral routing)
static void pwm_pin_restore(const machine_pin_obj_t *pin) {
    GPIO_PRT_Type *port = Cy_GPIO_PortToAddr(pin->port);
    Cy_GPIO_SetHSIOM(port, pin->pin, HSIOM_SEL_GPIO);    
    Cy_GPIO_SetDrivemode(port, pin->pin, CY_GPIO_DM_HIGHZ);
}

// methods for machine.PWM
static void pwm_config(machine_pwm_obj_t *self) {

    if (self->frequency == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("PWM frequency must be greater than 0"));
    }
    self->pwm_obj.period0 = CYBSP_PWM_LED_CTRL_CLK_HZ / self->frequency;
    self->pwm_obj.period1 = CYBSP_PWM_LED_CTRL_CLK_HZ / self->frequency;
    
    if (self->duty_type == DUTY_U16) {
        self->pwm_obj.compare0 = pwm_duty_cycle_u16_to_compare(self->duty, self->pwm_obj.period0);
        self->pwm_obj.compare1 = pwm_duty_cycle_u16_to_compare(self->duty, self->pwm_obj.period1);
    } else if (self->duty_type == DUTY_NS) {
        self->pwm_obj.compare0 = pwm_duty_cycle_ns_to_u16(self->duty, self->frequency);
        self->pwm_obj.compare1 = pwm_duty_cycle_ns_to_u16(self->duty, self->frequency);
    }

    self->pwm_obj.invertPWMOut = self->invert;
}

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_invert};

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq,     MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_invert,   MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_freq].u_int != VALUE_NOT_SET) {
        self->frequency = args[ARG_freq].u_int;
    }

    if (args[ARG_invert].u_bool) {
        self->invert = true;
    }

    if (args[ARG_duty_u16].u_int != VALUE_NOT_SET &&
        args[ARG_duty_ns].u_int != VALUE_NOT_SET) {
        mp_raise_ValueError(MP_ERROR_TEXT("PWM duty should be specified only in one format"));
    }

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
    
    /* Apply frequency and duty cycle to the PWM config struct */
    pwm_config(self);

    /* Route the TCPWM output to the configured GPIO pin */
    pwm_pin_config(self->pin);

    /* Assign the pre-configured 2000 Hz clock divider (16-bit div #3, peri group 1)
     * to this counter's PCLK destination so it runs at the same clock as counter 0 */
    Cy_SysClk_PeriPclkAssignDivider(self->pclk_dst, CY_SYSCLK_DIV_16_BIT, CYBSP_PWM_LED_CTRL_CLK_DIV_NUM);

    cy_rslt_t result = Cy_TCPWM_PWM_Init(TCPWM0,
            self->counter_num, &self->pwm_obj);
    pwm_assert_raise_val("PWM init failed with return code %lx !", result);

    /* Enable the TCPWM block */
    Cy_TCPWM_PWM_Enable(TCPWM0, self->counter_num);

    /* Start the PWM */
    Cy_TCPWM_TriggerReloadOrIndex_Single(TCPWM0, self->counter_num);
}

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(Pin.cpu.%q, freq=%u, duty=%.2f%%)",
        self->pin->name,
        self->frequency,
        self->duty_type == DUTY_U16 ? pwm_duty_cycle_u16_to_percent(self->duty) : pwm_duty_cycle_ns_to_percent(self->duty, self->frequency));
}

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

    // Get static peripheral object.
    machine_pwm_obj_t *self = pwm_obj_alloc();
    if (self == NULL) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM: maximum number of instances (%d) reached"), MICROPY_PY_MACHINE_PWM_MAX_OBJS);
    }

    self->pin = pin;

    // Look up the TCPWM counter and PCLK destination for this pin
    if (!pwm_pin_get_counter(self->pin->port, self->pin->pin, &self->counter_num, &self->pclk_dst)) {
        pwm_obj_free(self);
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin %q does not support PWM output"), self->pin->name);
    }

    // Default config parameters for the PWM object.
    self->pwm_obj.pwmMode = CY_TCPWM_PWM_MODE_PWM;
    self->pwm_obj.clockPrescaler = CY_TCPWM_PWM_PRESCALER_DIVBY_1;
    self->pwm_obj.pwmAlignment = CY_TCPWM_PWM_LEFT_ALIGN;  
    self->pwm_obj.deadTimeClocks = 0;
    self->pwm_obj.runMode = CY_TCPWM_PWM_CONTINUOUS;
    self->pwm_obj.enablePeriodSwap = false;
    self->pwm_obj.enableCompareSwap = false;
    self->pwm_obj.interruptSources = (CY_TCPWM_INT_ON_TC & 0U) | (CY_TCPWM_INT_ON_CC0 & 0U) | (CY_TCPWM_INT_ON_CC1 & 0U);    
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
    self->pwm_obj.tapsEnabled = 45;
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
    self->pwm_obj.deadTimeClocks_linecompl_out = 0;

    // Process the remaining parameters (skip dest at args[0]).
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    Cy_TCPWM_PWM_Disable(TCPWM0, self->counter_num);
    pwm_pin_restore(self->pin);
    pwm_obj_free(self);
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("PWM not implemented"));
}


