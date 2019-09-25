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
#if defined(MICROPY_PY_I2C) && MICROPY_PY_I2C
#include BOARD_I2C_CMSIS_HEADER
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mpconfig.h"


#include "pybi2c.h"
#include "i2c.h"
#include "bufferhelper.h"


typedef struct _i2c_obj
{
    mp_obj_base_t base;
    struct _ARM_DRIVER_I2C *Driver_I2C;
    uint8_t instance;
}i2c_obj_t;

STATIC mp_obj_t pyb_i2c_make_new(const mp_obj_type_t* type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    i2c_obj_t* self = m_new_obj(i2c_obj_t);
    uint8_t instance;

    i2c_init();
    self->Driver_I2C = i2c_find_index(args[0], &instance);
    self->base.type = type;
    self->instance = instance;
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_i2c_InstallCallback(mp_obj_t self_in, mp_obj_t callback)
{
    i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    MP_STATE_PORT(pyb_i2c_callback)[self->instance] = callback;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_i2c_InstallCallback_obj, pyb_i2c_InstallCallback);

STATIC mp_obj_t pyb_i2c_Initialize(mp_obj_t self_in)
{
    i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ARM_I2C_SignalEvent_t cb_event = i2c_find_callback(self->instance);
    (self->Driver_I2C)->Initialize(cb_event);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_Initialize_obj, pyb_i2c_Initialize);

STATIC mp_obj_t pyb_i2c_Uninitialize(mp_obj_t self_in)
{
    i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    (self->Driver_I2C)->Uninitialize();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_Uninitialize_obj, pyb_i2c_Uninitialize);

STATIC mp_obj_t pyb_i2c_PowerControl(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    static const mp_arg_t pyb_i2c_PowerControl_args[] = {
        { MP_QSTR_state, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t vals[MP_ARRAY_SIZE(pyb_i2c_PowerControl_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(pyb_i2c_PowerControl_args), pyb_i2c_PowerControl_args, vals);
    (self->Driver_I2C)->PowerControl((ARM_POWER_STATE)vals[0].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_PowerControl_obj, 1, pyb_i2c_PowerControl);

STATIC mp_obj_t pyb_i2c_MasterTransmit(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    static const mp_arg_t pyb_i2c_MasterTransmit_args[] = {
        { MP_QSTR_addr, MP_ARG_REQUIRED|MP_ARG_INT, {.u_int = 0U} },
        { MP_QSTR_TxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 0U} },
        { MP_QSTR_xfer_pending, MP_ARG_REQUIRED|MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t vals[MP_ARRAY_SIZE(pyb_i2c_MasterTransmit_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(pyb_i2c_MasterTransmit_args), pyb_i2c_MasterTransmit_args, vals);

    /* get the buffer to send */
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(vals[1].u_obj, &bufinfo, data);
    (self->Driver_I2C)->MasterTransmit(vals[0].u_int, bufinfo.buf, vals[2].u_int, vals[3].u_bool);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_MasterTransmit_obj, 1, pyb_i2c_MasterTransmit);

STATIC mp_obj_t pyb_i2c_MasterReceive(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_addr, MP_ARG_REQUIRED|MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_RxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 0U} },
        { MP_QSTR_xfer_pending, MP_ARG_REQUIRED|MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, vals);

    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[1].u_obj, &vstr);
    (self->Driver_I2C)->MasterReceive(vals[0].u_int, (uint8_t *)vstr.buf, vals[2].u_int, vals[3].u_bool);

    if(o_ret != MP_OBJ_NULL)
    {
        return o_ret;
    }
    else
    {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_MasterReceive_obj, 1, pyb_i2c_MasterReceive);

STATIC mp_obj_t pyb_i2c_SlaveTransmit(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_TxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 0U} },
    };
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, vals);

    /* get the buffer to send */
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(vals[0].u_obj, &bufinfo, data);
    (self->Driver_I2C)->SlaveTransmit(bufinfo.buf, vals[1].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_SlaveTransmit_obj, 1, pyb_i2c_SlaveTransmit);

STATIC mp_obj_t pyb_i2c_SlaveReceive(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_RxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 0U} },
    };
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, vals);

    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &vstr);
    (self->Driver_I2C)->SlaveReceive((uint8_t *)vstr.buf, vals[1].u_int);

    if(o_ret != MP_OBJ_NULL)
    {
        return o_ret;
    }
    else
    {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_SlaveReceive_obj, 1, pyb_i2c_SlaveReceive);

STATIC mp_obj_t pyb_i2c_GetDataCount(mp_obj_t self_in)
{
    i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t datacounts;

    datacounts = (self->Driver_I2C)->GetDataCount();
    return mp_obj_new_int_from_uint(datacounts);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_GetDataCount_obj, pyb_i2c_GetDataCount);

STATIC mp_obj_t pyb_i2c_Control(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_control_code, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_arg, MP_ARG_INT, {.u_int = 0} },
    };

    /* parse args */
    i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, vals);

    (self->Driver_I2C)->Control(vals[0].u_int, vals[1].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_Control_obj, 1, pyb_i2c_Control);

STATIC mp_obj_t pyb_i2c_GetStatus(mp_obj_t self_in)
{
    i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ARM_I2C_STATUS status;
    mp_obj_t status_dict = mp_obj_new_dict(0);

    status = (self->Driver_I2C)->GetStatus();

    if(status.busy)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("Busy Flag", strlen("Busy Flag")), mp_obj_new_str("Busy", strlen("Busy")));
    }
    else if(!status.busy)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("Busy Flag", strlen("Busy Flag")), mp_obj_new_str("Idle", strlen("Idle")));
    }
    if(status.mode)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("Mode", strlen("Mode")), mp_obj_new_str("Master", strlen("Master")));
    }
    else if(!status.mode)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("Mode", strlen("Mode")), mp_obj_new_str("Slave", strlen("Slave")));
    }
    if (status.direction)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("Direction", strlen("Direction")), mp_obj_new_str("Receiver", strlen("Receiver")));
    }
    else if(!status.direction)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("Direction", strlen("Direction")), mp_obj_new_str("Transmitter", strlen("Transmitter")));
    }
    if (status.general_call)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("General Call", strlen("General Call")), mp_obj_new_str("Yes", strlen("Yes")));
    }
    else if (!status.general_call)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("General Call", strlen("General Call")), mp_obj_new_str("No", strlen("No")));
    }
    if(status.arbitration_lost)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("Arbitration Lost", strlen("Arbitration Lost")), mp_obj_new_str("Yes", strlen("Yes")));
    }
    else if(!status.arbitration_lost)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("Arbitration Lost", strlen("Arbitration Lost")), mp_obj_new_str("No", strlen("No")));
    }
    if(status.bus_error)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("Bus Error", strlen("Bus Error")), mp_obj_new_str("Yes", strlen("Yes")));
    }
    else if (!status.bus_error)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("Bus Error", strlen("Bus Error")), mp_obj_new_str("No", strlen("No")));
    }
    mp_obj_print(status_dict, PRINT_STR);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_GetStatus_obj, pyb_i2c_GetStatus);

STATIC const mp_rom_map_elem_t pyb_i2c_locals_dict_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_InstallCallback), MP_ROM_PTR(&pyb_i2c_InstallCallback_obj) },
    { MP_ROM_QSTR(MP_QSTR_Initialize), MP_ROM_PTR(&pyb_i2c_Initialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_Uninitialize), MP_ROM_PTR(&pyb_i2c_Uninitialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_PowerControl), MP_ROM_PTR(&pyb_i2c_PowerControl_obj) },
    { MP_ROM_QSTR(MP_QSTR_MasterTransmit), MP_ROM_PTR(&pyb_i2c_MasterTransmit_obj) },
    { MP_ROM_QSTR(MP_QSTR_MasterReceive), MP_ROM_PTR(&pyb_i2c_MasterReceive_obj) },
    { MP_ROM_QSTR(MP_QSTR_SlaveTransmit), MP_ROM_PTR(&pyb_i2c_SlaveTransmit_obj) },
    { MP_ROM_QSTR(MP_QSTR_SlaveReceive), MP_ROM_PTR(&pyb_i2c_SlaveReceive_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetDataCount), MP_ROM_PTR(&pyb_i2c_GetDataCount_obj) },
    { MP_ROM_QSTR(MP_QSTR_Control), MP_ROM_PTR(&pyb_i2c_Control_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetStatus), MP_ROM_PTR(&pyb_i2c_GetStatus_obj) },

    /* I2C control code */
    { MP_ROM_QSTR(MP_QSTR_OWN_ADDRESS), MP_ROM_INT(ARM_I2C_OWN_ADDRESS) },
    { MP_ROM_QSTR(MP_QSTR_BUS_SPEED), MP_ROM_INT(ARM_I2C_BUS_SPEED) },
    { MP_ROM_QSTR(MP_QSTR_BUS_CLEAR), MP_ROM_INT(ARM_I2C_BUS_CLEAR) },
    { MP_ROM_QSTR(MP_QSTR_ABORT_TRANSFER), MP_ROM_INT(ARM_I2C_ABORT_TRANSFER) },

    /* power states */
    { MP_ROM_QSTR(MP_QSTR_ARM_POWER_OFF), MP_ROM_INT(ARM_POWER_OFF) },
    { MP_ROM_QSTR(MP_QSTR_ARM_POWER_LOW), MP_ROM_INT(ARM_POWER_LOW) },
    { MP_ROM_QSTR(MP_QSTR_ARM_POWER_FULL), MP_ROM_INT(ARM_POWER_FULL) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_i2c_locals_dict, pyb_i2c_locals_dict_dict_table);

const mp_obj_type_t pyb_i2c_type = {
    {&mp_type_type},
    .make_new = pyb_i2c_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_i2c_locals_dict,
};
#endif /* MICROPY_PY_I2C */