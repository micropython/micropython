#include "py/obj.h"
#include "py/runtime.h"
#include "py/mpconfig.h"

#if defined(MICROPY_PY_USART) && MICROPY_PY_USART
#include BOARD_USART_CMSIS_HEADER
#include "pybusart.h"
#include "usart.h"
#include "bufferhelper.h"

typedef struct _usart_obj
{
    mp_obj_base_t base;
    struct _ARM_DRIVER_USART* Driver_USART;
    uint8_t  instance;
}usart_obj_t;

STATIC mp_obj_t pyb_usart_make_new(const mp_obj_type_t* type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    usart_obj_t *self = m_new_obj(usart_obj_t);
    uint8_t instance;

    self->base.type = type;
    self->Driver_USART = usart_find_index(args[0], &instance);;
    self->instance = instance;
    usart_init();
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_usart_install_callback(mp_obj_t self_in, mp_obj_t callback)
{
    usart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    MP_STATE_PORT(pyb_usart_callback)[self->instance] = callback;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_usart_install_callback_obj, pyb_usart_install_callback);

STATIC mp_obj_t pyb_usart_Initialize(mp_obj_t self_in)
{
    usart_obj_t* self = MP_OBJ_TO_PTR(self_in);

    (self->Driver_USART)->Initialize(usart_find_callback(self->instance));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_usart_Initialize_obj, pyb_usart_Initialize);

STATIC mp_obj_t pyb_usart_Uninitialize(mp_obj_t self_in)
{
    usart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (self->Driver_USART)->Uninitialize();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_usart_Uninitialize_obj, pyb_usart_Uninitialize);

STATIC mp_obj_t pyb_usart_PowerControl(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_state, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    };
    usart_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    (self->Driver_USART)->PowerControl(args[0].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usart_PowerControl_obj, 1, pyb_usart_PowerControl);

STATIC mp_obj_t pyb_usart_Send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_TxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 0} },
    };
    usart_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    /* get the buffer to send from */
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[0].u_obj, &bufinfo, data);
    (self->Driver_USART)->Send(bufinfo.buf, args[0].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usart_Send_obj, 3, pyb_usart_Send);

STATIC mp_obj_t pyb_usart_Receive(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_RxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 0} },
    };
    usart_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    vstr_t retstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(args[0].u_obj, &retstr);
    (self->Driver_USART)->Receive(retstr.buf, args[1].u_int);

    if(o_ret != MP_OBJ_NULL)
    {
        return o_ret;
    }
    else
    {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &retstr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usart_Receive_obj, 1, pyb_usart_Receive);

STATIC mp_obj_t pyb_usart_Transfer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_TxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_RxData, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 0} },
    };
    usart_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
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

    (self->Driver_USART)->Transfer(bufinfo_send.buf, bufinfo_recv.buf, args[2].u_int);
    if (o_ret != MP_OBJ_NULL) 
    {
        return o_ret;
    } 
    else 
    {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr_recv);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usart_Transfer_obj, 1, pyb_usart_Transfer);

STATIC mp_obj_t pyb_usart_GetTxCount(mp_obj_t self_in)
{
    usart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t TxCount;

    TxCount = (self->Driver_USART)->GetTxCount();
    return mp_obj_new_int(TxCount);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_usart_GetTxCount_obj, pyb_usart_GetTxCount);

STATIC mp_obj_t pyb_usart_GetRxCount(mp_obj_t self_in)
{
    usart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t RxCount;

    RxCount = (self->Driver_USART)->GetRxCount();
    return mp_obj_new_int(RxCount);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_usart_GetRxCount_obj, pyb_usart_GetRxCount);

STATIC mp_obj_t pyb_usart_Control(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_control_code, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_arg, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    };
    usart_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    (self->Driver_USART)->Control(args[0].u_int, args[1].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usart_Control_obj, 1, pyb_usart_Control);

STATIC mp_obj_t pyb_usart_GetStatus(mp_obj_t self_in)
{
    usart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ARM_USART_STATUS status;
    mp_obj_t statusList;

    statusList = mp_obj_new_list(0,NULL);
    status = (self->Driver_USART)->GetStatus();
    if(status.tx_busy)
    {
        mp_obj_list_append(statusList, mp_obj_new_str("Transmitter busy", strlen("Transmitter busy")));
    }
    if(status.rx_busy)
    {
        mp_obj_list_append(statusList, mp_obj_new_str("Receiver busy", strlen("Receiver busy")));
    }
    if(status.tx_underflow)
    {
        mp_obj_list_append(statusList, mp_obj_new_str("Transmit data underflow", strlen("Transmit data underflow")));
    }
    if(status.rx_overflow)
    {
        mp_obj_list_append(statusList, mp_obj_new_str("Receive data overflow", strlen("Receive data overflow")));
    }
    if(status.rx_break)
    {
        mp_obj_list_append(statusList, mp_obj_new_str("Break detect on receive", strlen("Break detect on receive")));
    }
    if(status.rx_framing_error)
    {
        mp_obj_list_append(statusList, mp_obj_new_str("Framing error detected on receive", strlen("Framing error detected on receive")));
    }
    if(status.rx_parity_error)
    {
        mp_obj_list_append(statusList, mp_obj_new_str("Parity error detected on receive", strlen("Parity error detected on receive")));
    }
    return statusList;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_usart_GetStatus_obj, pyb_usart_GetStatus);

STATIC mp_obj_t pyb_usart_GetModemStatus(mp_obj_t self_in)
{
    usart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ARM_USART_MODEM_STATUS status;
    mp_obj_t status_dict = mp_obj_new_dict(0);

    status = (self->Driver_USART)->GetModemStatus();
    if(status.cts)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("cts", strlen("cts")), mp_obj_new_str("Active", strlen("Active")));
    }
    else if(!status.cts)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("cts", strlen("cts")), mp_obj_new_str("Inactive", strlen("Inactive")));
    }
    if(status.dsr)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("dsr", strlen("dsr")), mp_obj_new_str("Active", strlen("Active")));
    }
    else if(!status.dsr)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("dsr", strlen("dsr")), mp_obj_new_str("Inactive", strlen("Inactive")));
    }
    if(status.dcd)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("dcd", strlen("dcd")), mp_obj_new_str("Active", strlen("Active")));
    }
    else if (!status.dcd)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("dcd", strlen("dcd")), mp_obj_new_str("Inactive", strlen("Inactive")));
    }
    if(status.ri)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("ri", strlen("ri")), mp_obj_new_str("Active", strlen("Active")));
    }
    else if(!status.ri)
    {
        mp_obj_dict_store(status_dict, mp_obj_new_str("ri", strlen("ri")), mp_obj_new_str("Inactive", strlen("Inactive")));
    }
    mp_obj_print(status_dict, PRINT_STR);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_usart_GetModemStatus_obj, pyb_usart_GetModemStatus);

STATIC const mp_rom_map_elem_t pyb_usart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_InstallCallback), MP_ROM_PTR(&pyb_usart_install_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_Initialize), MP_ROM_PTR(&pyb_usart_Initialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_Uninitialize), MP_ROM_PTR(&pyb_usart_Uninitialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_PowerControl), MP_ROM_PTR(&pyb_usart_PowerControl_obj) },
    { MP_ROM_QSTR(MP_QSTR_Send), MP_ROM_PTR(&pyb_usart_Send_obj) },
    { MP_ROM_QSTR(MP_QSTR_Receive), MP_ROM_PTR(&pyb_usart_Receive_obj) },
    { MP_ROM_QSTR(MP_QSTR_Transfer), MP_ROM_PTR(&pyb_usart_Transfer_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetTxCount), MP_ROM_PTR(&pyb_usart_GetTxCount_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetRxCount), MP_ROM_PTR(&pyb_usart_GetRxCount_obj) },
    { MP_ROM_QSTR(MP_QSTR_Control), MP_ROM_PTR(&pyb_usart_Control_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetStatus), MP_ROM_PTR(&pyb_usart_GetStatus_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetModemStatus), MP_ROM_PTR(&pyb_usart_GetModemStatus_obj) },

    /* class constants */
    /* SPI control mode */
    { MP_ROM_QSTR(MP_QSTR_ASYNCHRONOUS), MP_ROM_INT(ARM_USART_MODE_ASYNCHRONOUS) },
    { MP_ROM_QSTR(MP_QSTR_CONTROL_TX), MP_ROM_INT(ARM_USART_CONTROL_TX) },
    { MP_ROM_QSTR(MP_QSTR_CONTROL_RX), MP_ROM_INT(ARM_USART_CONTROL_RX) },
    { MP_ROM_QSTR(MP_QSTR_DATA_BITS_7), MP_ROM_INT(ARM_USART_DATA_BITS_7) },
    { MP_ROM_QSTR(MP_QSTR_DATA_BITS_8), MP_ROM_INT(ARM_USART_DATA_BITS_8) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_NONE), MP_ROM_INT(ARM_USART_PARITY_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_EVEN), MP_ROM_INT(ARM_USART_PARITY_EVEN) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_ODD), MP_ROM_INT(ARM_USART_PARITY_ODD) },
    { MP_ROM_QSTR(MP_QSTR_STOP_BITS_1), MP_ROM_INT(ARM_USART_STOP_BITS_1) },
    { MP_ROM_QSTR(MP_QSTR_STOP_BITS_2), MP_ROM_INT(ARM_USART_STOP_BITS_2) },
    { MP_ROM_QSTR(MP_QSTR_ABORT_SEND), MP_ROM_INT(ARM_USART_ABORT_SEND) },
    { MP_ROM_QSTR(MP_QSTR_ABORT_RECEIVE), MP_ROM_QSTR(ARM_USART_ABORT_RECEIVE) },

    /* power states */
    { MP_ROM_QSTR(MP_QSTR_ARM_POWER_OFF), MP_ROM_INT(ARM_POWER_OFF) },
    { MP_ROM_QSTR(MP_QSTR_ARM_POWER_LOW), MP_ROM_INT(ARM_POWER_LOW) },
    { MP_ROM_QSTR(MP_QSTR_ARM_POWER_FULL), MP_ROM_INT(ARM_POWER_FULL) }
};
STATIC MP_DEFINE_CONST_DICT(pyb_usart_locals_dict, pyb_usart_locals_dict_table);

const mp_obj_type_t pyb_usart_type = {
    {&mp_type_type},
    .make_new = pyb_usart_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_usart_locals_dict,
};

#endif /* MICROPY_PY_USART */