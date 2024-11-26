#include "py/mphal.h"
#include "py/runtime.h"
#include "stm32f4xx.h"

// 自定义的 gpio_write 函数：设置 GPIOA 的某个引脚为 HIGH 或 LOW
static mp_obj_t gpioa_write(mp_obj_t pin_obj, mp_obj_t value_obj) {
    uint32_t pin = mp_obj_get_int(pin_obj);      // 获取引脚号
    uint32_t value = mp_obj_get_int(value_obj);  // 获取值（0 或 1）
    GPIOA->BSRR = (value ? (1 << pin) : (1 << (pin + 16)));
    return mp_const_none;  // 返回 None，表示函数执行完毕
}

static mp_obj_t gpiob_write(mp_obj_t pin_obj, mp_obj_t value_obj) {
    uint32_t pin = mp_obj_get_int(pin_obj);      // 获取引脚号
    uint32_t value = mp_obj_get_int(value_obj);  // 获取值（0 或 1）
    GPIOB->BSRR = (value ? (1 << pin) : (1 << (pin + 16)));
    return mp_const_none;  // 返回 None，表示函数执行完毕
}

static mp_obj_t gpioc_write(mp_obj_t pin_obj, mp_obj_t value_obj) {
    uint32_t pin = mp_obj_get_int(pin_obj);      // 获取引脚号
    uint32_t value = mp_obj_get_int(value_obj);  // 获取值（0 或 1）
    GPIOC->BSRR = (value ? (1 << pin) : (1 << (pin + 16)));
    return mp_const_none;  // 返回 None，表示函数执行完毕
}


// 使用宏定义函数对象
static MP_DEFINE_CONST_FUN_OBJ_2(gpioa_write_obj, gpioa_write);
static MP_DEFINE_CONST_FUN_OBJ_2(gpiob_write_obj, gpiob_write);
static MP_DEFINE_CONST_FUN_OBJ_2(gpioc_write_obj, gpioc_write);

// 定义模块全局方法表，包含我们新增的 gpio_write 函数
static const mp_rom_map_elem_t mymodule_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_gpioa_write), MP_ROM_PTR(&gpioa_write_obj) },  
    { MP_ROM_QSTR(MP_QSTR_gpiob_write), MP_ROM_PTR(&gpiob_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_gpioc_write), MP_ROM_PTR(&gpioc_write_obj) },
};

// 定义模块全局字典
static MP_DEFINE_CONST_DICT(mymodule_globals, mymodule_globals_table);

// 定义模块对象
const mp_obj_module_t mymodule_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mymodule_globals,
};

// 注册模块
MP_REGISTER_MODULE(MP_QSTR_mymodule, mymodule_module);
