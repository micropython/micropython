#include <stdint.h>

#include "mpconfigport.h"
#include "fsl_common.h"
#if MICROPY_USE_IMXRT1064_MCU || MICROPY_USE_IMXRT1060_MCU || MICROPY_USE_IMXRT1050_MCU || MICROPY_USE_IMXRT1020_MCU
#include "fsl_gpio.h"
#else
#include "gpio/gpio.h"
#endif
#include "py/obj.h"
#include "py/runtime.h"
#include "mpgpio.h"
#include "pybled.h"
#include "pybgpio.h"

typedef struct
{
    char* gpio_name;
    uint8_t pin_num;
}led_info_t;

typedef struct
{
    mp_obj_base_t base;
    uint8_t led_id;
    gpio_obj_t gpio;
}led_obj_t;

static void pyb_led_init_helper(led_obj_t* self_in)
{
#if MICROPY_USE_IMXRT1064_MCU || MICROPY_USE_IMXRT1060_MCU || MICROPY_USE_IMXRT1050_MCU || MICROPY_USE_IMXRT1020_MCU
    gpio_pin_config_t config;
    config.direction = kGPIO_DigitalOutput;
    config.outputLogic = 1;
    config.interruptMode = kGPIO_NoIntmode;
    GPIO_PinInit(self_in->gpio.port, self_in->gpio.pin, &config);
#else 
    hal_gpio_status_t status;
    self_in->gpio.config.direction = 0U;
    status = HAL_GpioInit(self_in->gpio.handle, &(self_in->gpio.config));
    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_NotImplementedError("Fail to Init gpio");
    }
#endif
}

STATIC void pyb_led_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<LED %d>", self->led_id);
}

STATIC mp_obj_t pyb_led_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, 1,false);
    led_obj_t *self = m_new_obj(led_obj_t);
    static led_info_t led_info_array[] = LED_INFO;
    uint8_t led_id = 0;

    led_id = mp_obj_get_int(args[0]);
    if(led_id == 0)
    {
        mp_raise_ValueError("LED id must larger than 0");
    }
    self->base.type = type;
    self->led_id = led_id;
    #if MICROPY_USE_IMXRT1064_MCU || MICROPY_USE_IMXRT1060_MCU || MICROPY_USE_IMXRT1050_MCU || MICROPY_USE_IMXRT1020_MCU
    static GPIO_Type *const gpioBases_ARRAY[] = GPIO_BASE_PTRS;
    self->gpio.base.type = &pyb_gpio_type;
    self->gpio.port = gpioBases_ARRAY[gpio_find_index(led_info_array[led_id].gpio_name)];
    self->gpio.pin = led_info_array[led_id].pin_num;
    #else
    self->gpio.base.type = &pyb_gpio_type;
    self->gpio.config.port = gpio_find_index(led_info_array[led_id].gpio_name);
    self->gpio.config.pin = led_info_array[led_id].pin_num;
    self->gpio.callback = NULL;
    #endif
    pyb_led_init_helper(self);
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_led_on(mp_obj_t self_in)
{
    led_obj_t* self = MP_OBJ_TO_PTR(self_in);
    pyb_gpio_SetOutput(MP_OBJ_FROM_PTR(&(self->gpio)), mp_obj_new_int(0U));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_led_on_obj, pyb_led_on);

STATIC mp_obj_t pyb_led_off(mp_obj_t self_in)
{
    led_obj_t* self = MP_OBJ_TO_PTR(self_in);
    pyb_gpio_SetOutput(MP_OBJ_FROM_PTR(&(self->gpio)), mp_obj_new_int(1U));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_led_off_obj, pyb_led_off);

#if MICROPY_USE_IMXRT1064_MCU || MICROPY_USE_IMXRT1060_MCU || MICROPY_USE_IMXRT1050_MCU || MICROPY_USE_IMXRT1020_MCU
STATIC mp_obj_t pyb_led_toggle(mp_obj_t self_in)
{
    led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    pyb_gpio_Toggle(MP_OBJ_FROM_PTR(&(self->gpio)));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_led_toggle_obj, pyb_led_toggle);
#endif 

STATIC const mp_rom_map_elem_t pyb_led_locals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&pyb_led_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&pyb_led_off_obj) },
#if MICROPY_USE_IMXRT1064_MCU || MICROPY_USE_IMXRT1060_MCU || MICROPY_USE_IMXRT1050_MCU || MICROPY_USE_IMXRT1020_MCU
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&pyb_led_toggle_obj) },
#endif
};
STATIC MP_DEFINE_CONST_DICT(pyb_gpio_locals_dict, pyb_led_locals_dict_table);

const mp_obj_type_t pyb_led_type = {
    {&mp_type_type},
    .name = MP_QSTR_led,
    .print = pyb_led_print,
    .make_new = pyb_led_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_gpio_locals_dict,
};
