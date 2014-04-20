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
#include "i2c.h"

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
    i2c->Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    i2c->Init.ClockSpeed      = 400000;
    i2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    i2c->Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
    i2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    i2c->Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;
    i2c->Init.OwnAddress1     = 0xfe; // unused
    i2c->Init.OwnAddress2     = 0xfe; // unused

    if (HAL_I2C_Init(i2c) != HAL_OK) {
        // init error
        // TODO should raise an exception, but this function is not necessarily going to be
        // called via Python, so may not be properly wrapped in an NLR handler
        printf("HardwareError: HAL_I2C_Init failed\n");
        return;
    }
}

/******************************************************************************/
/* Micro Python bindings                                                      */

typedef struct _pyb_i2c_obj_t {
    mp_obj_base_t base;
    I2C_HandleTypeDef *i2c;
} pyb_i2c_obj_t;

STATIC const pyb_i2c_obj_t pyb_i2c_obj[] = {
#if MICROPY_HW_ENABLE_I2C1
    {{&pyb_i2c_type}, &I2CHandle1},
#else
    {{&pyb_i2c_type}, NULL},
#endif
    {{&pyb_i2c_type}, &I2CHandle2}
};
#define PYB_NUM_I2C (sizeof(pyb_i2c_obj) / sizeof(pyb_i2c_obj[0]))

STATIC mp_obj_t pyb_i2c_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get i2c number
    machine_int_t i2c_id = mp_obj_get_int(args[0]) - 1;

    // check i2c number
    if (!(0 <= i2c_id && i2c_id < PYB_NUM_I2C && pyb_i2c_obj[i2c_id].i2c != NULL)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "I2C bus %d does not exist", i2c_id + 1));
    }

    // get i2c object
    const pyb_i2c_obj_t *i2c_obj = &pyb_i2c_obj[i2c_id];

    // start the peripheral
    i2c_init(i2c_obj->i2c);

    return (mp_obj_t)i2c_obj;
}

// Check if an I2C device responds to the given address.
STATIC mp_obj_t pyb_i2c_is_ready(mp_obj_t self_in, mp_obj_t i2c_addr_o) {
    pyb_i2c_obj_t *self = self_in;
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

STATIC mp_obj_t pyb_i2c_read(mp_obj_t self_in, mp_obj_t i2c_addr_in, mp_obj_t n_in) {
    pyb_i2c_obj_t *self = self_in;
    machine_uint_t i2c_addr = mp_obj_get_int(i2c_addr_in) << 1;
    machine_uint_t n = mp_obj_get_int(n_in);

    byte *data;
    mp_obj_t o = mp_obj_str_builder_start(&mp_type_bytes, n, &data);
    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(self->i2c, i2c_addr, data, n, 500);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_I2C_Master_Receive failed with code %d", status));
    }

    return mp_obj_str_builder_end(o);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_i2c_read_obj, pyb_i2c_read);

STATIC mp_obj_t pyb_i2c_write(mp_obj_t self_in, mp_obj_t i2c_addr_in, mp_obj_t data_in) {
    pyb_i2c_obj_t *self = self_in;
    machine_uint_t i2c_addr = mp_obj_get_int(i2c_addr_in) << 1;
    HAL_StatusTypeDef status;
    if (MP_OBJ_IS_INT(data_in)) {
        uint8_t data[1] = {mp_obj_get_int(data_in)};
        status = HAL_I2C_Master_Transmit(self->i2c, i2c_addr, data, 1, 500);
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);
        status = HAL_I2C_Master_Transmit(self->i2c, i2c_addr, bufinfo.buf, bufinfo.len, 500);
    }

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_I2C_Master_Transmit failed with code %d", status));
    }

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_i2c_write_obj, pyb_i2c_write);

STATIC mp_obj_t pyb_i2c_mem_read(uint n_args, const mp_obj_t *args) {
    pyb_i2c_obj_t *self = args[0];
    machine_uint_t i2c_addr = mp_obj_get_int(args[1]) << 1;
    machine_uint_t mem_addr = mp_obj_get_int(args[2]);
    machine_uint_t n = mp_obj_get_int(args[3]);

    byte *data;
    mp_obj_t o = mp_obj_str_builder_start(&mp_type_bytes, n, &data);
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(self->i2c, i2c_addr, mem_addr, I2C_MEMADD_SIZE_8BIT, data, n, 200);

    //printf("Read got %d\n", status);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_I2C_Mem_Read failed with code %d", status));
    }

    return mp_obj_str_builder_end(o);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_i2c_mem_read_obj, 4, 4, pyb_i2c_mem_read);

STATIC mp_obj_t pyb_i2c_mem_write(uint n_args, const mp_obj_t *args) {
    pyb_i2c_obj_t *self = args[0];
    machine_uint_t i2c_addr = mp_obj_get_int(args[1]) << 1;
    machine_uint_t mem_addr = mp_obj_get_int(args[2]);
    HAL_StatusTypeDef status;
    if (MP_OBJ_IS_INT(args[3])) {
        uint8_t data[1] = {mp_obj_get_int(args[3])};
        status = HAL_I2C_Mem_Write(self->i2c, i2c_addr, mem_addr, I2C_MEMADD_SIZE_8BIT, data, 1, 200);
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
        status = HAL_I2C_Mem_Write(self->i2c, i2c_addr, mem_addr, I2C_MEMADD_SIZE_8BIT, bufinfo.buf, bufinfo.len, 200);
    }

    //printf("Write got %d\n", status);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_I2C_Mem_Write failed with code %d", status));
    }

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_i2c_mem_write_obj, 4, 4, pyb_i2c_mem_write);

STATIC const mp_map_elem_t pyb_i2c_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_is_ready), (mp_obj_t)&pyb_i2c_is_ready_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan), (mp_obj_t)&pyb_i2c_scan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&pyb_i2c_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&pyb_i2c_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_read), (mp_obj_t)&pyb_i2c_mem_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_write), (mp_obj_t)&pyb_i2c_mem_write_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_i2c_locals_dict, pyb_i2c_locals_dict_table);

const mp_obj_type_t pyb_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .make_new = pyb_i2c_make_new,
    .locals_dict = (mp_obj_t)&pyb_i2c_locals_dict,
};
