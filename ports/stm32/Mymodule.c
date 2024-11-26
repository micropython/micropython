#include "py/mphal.h"
#include "py/runtime.h"
#include "stm32f4xx.h"

// 定义 GPIO 端口常量，暴露给 Python 使用
static const mp_obj_t GPIOA_obj = (mp_obj_t)&GPIOA;
static const mp_obj_t GPIOB_obj = (mp_obj_t)&GPIOB;
static const mp_obj_t GPIOC_obj = (mp_obj_t)&GPIOC;

// 自定义的通用 gpio_write 函数：设置指定 GPIO 引脚为 HIGH 或 LOW
static mp_obj_t gpio_write(mp_obj_t gpio_obj, mp_obj_t pin_obj, mp_obj_t value_obj) {
    uint32_t pin = mp_obj_get_int(pin_obj);      // 获取引脚号
    uint32_t value = mp_obj_get_int(value_obj);  // 获取值（0 或 1）
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)mp_obj_get_int(gpio_obj); 
    gpio->BSRR = (value ? (1 << pin) : (1 << (pin + 16)));
    return mp_const_none;  // 返回 None，表示函数执行完毕
}

// 使用宏定义函数对象
static MP_DEFINE_CONST_FUN_OBJ_3(gpio_write_obj, gpio_write);

// 定义模块全局方法表，包含我们新增的 gpio_write 函数
static const mp_rom_map_elem_t mymodule_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_gpio_write), MP_ROM_PTR(&gpio_write_obj) },  
    { MP_ROM_QSTR(MP_QSTR_GPIOA), MP_ROM_PTR(&GPIOA_obj) },  // 注册 GPIOA
    { MP_ROM_QSTR(MP_QSTR_GPIOB), MP_ROM_PTR(&GPIOB_obj) },  // 注册 GPIOB
    { MP_ROM_QSTR(MP_QSTR_GPIOC), MP_ROM_PTR(&GPIOC_obj) },  // 注册 GPIOC
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
