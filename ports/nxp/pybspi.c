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
#if defined(MICROPY_PY_SPI) && MICROPY_PY_SPI
#include BOARD_SPI_CMSIS_HEADER
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mpconfig.h"

#include "pybspi.h"
#include "spi.h"
#include "bufferhelper.h"

typedef struct _spi_obj
{
    mp_obj_base_t base;
    struct _ARM_DRIVER_SPI *Driver_SPI;
    uint8_t instance;
}spi_obj_t;

STATIC mp_obj_t pyb_spi_make_new(const mp_obj_type_t* type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    spi_obj_t *self = m_new_obj(spi_obj_t);
    uint8_t instance;
    
    spi_init();
    self->Driver_SPI = spi_find_index(args[0], &instance);
    self->base.type = type;
    self->instance = instance;
    return MP_OBJ_FROM_PTR(self); 
}

STATIC mp_obj_t pyb_spi_install_callback(mp_obj_t self_in, mp_obj_t callback)
{
    spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    MP_STATE_PORT(pyb_spi_callback)[self->instance] = callback;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_spi_install_callback_obj, pyb_spi_install_callback);

STATIC mp_obj_t pyb_spi_Initialize(mp_obj_t self_in)
{
    spi_obj_t *self = MP_OBJ_TO_PTR(self_in);

    ARM_SPI_SignalEvent_t cb_event = spi_find_callback(self->instance);
    (self->Driver_SPI)->Initialize(cb_event);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_spi_Initialize_obj, pyb_spi_Initialize);

STATIC mp_obj_t pyb_spi_Uninitialize(mp_obj_t self_in)
{
    spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    (self->Driver_SPI)->Uninitialize();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_spi_Uninitialize_obj, pyb_spi_Uninitialize);

STATIC const mp_arg_t pyb_spi_PowerControl_args[] = {
    { MP_QSTR_state, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
};

STATIC mp_obj_t pyb_spi_PowerControl(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t vals[MP_ARRAY_SIZE(pyb_spi_PowerControl_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(pyb_spi_PowerControl_args), pyb_spi_PowerControl_args, vals);
    (self->Driver_SPI)->PowerControl((ARM_POWER_STATE)vals[0].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_PowerControl_obj, 1, pyb_spi_PowerControl);

STATIC mp_obj_t pyb_spi_Send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_TxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 0} },
    };

    /* parse args */
    spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    /* get the buffer to send from */
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[0].u_obj, &bufinfo, data);
    (self->Driver_SPI)->Send(bufinfo.buf, args[1].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_Send_obj, 1, pyb_spi_Send);

STATIC mp_obj_t pyb_spi_Receive(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_RxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 0} },
    };

    /* parse args */
    spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(args[0].u_obj, &vstr);
    (self->Driver_SPI)->Receive(vstr.buf, args[1].u_int);

    if (o_ret != MP_OBJ_NULL) 
    {
        return o_ret;
    } 
    else 
    {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_Receive_obj, 1, pyb_spi_Receive);

STATIC mp_obj_t pyb_spi_Transfer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[]={
        { MP_QSTR_TxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_RxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_num, MP_ARG_INT, {.u_obj = 0} },
    };
    /* parse args */
    spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    /* get the buffer to send and receive */
    mp_buffer_info_t bufinfo_send;
    uint8_t data_send[1];
    mp_buffer_info_t bufinfo_recv;
    vstr_t vstr_recv;
    mp_obj_t o_ret;

    if (args[0].u_obj == args[1].u_obj) {
        // same object for send and receive, it must be a r/w buffer
        mp_get_buffer_raise(args[0].u_obj, &bufinfo_send, MP_BUFFER_RW);
        bufinfo_recv = bufinfo_send;
        o_ret = args[0].u_obj;
    } else {
        // get the buffer to send from
        pyb_buf_get_for_send(args[0].u_obj, &bufinfo_send, data_send);

        // get the buffer to receive into
        if (args[1].u_obj == MP_OBJ_NULL) {
            // only send argument given, so create a fresh buffer of the send length
            vstr_init_len(&vstr_recv, bufinfo_send.len);
            bufinfo_recv.len = vstr_recv.len;
            bufinfo_recv.buf = vstr_recv.buf;
            o_ret = MP_OBJ_NULL;
        } else {
            // recv argument given
            mp_get_buffer_raise(args[1].u_obj, &bufinfo_recv, MP_BUFFER_WRITE);
            if (bufinfo_recv.len != bufinfo_send.len) {
                mp_raise_ValueError("recv must be same length as send");
            }
            o_ret = args[1].u_obj;
        }
    }

    (self->Driver_SPI)->Transfer(bufinfo_send.buf, bufinfo_recv.buf, args[2].u_int);
    if (o_ret != MP_OBJ_NULL) 
    {
        return o_ret;
    } 
    else 
    {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr_recv);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_Transfer_obj, 1, pyb_spi_Transfer);

STATIC mp_obj_t pyb_spi_GetDataCount(mp_obj_t self_in)
{
    spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t datacounts;

    datacounts = (self->Driver_SPI)->GetDataCount();
    return mp_obj_new_int_from_uint(datacounts);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_spi_GetDataCount_obj, pyb_spi_GetDataCount);

STATIC mp_obj_t pyb_spi_Control(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] ={
        { MP_QSTR_control_code, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_arg, MP_ARG_INT, {.u_int = 0} },
    };

    uint32_t ret;

    /* parse args */
    spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, vals);

    if(vals[0].u_int == ARM_SPI_GET_BUS_SPEED)
    {
        ret = (self->Driver_SPI)->Control(vals[0].u_int, vals[1].u_int);
        return mp_obj_new_int(ret);
    }
    else
    {

        ret = (self->Driver_SPI)->Control(vals[0].u_int, vals[1].u_int);
        if(ret != ARM_DRIVER_OK)
        {
            mp_raise_msg(&mp_type_ValueError, "Fail to control spi");
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_Control_obj, 1, pyb_spi_Control);

STATIC mp_obj_t pyb_spi_GetStatus(mp_obj_t self_in)
{
    spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ARM_SPI_STATUS status;

    status = (self->Driver_SPI)->GetStatus();
    if(status.busy)
    {
        return mp_obj_new_str("SPI busy", strlen("SPI busy"));
    }
    if(status.data_lost)
    {
        return mp_obj_new_str("Data Lost", strlen("Data Lost"));
    }
    if(status.mode_fault)
    {
        return mp_obj_new_str("Mode fault detected", strlen("Mode fault detected"));
    }
    else
    {
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_spi_GetStatus_obj, pyb_spi_GetStatus);

STATIC const mp_rom_map_elem_t pyb_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_InstallCallback), MP_ROM_PTR(&pyb_spi_install_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_Initialize), MP_ROM_PTR(&pyb_spi_Initialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_Uninitialize), MP_ROM_PTR(&pyb_spi_Uninitialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_PowerControl), MP_ROM_PTR(&pyb_spi_PowerControl_obj) },
    { MP_ROM_QSTR(MP_QSTR_Send), MP_ROM_PTR(&pyb_spi_Send_obj) },
    { MP_ROM_QSTR(MP_QSTR_Receive), MP_ROM_PTR(&pyb_spi_Receive_obj) },
    { MP_ROM_QSTR(MP_QSTR_Transfer), MP_ROM_PTR(&pyb_spi_Transfer_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetDataCount), MP_ROM_PTR(&pyb_spi_GetDataCount_obj) },
    { MP_ROM_QSTR(MP_QSTR_Control), MP_ROM_PTR(&pyb_spi_Control_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetStatus), MP_ROM_PTR(&pyb_spi_GetStatus_obj) },

    /* class constants */
    /* SPI control mode */
    { MP_ROM_QSTR(MP_QSTR_MODE_INACTIVE), MP_ROM_INT(ARM_SPI_MODE_INACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_MODE_MASTER), MP_ROM_INT(ARM_SPI_MODE_MASTER) },
    { MP_ROM_QSTR(MP_QSTR_MODE_SLAVE), MP_ROM_INT(ARM_SPI_MODE_SLAVE) },
    { MP_ROM_QSTR(MP_QSTR_SET_BUS_SPEED), MP_ROM_INT(ARM_SPI_SET_BUS_SPEED) },
    { MP_ROM_QSTR(MP_QSTR_GET_BUS_SPEED), MP_ROM_INT(ARM_SPI_GET_BUS_SPEED) },
    { MP_ROM_QSTR(MP_QSTR_CONTROL_SS), MP_ROM_INT(ARM_SPI_CONTROL_SS) },
    { MP_ROM_QSTR(MP_QSTR_ABORT_TRANSFER), MP_ROM_INT(ARM_SPI_ABORT_TRANSFER) },
    { MP_ROM_QSTR(MP_QSTR_SET_DEFAULT_TX_VALUE), MP_ROM_INT(ARM_SPI_SET_DEFAULT_TX_VALUE) },
    { MP_ROM_QSTR(MP_QSTR_MODE_MASTER_SIMPLEX), MP_ROM_INT(ARM_SPI_MODE_MASTER_SIMPLEX) },
    { MP_ROM_QSTR(MP_QSTR_MODE_SLAVE_SIMPLEX), MP_ROM_INT(ARM_SPI_MODE_SLAVE_SIMPLEX) },

    /* power states */
    { MP_ROM_QSTR(MP_QSTR_ARM_POWER_OFF), MP_ROM_INT(ARM_POWER_OFF) },
    { MP_ROM_QSTR(MP_QSTR_ARM_POWER_LOW), MP_ROM_INT(ARM_POWER_LOW) },
    { MP_ROM_QSTR(MP_QSTR_ARM_POWER_FULL), MP_ROM_INT(ARM_POWER_FULL) }
};

STATIC MP_DEFINE_CONST_DICT(pyb_spi_locals_dict, pyb_spi_locals_dict_table);

const mp_obj_type_t pyb_spi_type = {
    {&mp_type_type},
    .make_new = pyb_spi_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_spi_locals_dict,
};
#endif /* MICROPY_PY_SPI */