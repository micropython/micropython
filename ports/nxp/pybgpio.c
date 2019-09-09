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

#include "py/runtime.h"
#include "py/obj.h"
#include "py/mpconfig.h"

#if defined (MICROPY_PY_GPIO) && MICROPY_PY_GPIO
#include "pybgpio.h"

typedef struct 
{
    mp_obj_base_t base;
    hal_gpio_pin_config_t config;
    hal_gpio_handle_t handle;
}gpio_obj_t;

STATIC mp_obj_t GPIO_Callback_Args[5];

STATIC void GPIO_Callback(void* param)
{
    gpio_obj_t *gpio_obj = param;
    if(gpio_obj->base.type == &pyb_gpio_type)
    {
        mp_obj_t *GPIO_cb = &(MP_STATE_PORT(pyb_gpio_callback)[gpio_obj->config.port]);
        if(*GPIO_cb != mp_const_none)
        {
            nlr_buf_t nlr;
            if(nlr_push(&nlr) == 0)
            {
                mp_call_function_1(*GPIO_cb, GPIO_Callback_Args[gpio_obj->config.port]);
                nlr_pop();
            }
        }
    }
}

STATIC const mp_arg_t pyb_gpio_make_new_args[] = {
    { MP_QSTR_direction, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_port, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 1} },
    { MP_QSTR_pin, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
};
STATIC mp_obj_t pyb_gpio_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_val_t vals[MP_ARRAY_SIZE(pyb_gpio_make_new_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(pyb_gpio_make_new_args), pyb_gpio_make_new_args, vals);

    gpio_obj_t *self = m_new_obj(gpio_obj_t);
    hal_gpio_status_t status;

    self->base.type = type;
    self->config.direction = vals[0].u_int;
    self->config.port = vals[1].u_int;
    self->config.pin = vals[2].u_int;

    status = HAL_GpioInit(self->handle, &(self->config));

    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError,"Fail to Init gpio");
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

STATIC mp_obj_t pyb_gpio_SetOutput(mp_obj_t self_in, mp_obj_t pinValue)
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

STATIC mp_obj_t pyb_gpio_InstallCallback(mp_obj_t self_in, mp_obj_t callback, mp_obj_t param)
{
    gpio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    status_t status;

    MP_STATE_PORT(pyb_gpio_callback)[self->config.port] = callback;
    GPIO_Callback_Args[self->config.port] = param;

    status = HAL_GpioInstallCallback(self->handle, GPIO_Callback, self);
    if(status != kStatus_HAL_GpioSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError, "Fail to Install callback function");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(pyb_gpio_InstallCallback_obj, pyb_gpio_InstallCallback);

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

STATIC const mp_rom_map_elem_t pyb_gpio_locals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR_Deinit), MP_ROM_PTR(&pyb_gpio_Deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetInput), MP_ROM_PTR(&pyb_gpio_GetInput_obj) },
    { MP_ROM_QSTR(MP_QSTR_SetOutput), MP_ROM_PTR(&pyb_gpio_SetOutput_obj) },
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
};
STATIC MP_DEFINE_CONST_DICT(pyb_gpio_locals_dict, pyb_gpio_locals_dict_table);

const mp_obj_type_t pyb_gpio_type = {
    {&mp_type_type},
    .name = MP_QSTR_GPIO,
    .make_new = pyb_gpio_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_gpio_locals_dict,
};
#endif /*MICROPY_PY_GPIO*/