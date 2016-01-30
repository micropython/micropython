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
#ifndef MBED_ANALOGOUT_API_H
#define MBED_ANALOGOUT_API_H

#include "device.h"

#if DEVICE_ANALOGOUT

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dac_s dac_t;

void     analogout_init     (dac_t *obj, PinName pin);
void     analogout_free     (dac_t *obj);
void     analogout_write    (dac_t *obj, float value);
void     analogout_write_u16(dac_t *obj, uint16_t value);
float    analogout_read     (dac_t *obj);
uint16_t analogout_read_u16 (dac_t *obj);

#ifdef __cplusplus
}
#endif

#endif

#endif
