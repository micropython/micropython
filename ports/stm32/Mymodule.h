#ifndef MICROPY_INCLUDED_MYMODULE_H
#define MICROPY_INCLUDED_MYMODULE_H

#include "py/obj.h"

// 模块的函数声明
mp_obj_t gpio_toggle(mp_obj_t pin_obj);
mp_obj_t gpio_write(mp_obj_t pin_obj, mp_obj_t value_obj);

// 模块的函数对象声明
extern const mp_obj_fun_builtin_t gpio_toggle_obj;
extern const mp_obj_fun_builtin_t gpio_write_obj;

// 模块的全局字典声明
extern const mp_obj_module_t mymodule;

#endif // MICROPY_INCLUDED_MYMODULE_H
