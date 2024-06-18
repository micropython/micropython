/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 @bskp - https://github.com/bskp
 * Copyright (c) 2023 Ihor Nehrutsa
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

#include <stdio.h>

#define MP_DEBUG_PRINT_LEVEL 100
#include "py/mpprint.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "machine_pin.h"

#include "driver/mcpwm_prelude.h"
#include "mpconfigport.h"
#include "mphalport.h"
#include "esp_err.h"

#if 1 // MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

#if SOC_MCPWM_SUPPORTED

// #if SOC_TEMP_SENSOR_SUPPORTED

// static const char *TAG = "ESP32_mcpwm";
// static char *ERRMSG_FREQ = "PWM frequency too low";
// static char *ERRMSG_INIT = "PWM set-up failed";
static char *ERRMSG_VALUE = "value larger than period";

#define PWM_FULL_SCALE (65536UL)
#define VALUE_NOT_SET  (-1)

#define PWM_MIDDLE     (0)
#define PWM_BEGIN      (1)
#define PWM_END        (2)

#define BDC_MCPWM_TIMER_RESOLUTION_HZ (10 * 1000 * 1000) // 10000000 // 10MHz, 1 tick = 0.1us = 100ns
// #define BDC_MCPWM_FREQ_HZ             25000    // 25KHz PWM
// #define BDC_MCPWM_DUTY_TICK_MAX       (BDC_MCPWM_TIMER_RESOLUTION_HZ / BDC_MCPWM_FREQ_HZ) // maximum value we can set for the duty cycle, in ticks

#define NS_TO_TICKS(ns)  (((uint64_t)BDC_MCPWM_TIMER_RESOLUTION_HZ * ns + 500000000ULL) / 1000000000ULL)

#define MCPWM_CHANNEL_MAX (SOC_MCPWM_TIMERS_PER_GROUP * SOC_MCPWM_GROUPS)

typedef struct _mcpwm_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    gpio_num_t pwma_gpio_num;
    gpio_num_t pwmb_gpio_num;

    // Derived values for easier IDF API calls.
    int8_t group_id;

    uint32_t resolution_hz; // MCPWM timer resolution
    uint32_t pwm_freq_hz;
    uint32_t period_ticks; // maximum value we can set for the duty cycle, in ticks

    uint8_t complementary;
    uint8_t invert;
    bool sync;
    int32_t duty_u16;
    int32_t duty_ns;
    uint16_t center;
    uint32_t deadtime;
    uint32_t deadtime_ticks;
    uint8_t xor;
    int8_t action;

    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t operator;
    mcpwm_cmpr_handle_t cmpa;
    mcpwm_cmpr_handle_t cmpb;
    mcpwm_gen_handle_t gena;
    mcpwm_gen_handle_t genb;

} esp32_mcpwm_obj_t;

// Forward dec'l
extern const mp_obj_type_t esp32_mcpwm_type;

// Single Edge Asymmetric Waveform - Active High
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#single-edge-asymmetric-waveform-active-high
static void gen_action_config1(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_action_on_timer_event(gena,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW)));
    check_esp_err(mcpwm_generator_set_action_on_timer_event(genb,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(genb,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW)));
}

// Single Edge Asymmetric Waveform - Active Low
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#single-edge-asymmetric-waveform-active-low
static void gen_action_config2(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_action_on_timer_event(gena,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_timer_event(genb,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(genb,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_HIGH)));
}

// Pulse Placement Asymmetric Waveform
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#pulse-placement-asymmetric-waveform
static void gen_action_config3(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_actions_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_HIGH),
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW),
        MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    check_esp_err(mcpwm_generator_set_actions_on_timer_event(genb,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_TOGGLE),
        MCPWM_GEN_TIMER_EVENT_ACTION_END()));
}

// Dual Edge Asymmetric Waveform - Active Low
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#dual-edge-asymmetric-waveform-active-low
static void gen_action_config4(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_actions_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_HIGH),
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, cmpb, MCPWM_GEN_ACTION_LOW),
        MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    check_esp_err(mcpwm_generator_set_actions_on_timer_event(genb,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_LOW),
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_HIGH),
        MCPWM_GEN_TIMER_EVENT_ACTION_END()));
}

// Dual Edge Symmetric Waveform - Active Low
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#dual-edge-symmetric-waveform-active-low
static void gen_action_config5(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_actions_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_HIGH),
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, cmpa, MCPWM_GEN_ACTION_LOW),
        MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    check_esp_err(mcpwm_generator_set_actions_on_compare_event(genb,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_HIGH),
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, cmpb, MCPWM_GEN_ACTION_LOW),
        MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

// Dual Edge Symmetric Waveform - Complementary
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#dual-edge-symmetric-waveform-complementary
static void gen_action_config6(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_actions_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_HIGH),
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, cmpa, MCPWM_GEN_ACTION_LOW),
        MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    check_esp_err(mcpwm_generator_set_actions_on_compare_event(genb,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW),
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, cmpb, MCPWM_GEN_ACTION_HIGH),
        MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

// Dead Time Configurations for Classical PWM Waveforms
// Active High Complementary
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#active-high-complementary
static void gen_action_config7(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_action_on_timer_event(gena,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW)));
}
// +
static void dead_time_config7(esp32_mcpwm_obj_t *self, mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb) {
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = self->deadtime_ticks,
        .negedge_delay_ticks = 0,
    };
    check_esp_err(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    dead_time_config.posedge_delay_ticks = 0;
    dead_time_config.negedge_delay_ticks = self->deadtime_ticks;
    dead_time_config.flags.invert_output = true;
    check_esp_err(mcpwm_generator_set_dead_time(gena, genb, &dead_time_config));
}

// Active Low, Complementary
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#active-low-complementary
static void gen_action_config8(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_action_on_timer_event(gena,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW)));
}
// +
static void dead_time_config8(esp32_mcpwm_obj_t *self, mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb) {
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = self->deadtime_ticks,
        .negedge_delay_ticks = 0,
        .flags.invert_output = true,
    };
    check_esp_err(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    dead_time_config.posedge_delay_ticks = 0;
    dead_time_config.negedge_delay_ticks = self->deadtime_ticks;
    dead_time_config.flags.invert_output = false;
    check_esp_err(mcpwm_generator_set_dead_time(gena, genb, &dead_time_config));
}

// Active High
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#active-high
static void gen_action_config9(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_action_on_timer_event(gena,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW)));
}
// +
static void dead_time_config9(esp32_mcpwm_obj_t *self, mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb) {
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = self->deadtime_ticks,
        .negedge_delay_ticks = 0,
    };
    check_esp_err(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    dead_time_config.posedge_delay_ticks = 0;
    dead_time_config.negedge_delay_ticks = self->deadtime_ticks;
    check_esp_err(mcpwm_generator_set_dead_time(gena, genb, &dead_time_config));
}

// Active Low
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#active-low
static void gen_action_config10(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_action_on_timer_event(gena,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW)));
}
// +
static void dead_time_config10(esp32_mcpwm_obj_t *self, mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb) {
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = self->deadtime_ticks,
        .negedge_delay_ticks = 0,
        .flags.invert_output = true,
    };
    check_esp_err(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    dead_time_config.posedge_delay_ticks = 0;
    dead_time_config.negedge_delay_ticks = self->deadtime_ticks;
    check_esp_err(mcpwm_generator_set_dead_time(gena, genb, &dead_time_config));
}

// Rising Delay on PWMA and Bypass Dead Time for PWMB
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#rising-delay-on-pwma-and-bypass-dead-time-for-pwmb
static void gen_action_config11(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_action_on_timer_event(gena,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW)));
    check_esp_err(mcpwm_generator_set_action_on_timer_event(genb,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(genb,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW)));
}
// +
static void dead_time_config11(esp32_mcpwm_obj_t *self, mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb) {
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = self->deadtime_ticks,
        .negedge_delay_ticks = 0,
    };
    // apply deadtime to generator_a
    check_esp_err(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    // bypass deadtime module for generator_b
    dead_time_config.posedge_delay_ticks = 0;
    check_esp_err(mcpwm_generator_set_dead_time(genb, genb, &dead_time_config));
}

// Falling Delay on PWMB and Bypass Dead Time for PWMA
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#falling-delay-on-pwmb-and-bypass-dead-time-for-pwma
static void gen_action_config12(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_action_on_timer_event(gena,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW)));
    check_esp_err(mcpwm_generator_set_action_on_timer_event(genb,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(genb,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW)));
}
// +
static void dead_time_config12(esp32_mcpwm_obj_t *self, mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb) {
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = 0,
        .negedge_delay_ticks = 0,
    };
    // generator_a bypass the deadtime module (no delay)
    check_esp_err(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    // apply dead time to generator_b
    dead_time_config.negedge_delay_ticks = self->deadtime_ticks;
    check_esp_err(mcpwm_generator_set_dead_time(genb, genb, &dead_time_config));

}

// Rising and Falling Delay on PWMB and Bypass Dead Time for PWMA
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#rising-and-falling-delay-on-pwmb-and-bypass-dead-time-for-pwma
static void gen_action_config13(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_action_on_timer_event(gena,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW)));
    check_esp_err(mcpwm_generator_set_action_on_timer_event(genb,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(genb,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW)));
}
// +
static void dead_time_config13(esp32_mcpwm_obj_t *self, mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb) {
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = 0,
        .negedge_delay_ticks = 0,
    };
    // generator_a bypass the deadtime module (no delay)
    check_esp_err(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    // apply dead time on both edge for generator_b
    dead_time_config.negedge_delay_ticks = self->deadtime_ticks;
    dead_time_config.posedge_delay_ticks = self->deadtime_ticks;
    check_esp_err(mcpwm_generator_set_dead_time(genb, genb, &dead_time_config));
}

// https://github.com/espressif/esp-idf/blob/3b748a6cb76c2db7c6368a0dea32a88bc58bc44d/examples/peripherals/mcpwm/mcpwm_bldc_hall_control/main/mcpwm_bldc_hall_control_example_main.c#L263
static void gen_action_config14(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb) {
    check_esp_err(mcpwm_generator_set_action_on_timer_event(gena,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(gena,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW)));
    check_esp_err(mcpwm_generator_set_action_on_brake_event(gena,
        MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_LOW)));
    check_esp_err(mcpwm_generator_set_action_on_brake_event(gena,
        MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_LOW)));

    check_esp_err(mcpwm_generator_set_action_on_timer_event(genb,
        MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    check_esp_err(mcpwm_generator_set_action_on_compare_event(genb,
        MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW)));
    check_esp_err(mcpwm_generator_set_action_on_brake_event(genb,
        MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_LOW)));
    check_esp_err(mcpwm_generator_set_action_on_brake_event(genb,
        MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_LOW)));
}
// +
static void dead_time_config14(esp32_mcpwm_obj_t *self, mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb) {
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = self->deadtime_ticks,
        .negedge_delay_ticks = 0,
        .flags.invert_output = false,
    };
    check_esp_err(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    dead_time_config.posedge_delay_ticks = 0;
    dead_time_config.negedge_delay_ticks = self->deadtime_ticks;
    dead_time_config.flags.invert_output = true;
    check_esp_err(mcpwm_generator_set_dead_time(genb, genb, &dead_time_config));
}

static void esp32_mcpwm_motor_force_10(esp32_mcpwm_obj_t *self) {
    // because gen_low is inverted by dead time module, so we need to set force level to 1 and get 0
    check_esp_err(mcpwm_generator_set_force_level(self->genb, 1, true));
    check_esp_err(mcpwm_generator_set_force_level(self->gena, -1, true));
}

static void esp32_mcpwm_motor_force0_1(esp32_mcpwm_obj_t *self) {
    check_esp_err(mcpwm_generator_set_force_level(self->gena, 0, true));
    check_esp_err(mcpwm_generator_set_force_level(self->genb, -1, true));
}
/*
static void esp32_mcpwm_motor_coast(esp32_mcpwm_obj_t *self) {
    check_esp_err(mcpwm_generator_set_force_level(self->gena, 0, true));
    check_esp_err(mcpwm_generator_set_force_level(self->genb, 0, true));
}

static void esp32_mcpwm_motor_brake(esp32_mcpwm_obj_t *self) {
    check_esp_err(mcpwm_generator_set_force_level(self->gena, 1, true));
    check_esp_err(mcpwm_generator_set_force_level(self->genb, 1, true));
}
*/
static void esp32_mcpwm_motor_force00(esp32_mcpwm_obj_t *self) {
    check_esp_err(mcpwm_generator_set_force_level(self->gena, 0, true));
    // because gen_low is inverted by dead time module, so we need to set force level to 1 and get 0
    check_esp_err(mcpwm_generator_set_force_level(self->genb, 1, true));
}

static void esp32_mcpwm_motor_force10(esp32_mcpwm_obj_t *self) {
    check_esp_err(mcpwm_generator_set_force_level(self->gena, 1, true));
    // because gen_low is inverted by dead time module, so we need to set force level to 1 and get 0
    check_esp_err(mcpwm_generator_set_force_level(self->genb, 1, true));
}

static void esp32_mcpwm_motor_force01(esp32_mcpwm_obj_t *self) {
    check_esp_err(mcpwm_generator_set_force_level(self->gena, 0, true));
    // because gen_low is inverted by dead time module, so we need to set force level to 0 and get 1
    check_esp_err(mcpwm_generator_set_force_level(self->genb, 0, true));
}

static void esp32_mcpwm_motor_force_1_1(esp32_mcpwm_obj_t *self) {
    check_esp_err(mcpwm_generator_set_force_level(self->gena, -1, true));
    check_esp_err(mcpwm_generator_set_force_level(self->genb, -1, true));
}

static mp_obj_t mcpwm_force00(mp_obj_t self) {
    esp32_mcpwm_motor_force00(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_force00_obj, mcpwm_force00);

static mp_obj_t mcpwm_force10(mp_obj_t self) {
    esp32_mcpwm_motor_force10(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_force10_obj, mcpwm_force10);

static mp_obj_t mcpwm_force01(mp_obj_t self) {
    esp32_mcpwm_motor_force01(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_force01_obj, mcpwm_force01);

static mp_obj_t mcpwm_force_1_1(mp_obj_t self) {
    esp32_mcpwm_motor_force_1_1(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_force_1_1_obj, mcpwm_force_1_1);

static mp_obj_t mcpwm_force_10(mp_obj_t self) {
    esp32_mcpwm_motor_force_10(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_force_10_obj, mcpwm_force_10);

static mp_obj_t mcpwm_force0_1(mp_obj_t self) {
    esp32_mcpwm_motor_force0_1(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_force0_1_obj, mcpwm_force0_1);
/*
static mp_obj_t mcpwm_coast(mp_obj_t self) {
    esp32_mcpwm_motor_coast(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_coast_obj, mcpwm_coast);

static mp_obj_t mcpwm_brake(mp_obj_t self) {
    esp32_mcpwm_motor_brake(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_brake_obj, mcpwm_brake);
*/
static void esp32_mcpwm_set_compare_value(esp32_mcpwm_obj_t *self, uint32_t compare_value) {
    check_esp_err(mcpwm_comparator_set_compare_value(self->cmpa, compare_value));
    check_esp_err(mcpwm_comparator_set_compare_value(self->cmpb, compare_value));
}

static void esp32_mcpwm_enable(esp32_mcpwm_obj_t *self) {
    check_esp_err(mcpwm_timer_enable(self->timer));
    check_esp_err(mcpwm_timer_start_stop(self->timer, MCPWM_TIMER_START_NO_STOP));
}

static void esp32_mcpwm_disable(esp32_mcpwm_obj_t *self) {
    check_esp_err(mcpwm_timer_start_stop(self->timer, MCPWM_TIMER_STOP_EMPTY));
    check_esp_err(mcpwm_timer_disable(self->timer));
}

static mp_obj_t mcpwm_resume(mp_obj_t self) {
    esp32_mcpwm_enable(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_resume_obj, mcpwm_resume);

static mp_obj_t mcpwm_pause(mp_obj_t self) {
    esp32_mcpwm_disable(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_pause_obj, mcpwm_pause);

// FREQ

void mcpwm_set_frequency(esp32_mcpwm_obj_t *self, uint32_t frequency) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("splitting with sub-captures"));
}

uint32_t mcpwm_get_frequency(esp32_mcpwm_obj_t *self) {
    return self->pwm_freq_hz;
}

static mp_obj_t mcpwm_freq(size_t n_args, const mp_obj_t *args) {
    esp32_mcpwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(mcpwm_get_frequency(self));
    }
    mcpwm_set_frequency(self, mp_obj_get_int(args[1]));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mcpwm_freq_obj, 1, 2, mcpwm_freq);

// DUTY

// Utility functions for decoding and converting
static uint32_t duty_ns_to_duty_u16(uint32_t freq, uint32_t duty_ns) {
    uint64_t duty_u16 = ((uint64_t)duty_ns * freq * PWM_FULL_SCALE + 500000000ULL) / 1000000000ULL;
    if (duty_u16 > PWM_FULL_SCALE) {
        mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_VALUE));
    }
    return (uint32_t)duty_u16;
}

static uint32_t duty_u16_to_duty_ns(uint32_t freq, uint32_t duty_u16) {
    uint64_t duty_ns = 1000000000ULL * duty_u16 / ((uint64_t)freq * PWM_FULL_SCALE);
    return (uint32_t)duty_ns;
}

static void mcpwm_set_duty(esp32_mcpwm_obj_t *self) {
    uint32_t compare_value = (self->duty_u16 * self->period_ticks + PWM_FULL_SCALE / 2) / PWM_FULL_SCALE;
    esp32_mcpwm_set_compare_value(self, compare_value);
}

int32_t mcpwm_get_duty(esp32_mcpwm_obj_t *self) {
    return self->duty_ns != VALUE_NOT_SET ? self->duty_ns : self->duty_u16;
}

static mp_obj_t mcpwm_duty_u16(size_t n_args, const mp_obj_t *args) {
    esp32_mcpwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(self->duty_u16);
    }
    self->duty_ns = VALUE_NOT_SET;
    self->duty_u16 = mp_obj_get_int(args[1]);
    mcpwm_set_duty(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mcpwm_duty_u16_obj, 1, 2, mcpwm_duty_u16);

static mp_obj_t mcpwm_duty_ns(size_t n_args, const mp_obj_t *args) {
    esp32_mcpwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        if (self->duty_ns != VALUE_NOT_SET) {
            return MP_OBJ_NEW_SMALL_INT(self->duty_ns);
        } else {
            return MP_OBJ_NEW_SMALL_INT(duty_u16_to_duty_ns(self->pwm_freq_hz, self->duty_u16));
        }
    }
    self->duty_ns = mp_obj_get_int(args[1]);
    self->duty_u16 = duty_ns_to_duty_u16(self->pwm_freq_hz, self->duty_ns);
    mcpwm_set_duty(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mcpwm_duty_ns_obj, 1, 2, mcpwm_duty_ns);

static void configure_mcpwm(esp32_mcpwm_obj_t *self) {
    MP_DEBUG_PRINT(6, "configure_mcpwm() group_id=%d resolution_hz=%d period_ticks=%d action=%d", self->group_id, self->resolution_hz, self->period_ticks, self->action);
    DEBUG_printf("configure_mcpwm() group_id=%d resolution_hz=%d period_ticks=%d action=%d\n", self->group_id, self->resolution_hz, self->period_ticks, self->action);

    mcpwm_timer_config_t timer_config = {
        .group_id = self->group_id,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = self->resolution_hz,
        .period_ticks = self->period_ticks,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    check_esp_err(mcpwm_new_timer(&timer_config, &self->timer));

    mcpwm_operator_config_t operator_config = {
        .group_id = self->group_id,
    };
    check_esp_err(mcpwm_new_operator(&operator_config, &self->operator));

    check_esp_err(mcpwm_operator_connect_timer(self->operator, self->timer));

    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    check_esp_err(mcpwm_new_comparator(self->operator, &comparator_config, &self->cmpa));
    check_esp_err(mcpwm_new_comparator(self->operator, &comparator_config, &self->cmpb));

    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = self->pwma_gpio_num,
    };
    check_esp_err(mcpwm_new_generator(self->operator, &generator_config, &self->gena));
    generator_config.gen_gpio_num = self->pwmb_gpio_num;
    check_esp_err(mcpwm_new_generator(self->operator, &generator_config, &self->genb));

    switch (self->action)
    {
        case 1:
            gen_action_config1(self->gena, self->genb, self->cmpa, self->cmpb);
            break;
        case 2:
            gen_action_config2(self->gena, self->genb, self->cmpa, self->cmpb);
            break;
        case 3:
            gen_action_config3(self->gena, self->genb, self->cmpa, self->cmpb);
            break;
        case 4:
            gen_action_config4(self->gena, self->genb, self->cmpa, self->cmpb);
            break;
        case 5:
            gen_action_config5(self->gena, self->genb, self->cmpa, self->cmpb);
            break;
        case 6:
            gen_action_config6(self->gena, self->genb, self->cmpa, self->cmpb);
            break;
        case 7:
            gen_action_config7(self->gena, self->genb, self->cmpa, self->cmpb);
            dead_time_config7(self, self->gena, self->genb);
            break;
        case 8:
            gen_action_config8(self->gena, self->genb, self->cmpa, self->cmpb);
            dead_time_config8(self, self->gena, self->genb);
            break;
        case 9:
            gen_action_config9(self->gena, self->genb, self->cmpa, self->cmpb);
            dead_time_config9(self, self->gena, self->genb);
            break;
        case 10:
            gen_action_config10(self->gena, self->genb, self->cmpa, self->cmpb);
            dead_time_config10(self, self->gena, self->genb);
            break;
        case 11:
            gen_action_config11(self->gena, self->genb, self->cmpa, self->cmpb);
            dead_time_config11(self, self->gena, self->genb);
            break;
        case 12:
            gen_action_config12(self->gena, self->genb, self->cmpa, self->cmpb);
            dead_time_config12(self, self->gena, self->genb);
            break;
        case 13:
            gen_action_config13(self->gena, self->genb, self->cmpa, self->cmpb);
            dead_time_config13(self, self->gena, self->genb);
            break;
        case 14:
            gen_action_config14(self->gena, self->genb, self->cmpa, self->cmpb);
            dead_time_config14(self, self->gena, self->genb);
            break;
        default:
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("out of MCPWM action range [1-13]:%d"), self->action);
            break;
    }
    mcpwm_set_duty(self);
    esp32_mcpwm_enable(self);
}

static void mp_esp32_mcpwm_init_helper(esp32_mcpwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    MP_DEBUG_PRINT(6, "mp_esp32_mcpwm_init_helper() n_args=%d", n_args);
    enum { ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_center, ARG_align,
           ARG_invert, ARG_sync, ARG_xor, ARG_deadtime, ARG_waveform };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_u16, MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_ns, MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_center, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_align, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_sync, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_xor, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_deadtime, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_waveform, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 14}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if ((n_args + kw_args->used) > 0) {
        if (args[ARG_freq].u_int > 0) {
            self->pwm_freq_hz = args[ARG_freq].u_int;
        }
        self->period_ticks = (self->resolution_hz + self->pwm_freq_hz / 2) / self->pwm_freq_hz;

        // Set duty_u16 cycle?
        int32_t duty = args[ARG_duty_u16].u_int;
        if (duty >= 0) {
            if (duty > PWM_FULL_SCALE) {
                mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_VALUE));
            }
            self->duty_ns = VALUE_NOT_SET;
            self->duty_u16 = duty;
        }
        // Set duty_ns value?
        duty = args[ARG_duty_ns].u_int;
        if (duty >= 0) {
            self->duty_ns = duty;
            self->duty_u16 = duty_ns_to_duty_u16(self->pwm_freq_hz, self->duty_ns);
        }

        // Set center value?
        int32_t center = args[ARG_center].u_int;
        if (center >= 0) {
            if (center > PWM_FULL_SCALE) {
                mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_VALUE));
            }
            self->center = center;
        } else {  // Use alignment setting shortcut
            if (args[ARG_align].u_int >= 0) {
                uint8_t align = args[ARG_align].u_int & 3; // limit to 0..3
                if (align == PWM_BEGIN) {
                    self->center = self->duty_u16 / 2;
                } else if (align == PWM_END) {
                    self->center = PWM_FULL_SCALE - self->duty_u16 / 2;
                } else {
                    self->center = 32768; // Default value: mid.
                }
            }
        }

        if (args[ARG_invert].u_int >= 0) {
            self->invert = args[ARG_invert].u_int; // & (PWM_CHANNEL1 | PWM_CHANNEL2);
        }

        if (args[ARG_sync].u_int >= 0) {
            self->sync = args[ARG_sync].u_int != false; // && self->submodule != 0;
        }

        if (args[ARG_xor].u_int >= 0) {
            self->xor = args[ARG_xor].u_int & 0x03;
        }

        if (args[ARG_deadtime].u_int >= 0) {
            self->deadtime = args[ARG_deadtime].u_int;
            self->deadtime_ticks = NS_TO_TICKS(self->deadtime);
        }
        self->action = args[ARG_waveform].u_int;
        configure_mcpwm(self);
    }
}

static mp_obj_t mcpwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    MP_DEBUG_PRINT(6, "mcpwm_make_new() n_args=%d, n_kw=%d", n_args, n_kw);
    mp_arg_check_num(n_args, n_kw, 1, 3, true);

    int block_id = mp_obj_get_int(args[0]);
    if (block_id >= MCPWM_CHANNEL_MAX) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "id %d (0-5 allowed)", block_id));
    }

    // create PWM object from the given timer block
    esp32_mcpwm_obj_t *self = m_new_obj(esp32_mcpwm_obj_t);
    self->base.type = &esp32_mcpwm_type;
    self->id = block_id;
    self->group_id = self->id / SOC_MCPWM_TIMERS_PER_GROUP;
    self->resolution_hz = BDC_MCPWM_TIMER_RESOLUTION_HZ;
    self->pwm_freq_hz = 5000;
    self->duty_ns = VALUE_NOT_SET;
    self->duty_u16 = PWM_FULL_SCALE / 2;

    // Get referred Pin object(s)
    mp_obj_t *pins;
    if (mp_obj_is_type(args[1], &mp_type_tuple)) {
        mp_obj_get_array_fixed_n(args[1], 2, &pins);
        self->pwma_gpio_num = machine_pin_get_id(pins[0]);
        self->pwmb_gpio_num = machine_pin_get_id(pins[1]);
    } else {
        self->pwma_gpio_num = machine_pin_get_id(args[1]);
        self->pwmb_gpio_num = GPIO_NUM_NC;
    }
    MP_DEBUG_PRINT(3, "mcpwm_make_new() self->pwma_gpio_num=%d, self->pwmb_gpio_num=%d", self->pwma_gpio_num, self->pwmb_gpio_num);

    // Process the remaining parameters.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_esp32_mcpwm_init_helper(self, n_args - 2, args + 2, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

// BIND
static mp_obj_t _mcpwm_bind(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_complementary_pin };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin_complementary, MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    esp32_mcpwm_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    gpio_num_t pin_id = machine_pin_get_id(pos_args[1]);
    self->pwma_gpio_num = pin_id;
    self->pwmb_gpio_num = args[ARG_complementary_pin].u_int;

    configure_mcpwm(self);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(mcpwm_bind_obj, 1, _mcpwm_bind);

// DEINIT
static mp_obj_t mcpwm_deinit(mp_obj_t self_in) {
    esp32_mcpwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    esp32_mcpwm_disable(self);
    check_esp_err(mcpwm_generator_set_force_level(self->gena, 0, true));
    check_esp_err(mcpwm_generator_set_force_level(self->genb, 0, true));

    if (self->gena) {
        check_esp_err(mcpwm_del_generator(self->gena));
    }
    if (self->genb) {
        check_esp_err(mcpwm_del_generator(self->genb));
    }
    if (self->cmpa) {
        check_esp_err(mcpwm_del_comparator(self->cmpa));
    }
    if (self->cmpb) {
        check_esp_err(mcpwm_del_comparator(self->cmpb));
    }
    if (self->operator) {
        check_esp_err(mcpwm_del_operator(self->operator));
    }
    if (self->timer) {
        check_esp_err(mcpwm_del_timer(self->timer));
    }

    m_del_obj(esp32_mcpwm_obj_t, self); // ???
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcpwm_deinit_obj, mcpwm_deinit);

static void mcpwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_mcpwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "MCPWM(%u, ", self->id);
    if (self->pwmb_gpio_num >= 0) {
        mp_printf(print, "(Pin(%u), Pin(%u)", self->pwma_gpio_num, self->pwmb_gpio_num);
    } else {
        mp_printf(print, "Pin(%u", self->pwma_gpio_num);
    }
    int32_t freq = mcpwm_get_frequency(self);
    mp_printf(print, "), freq=%uHz", freq);
    int32_t duty = mcpwm_get_duty(self);
    if (self->duty_ns != VALUE_NOT_SET) {
        mp_printf(print, ", duty_ns=%d", duty);
    } else {
        mp_printf(print, ", duty_u16=%d", duty);
    }
    #if MICROPY_ERROR_REPORTING > MICROPY_ERROR_REPORTING_NORMAL
    mp_printf(print, ", deadtime=%uns=%dticks", self->deadtime, self->deadtime_ticks);
    mp_printf(print, ", resolution_hz=%d, period_ticks=%d action=%d", self->resolution_hz, self->period_ticks, self->action);
    #endif
    mp_printf(print, ")");
}

static const mp_rom_map_elem_t mcpwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_bind), MP_ROM_PTR(&mcpwm_bind_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mcpwm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty_u16), MP_ROM_PTR(&mcpwm_duty_u16_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty_ns), MP_ROM_PTR(&mcpwm_duty_ns_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&mcpwm_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&mcpwm_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&mcpwm_resume_obj) },
    { MP_ROM_QSTR(MP_QSTR_force_10), MP_ROM_PTR(&mcpwm_force_10_obj) },
    { MP_ROM_QSTR(MP_QSTR_force0_1), MP_ROM_PTR(&mcpwm_force0_1_obj) },
    /*
    { MP_ROM_QSTR(MP_QSTR_coast), MP_ROM_PTR(&mcpwm_coast_obj) },
    { MP_ROM_QSTR(MP_QSTR_brake), MP_ROM_PTR(&mcpwm_brake_obj) },
    */
    { MP_ROM_QSTR(MP_QSTR_force00), MP_ROM_PTR(&mcpwm_force00_obj) },
    { MP_ROM_QSTR(MP_QSTR_force10), MP_ROM_PTR(&mcpwm_force10_obj) },
    { MP_ROM_QSTR(MP_QSTR_force01), MP_ROM_PTR(&mcpwm_force01_obj) },
    { MP_ROM_QSTR(MP_QSTR_force_1_1), MP_ROM_PTR(&mcpwm_force_1_1_obj) },
};
static MP_DEFINE_CONST_DICT(mcpwm_locals_dict, mcpwm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_mcpwm_type,
    MP_QSTR_MCPWM,
    MP_TYPE_FLAG_NONE,
    print, mcpwm_print,
    make_new, mcpwm_make_new,
    locals_dict, &mcpwm_locals_dict
    );

#endif
