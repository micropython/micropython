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
#ifndef MBED_PINMAP_H
#define MBED_PINMAP_H

#include "PinNames.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    PinName pin;
    int peripheral;
    int function;
} PinMap;

void pin_function(PinName pin, int function);
void pin_mode    (PinName pin, PinMode mode);

uint32_t pinmap_peripheral(PinName pin, const PinMap* map);
uint32_t pinmap_merge     (uint32_t a, uint32_t b);
void     pinmap_pinout    (PinName pin, const PinMap *map);
uint32_t pinmap_find_peripheral(PinName pin, const PinMap* map);

#ifdef __cplusplus
}
#endif

#endif
