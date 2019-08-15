#include <stdio.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "fsl_common.h"
#include "timer/timer.h"

#include "pybgpt.h"

typedef struct 
{
    mp_obj_base_t base;
    hal_timer_handle_t handle;
    hal_timer_config_t config;
}gpt_obj_t;

STATIC mp_obj_t GPT_Callback_Args[3];

STATIC void GPT_Callback(void* param)
{
    gpt_obj_t *gpt_obj = param;
    if(gpt_obj->base.type == &pyb_gpt_type)
    {
        mp_obj_t* GPT_cb = &(MP_STATE_PORT(pyb_gpt_callback)[gpt_obj->config.instance]);
        if(*GPT_cb != mp_const_none)
        {
            nlr_buf_t nlr;
            if(nlr_push(&nlr) == 0)
            {    
                mp_call_function_1(*GPT_cb, GPT_Callback_Args[gpt_obj->config.instance]);
                nlr_pop();
            }
        }
    }
}

STATIC const mp_arg_t GPT_make_new_args[] = {
    { MP_QSTR_instance, MP_ARG_REQUIRED|MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_srcClock, MP_ARG_REQUIRED|MP_ARG_INT, {.u_int = 0} },
};

STATIC mp_obj_t pyb_GPT_Makenew(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_val_t vals[MP_ARRAY_SIZE(GPT_make_new_args)];

    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(GPT_make_new_args), GPT_make_new_args, vals);    

    gpt_obj_t *self = m_new_obj(gpt_obj_t);

    self->base.type = type;

    self->config.instance = vals[0].u_int;
    self->config.srcClock_Hz = vals[1].u_int;
    hal_timer_handle_t handle = m_malloc(sizeof(hal_timer_handle_t));
    self->handle = handle;

    return MP_OBJ_FROM_PTR(self);
}

STATIC void pyb_GPT_Print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    gpt_obj_t* self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "GPT[%d] source clock[%d]",self->config.instance, self->config.srcClock_Hz);
}

STATIC mp_obj_t pyb_GPT_Init(mp_obj_t self_in, mp_obj_t timeout)
{
    gpt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    hal_timer_status_t status;
    uint8_t i = 0;

    self->config.timeout = mp_obj_get_int(timeout);
    status = HAL_TimerInit(self->handle, &self->config);
    if(status != kStatus_HAL_TimerSuccess)
    {
        mp_raise_msg(&mp_type_TypeError, "GPT init failed");
    }
    for(i=0; i<3; i++)
    {
        if(MP_STATE_PORT(pyb_gpt_callback)[i] == mp_const_none)
            continue;
        MP_STATE_PORT(pyb_gpt_callback)[i] = mp_const_none;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(GPT_Init_obj, pyb_GPT_Init);

STATIC mp_obj_t pyb_GPT_Deinit(mp_obj_t self_in)
{
    gpt_obj_t *self = MP_OBJ_TO_PTR(self_in);

    HAL_TimerDeinit(self->handle);
    m_del_obj(gpt_obj_t, self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(GPT_Deinit_obj, pyb_GPT_Deinit);

STATIC mp_obj_t pyb_GPT_Enable(mp_obj_t self_in)
{
    gpt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    
    HAL_TimerEnable(self->handle);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(GPT_Enable_obj, pyb_GPT_Enable);

STATIC mp_obj_t pyb_GPT_Disable(mp_obj_t self_in)
{
    gpt_obj_t *self = MP_OBJ_TO_PTR(self_in);

    HAL_TimerDisable(self->handle);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(GPT_Disable_obj, pyb_GPT_Disable);

STATIC mp_obj_t pyb_GPT_InstallCallback(mp_obj_t self_in, mp_obj_t callback, mp_obj_t param)
{
    gpt_obj_t *self = MP_OBJ_TO_PTR(self_in);

    MP_STATE_PORT(pyb_gpt_callback)[self->config.instance] = callback;
    GPT_Callback_Args[self->config.instance] = param;

    HAL_TimerInstallCallback(self->handle, GPT_Callback, self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(GPT_InstallCallback_obj, pyb_GPT_InstallCallback);

STATIC mp_obj_t pyb_GPT_GetMaxTimeout(mp_obj_t self_in)
{
    gpt_obj_t* self = MP_OBJ_TO_PTR(self_in);
    uint32_t Count;

    Count = HAL_TimerGetCurrentTimerCount(self->handle);    

    return mp_obj_new_int(Count);
}
MP_DEFINE_CONST_FUN_OBJ_1(GPT_GetMaxTimeout_obj, pyb_GPT_GetMaxTimeout);

STATIC mp_obj_t pyb_GPT_GetCurrentTimerCount(mp_obj_t self_in)
{
    gpt_obj_t* self = MP_OBJ_TO_PTR(self_in);
    uint32_t Count;

    Count = HAL_TimerGetCurrentTimerCount(self->handle);

    return mp_obj_new_int(Count);
}
MP_DEFINE_CONST_FUN_OBJ_1(GPT_GetCurrentTimerCount_obj, pyb_GPT_GetCurrentTimerCount);

STATIC mp_obj_t pyb_GPT_UpdateTimeout(mp_obj_t self_in, mp_obj_t timeout)
{
    gpt_obj_t* self = MP_OBJ_TO_PTR(self_in);
    hal_timer_status_t status;

    status = HAL_TimerUpdateTimeout(self->handle, mp_obj_get_int(timeout));
    if(status != kStatus_HAL_TimerSuccess)
    {
        mp_raise_msg(&mp_type_TypeError, "GPT init failed");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(GPT_UpdateTimeout_obj, pyb_GPT_UpdateTimeout);

STATIC const mp_rom_map_elem_t pyb_GPT_locals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR_Init), MP_ROM_PTR(&GPT_Init_obj) },
    { MP_ROM_QSTR(MP_QSTR_Deinit), MP_ROM_PTR(&GPT_Deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_Enable), MP_ROM_PTR(&GPT_Enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_Disable), MP_ROM_PTR(&GPT_Disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_InstallCallback), MP_ROM_PTR(&GPT_InstallCallback_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetMaxTimeout), MP_ROM_PTR(&GPT_GetMaxTimeout_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetCurrentTimeCount), MP_ROM_PTR(&GPT_GetCurrentTimerCount_obj) },
    { MP_ROM_QSTR(MP_QSTR_UpdateTimeout), MP_ROM_PTR(&GPT_UpdateTimeout_obj) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_GPT_locals_dict, pyb_GPT_locals_dict_table);

const mp_obj_type_t pyb_gpt_type = {
    {&mp_type_type},
    .name = MP_QSTR_GPT,
    .make_new = pyb_GPT_Makenew,
    .print = pyb_GPT_Print,
    .locals_dict = (mp_obj_dict_t*)&pyb_GPT_locals_dict,
};