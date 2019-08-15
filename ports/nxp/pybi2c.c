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

#include "pybi2c.h"

typedef union _lpi2c_config
{
    hal_i2c_master_config_t master_config;
    hal_i2c_slave_config_t slave_config;
}lpi2c_config;

typedef union _lpi2c_handle_t
{
    hal_i2c_master_handle_t master_handle;
    hal_i2c_slave_handle_t slave_handle;
}lpi2c_handle_t;

typedef struct {
    mp_obj_base_t base;
    lpi2c_config config;
    lpi2c_handle_t handle;
}lpi2c_obj_t;

STATIC mp_obj_t Lpi2cCallbackArgs[4];

STATIC void lpi2c_master_transfer_callback(hal_i2c_master_handle_t handle, hal_i2c_status_t completionStatus, void *callbackParam)
{
    assert(handle);
    lpi2c_obj_t *lpi2c_obj = (lpi2c_obj_t *)callbackParam;
    mp_obj_t *cb = NULL;
    mp_obj_t CallbackArgs;
    if(lpi2c_obj->config.master_config.enableMaster)
    {
        cb = &MP_STATE_PORT(pyb_lpi2c_callback)[lpi2c_obj->config.master_config.instance];
        CallbackArgs = Lpi2cCallbackArgs[lpi2c_obj->config.master_config.instance];
    }
    if(*cb != mp_const_none)
    {
        nlr_buf_t nlr;
        if(nlr_push(&nlr) == 0)
        {
            mp_call_function_1(*cb, CallbackArgs);
            nlr_pop();
        }
    }
}

STATIC void lpi2c_slave_transfer_callback(hal_i2c_slave_handle_t handle, hal_i2c_slave_transfer_t *transfer, void *callbackParam)
{
    assert(handle);

    lpi2c_obj_t *lpi2c_obj = (lpi2c_obj_t *)callbackParam;
    mp_obj_t *cb = NULL;
    mp_obj_t CallbackArgs;
    if(lpi2c_obj->config.slave_config.enableSlave)
    {
        cb = &MP_STATE_PORT(pyb_lpi2c_callback)[lpi2c_obj->config.slave_config.instance];
        CallbackArgs = Lpi2cCallbackArgs[lpi2c_obj->config.slave_config.instance];
    }
    if(*cb != mp_const_none)
    {
        nlr_buf_t nlr;
        if(nlr_push(&nlr) == 0)
        {
            mp_call_function_1(*cb, CallbackArgs);
            nlr_pop();
        }
    }
}


STATIC const mp_arg_t pyb_lpi2c_make_new_args[] = {
    { MP_QSTR_instance, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_Baudrate_Address, MP_ARG_REQUIRED|MP_ARG_INT, {.u_int = 0} },  
    { MP_QSTR_srcclockHz, MP_ARG_REQUIRED|MP_ARG_INT, {.u_int = 0} },  
};

#define PYB_LPI2C_MAKE_NEW_NUM_ARGS MP_ARRAY_SIZE(pyb_lpi2c_make_new_args)

/*
 * construct 
 * param: mode = MASTER, instance = 0,
 */
STATIC mp_obj_t pyb_lpi2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_val_t vals[PYB_LPI2C_MAKE_NEW_NUM_ARGS];
    printf("n_args is %d n_kw is %d\r\n",n_args, n_kw);
    mp_arg_parse_all_kw_array(n_args, n_kw, args, PYB_LPI2C_MAKE_NEW_NUM_ARGS, pyb_lpi2c_make_new_args, vals);

    lpi2c_obj_t* self = m_new_obj(lpi2c_obj_t);
    uint8_t instance = vals[0].u_int;

    self->base.type = type;
    if(vals[1].u_int == MASTER)
    {
        self->config.master_config.enableMaster = true;
        self->config.master_config.baudRate_Bps = vals[2].u_int;
        self->config.master_config.srcClock_Hz = vals[3].u_int;
        self->config.master_config.instance = instance;

        hal_i2c_master_handle_t master_handle = m_malloc(sizeof(hal_i2c_master_handle_t));
        self->handle.master_handle = master_handle;
    }
    else if(vals[1].u_int == SLAVE)
    {
        self->config.slave_config.enableSlave = true;
        self->config.slave_config.slaveAddress = vals[2].u_int;
        self->config.slave_config.srcClock_Hz = vals[3].u_int;
        self->config.slave_config.instance = instance;

        hal_i2c_slave_handle_t slave_handle = m_malloc(sizeof(hal_i2c_slave_handle_t));
        self->handle.slave_handle = slave_handle;
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_lpi2c_init(mp_obj_t self_in)
{
    lpi2c_obj_t* self = MP_OBJ_TO_PTR(self_in);
    uint8_t i = 0;

    if(self->config.master_config.enableMaster)
    {
        /* master mode */
        HAL_I2cMasterInit(self->handle.master_handle, &(self->config.master_config));
    }
    else if(self->config.slave_config.enableSlave)
    {
        /* slave mode */
        HAL_I2cSlaveInit(self->handle.slave_handle, &(self->config.slave_config));
    }

    for(i=0; i<5; i++)
    {
        if(MP_STATE_PORT(pyb_lpi2c_callback)[i] == mp_const_none)
            continue;
        MP_STATE_PORT(pyb_lpi2c_callback)[i] = mp_const_none;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_lpi2c_init_obj, pyb_lpi2c_init);

STATIC mp_obj_t pyb_lpi2c_deinit(mp_obj_t self_in)
{
    lpi2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if(self->config.master_config.enableMaster)
    {
        /* master mode */
        HAL_I2cMasterDeinit(self->handle.master_handle);
    }
    else if(self->config.slave_config.enableSlave)
    {
        /* slave mode */
        HAL_I2cSlaveDeinit(self->handle.slave_handle);
    }
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_lpi2c_deinit_obj, pyb_lpi2c_deinit);

STATIC mp_obj_t pyb_lpi2c_install_callback(mp_obj_t self_in, mp_obj_t callback, mp_obj_t data)
{
    lpi2c_obj_t* self = MP_OBJ_TO_PTR(self_in);
    hal_i2c_status_t status = kStatus_HAL_I2cSuccess;

    if(self->config.master_config.enableMaster)
    {
        /* master mode */
        MP_STATE_PORT(pyb_lpi2c_callback)[self->config.master_config.instance] = callback;
        Lpi2cCallbackArgs[self->config.master_config.instance] = data;
        status = HAL_I2cMasterTransferInstallCallback(self->handle.master_handle, lpi2c_master_transfer_callback, self);
    }
    else if(self->config.slave_config.enableSlave)
    {
        /* slave mode */
        MP_STATE_PORT(pyb_lpi2c_callback)[self->config.slave_config.instance] = callback;
        Lpi2cCallbackArgs[self->config.slave_config.instance] = data;
        status = HAL_I2cSlaveTransferInstallCallback(self->handle.slave_handle, lpi2c_slave_transfer_callback, self);
    }

    if(status != kStatus_HAL_I2cSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError,"Fail to install callback");
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_lpi2c_install_callback_obj, pyb_lpi2c_install_callback);

STATIC const mp_arg_t master_transfer_allow_args[] = {
    { MP_QSTR_flags, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = LPI2C_TransferDefaultFlag} },
    { MP_QSTR_slaveAddress, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0x00U} },
    { MP_QSTR_direction, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = LPI2C_Write } },
    { MP_QSTR_subaddress, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0x00U} },
    { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_data_size, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_transfer_mode, MP_ARG_REQUIRED | MP_ARG_BOOL, {.u_bool = false} },
};

/* 
 * instance method master_tansfer
 * param:
 *  flags 
 *  slaveAddress 
 *  direction 
 *  subaddress 
 *  data 
 *  datasize 
 *  transfermode: 
 *             true --> noblocking
 *             false --> blocking
 */
STATIC mp_obj_t pyb_lpi2c_master_transfer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    lpi2c_obj_t* self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(master_transfer_allow_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(master_transfer_allow_args), master_transfer_allow_args, args);

    hal_i2c_status_t status;

    if(self->config.master_config.enableMaster)
    {
        /* master mode */
        hal_i2c_master_transfer_t master_xfer;
        master_xfer.flags = args[0].u_int;
        master_xfer.slaveAddress = args[1].u_int;
        master_xfer.direction = args[2].u_int;
        master_xfer.subaddress = args[3].u_int;
        master_xfer.data = MP_OBJ_TO_PTR(args[4].u_obj);
        master_xfer.dataSize = args[5].u_int;
        master_xfer.subaddressSize = 1;

        if(args[6].u_bool)
        {
            /* make sure the related IRQ has been enabled */
            status = HAL_I2cMasterTransferNonBlocking(self->handle.master_handle, &master_xfer);
        }
        else if(!args[6].u_bool)
        {
            status = HAL_I2cMasterTransferBlocking(self->handle.master_handle, &master_xfer);
        }
    }
    else
    {
        mp_raise_msg(&mp_type_ValueError, "The selected LPI2C work on slave mode!");
    }
    if (status != kStatus_HAL_I2cSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError, "Master Transfer Failed");
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_lpi2c_master_transfer_obj, 7, pyb_lpi2c_master_transfer);

STATIC const mp_arg_t slave_transfer_args[]={
    { MP_QSTR_event, MP_ARG_REQUIRED|MP_ARG_INT, {.u_int = 0x01U} },
};

STATIC mp_obj_t pyb_lpi2c_slave_tranfer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    lpi2c_obj_t* self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(slave_transfer_args)];    
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(slave_transfer_args), slave_transfer_args, args);

    uint32_t eventMask = mp_obj_get_int(&args[0].u_int);
    hal_i2c_status_t status = kStatus_HAL_I2cSuccess;
    if(self->config.slave_config.enableSlave)
    {
        status = HAL_I2cSlaveTransferNonBlocking(self->handle.slave_handle, eventMask);
    }
    if(status != kStatus_HAL_I2cSuccess)
        mp_raise_msg(&mp_type_SyntaxError, "Slave Transfer Failed");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_lpi2c_slave_transfer_obj, 1, pyb_lpi2c_slave_tranfer);

STATIC mp_obj_t pyb_lpi2c_TransferGetCount(mp_obj_t self_in)
{
    lpi2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t count;

    hal_i2c_status_t status = kStatus_HAL_I2cSuccess;
    if(self->config.master_config.enableMaster)
    {
        /* master mode */
        status = HAL_I2cMasterTransferGetCount(self->handle.master_handle, &count);
    }
    else if(self->config.slave_config.enableSlave)
    {
        /* slave mode */
        status = HAL_I2cSlaveTransferGetCount(self->handle.slave_handle, &count);
    }
    if(status != kStatus_HAL_I2cSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError, "Get Transfer Count Failed");
    }
    return MP_OBJ_TO_PTR(count);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_lpi2c_TransferGetCount_obj, pyb_lpi2c_TransferGetCount);

STATIC mp_obj_t pyb_lpi2c_TransferAbort(mp_obj_t self_in)
{
    lpi2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    hal_i2c_status_t status = kStatus_HAL_I2cSuccess;
    if(self->config.master_config.enableMaster)
    {
        /* master mode */
        status = HAL_I2cMasterTransferAbort(self->handle.master_handle);
    }
    else if(self->config.slave_config.enableSlave)
    {
        /* slave mode */
        status = HAL_I2cSlaveTransferAbort(self->handle.slave_handle);
    }
    if(status == kStatus_HAL_I2cSuccess)
    {
        mp_raise_msg(&mp_type_SyntaxError, "Transfer Abort Failed.");
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_lpi2c_TransferAbort_obj, pyb_lpi2c_TransferAbort);

STATIC const mp_rom_map_elem_t pyb_lpi2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_Init), MP_ROM_PTR(&pyb_lpi2c_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_Deinit), MP_ROM_PTR(&pyb_lpi2c_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_InstallCallback), MP_ROM_PTR(&pyb_lpi2c_install_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_MasterTransfer), MP_ROM_PTR(&pyb_lpi2c_master_transfer_obj) },
    { MP_ROM_QSTR(MP_QSTR_SlaveTransfer), MP_ROM_PTR(&pyb_lpi2c_slave_transfer_obj) },
    { MP_ROM_QSTR(MP_QSTR_TransferAbort), MP_ROM_PTR(&pyb_lpi2c_TransferAbort_obj) },
    { MP_ROM_QSTR(MP_QSTR_TransferGetCount), MP_ROM_PTR(&pyb_lpi2c_TransferGetCount_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_master), MP_ROM_INT(MASTER) },
    { MP_ROM_QSTR(MP_QSTR_slave), MP_ROM_INT(SLAVE) },
    { MP_ROM_QSTR(MP_QSTR_I2cWrite), MP_ROM_INT(kHAL_I2cWrite) },
    { MP_ROM_QSTR(MP_QSTR_I2cRead), MP_ROM_INT(kHAL_I2cRead) },
    { MP_ROM_QSTR(MP_QSTR_SlaveAddressMatchEvent), MP_ROM_INT(kHAL_I2cSlaveAddressMatchEvent) },
    { MP_ROM_QSTR(MP_QSTR_SlaveTransmitEvent), MP_ROM_INT(kHAL_I2cSlaveTransmitEvent) },
    { MP_ROM_QSTR(MP_QSTR_SlaveReceiveEvent), MP_ROM_INT(kHAL_I2cSlaveReceiveEvent) },
    { MP_ROM_QSTR(MP_QSTR_SlaveTransmitAckEvent), MP_ROM_INT(kHAL_I2cSlaveTransmitAckEvent) },
    { MP_ROM_QSTR(MP_QSTR_SlaveCompletionEvent), MP_ROM_INT(kHAL_I2cSlaveCompletionEvent) },
    { MP_ROM_QSTR(MP_QSTR_SlaveStartEvent), MP_ROM_INT(kHAL_I2cSlaveStartEvent) },
    { MP_ROM_QSTR(MP_QSTR_SlaveGenaralcallEvent), MP_ROM_INT(kHAL_I2cSlaveGenaralcallEvent) },
    { MP_ROM_QSTR(MP_QSTR_SlaveAllEvent), MP_ROM_INT(kHAL_I2cSlaveAllEvents) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_lpi2c_dict, pyb_lpi2c_locals_dict_table);


const mp_obj_type_t pyb_i2c_type = {
    {&mp_type_type},
    .name = MP_QSTR_LPI2C,
    .make_new = pyb_lpi2c_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_lpi2c_dict,
};
