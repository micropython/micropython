#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "map.h"

#include "gpio_api.h"
#include "hal.h"
#include "hal_gpio.h"

static mp_obj_t gpio_obj_mode(mp_obj_t self_in, mp_obj_t mode_obj) {
    hal_gpio_obj_t *self = self_in;
    int mode = mp_obj_get_int(mode_obj);
    gpio_mode(&self->gpio, mode);
    return mp_const_none;
}

static mp_obj_t gpio_obj_dir(mp_obj_t self_in, mp_obj_t dir_obj) {
    hal_gpio_obj_t *self = self_in;
    int dir = mp_obj_get_int(dir_obj);
    gpio_dir(&self->gpio, dir);
    return mp_const_none;
}

static mp_obj_t gpio_obj_read(mp_obj_t self_in) {
    hal_gpio_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(gpio_read(&self->gpio));
}

static mp_obj_t gpio_obj_write(mp_obj_t self_in, mp_obj_t value_obj) {
    hal_gpio_obj_t *self = self_in;
    int value = mp_obj_get_int(value_obj);
    gpio_dir(&self->gpio, value);
    return mp_const_none;
}

static void gpio_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    hal_gpio_obj_t *self = self_in;
    print(env, "<GPIO %u>", self->pin);
}

static MP_DEFINE_CONST_FUN_OBJ_2(gpio_obj_mode_obj,  gpio_obj_mode);
static MP_DEFINE_CONST_FUN_OBJ_2(gpio_obj_dir_obj,   gpio_obj_dir);
static MP_DEFINE_CONST_FUN_OBJ_1(gpio_obj_read_obj,  gpio_obj_read);
static MP_DEFINE_CONST_FUN_OBJ_2(gpio_obj_write_obj, gpio_obj_write);

static bool gpio_call_kwarg(mp_obj_t param, mp_obj_t key, mp_obj_t value) {
    hal_gpio_obj_t *self = param;
    mp_obj_t fn = rt_load_attr(self, mp_obj_str_get_qstr(key));
    if (fn != MP_OBJ_NULL) {
        rt_call_function_1(fn, value);
    }
    return true;
}

mp_obj_t hal_Gpio_init(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    hal_gpio_obj_t *self = args[0];
    int pin = mp_obj_get_int(args[1]);
    gpio_init(&self->gpio, pin, PIN_INPUT);
    mp_map_walk(kwargs, gpio_call_kwarg, self);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN_KW(hal_Gpio_init_obj, 2, 2, hal_Gpio_init);

void hal_init_Gpio_class(mp_obj_t module_obj) {
    mp_obj_t gpio_class_type = mp_obj_new_type("Gpio", mp_const_empty_tuple, mp_obj_new_dict(0));
    ((mp_obj_type_t *)gpio_class_type)->print = gpio_obj_print;
    rt_store_attr(gpio_class_type, QSTR_FROM_STR_STATIC("__init__"), (mp_obj_t)&hal_Gpio_init_obj);
    rt_store_attr(gpio_class_type, QSTR_FROM_STR_STATIC("dir"), (mp_obj_t)&gpio_obj_dir_obj);
    rt_store_attr(gpio_class_type, QSTR_FROM_STR_STATIC("mode"), (mp_obj_t)&gpio_obj_mode_obj);
    rt_store_attr(gpio_class_type, QSTR_FROM_STR_STATIC("read"), (mp_obj_t)&gpio_obj_read_obj);
    rt_store_attr(gpio_class_type, QSTR_FROM_STR_STATIC("write"), (mp_obj_t)&gpio_obj_write_obj);
    rt_store_attr(module_obj, QSTR_FROM_STR_STATIC("Gpio"), gpio_class_type);

    // Allow the target to add its own constants and methods
    hal_target_init_Gpio_class(gpio_class_type);
}
