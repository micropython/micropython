/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 "Andreas Valder" <andreas.valder@serioese.gmbh>
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
#include "hal/adc_types.h"
#include "driver/rtc_io.h"
#include "py/mphal.h"
#include "modesp32.h"

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3

#if ULP_TYPE_RISCV
#include "ulp_riscv.h"
#else
#include "ulp_common.h"
#include "ulp_fsm_common.h"
#endif
#include "ulp_adc.h"

typedef struct _esp32_ulp_obj_t {
    mp_obj_base_t base;
} esp32_ulp_obj_t;

const mp_obj_type_t esp32_ulp_type;

// singleton ULP object
static const esp32_ulp_obj_t esp32_ulp_obj = {{&esp32_ulp_type}};

static mp_obj_t esp32_ulp_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return constant object
    return (mp_obj_t)&esp32_ulp_obj;
}

static mp_obj_t esp32_ulp_set_wakeup_period(mp_obj_t self_in, mp_obj_t period_index_in, mp_obj_t period_us_in) {
    mp_uint_t period_index = 0;
    #if CONFIG_IDF_TARGET_ESP32
    // only the original ESP32 allows multiple periods to choose from
    period_index = mp_obj_get_int(period_index_in);
    #endif

    mp_uint_t period_us = mp_obj_get_int(period_us_in);
    int _errno = ulp_set_wakeup_period(period_index, period_us);
    if (_errno != ESP_OK) {
        mp_raise_OSError(_errno);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(esp32_ulp_set_wakeup_period_obj, esp32_ulp_set_wakeup_period);


#if ULP_TYPE_RISCV
static mp_obj_t esp32_ulp_load_binary(mp_obj_t self_in, mp_obj_t program_binary_in) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(program_binary_in, &bufinfo, MP_BUFFER_READ);

    ulp_riscv_timer_stop();
    ulp_riscv_halt();
    ulp_riscv_reset();

    // RISCV does not allow a custom entry point offset like the FSM did
    int _errno = ulp_riscv_load_binary(bufinfo.buf, bufinfo.len);
    if (_errno != ESP_OK) {
        mp_raise_OSError(_errno);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_ulp_load_binary_obj, esp32_ulp_load_binary);

static mp_obj_t esp32_ulp_run(mp_obj_t self_in) {

    int _errno = ulp_riscv_run();
    if (_errno != ESP_OK) {
        mp_raise_OSError(_errno);
    }
    ulp_riscv_timer_resume();

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ulp_run_obj, esp32_ulp_run);

#else

static mp_obj_t esp32_ulp_load_binary(mp_obj_t self_in, mp_obj_t load_addr_in, mp_obj_t program_binary_in) {
    mp_uint_t load_addr = mp_obj_get_int(load_addr_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(program_binary_in, &bufinfo, MP_BUFFER_READ);

    ulp_timer_stop();

    int _errno = ulp_load_binary(load_addr, bufinfo.buf, bufinfo.len / sizeof(uint32_t));
    if (_errno != ESP_OK) {
        mp_raise_OSError(_errno);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(esp32_ulp_load_binary_obj, esp32_ulp_load_binary);

static mp_obj_t esp32_ulp_run(mp_obj_t self_in, mp_obj_t entry_point_in) {
    mp_uint_t entry_point = mp_obj_get_int(entry_point_in);
    int _errno = ulp_run(entry_point / sizeof(uint32_t));
    if (_errno != ESP_OK) {
        mp_raise_OSError(_errno);
    }
    ulp_timer_resume();

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_ulp_run_obj, esp32_ulp_run);
#endif


static mp_obj_t esp32_ulp_pause(mp_obj_t self_in) {
    #if ULP_TYPE_RISCV
    ulp_riscv_timer_stop();
    ulp_riscv_halt();
    #else
    ulp_timer_stop();
    #endif
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ulp_pause_obj, esp32_ulp_pause);

static mp_obj_t esp32_ulp_resume(mp_obj_t self_in) {
    #if ULP_TYPE_RISCV
    ulp_riscv_timer_resume();
    #else
    ulp_timer_resume();
    #endif
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ulp_resume_obj, esp32_ulp_resume);


/*
RTC pin functionality, can be moved to Pin class if needed
*/

static mp_obj_t esp32_ulp_rtc_init(mp_obj_t self_in, mp_obj_t pin_in) {
    gpio_num_t gpio_id = mp_obj_get_int(pin_in);

    if (!rtc_gpio_is_valid_gpio(gpio_id)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }
    rtc_gpio_init(gpio_id);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_ulp_rtc_init_obj, esp32_ulp_rtc_init);


static mp_obj_t esp32_ulp_rtc_deinit(mp_obj_t self_in, mp_obj_t pin_in) {
    gpio_num_t gpio_id = mp_obj_get_int(pin_in);

    if (!rtc_gpio_is_valid_gpio(gpio_id)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }
    rtc_gpio_deinit(gpio_id);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_ulp_rtc_deinit_obj, esp32_ulp_rtc_deinit);


static mp_obj_t esp32_ulp_adc_init(mp_obj_t self_in, mp_obj_t channel_in) {
    int channel = mp_obj_get_int(channel_in);
    ulp_adc_cfg_t cfg = {
        .adc_n = ADC_UNIT_1,
        .channel = channel,
        #if CONFIG_IDF_TARGET_ESP32S2
        .width = ADC_BITWIDTH_13,
        #else
        .width = ADC_BITWIDTH_12,
        #endif
        .atten = ADC_ATTEN_DB_11,
        #if ULP_TYPE_RISCV
        .ulp_mode = ADC_ULP_MODE_RISCV,
        #else
        .ulp_mode = ADC_ULP_MODE_FSM,
        #endif
    };
    int _errno = ulp_adc_init(&cfg);
    if (_errno != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("ULP-ADC unit already in use or invalid channel"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_ulp_adc_init_obj, esp32_ulp_adc_init);

static mp_obj_t esp32_ulp_adc_deinit(mp_obj_t self_in) {
    int _errno = ulp_adc_deinit();
    if (_errno != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("ULP-ADC unit already de-initialised"));
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ulp_adc_deinit_obj, esp32_ulp_adc_deinit);

static const mp_rom_map_elem_t esp32_ulp_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_wakeup_period), MP_ROM_PTR(&esp32_ulp_set_wakeup_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_binary), MP_ROM_PTR(&esp32_ulp_load_binary_obj) },
    { MP_ROM_QSTR(MP_QSTR_run), MP_ROM_PTR(&esp32_ulp_run_obj) },
    { MP_ROM_QSTR(MP_QSTR_RESERVE_MEM), MP_ROM_INT(CONFIG_ULP_COPROC_RESERVE_MEM) },

    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&esp32_ulp_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&esp32_ulp_resume_obj) },

    { MP_ROM_QSTR(MP_QSTR_rtc_init), MP_ROM_PTR(&esp32_ulp_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_rtc_deinit), MP_ROM_PTR(&esp32_ulp_rtc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_adc_init), MP_ROM_PTR(&esp32_ulp_adc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_adc_deinit), MP_ROM_PTR(&esp32_ulp_adc_deinit_obj) },
};
static MP_DEFINE_CONST_DICT(esp32_ulp_locals_dict, esp32_ulp_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_ulp_type,
    #if ULP_TYPE_RISCV
    MP_QSTR_ULP_RV,
    #else
    MP_QSTR_ULP,
    #endif
    MP_TYPE_FLAG_NONE,
    make_new, esp32_ulp_make_new,
    locals_dict, &esp32_ulp_locals_dict
    );

#endif // CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
