/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_I2C_API_H
#define MBED_I2C_API_H

#include "device.h"

#if DEVICE_I2C

#ifdef __cplusplus
extern "C" {
#endif

typedef struct i2c_s i2c_t;

enum {
  I2C_ERROR_NO_SLAVE = -1,
  I2C_ERROR_BUS_BUSY = -2
};

void i2c_init         (i2c_t *obj, PinName sda, PinName scl);
void i2c_frequency    (i2c_t *obj, int hz);
int  i2c_start        (i2c_t *obj);
int  i2c_stop         (i2c_t *obj);
int  i2c_read         (i2c_t *obj, int address, char *data, int length, int stop);
int  i2c_write        (i2c_t *obj, int address, const char *data, int length, int stop);
void i2c_reset        (i2c_t *obj);
int  i2c_byte_read    (i2c_t *obj, int last);
int  i2c_byte_write   (i2c_t *obj, int data);

#if DEVICE_I2CSLAVE
void i2c_slave_mode   (i2c_t *obj, int enable_slave);
int  i2c_slave_receive(i2c_t *obj);
int  i2c_slave_read   (i2c_t *obj, char *data, int length);
int  i2c_slave_write  (i2c_t *obj, const char *data, int length);
void i2c_slave_address(i2c_t *obj, int idx, uint32_t address, uint32_t mask);
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
