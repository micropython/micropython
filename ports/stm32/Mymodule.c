//#include "py/obj.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "stm32f4xx.h"

// 自定义的 gpio_toggle 函数：翻转 GPIOA 的某个引脚
STATIC mp_obj_t gpio_toggle(mp_obj_t pin_obj) {
    uint32_t pin = mp_obj_get_int(pin_obj);
    GPIOA->ODR ^= (1 << pin);  // 直接操作寄存器翻转 GPIOA 引脚
    return mp_const_none;
}

// 自定义的 gpio_write 函数：设置 GPIOA 的某个引脚为 HIGH 或 LOW
STATIC mp_obj_t gpio_write(mp_obj_t pin_obj, mp_obj_t value_obj) {
    uint32_t pin = mp_obj_get_int(pin_obj);  // 获取引脚号
    uint32_t value = mp_obj_get_int(value_obj);  // 获取值（0 或 1）

    if (value) {
        GPIOA->ODR |= (1 << pin);  // 设置引脚为高电平
    } else {
        GPIOA->ODR &= ~(1 << pin);  // 设置引脚为低电平
    }
    return mp_const_none;  // 返回 None，表示函数执行完毕
}

// 使用宏定义函数对象
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_toggle_obj, gpio_toggle);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(gpio_write_obj, gpio_write);

// 定义模块全局方法表，包含我们新增的 gpio_write 函数
STATIC const mp_rom_map_elem_t mymodule_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_gpio_toggle), MP_ROM_PTR(&gpio_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_gpio_write), MP_ROM_PTR(&gpio_write_obj) },  // 新增的 gpio_write 函数
};

// 定义模块全局字典
STATIC MP_DEFINE_CONST_DICT(mymodule_globals, mymodule_globals_table);

// 定义模块对象
const mp_obj_module_t mymodule_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mymodule_globals,
};

// 注册模块
MP_REGISTER_MODULE(MP_QSTR_mymodule, mymodule_module);
