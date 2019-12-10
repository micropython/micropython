/* The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "mpconfigport.h"

//Headers of ESP-IDF library
#include "soc/dport_reg.h"
#include "driver/can.h"
#include "esp_err.h"
#include "esp_log.h"

#include <machine_can.h>

//#if MICROPY_HW_ENABLE_CAN

// Internal Functions
STATIC can_state_t _machine_hw_can_get_state();


//Python object definition
const mp_obj_type_t machine_can_type = {
    { &mp_type_type },
    .name = MP_QSTR_CAN,
    .print = machine_hw_can_print, // give it a print-function
    .make_new = machine_hw_can_make_new,  // give it a constructor
    .locals_dict = (mp_obj_dict_t*)&machine_can_locals_dict, // and the global members
};

// singleton CAN device object
machine_can_config_t can_config = {.general = &((can_general_config_t)CAN_GENERAL_CONFIG_DEFAULT(2,4,0)),
                                   .filter = &((can_filter_config_t)CAN_FILTER_CONFIG_ACCEPT_ALL()),
                                   .timing = &((can_timing_config_t)CAN_TIMING_CONFIG_25KBITS()),
                                   .initialized = false};
                                   
STATIC const machine_can_obj_t machine_can_obj = {{&machine_can_type}, .config=&can_config};

//Return status information
STATIC can_status_info_t _machine_hw_can_get_status(){
    can_status_info_t status;
    if(can_get_status_info(&status)!=ESP_OK){
        mp_raise_ValueError("Unable to get CAN status");
    }
    return status;
}

// Force a software restart of the controller, to allow transmission after a bus error
STATIC mp_obj_t machine_hw_can_restart(mp_obj_t self_in){
    mp_raise_NotImplementedError(""); //TODO: Implement this function
    return self_in;
}

// any(fifo) - return `True` if any message waiting on the FIFO, else `False`
STATIC mp_obj_t machine_hw_can_any(mp_obj_t self_in, mp_obj_t fifo_in){
    can_status_info_t status = _machine_hw_can_get_status();
    if(mp_obj_new_int(status.msgs_to_rx)>0){
        return mp_const_true;
    }else{
        return mp_const_false;
    }
}

// Get the state of the controller
STATIC mp_obj_t machine_hw_can_state(mp_obj_t self_in) {
    can_status_info_t status = _machine_hw_can_get_status();
    return mp_obj_new_int(status.state);
}

STATIC can_state_t _machine_hw_can_get_state(){
    can_status_info_t status = _machine_hw_can_get_status();
    return status.state;
}

// Get info about error states and TX/RX buffers
STATIC mp_obj_t machine_hw_can_info(size_t n_args, const mp_obj_t *args) {
    //machine_can_obj_t *self = MP_OBJ_TO_PTR(args[0]); //FIXME: Remove it if useless
    mp_obj_list_t *list;
    if (n_args == 1) {
        list = MP_OBJ_TO_PTR(mp_obj_new_list(8, NULL));
    } else {
        if (!mp_obj_is_type(args[1], &mp_type_list)) {
            mp_raise_TypeError(NULL);
        }
        list = MP_OBJ_TO_PTR(args[1]);
        if (list->len < 8) {
            mp_raise_ValueError(NULL);
        }
    }
    can_status_info_t status = _machine_hw_can_get_status();
    list->items[0] = MP_OBJ_NEW_SMALL_INT(status.tx_error_counter);
    list->items[1] = MP_OBJ_NEW_SMALL_INT(status.rx_error_counter);
    //list->items[2] = MP_OBJ_NEW_SMALL_INT(self->num_error_warning); //FIXME:
    //list->items[3] = MP_OBJ_NEW_SMALL_INT(self->num_error_passive); //FIXME:
    //list->items[4] = MP_OBJ_NEW_SMALL_INT(self->num_bus_off); //FIXME:
    list->items[5] = MP_OBJ_NEW_SMALL_INT(status.msgs_to_tx);
    //list->items[6] = MP_OBJ_NEW_SMALL_INT(can->RF0R >> CAN_RF0R_FMP0_Pos & 3); //FIXME:
    //list->items[7] = MP_OBJ_NEW_SMALL_INT(can->RF1R >> CAN_RF1R_FMP1_Pos & 3); //FIXME:
    return MP_OBJ_FROM_PTR(list);
}

//Clear TX Queue
STATIC mp_obj_t machine_hw_can_clear_tx_queue(mp_obj_t self_in){
    return mp_obj_new_bool(can_clear_transmit_queue()==ESP_OK);
}
//Clear RX Queue
STATIC mp_obj_t machine_hw_can_clear_rx_queue(mp_obj_t self_in){
    return mp_obj_new_bool(can_clear_receive_queue()==ESP_OK);
}

//send([data], id, timeout=0, rtr=False, self_flag=False)
STATIC mp_obj_t machine_hw_can_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_data, ARG_id, ARG_timeout, ARG_rtr, ARG_self };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_id,      MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_rtr,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_self_flag,  MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    // populate message
    size_t length;
    mp_obj_t *items;
    mp_obj_get_array(args[ARG_data].u_obj,&length, &items);
    if (length>8){
        mp_raise_ValueError("CAN data field too long");
    }
    uint8_t flags = (args[ARG_rtr].u_bool==true ? CAN_MSG_FLAG_RTR : CAN_MSG_FLAG_NONE);
    if (args[ARG_id].u_int>0x7ff){
        flags += CAN_MSG_FLAG_EXTD;
    }
    if (args[ARG_self].u_bool){
        flags += CAN_MSG_FLAG_SELF;
    }
    
    can_message_t tx_msg = {.data_length_code = length, 
                            .identifier = args[ARG_id].u_int & 0x1FFFFFFF, 
                            .flags = flags};
    for (uint8_t i=0; i<length; i++ ){
        tx_msg.data[i] = mp_obj_get_int(items[i]);
    }
    if (_machine_hw_can_get_state()==CAN_STATE_RUNNING){
        switch(can_transmit(&tx_msg, args[ARG_timeout].u_int)){
            case ESP_ERR_INVALID_ARG: ESP_LOGW(DEVICE_NAME, "Arguments are invalid"); break;
            case ESP_ERR_TIMEOUT: //TX Queue is full
            case ESP_FAIL: ESP_LOGW(DEVICE_NAME, "Message transmission failed"); break;
            case ESP_ERR_INVALID_STATE: ESP_LOGW(DEVICE_NAME, "Device is not initialized"); break;
            case ESP_ERR_NOT_SUPPORTED: ESP_LOGW(DEVICE_NAME, "Listen Only Mode active"); break;
        }

        return 0; //machine_hw_can_get_tx_waiting_messages(pos_args[0]);
    }else{
        ESP_LOGW(DEVICE_NAME, "Unable to send the message");
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}

// recv(timeout=5000)
STATIC mp_obj_t machine_hw_can_recv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_timeout };
    mp_obj_t ret_obj = mp_obj_new_tuple(3, NULL);
    mp_obj_t *items = ((mp_obj_tuple_t*)MP_OBJ_TO_PTR(ret_obj))->items;
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    can_message_t rx_message;
    switch(can_receive(&rx_message, 1)){
        case ESP_OK:
            items[0] = MP_OBJ_NEW_SMALL_INT(rx_message.identifier);
            items[1] = mp_obj_new_bytes(rx_message.data, rx_message.data_length_code);
            items[2] = MP_OBJ_NEW_SMALL_INT(rx_message.flags);
            return ret_obj;
        case ESP_ERR_TIMEOUT:
            mp_raise_ValueError("Timeout");
        case ESP_ERR_INVALID_ARG:
            mp_raise_ValueError("Invalid Args");
        case ESP_ERR_INVALID_STATE:
            mp_raise_ValueError("Invalid State");
        default:
            mp_raise_ValueError("");
    };
}

STATIC void machine_hw_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->config->initialized){
        qstr mode;
        switch(self->config->general->mode){
            case CAN_MODE_LISTEN_ONLY: mode = MP_QSTR_LISTEN; break;
            case CAN_MODE_NO_ACK: mode = MP_QSTR_NO_ACK; break;
            case CAN_MODE_NORMAL: mode = MP_QSTR_NORMAL; break;
            default: mode = MP_QSTR_UNKNOWN; break;
        }
        mp_printf(print, 
                    "CAN(tx=%u, rx=%u, baudrate=%ukb, mode=%q)", 
                    self->config->general->tx_io, 
                    self->config->general->rx_io, 
                    self->config->baudrate, mode);
    }else{
        mp_printf(print, "CAN Device is not initialized");
    }
    
}

// CAN(...) No argument to get the object
// Any argument will be used to init the device through init function
mp_obj_t machine_hw_can_make_new(const mp_obj_type_t *type, size_t n_args, 
                                size_t n_kw, const mp_obj_t *args){
    const machine_can_obj_t *self =  &machine_can_obj;

    if (n_args > 0 || n_kw > 0) {
        if (self->config->initialized) {
            // The caller is requesting a reconfiguration of the hardware
            // this can only be done if the hardware is in init mode
            ESP_LOGW(DEVICE_NAME, "Device is going to be reconfigured");
            machine_hw_can_deinit(&self);
        }
        //TODO: implement callback
        /*self->rxcallback0 = mp_const_none;
        self->rxcallback1 = mp_const_none;
        self->rx_state0 = RX_STATE_FIFO_EMPTY;
        self->rx_state1 = RX_STATE_FIFO_EMPTY;*/

        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_hw_can_init_helper(self, n_args, args, &kw_args);
    }
    return MP_OBJ_FROM_PTR(self);
}

// init(tx, rx, baudrate, mode = CAN_MODE_NORMAL, tx_queue = 2, rx_queue = 5)
STATIC mp_obj_t machine_hw_can_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    const machine_can_obj_t *self =  MP_OBJ_TO_PTR(args[0]);
    if (self->config->initialized){
        ESP_LOGW(DEVICE_NAME, "Device is already initialized");
        return mp_const_false;
    }
    return machine_hw_can_init_helper(self, n_args - 1, args + 1, kw_args);
}

// INTERNAL FUNCTION init(tx, rx, baudrate, mode = CAN_MODE_NORMAL, tx_queue = 2, rx_queue = 5, filter_mask = 0xFFFFFFFF, filter_code = 0; single_filter = True)
STATIC mp_obj_t machine_hw_can_init_helper(const machine_can_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_tx, ARG_rx, ARG_baudrate, ARG_mode, ARG_tx_queue, ARG_rx_queue, ARG_filter_mask, ARG_filter_code, ARG_single_filter};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_tx,            MP_ARG_REQUIRED | MP_ARG_INT,    {.u_int = 4}                },
        { MP_QSTR_rx,            MP_ARG_REQUIRED | MP_ARG_INT,    {.u_int = 2}                },
        { MP_QSTR_baudrate,      MP_ARG_REQUIRED | MP_ARG_INT,    {.u_int = 500}              },
        { MP_QSTR_mode,          MP_ARG_INT,                      {.u_int  = CAN_MODE_NORMAL} },
        { MP_QSTR_tx_queue,      MP_ARG_INT,                      {.u_int  = 0}               },
        { MP_QSTR_rx_queue,      MP_ARG_INT,                      {.u_int  = 5}               },
        { MP_QSTR_filter_mask,   MP_ARG_INT,                      {.u_int = 0xFFFFFFFF}       },
        { MP_QSTR_filter_code,   MP_ARG_INT,                      {.u_int = 0}                },
        { MP_QSTR_single_filter, MP_ARG_BOOL,                     {.u_bool = true}            },
    };
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    //Check if device was already configured
    if (self->config->initialized){
        ESP_LOGE(DEVICE_NAME, "Device is already initialized: unable to re-init");
        return mp_const_none;
    }
    //Configure device
    can_general_config_t g_config = {.mode = args[ARG_mode].u_int,
                                    .tx_io = args[ARG_tx].u_int, 
                                    .rx_io = args[ARG_rx].u_int,
                                    .clkout_io = CAN_IO_UNUSED, 
                                    .bus_off_io = CAN_IO_UNUSED,
                                    .tx_queue_len = args[ARG_tx_queue].u_int, 
                                    .rx_queue_len = args[ARG_rx_queue].u_int,
                                    .alerts_enabled = CAN_ALERT_NONE,
                                    .clkout_divider = 0};
    self->config->general = &g_config;
    can_filter_config_t f_config = {
        .acceptance_code = args[ARG_filter_code].u_int, 
        .acceptance_mask = args[ARG_filter_mask].u_int, 
        .single_filter =  args[ARG_single_filter].u_bool
    };
    self->config->filter = &f_config;

    switch ((int)args[ARG_baudrate].u_int){
        case CAN_BAUDRATE_25k:
            self->config->timing = &((can_timing_config_t)CAN_TIMING_CONFIG_25KBITS());
            break;
        case CAN_BAUDRATE_50k:
             self->config->timing = &((can_timing_config_t)CAN_TIMING_CONFIG_50KBITS());
            break;
        case CAN_BAUDRATE_100k:
             self->config->timing = &((can_timing_config_t)CAN_TIMING_CONFIG_100KBITS());
            break;
        case CAN_BAUDRATE_125k:
             self->config->timing = &((can_timing_config_t)CAN_TIMING_CONFIG_125KBITS());
            break;
        case CAN_BAUDRATE_250k:
             self->config->timing = &((can_timing_config_t)CAN_TIMING_CONFIG_250KBITS());
            break;
        case CAN_BAUDRATE_500k:
             self->config->timing = &((can_timing_config_t)CAN_TIMING_CONFIG_500KBITS());
            break;
        case CAN_BAUDRATE_800k:
             self->config->timing = &((can_timing_config_t)CAN_TIMING_CONFIG_800KBITS());
            break;
        case CAN_BAUDRATE_1M:
             self->config->timing = &((can_timing_config_t)CAN_TIMING_CONFIG_1MBITS());
            break;
        default:
            ESP_LOGE(DEVICE_NAME, "Unable to set baudrate");
            self->config->baudrate = 0;
            return mp_const_none;
    }
    self->config->baudrate = args[ARG_baudrate].u_int;
    
    uint8_t status = can_driver_install(self->config->general, self->config->timing, self->config->filter);
    if (status != ESP_OK){
        ESP_LOGE(DEVICE_NAME, "Unable to init the device. ErrCode: %u",status);
    }else if (can_start() != ESP_OK){
        ESP_LOGE(DEVICE_NAME, "Unable to start the device. ErrCode: %u",status);
    }else{
        self->config->initialized = true;
    }
    
    return mp_const_none;
}

//deinit()
STATIC mp_obj_t machine_hw_can_deinit(const mp_obj_t self_in){
    const machine_can_obj_t *self =  &machine_can_obj;
    if (self->config->initialized != true){
        ESP_LOGW(DEVICE_NAME, "Device is not initialized");
        return mp_const_false;
    }
	if(can_stop()!=ESP_OK){
        ESP_LOGW(DEVICE_NAME, "Unable to stop device");
    }
    if (can_driver_uninstall() != ESP_OK){
        ESP_LOGW(DEVICE_NAME, "Unable to uninstall device");
        return mp_const_false;
    }
    self->config->initialized = false;
    return mp_const_true;
}

//#endif // MICROPY_HW_ENABLE_CAN