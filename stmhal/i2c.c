#include <stdio.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "bufhelper.h"
#include "i2c.h"

#define PYB_I2C_MASTER (0)
#define PYB_I2C_SLAVE  (1)

#if MICROPY_HW_ENABLE_I2C1
I2C_HandleTypeDef I2CHandle1 = {.Instance = NULL};
#endif
I2C_HandleTypeDef I2CHandle2 = {.Instance = NULL};

void i2c_init0(void) {
    // reset the I2C1 handles
#if MICROPY_HW_ENABLE_I2C1
    memset(&I2CHandle1, 0, sizeof(I2C_HandleTypeDef));
    I2CHandle1.Instance = I2C1;
#endif
    memset(&I2CHandle2, 0, sizeof(I2C_HandleTypeDef));
    I2CHandle2.Instance = I2C2;
}

void i2c_init(I2C_HandleTypeDef *i2c) {
    // init the GPIO lines
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_NOPULL; // have external pull-up resistors on both lines

    const pin_obj_t *pins[2];
    if (0) {
#if MICROPY_HW_ENABLE_I2C1
    } else if (i2c == &I2CHandle1) {
        // X-skin: X9=PB6=SCL, X10=PB7=SDA
        pins[0] = &pin_B6;
        pins[1] = &pin_B7;
        GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
        // enable the I2C clock
        __I2C1_CLK_ENABLE();
#endif
    } else if (i2c == &I2CHandle2) {
        // Y-skin: Y9=PB10=SCL, Y10=PB11=SDA
        pins[0] = &pin_B10;
        pins[1] = &pin_B11;
        GPIO_InitStructure.Alternate = GPIO_AF4_I2C2;
        // enable the I2C clock
        __I2C2_CLK_ENABLE();
    } else {
        // I2C does not exist for this board (shouldn't get here, should be checked by caller)
        return;
    }

    // init the GPIO lines
    for (uint i = 0; i < 2; i++) {
        GPIO_InitStructure.Pin = pins[i]->pin_mask;
        HAL_GPIO_Init(pins[i]->gpio, &GPIO_InitStructure);
    }

    // init the I2C device
    if (HAL_I2C_Init(i2c) != HAL_OK) {
        // init error
        // TODO should raise an exception, but this function is not necessarily going to be
        // called via Python, so may not be properly wrapped in an NLR handler
        printf("HardwareError: HAL_I2C_Init failed\n");
        return;
    }
}

void i2c_deinit(I2C_HandleTypeDef *i2c) {
    HAL_I2C_DeInit(i2c);
    if (0) {
#if MICROPY_HW_ENABLE_I2C1
    } else if (i2c->Instance == I2C1) {
        __I2C1_FORCE_RESET();
        __I2C1_RELEASE_RESET();
        __I2C1_CLK_DISABLE();
#endif
    } else if (i2c->Instance == I2C2) {
        __I2C2_FORCE_RESET();
        __I2C2_RELEASE_RESET();
        __I2C2_CLK_DISABLE();
    }
}

/******************************************************************************/
/* Micro Python bindings                                                      */

typedef struct _pyb_i2c_obj_t {
    mp_obj_base_t base;
    I2C_HandleTypeDef *i2c;
} pyb_i2c_obj_t;

STATIC inline bool in_master_mode(pyb_i2c_obj_t *self) { return self->i2c->Init.OwnAddress1 == PYB_I2C_MASTER_ADDRESS; }

STATIC const pyb_i2c_obj_t pyb_i2c_obj[] = {
#if MICROPY_HW_ENABLE_I2C1
    {{&pyb_i2c_type}, &I2CHandle1},
#else
    {{&pyb_i2c_type}, NULL},
#endif
    {{&pyb_i2c_type}, &I2CHandle2}
};
#define PYB_NUM_I2C (sizeof(pyb_i2c_obj) / sizeof(pyb_i2c_obj[0]))

STATIC void pyb_i2c_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_i2c_obj_t *self = self_in;

    uint i2c_num;
    if (self->i2c->Instance == I2C1) { i2c_num = 1; }
    else { i2c_num = 2; }

    if (self->i2c->State == HAL_I2C_STATE_RESET) {
        print(env, "I2C(%u)", i2c_num);
    } else {
        if (in_master_mode(self)) {
            print(env, "I2C(%u, I2C.MASTER, baudrate=%u)", i2c_num, self->i2c->Init.ClockSpeed);
        } else {
            print(env, "I2C(%u, I2C.SLAVE, addr=0x%02x)", i2c_num, (self->i2c->Instance->OAR1 >> 1) & 0x7f);
        }
    }
}

STATIC const mp_arg_parse_t pyb_i2c_init_accepted_args[] = {
    { MP_QSTR_mode,     MP_ARG_PARSE_REQUIRED | MP_ARG_PARSE_INT, {.u_int = 0} },
    { MP_QSTR_addr,     MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT, {.u_int = 0x12} },
    { MP_QSTR_baudrate, MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT, {.u_int = 400000} },
    { MP_QSTR_gencall,  MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_BOOL, {.u_bool = false} },
};
#define PYB_I2C_INIT_NUM_ARGS (sizeof(pyb_i2c_init_accepted_args) / sizeof(pyb_i2c_init_accepted_args[0]))

STATIC mp_obj_t pyb_i2c_init_helper(const pyb_i2c_obj_t *self, uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_parse_val_t vals[PYB_I2C_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PYB_I2C_INIT_NUM_ARGS, pyb_i2c_init_accepted_args, vals);

    // set the I2C configuration values
    I2C_InitTypeDef *init = &self->i2c->Init;

    if (vals[0].u_int == PYB_I2C_MASTER) {
        // use a special address to indicate we are a master
        init->OwnAddress1 = PYB_I2C_MASTER_ADDRESS;
    } else {
        init->OwnAddress1 = (vals[1].u_int << 1) & 0xfe;
    }

    init->AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    init->ClockSpeed      = MIN(vals[2].u_int, 400000);
    init->DualAddressMode = I2C_DUALADDRESS_DISABLED;
    init->DutyCycle       = I2C_DUTYCYCLE_16_9;
    init->GeneralCallMode = vals[3].u_bool ? I2C_GENERALCALL_ENABLED : I2C_GENERALCALL_DISABLED;
    init->NoStretchMode   = I2C_NOSTRETCH_DISABLED;
    init->OwnAddress2     = 0xfe; // unused

    // init the I2C bus
    i2c_init(self->i2c);

    return mp_const_none;
}

STATIC mp_obj_t pyb_i2c_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get i2c number
    machine_int_t i2c_id = mp_obj_get_int(args[0]) - 1;

    // check i2c number
    if (!(0 <= i2c_id && i2c_id < PYB_NUM_I2C && pyb_i2c_obj[i2c_id].i2c != NULL)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "I2C bus %d does not exist", i2c_id + 1));
    }

    // get I2C object
    const pyb_i2c_obj_t *i2c_obj = &pyb_i2c_obj[i2c_id];

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_i2c_init_helper(i2c_obj, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)i2c_obj;
}

STATIC mp_obj_t pyb_i2c_init(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_i2c_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_init_obj, 1, pyb_i2c_init);

STATIC mp_obj_t pyb_i2c_deinit(mp_obj_t self_in) {
    pyb_i2c_obj_t *self = self_in;
    i2c_deinit(self->i2c);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_deinit_obj, pyb_i2c_deinit);

// Check if an I2C device responds to the given address.
STATIC mp_obj_t pyb_i2c_is_ready(mp_obj_t self_in, mp_obj_t i2c_addr_o) {
    pyb_i2c_obj_t *self = self_in;

    if (!in_master_mode(self)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "I2C must be a master"));
    }

    machine_uint_t i2c_addr = mp_obj_get_int(i2c_addr_o) << 1;

    for (int i = 0; i < 10; i++) {
        HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(self->i2c, i2c_addr, 10, 200);
        if (status == HAL_OK) {
            return mp_const_true;
        }
    }

    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_i2c_is_ready_obj, pyb_i2c_is_ready);

// Scan all I2C addresses from 0x01 to 0x7f and return a list of those that respond.
STATIC mp_obj_t pyb_i2c_scan(mp_obj_t self_in) {
    pyb_i2c_obj_t *self = self_in;

    if (!in_master_mode(self)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "I2C must be a master"));
    }

    mp_obj_t list = mp_obj_new_list(0, NULL);

    for (uint addr = 1; addr <= 127; addr++) {
        for (int i = 0; i < 10; i++) {
            HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(self->i2c, addr << 1, 10, 200);
            if (status == HAL_OK) {
                mp_obj_list_append(list, mp_obj_new_int(addr));
                break;
            }
        }
    }

    return list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_i2c_scan_obj, pyb_i2c_scan);

STATIC const mp_arg_parse_t pyb_i2c_send_accepted_args[] = {
    { MP_QSTR_send,    MP_ARG_PARSE_REQUIRED | MP_ARG_PARSE_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_addr,    MP_ARG_PARSE_INT, {.u_int = PYB_I2C_MASTER_ADDRESS} },
    { MP_QSTR_timeout, MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT, {.u_int = 5000} },
};
#define PYB_I2C_SEND_NUM_ARGS (sizeof(pyb_i2c_send_accepted_args) / sizeof(pyb_i2c_send_accepted_args[0]))

STATIC mp_obj_t pyb_i2c_send(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_i2c_obj_t *self = args[0];

    // parse args
    mp_arg_parse_val_t vals[PYB_I2C_SEND_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_I2C_SEND_NUM_ARGS, pyb_i2c_send_accepted_args, vals);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(vals[0].u_obj, &bufinfo, data);

    // send the data
    HAL_StatusTypeDef status;
    if (in_master_mode(self)) {
        if (vals[1].u_int == PYB_I2C_MASTER_ADDRESS) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "addr argument required"));
        }
        machine_uint_t i2c_addr = vals[1].u_int << 1;
        status = HAL_I2C_Master_Transmit(self->i2c, i2c_addr, bufinfo.buf, bufinfo.len, vals[2].u_int);
    } else {
        status = HAL_I2C_Slave_Transmit(self->i2c, bufinfo.buf, bufinfo.len, vals[2].u_int);
    }

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_I2C_xxx_Transmit failed with code %d", status));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_send_obj, 1, pyb_i2c_send);

STATIC const mp_arg_parse_t pyb_i2c_recv_accepted_args[] = {
    { MP_QSTR_recv,    MP_ARG_PARSE_REQUIRED | MP_ARG_PARSE_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_addr,    MP_ARG_PARSE_INT, {.u_int = PYB_I2C_MASTER_ADDRESS} },
    { MP_QSTR_timeout, MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT, {.u_int = 5000} },
};
#define PYB_I2C_RECV_NUM_ARGS (sizeof(pyb_i2c_recv_accepted_args) / sizeof(pyb_i2c_recv_accepted_args[0]))

STATIC mp_obj_t pyb_i2c_recv(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_i2c_obj_t *self = args[0];

    // parse args
    mp_arg_parse_val_t vals[PYB_I2C_RECV_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_I2C_RECV_NUM_ARGS, pyb_i2c_recv_accepted_args, vals);

    // get the buffer to receive into
    mp_buffer_info_t bufinfo;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &bufinfo);

    // receive the data
    HAL_StatusTypeDef status;
    if (in_master_mode(self)) {
        if (vals[1].u_int == PYB_I2C_MASTER_ADDRESS) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "addr argument required"));
        }
        machine_uint_t i2c_addr = vals[1].u_int << 1;
        status = HAL_I2C_Master_Receive(self->i2c, i2c_addr, bufinfo.buf, bufinfo.len, vals[2].u_int);
    } else {
        status = HAL_I2C_Slave_Receive(self->i2c, bufinfo.buf, bufinfo.len, vals[2].u_int);
    }

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_I2C_xxx_Receive failed with code %d", status));
    }

    // return the received data
    if (o_ret == MP_OBJ_NULL) {
        return vals[0].u_obj;
    } else {
        return mp_obj_str_builder_end(o_ret);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_recv_obj, 1, pyb_i2c_recv);

STATIC const mp_arg_parse_t pyb_i2c_mem_read_accepted_args[] = {
    { MP_QSTR_data,    MP_ARG_PARSE_REQUIRED | MP_ARG_PARSE_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_addr,    MP_ARG_PARSE_REQUIRED | MP_ARG_PARSE_INT, {.u_int = 0} },
    { MP_QSTR_memaddr, MP_ARG_PARSE_REQUIRED | MP_ARG_PARSE_INT, {.u_int = 0} },
    { MP_QSTR_timeout, MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT, {.u_int = 5000} },
};
#define PYB_I2C_MEM_READ_NUM_ARGS (sizeof(pyb_i2c_mem_read_accepted_args) / sizeof(pyb_i2c_mem_read_accepted_args[0]))

STATIC mp_obj_t pyb_i2c_mem_read(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_i2c_obj_t *self = args[0];

    if (!in_master_mode(self)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "I2C must be a master"));
    }

    // parse args
    mp_arg_parse_val_t vals[PYB_I2C_MEM_READ_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_I2C_MEM_READ_NUM_ARGS, pyb_i2c_mem_read_accepted_args, vals);

    // get the buffer to read into
    mp_buffer_info_t bufinfo;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &bufinfo);

    // get the addresses
    machine_uint_t i2c_addr = vals[1].u_int << 1;
    machine_uint_t mem_addr = vals[2].u_int;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(self->i2c, i2c_addr, mem_addr, I2C_MEMADD_SIZE_8BIT, bufinfo.buf, bufinfo.len, vals[3].u_int);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_I2C_Mem_Read failed with code %d", status));
    }

    // return the read data
    if (o_ret == MP_OBJ_NULL) {
        return vals[0].u_obj;
    } else {
        return mp_obj_str_builder_end(o_ret);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_mem_read_obj, 1, pyb_i2c_mem_read);

STATIC mp_obj_t pyb_i2c_mem_write(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_i2c_obj_t *self = args[0];

    if (!in_master_mode(self)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "I2C must be a master"));
    }

    // parse args (same as mem_read)
    mp_arg_parse_val_t vals[PYB_I2C_MEM_READ_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_I2C_MEM_READ_NUM_ARGS, pyb_i2c_mem_read_accepted_args, vals);

    // get the buffer to write from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(vals[0].u_obj, &bufinfo, data);

    // get the addresses
    machine_uint_t i2c_addr = vals[1].u_int << 1;
    machine_uint_t mem_addr = vals[2].u_int;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(self->i2c, i2c_addr, mem_addr, I2C_MEMADD_SIZE_8BIT, bufinfo.buf, bufinfo.len, vals[3].u_int);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_I2C_Mem_Write failed with code %d", status));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_mem_write_obj, 1, pyb_i2c_mem_write);

STATIC const mp_map_elem_t pyb_i2c_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_i2c_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_i2c_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_is_ready), (mp_obj_t)&pyb_i2c_is_ready_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan), (mp_obj_t)&pyb_i2c_scan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&pyb_i2c_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&pyb_i2c_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_read), (mp_obj_t)&pyb_i2c_mem_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_write), (mp_obj_t)&pyb_i2c_mem_write_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_MASTER),       MP_OBJ_NEW_SMALL_INT(PYB_I2C_MASTER) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLAVE),        MP_OBJ_NEW_SMALL_INT(PYB_I2C_SLAVE) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_i2c_locals_dict, pyb_i2c_locals_dict_table);

const mp_obj_type_t pyb_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = pyb_i2c_print,
    .make_new = pyb_i2c_make_new,
    .locals_dict = (mp_obj_t)&pyb_i2c_locals_dict,
};
