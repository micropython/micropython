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
#ifndef MBED_GPIO_API_H
#define MBED_GPIO_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Set the given pin as GPIO
 * @param pin The pin to be set as GPIO
 * @return The GPIO port mask for this pin
 **/
uint32_t gpio_set(PinName pin);

/* GPIO object */
void gpio_init(gpio_t *obj, PinName pin);

void gpio_mode (gpio_t *obj, PinMode mode);
void gpio_dir  (gpio_t *obj, PinDirection direction);

void gpio_write(gpio_t *obj, int value);
int  gpio_read (gpio_t *obj);

// the following set of functions are generic and are implemented in the common gpio.c file
void gpio_init_in(gpio_t* gpio, PinName pin);
void gpio_init_in_ex(gpio_t* gpio, PinName pin, PinMode mode);
void gpio_init_out(gpio_t* gpio, PinName pin);
void gpio_init_out_ex(gpio_t* gpio, PinName pin, int value);
void gpio_init_inout(gpio_t* gpio, PinName pin, PinDirection direction, PinMode mode, int value);

#ifdef __cplusplus
}
#endif

#endif
