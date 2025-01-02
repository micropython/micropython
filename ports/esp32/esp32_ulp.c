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

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3

#if CONFIG_IDF_TARGET_ULP_PREFER_FSM || CONFIG_IDF_TARGET_ESP32
    #define TYPE_RISCV 0
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    #define TYPE_RISCV 1
#endif

#if CONFIG_IDF_TARGET_ESP32
    #include "esp32/ulp.h"
#elif TYPE_RISCV
    #include "ulp_riscv.h"
#elif CONFIG_IDF_TARGET_ESP32S2
    #include "esp32s2/ulp.h"
#elif CONFIG_IDF_TARGET_ESP32S3
    #include "esp32s3/ulp.h"
#endif

#ifdef ULP_EMBEDDED_APP
    #include "ulp_embedded.h"
    extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_embedded_bin_start");
    extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_embedded_bin_end");
#endif

typedef struct _esp32_ulp_obj_t {
    mp_obj_base_t base;
} esp32_ulp_obj_t;

const mp_obj_type_t esp32_ulp_type;

// singleton ULP object
static const esp32_ulp_obj_t esp32_ulp_obj = {{&esp32_ulp_type}};

static mp_obj_t esp32_ulp_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    ulp_set_wakeup_period(0, 1000*10);//default wakeup 10ms
    #ifdef ULP_EMBEDDED_APP
    DEBUG_printf("embedded app size %d\n", (ulp_main_bin_end - ulp_main_bin_start));
    #endif
    // return constant object
    return (mp_obj_t)&esp32_ulp_obj;
}

static mp_obj_t esp32_ulp_set_wakeup_period(mp_obj_t self_in, mp_obj_t period_index_in, mp_obj_t period_us_in) {
    mp_uint_t period_index = mp_obj_get_int(period_index_in);
    mp_uint_t period_us = mp_obj_get_int(period_us_in);
    int _errno = ulp_set_wakeup_period(period_index, period_us);
    if (_errno != ESP_OK) {
        mp_raise_OSError(_errno);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(esp32_ulp_set_wakeup_period_obj, esp32_ulp_set_wakeup_period);

static mp_obj_t esp32_ulp_load_binary(mp_obj_t self_in, mp_obj_t load_addr_in, mp_obj_t program_binary_in) {
    mp_uint_t load_addr = mp_obj_get_int(load_addr_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(program_binary_in, &bufinfo, MP_BUFFER_READ);
    
    #if TYPE_RISCV
    int _errno = ulp_riscv_load_binary(bufinfo.buf, bufinfo.len / sizeof(uint32_t));
    #else
    int _errno = ulp_load_binary(load_addr, bufinfo.buf, bufinfo.len / sizeof(uint32_t));
    #endif    
    if (_errno != ESP_OK) {
        mp_raise_OSError(_errno);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(esp32_ulp_load_binary_obj, esp32_ulp_load_binary);

static mp_obj_t esp32_ulp_run(mp_obj_t self_in, mp_obj_t entry_point_in) {
    #if TYPE_RISCV
    int _errno = ulp_riscv_run();
    #else
    mp_uint_t entry_point = mp_obj_get_int(entry_point_in);
    int _errno = ulp_run(entry_point / sizeof(uint32_t));
    #endif
    if (_errno != ESP_OK) {
        mp_raise_OSError(_errno);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp32_ulp_run_obj, esp32_ulp_run);

#ifdef ULP_EMBEDDED_APP
static mp_obj_t esp32_ulp_load_and_run_embedded(mp_obj_t self_in) {
    int _errno;
    #if TYPE_RISCV
    _errno = ulp_riscv_load_binary(ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start));
    if (_errno != ESP_OK) mp_raise_OSError(_errno);

    _errno = ulp_riscv_run();
    if (_errno != ESP_OK) mp_raise_OSError(_errno);
    #else
    _errno = ulp_load_binary(0, ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start) / sizeof(uint32_t));
    if (_errno != ESP_OK) mp_raise_OSError(_errno);

    _errno = ulp_run(&ulp_entry - RTC_SLOW_MEM);
    if (_errno != ESP_OK) mp_raise_OSError(_errno);
    #endif
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ulp_load_and_run_embedded_obj, esp32_ulp_load_and_run_embedded);
#endif

static mp_obj_t esp32_ulp_pause(mp_obj_t self_in) {
    #if TYPE_RISCV
    ulp_riscv_timer_stop();
    #else
    ulp_timer_stop();
    #endif
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ulp_pause_obj, esp32_ulp_pause);

static mp_obj_t esp32_ulp_resume(mp_obj_t self_in) {
    #if TYPE_RISCV
    ulp_riscv_timer_resume();
    #else
    ulp_timer_resume();
    #endif
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_ulp_resume_obj, esp32_ulp_resume);

static const mp_rom_map_elem_t esp32_ulp_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_wakeup_period), MP_ROM_PTR(&esp32_ulp_set_wakeup_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_binary), MP_ROM_PTR(&esp32_ulp_load_binary_obj) },
    { MP_ROM_QSTR(MP_QSTR_run), MP_ROM_PTR(&esp32_ulp_run_obj) },
    #ifdef ULP_EMBEDDED_APP
    { MP_ROM_QSTR(MP_QSTR_run_embedded), MP_ROM_PTR(&esp32_ulp_load_and_run_embedded_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&esp32_ulp_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&esp32_ulp_resume_obj) },
    { MP_ROM_QSTR(MP_QSTR_RESERVE_MEM), MP_ROM_INT(CONFIG_ULP_COPROC_RESERVE_MEM) },
};
static MP_DEFINE_CONST_DICT(esp32_ulp_locals_dict, esp32_ulp_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_ulp_type,
    MP_QSTR_ULP,
    MP_TYPE_FLAG_NONE,
    make_new, esp32_ulp_make_new,
    locals_dict, &esp32_ulp_locals_dict
);

#endif //CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
