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
#ifndef I2C_EX_API_H
#define I2C_EX_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    I2C_TX_COMPLETE     = 0,
    I2C_RX_COMPLETE     = 1,
    I2C_ERR_OCCURRED    = 2
}I2CCallback;

void i2c_set_user_callback(i2c_t *obj, I2CCallback i2ccb, void(*i2c_callback)(void *));
void i2c_clear_user_callback(i2c_t *obj, I2CCallback i2ccb);

#ifdef __cplusplus
}
#endif

#endif


