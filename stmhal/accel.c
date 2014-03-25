#include <stdio.h>
#include <string.h>

#include <stm32f4xx_hal.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "i2c.h"
#include "accel.h"

#define MMA_ADDR (0x98)
#define MMA_REG_X (0)
#define MMA_REG_Y (1)
#define MMA_REG_Z (2)
#define MMA_REG_TILT (3)
#define MMA_REG_MODE (7)
#define MMA_AXIS_SIGNED_VALUE(i) (((i) & 0x3f) | ((i) & 0x20 ? (~0x1f) : 0))

void accel_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // PB5 is connected to AVDD; pull high to enable MMA accel device
    GPIOB->BSRRH = GPIO_PIN_5; // turn off AVDD
    GPIO_InitStructure.Pin = GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

STATIC void accel_start(void) {
    // start the I2C bus
    i2c_start(&I2cHandle_X);

    // turn off AVDD, wait 20ms, turn on AVDD, wait 20ms again
    GPIOB->BSRRH = GPIO_PIN_5; // turn off
    HAL_Delay(20);
    GPIOB->BSRRL = GPIO_PIN_5; // turn on
    HAL_Delay(20);

    HAL_StatusTypeDef status;

    //printf("IsDeviceReady\n");
    for (int i = 0; i < 10; i++) {
        status = HAL_I2C_IsDeviceReady(&I2cHandle_X, MMA_ADDR, 10, 200);
        //printf("  got %d\n", status);
        if (status == HAL_OK) {
            break;
        }
    }

    //printf("MemWrite\n");
    uint8_t data[1];
    data[0] = 1; // active mode
    status = HAL_I2C_Mem_Write(&I2cHandle_X, MMA_ADDR, MMA_REG_MODE, I2C_MEMADD_SIZE_8BIT, data, 1, 200);
    //printf("  got %d\n", status);
}

/******************************************************************************/
/* Micro Python bindings                                                      */

#define NUM_AXIS (3)
#define FILT_DEPTH (4)

typedef struct _pyb_accel_obj_t {
    mp_obj_base_t base;
    int16_t buf[NUM_AXIS * FILT_DEPTH];
} pyb_accel_obj_t;

STATIC pyb_accel_obj_t pyb_accel_obj;

STATIC mp_obj_t pyb_accel_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    if (!(n_args == 0 && n_kw == 0)) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "Accel accepts no arguments"));
    }

    // init accel object
    pyb_accel_obj.base.type = &pyb_accel_type;
    accel_start();

    return &pyb_accel_obj;
}

STATIC mp_obj_t read_axis(int axis) {
    uint8_t data[1];
    HAL_I2C_Mem_Read(&I2cHandle_X, MMA_ADDR, axis, I2C_MEMADD_SIZE_8BIT, data, 1, 200);
    return mp_obj_new_int(MMA_AXIS_SIGNED_VALUE(data[0]));
}

STATIC mp_obj_t pyb_accel_x(mp_obj_t self_in) {
    return read_axis(MMA_REG_X);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_accel_x_obj, pyb_accel_x);

STATIC mp_obj_t pyb_accel_y(mp_obj_t self_in) {
    return read_axis(MMA_REG_Y);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_accel_y_obj, pyb_accel_y);

STATIC mp_obj_t pyb_accel_z(mp_obj_t self_in) {
    return read_axis(MMA_REG_Z);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_accel_z_obj, pyb_accel_z);

STATIC mp_obj_t pyb_accel_tilt(mp_obj_t self_in) {
    uint8_t data[1];
    HAL_I2C_Mem_Read(&I2cHandle_X, MMA_ADDR, MMA_REG_TILT, I2C_MEMADD_SIZE_8BIT, data, 1, 200);
    return mp_obj_new_int(data[0]);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_accel_tilt_obj, pyb_accel_tilt);

STATIC mp_obj_t pyb_accel_filtered_xyz(mp_obj_t self_in) {
    pyb_accel_obj_t *self = self_in;

    memmove(self->buf, self->buf + NUM_AXIS, NUM_AXIS * (FILT_DEPTH - 1) * sizeof(int16_t));

    uint8_t data[NUM_AXIS];
    HAL_I2C_Mem_Read(&I2cHandle_X, MMA_ADDR, MMA_REG_X, I2C_MEMADD_SIZE_8BIT, data, NUM_AXIS, 200);

    mp_obj_t tuple[NUM_AXIS];
    for (int i = 0; i < NUM_AXIS; i++) {
        self->buf[NUM_AXIS * (FILT_DEPTH - 1) + i] = MMA_AXIS_SIGNED_VALUE(data[i]);
        int32_t val = 0;
        for (int j = 0; j < FILT_DEPTH; j++) {
            val += self->buf[i + NUM_AXIS * j];
        }
        tuple[i] = mp_obj_new_int(val);
    }

    return rt_build_tuple(3, tuple);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_accel_filtered_xyz_obj, pyb_accel_filtered_xyz);

STATIC mp_obj_t pyb_accel_read_reg(mp_obj_t self_in, mp_obj_t reg) {
    uint8_t data[1];
    HAL_I2C_Mem_Read(&I2cHandle_X, MMA_ADDR, mp_obj_get_int(reg), I2C_MEMADD_SIZE_8BIT, data, 1, 200);
    return mp_obj_new_int(data[0]);
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_accel_read_reg_obj, pyb_accel_read_reg);

STATIC mp_obj_t pyb_accel_write_reg(mp_obj_t self_in, mp_obj_t reg, mp_obj_t val) {
    uint8_t data[1];
    data[0] = mp_obj_get_int(val);
    HAL_I2C_Mem_Write(&I2cHandle_X, MMA_ADDR, mp_obj_get_int(reg), I2C_MEMADD_SIZE_8BIT, data, 1, 200);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_3(pyb_accel_write_reg_obj, pyb_accel_write_reg);

STATIC const mp_method_t pyb_accel_methods[] = {
    { "x", &pyb_accel_x_obj },
    { "y", &pyb_accel_y_obj },
    { "z", &pyb_accel_z_obj },
    { "tilt", &pyb_accel_tilt_obj },
    { "filtered_xyz", &pyb_accel_filtered_xyz_obj },
    { "read_reg", &pyb_accel_read_reg_obj },
    { "write_reg", &pyb_accel_write_reg_obj },
    { NULL, NULL },
};

const mp_obj_type_t pyb_accel_type = {
    { &mp_type_type },
    .name = MP_QSTR_Accel,
    .make_new = pyb_accel_make_new,
    .methods = pyb_accel_methods,
};
