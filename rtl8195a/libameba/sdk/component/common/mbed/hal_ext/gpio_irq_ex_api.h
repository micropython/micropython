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
#ifndef MBED_GPIO_IRQ_EX_API_H
#define MBED_GPIO_IRQ_EX_API_H

#include "device.h"

#if DEVICE_INTERRUPTIN

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    IRQ_LOW = 3,
    IRQ_HIGH =4
} gpio_irq_event_ex;

void gpio_irq_deinit(gpio_irq_t *obj);

#ifdef __cplusplus
}
#endif

#endif  // end of "#if DEVICE_INTERRUPTIN"

#endif  // end of #ifndef MBED_GPIO_IRQ_EX_API_H