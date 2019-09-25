/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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
#include "mpconfigport.h"

#if defined (MICROPY_PY_GPIO) && MICROPY_PY_GPIO
#include "fsl_common.h"
#include "gpio/gpio.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "pybgpio.h"
#include "mpgpio.h"

#if MICROPY_USE_IMXRT1064_MCU || MICROPY_USE_IMXRT1060_MCU || MICROPY_USE_IMXRT1050_MCU || MICROPY_USE_IMXRT1020_MCU
#include "fsl_gpio.h"


mp_obj_t pyb_gpio_init_helper(gpio_obj_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[]={
        { MP_QSTR_direction, MP_ARG_REQUIRED|MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_value, MP_ARG_REQUIRED|MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, vals);
    gpio_pin_direction_t dir = (gpio_pin_direction_t)vals[0].u_int;
    uint8_t value = vals[1].u_int;
    gpio_pin_config_t config;
    config.direction = dir;
    config.outputLogic = value;
    config.interruptMode = kGPIO_NoIntmode;
    GPIO_PinInit(self_in->port, self_in->pin, &config);
    return mp_const_none;
}

STATIC mp_obj_t pyb_gpio_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    static GPIO_Type *const gpioBases_ARRAY[] = GPIO_BASE_PTRS;
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    if(!mp_obj_is_type(args[0], &mp_type_tuple))
    {
        mp_raise_ValueError("Pin id must be tuple of (\"GPIO_x\", pin#)");
    }
    mp_obj_t *items;
    gpio_obj_t *self = m_new_obj(gpio_obj_t);

    mp_obj_get_array_fixed_n(args[0], 2, &items);
    self->port = gpioBases_ARRAY[gpio_find_index(mp_obj_str_get_str(items[0]))];
    if(self->port == NULL)
    {
        mp_raise_ValueError("None this gpio");
    }
    self->pin = mp_obj_get_int(items[1]);
    self->base.type = type;

    if(n_args > 1 || n_kw > 0)
    {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_gpio_init_helper(self, n_args-1, args+1, &kw_args);
    }
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_gpio_GetInput(mp_obj_t self_in)
{
    gpio_obj_t* self = MP_OBJ_TO_PTR(self_in);
    uint8_t pinvalue;

    pinvalue = GPIO_PinRead(self->port, self->pin);
    return mp_obj_new_int(pinvalue);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_gpio_GetInput_obj, pyb_gpio_GetInput);

mp_obj_t pyb_gpio_SetOutput(mp_obj_t self_in, mp_obj_t pinValue)
{
    gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    GPIO_PinWrite(self->port, self->pin, mp_obj_get_int(pinValue));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_gpio_SetOutput_obj, pyb_gpio_SetOutput);

mp_obj_t pyb_gpio_Toggle(mp_obj_t self_in)
{
    gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    GPIO_PortToggle(self->port, (1<<self->pin));
    return mp_const_none;
}

#else

void GPIO_Callback(void* param)
{
    gpio_obj_t *gpio_obj = param;
    if(gpio_obj->base.type == &pyb_gpio_type && gpio_obj->callback != mp_const_none)
    {
        mp_obj_t GPIO_cb = gpio_obj->callback;
        if(GPIO_cb != MP_OBJ_NULL)
        {
            nlr_buf_t nlr;
            if(nlr_push(&nlr) == 0)
            {
                mp_call_function_0(GPIO_cb);
                nlr_pop();
            }
        }
    }
}

mp_obj_t pyb_gpio_init_helper(gpio_obj_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_direction, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed_args)];
    hal_gpio_status_t status;

    mp_arg_parse_all(n_args-1, pos_args+1, kw_args ,MP_ARRAY_SIZE(allowed_args), allowed_args, vals);
    self_in->config.direction = vals[0].u_int;

    status = HAL_GpioInit(self_in->handle, &(self_in->config));
    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError,"Fail to Init gpio");
    }
    return mp_const_none;
}


STATIC mp_obj_t pyb_gpio_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    gpio_obj_t *self = m_new_obj(gpio_obj_t);

    if(!mp_obj_is_type(args[0], &mp_type_tuple))
    {
        mp_raise_ValueError("Pin id must be tuple of (\"GPIO_x\", pin#)");
    }
    mp_obj_t *items;
    gpio_obj_t *self = m_new_obj(gpio_obj_t);

    mp_obj_get_array_fixed_n(args[0], 2, &items);

    self->base.type = type;
    self->config.port = gpio_find_index(mp_obj_str_get_str(items[0]));
    self->config.pin = mp_obj_get_int(items[1]);
    self->callback = MP_OBJ_NULL;

    if(n_args > 1 || n_kw >0)
    {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw,args+n_args);
        pyb_gpio_init_helper(self, n_args-1, args+1, &kw_args);
    }
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_gpio_Deinit(mp_obj_t self_in)
{
    gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    hal_gpio_status_t status;

    status = HAL_GpioDeinit(self->handle);

    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError,"Fail to Deinit gpio");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_gpio_Deinit_obj, pyb_gpio_Deinit);

STATIC mp_obj_t pyb_gpio_GetInput(mp_obj_t self_in)
{
    gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t pinState;
    status_t status;

    status = HAL_GpioGetInput(self->handle, &pinState);
    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError,"Fail to get gpio state");
    }
    return mp_obj_new_int(pinState);
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_gpio_GetInput_obj, pyb_gpio_GetInput);

mp_obj_t pyb_gpio_SetOutput(mp_obj_t self_in, mp_obj_t pinValue)
{
    uint8_t pinState = mp_obj_get_int(pinValue);
    status_t status;
    gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);

    status = HAL_GpioSetOutput(self->handle, pinState);
    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError,"Fail to Set gpio state");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pyb_gpio_SetOutput_obj, pyb_gpio_SetOutput);

STATIC mp_obj_t pyb_gpio_GetTriggerMode(mp_obj_t self_in)
{
    gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    status_t status;
    hal_gpio_interrupt_trigger_t gpioTrigger;

    status = HAL_GpioGetTriggerMode(self->handle, &gpioTrigger);

    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError, "Fail to Get gpio trigger mode");
    }
    else
    {
        switch (gpioTrigger)
        {
        case kHAL_GpioInterruptDisable:
            printf("Interrupt Disable\r\n");
            break;
        case kHAL_GpioInterruptLogicZero:
            printf("Interrupt LogicZero\r\n");
            break;
        case kHAL_GpioInterruptRisingEdge:
            printf("Interrupt RisingEdge\r\n");
            break;
        case kHAL_GpioInterruptFallingEdge:
            printf("Interrupt FallingEdge\r\n");
            break;
        case kHAL_GpioInterruptEitherEdge:
            printf("Interrupt EitherEdge\r\n");
            break;
        case kHAL_GpioInterruptLogicOne:
            printf("Interrupt LogicOne\r\n");
            break;
        default:
            assert(false);
            break;
        }
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_gpio_GetTriggerMode_obj, pyb_gpio_GetTriggerMode);

STATIC mp_obj_t pyb_gpio_SetTriggerMode(mp_obj_t self_in, mp_obj_t triggerMode)
{
    gpio_obj_t *self = MP_OBJ_FROM_PTR(self_in);
    hal_gpio_interrupt_trigger_t pinTrigger = (hal_gpio_interrupt_trigger_t)mp_obj_get_int(triggerMode);
    status_t status;

    status = HAL_GpioSetTriggerMode(self->handle, pinTrigger);
    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError, "Fail to Set gpio trigger mode");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pyb_gpio_SetTriggerMode_obj, pyb_gpio_SetTriggerMode);

STATIC mp_obj_t pyb_gpio_InstallCallback(mp_obj_t self_in, mp_obj_t callback)
{
    gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    status_t status;

    self->callback = callback;

    status = HAL_GpioInstallCallback(self->handle, GPIO_Callback, self);
    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError, "Fail to Install callback function");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pyb_gpio_InstallCallback_obj, pyb_gpio_InstallCallback);

STATIC mp_obj_t pyb_gpio_WakeUpSetting(mp_obj_t self_in, mp_obj_t arg)
{
    gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t enable = mp_obj_get_int(arg);
    status_t status;

    status = HAL_GpioWakeUpSetting(self->handle, enable);
    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError, "Fail to Set wake up mode");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pyb_gpio_WakeUpSetting_obj, pyb_gpio_WakeUpSetting);

STATIC mp_obj_t pyb_gpio_EnterLowpower(mp_obj_t self_in)
{
    gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    status_t status;

    status = HAL_GpioEnterLowpower(self->handle);
    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError, "Fail to Set wake up mode");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_gpio_EnterLowpower_obj, pyb_gpio_EnterLowpower);

STATIC mp_obj_t pyb_gpio_ExitLowpower(mp_obj_t self_in)
{
    gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    status_t status;

    status = HAL_GpioExitLowpower(self->handle);
    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError, "Fail to Set wake up mode");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_gpio_ExitLowpower_obj, pyb_gpio_ExitLowpower);

#endif /* MICROPY_USE_IMXRT1064_MCU || MICROPY_USE_IMXRT1060_MCU || MICROPY_USE_IMXRT1020_MCU || MICROPY_USE_IMXRT1050_MCU*/

STATIC const mp_rom_map_elem_t pyb_gpio_locals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR_GetInput), MP_ROM_PTR(&pyb_gpio_GetInput_obj) },
    { MP_ROM_QSTR(MP_QSTR_SetOutput), MP_ROM_PTR(&pyb_gpio_SetOutput_obj) },
#if !defined(MICROPY_USE_IMXRT1064_MCU) && !defined(MICROPY_USE_IMXRT1060_MCU) && !defined(MICROPY_USE_IMXRT1020_MCU) && !defined(MICROPY_USE_IMXRT1050_MCU)
    { MP_ROM_QSTR(MP_QSTR_Deinit), MP_ROM_PTR(&pyb_gpio_Deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetTriggerMode), MP_ROM_PTR(&pyb_gpio_GetTriggerMode_obj) },
    { MP_ROM_QSTR(MP_QSTR_SetTriggerMode), MP_ROM_PTR(&pyb_gpio_SetTriggerMode_obj) },
    { MP_ROM_QSTR(MP_QSTR_InstallCallback), MP_ROM_PTR(&pyb_gpio_InstallCallback_obj) },
    { MP_ROM_QSTR(MP_QSTR_WakeUpSetting), MP_ROM_PTR(&pyb_gpio_WakeUpSetting_obj) },
    { MP_ROM_QSTR(MP_QSTR_EnterLowpower), MP_ROM_PTR(&pyb_gpio_EnterLowpower_obj) },
    { MP_ROM_QSTR(MP_QSTR_ExitLowpower), MP_ROM_PTR(&pyb_gpio_ExitLowpower_obj) },
    
    /* class constants */
    { MP_ROM_QSTR(MP_QSTR_InterruptDisable), MP_ROM_INT(kHAL_GpioInterruptDisable) },
    { MP_ROM_QSTR(MP_QSTR_InterruptLogicZero), MP_ROM_INT(kHAL_GpioInterruptLogicZero) },
    { MP_ROM_QSTR(MP_QSTR_InterruptRisingEdge), MP_ROM_INT(kHAL_GpioInterruptRisingEdge) },
    { MP_ROM_QSTR(MP_QSTR_InterruptFallingEdge), MP_ROM_INT(kHAL_GpioInterruptFallingEdge) },
    { MP_ROM_QSTR(MP_QSTR_InterruptEitherEdge), MP_ROM_INT(kHAL_GpioInterruptEitherEdge) },
    { MP_ROM_QSTR(MP_QSTR_InterruptLogicOne), MP_ROM_INT(kHAL_GpioInterruptLogicOne) },
#endif 
};
STATIC MP_DEFINE_CONST_DICT(pyb_gpio_locals_dict, pyb_gpio_locals_dict_table);

const mp_obj_type_t pyb_gpio_type = {
    {&mp_type_type},
    .name = MP_QSTR_GPIO,
    .make_new = pyb_gpio_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_gpio_locals_dict,
};
#endif /*MICROPY_PY_GPIO*/