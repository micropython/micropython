#include <stdio.h>
#include <string.h>

#include <stm32f4xx_hal.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "runtime.h"
#include "i2c.h"

I2C_HandleTypeDef I2cHandle_X;
I2C_HandleTypeDef I2cHandle_Y;

void i2c_init(void) {
    // init the I2C1 device
    memset(&I2cHandle_X, 0, sizeof(I2C_HandleTypeDef));
    I2cHandle_X.Instance = I2C1;

    // init the I2C2 device
    memset(&I2cHandle_Y, 0, sizeof(I2C_HandleTypeDef));
    I2cHandle_Y.Instance = I2C2;
}

void i2c_start(I2C_HandleTypeDef *i2c_handle) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // init the GPIO lines
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_NOPULL; // have external pull-up resistors on both lines

    if (i2c_handle == &I2cHandle_X) {
        // X-skin: X9=PB6=SCL, X10=PB7=SDA
        GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
        // enable the I2C clock
        __I2C1_CLK_ENABLE();
    } else {
        // Y-skin: Y9=PB10=SCL, Y10=PB11=SDA
        GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_InitStructure.Alternate = GPIO_AF4_I2C2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
        // enable the I2C clock
        __I2C2_CLK_ENABLE();
    }

    // init the I2C device
    i2c_handle->Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    i2c_handle->Init.ClockSpeed      = 400000;
    i2c_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    i2c_handle->Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
    i2c_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    i2c_handle->Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;
    i2c_handle->Init.OwnAddress1     = 0xfe; // unused
    i2c_handle->Init.OwnAddress2     = 0xfe; // unused

    if (HAL_I2C_Init(i2c_handle) != HAL_OK) {
        // init error
        printf("accel_init: HAL_I2C_Init failed\n");
        return;
    }
}

/******************************************************************************/
/* Micro Python bindings                                                      */

#define PYB_NUM_I2C (2)

typedef struct _pyb_i2c_obj_t {
    mp_obj_base_t base;
    I2C_HandleTypeDef *i2c_handle;
} pyb_i2c_obj_t;

STATIC pyb_i2c_obj_t pyb_i2c_obj[PYB_NUM_I2C] = {{{&pyb_i2c_type}, &I2cHandle_X}, {{&pyb_i2c_type}, &I2cHandle_Y}};

STATIC mp_obj_t pyb_i2c_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    rt_check_nargs(n_args, 1, 1, n_kw, false);

    // get i2c number
    machine_int_t i2c_id = mp_obj_get_int(args[0]) - 1;

    // check i2c number
    if (!(0 <= i2c_id && i2c_id < PYB_NUM_I2C)) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "I2C bus %d does not exist", i2c_id + 1));
    }

    // get i2c object
    pyb_i2c_obj_t *i2c_obj = &pyb_i2c_obj[i2c_id];

    // start the peripheral
    i2c_start(i2c_obj->i2c_handle);

    return &pyb_i2c_obj;
}

STATIC mp_obj_t pyb_i2c_is_ready(mp_obj_t self_in, mp_obj_t i2c_addr_o) {
    pyb_i2c_obj_t *self = self_in;
    machine_uint_t i2c_addr = mp_obj_get_int(i2c_addr_o) << 1;

    //printf("IsDeviceReady\n");
    for (int i = 0; i < 10; i++) {
        HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(self->i2c_handle, i2c_addr, 10, 200);
        //printf("  got %d\n", status);
        if (status == HAL_OK) {
            return mp_const_true;
        }
    }

    return mp_const_false;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_i2c_is_ready_obj, pyb_i2c_is_ready);

STATIC mp_obj_t pyb_i2c_mem_read(uint n_args, const mp_obj_t *args) {
    pyb_i2c_obj_t *self = args[0];
    machine_uint_t i2c_addr = mp_obj_get_int(args[1]) << 1;
    machine_uint_t mem_addr = mp_obj_get_int(args[2]);
    machine_uint_t n = mp_obj_get_int(args[3]);

    byte *data;
    mp_obj_t o = mp_obj_str_builder_start(&bytes_type, n, &data);
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(self->i2c_handle, i2c_addr, mem_addr, I2C_MEMADD_SIZE_8BIT, data, n, 200);

    //printf("Read got %d\n", status);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_I2C_Mem_Read failed with code %d", status));
    }

    return mp_obj_str_builder_end(o);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_i2c_mem_read_obj, 4, 4, pyb_i2c_mem_read);

STATIC mp_obj_t pyb_i2c_mem_write(uint n_args, const mp_obj_t *args) {
    pyb_i2c_obj_t *self = args[0];
    machine_uint_t i2c_addr = mp_obj_get_int(args[1]) << 1;
    machine_uint_t mem_addr = mp_obj_get_int(args[2]);
    HAL_StatusTypeDef status;
    mp_obj_type_t *type = mp_obj_get_type(args[3]);
    if (type->buffer_p.get_buffer != NULL) {
        buffer_info_t bufinfo;
        type->buffer_p.get_buffer(args[3], &bufinfo, BUFFER_READ);
        status = HAL_I2C_Mem_Write(self->i2c_handle, i2c_addr, mem_addr, I2C_MEMADD_SIZE_8BIT, bufinfo.buf, bufinfo.len, 200);
    } else if (MP_OBJ_IS_INT(args[3])) {
        uint8_t data[1] = {mp_obj_get_int(args[3])};
        status = HAL_I2C_Mem_Write(self->i2c_handle, i2c_addr, mem_addr, I2C_MEMADD_SIZE_8BIT, data, 1, 200);
    } else {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "data argument must be an integer or support the buffer protocol"));
    }

    //printf("Write got %d\n", status);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_I2C_Mem_Write failed with code %d", status));
    }

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_i2c_mem_write_obj, 4, 4, pyb_i2c_mem_write);

STATIC const mp_map_elem_t pyb_i2c_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_is_ready), (mp_obj_t)&pyb_i2c_is_ready_obj },
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
