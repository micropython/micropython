#include <stdio.h>
#include <string.h>

#include <stm32f4xx_hal.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "accel.h"

#define MMA_ADDR (0x98)
#define MMA_REG_MODE (7)

STATIC I2C_HandleTypeDef I2cHandle;

void accel_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // PB5 is connected to AVDD; pull high to enable MMA accel device
    GPIOB->BSRRH = GPIO_PIN_5; // turn off AVDD
    GPIO_InitStructure.Pin = GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    // wait 20ms, then turn on AVDD, then wait 20ms again
    HAL_Delay(20);
    GPIOB->BSRRL = GPIO_PIN_5;
    HAL_Delay(20);

    // PB6=SCL, PB7=SDA
    GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_NOPULL; // have external pull-up resistors on both lines
    GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    // enable the I2C1 clock
    __I2C1_CLK_ENABLE();

    // set up the I2C1 device
    memset(&I2cHandle, 0, sizeof(I2C_HandleTypeDef));
    I2cHandle.Instance = I2C1;
    I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.ClockSpeed      = 400000;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;
    I2cHandle.Init.OwnAddress1     = 0xfe; // unused
    I2cHandle.Init.OwnAddress2     = 0xfe; // unused

    if (HAL_I2C_Init(&I2cHandle) != HAL_OK) {
        // init error
        printf("accel_init: HAL_I2C_Init failed\n");
        return;
    }

    HAL_StatusTypeDef status;

    //printf("IsDeviceReady\n");
    for (int i = 0; i < 10; i++) {
        status = HAL_I2C_IsDeviceReady(&I2cHandle, MMA_ADDR, 10, 200);
        //printf("  got %d\n", status);
        if (status == HAL_OK) {
            break;
        }
    }

    //printf("MemWrite\n");
    uint8_t data[1];
    data[0] = 1; // active mode
    status = HAL_I2C_Mem_Write(&I2cHandle, MMA_ADDR, MMA_REG_MODE, I2C_MEMADD_SIZE_8BIT, data, 1, 200);
    //printf("  got %d\n", status);
}

/******************************************************************************/
/* Micro Python bindings                                                      */

int accel_buf[12];

mp_obj_t pyb_accel_read(void) {
    for (int i = 0; i <= 6; i += 3) {
        accel_buf[0 + i] = accel_buf[0 + i + 3];
        accel_buf[1 + i] = accel_buf[1 + i + 3];
        accel_buf[2 + i] = accel_buf[2 + i + 3];
    }

    uint8_t data_[4];
    HAL_I2C_Mem_Read(&I2cHandle, MMA_ADDR, 0, I2C_MEMADD_SIZE_8BIT, data_, 4, 200);
    accel_buf[9] = data_[0] & 0x3f; if (accel_buf[9] & 0x20) accel_buf[9] |= ~0x1f;
    accel_buf[10] = data_[1] & 0x3f; if (accel_buf[10] & 0x20) accel_buf[10] |= ~0x1f;
    accel_buf[11] = data_[2] & 0x3f; if (accel_buf[11] & 0x20) accel_buf[11] |= ~0x1f;
    int jolt_info = data_[3];

    mp_obj_t data[4];
    data[0] = mp_obj_new_int(accel_buf[0] + accel_buf[3] + accel_buf[6] + accel_buf[9]);
    data[1] = mp_obj_new_int(accel_buf[1] + accel_buf[4] + accel_buf[7] + accel_buf[10]);
    data[2] = mp_obj_new_int(accel_buf[2] + accel_buf[5] + accel_buf[8] + accel_buf[11]);
    data[3] = mp_obj_new_int(jolt_info);

    return rt_build_tuple(4, data);
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_accel_read_obj, pyb_accel_read);

/*
mp_obj_t pyb_accel_read_all(void) {
    mp_obj_t data[11];
    accel_start(MMA_ADDR, 1);
    accel_send_byte(0);
    accel_restart(MMA_ADDR, 0);
    for (int i = 0; i <= 9; i++) {
        data[i] = mp_obj_new_int(accel_read_ack());
    }
    data[10] = mp_obj_new_int(accel_read_nack());

    return rt_build_tuple(11, data);
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_accel_read_all_obj, pyb_accel_read_all);

mp_obj_t pyb_accel_write_mode(mp_obj_t o_int, mp_obj_t o_mode) {
    accel_start(MMA_ADDR, 1);
    accel_send_byte(6); // start at int
    accel_send_byte(mp_obj_get_int(o_int));
    accel_send_byte(mp_obj_get_int(o_mode));
    accel_stop();
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_accel_write_mode_obj, pyb_accel_write_mode);
*/
