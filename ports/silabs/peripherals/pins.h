/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __MICROPY_INCLUDED_EFR32_PERIPHERALS_PINS_H__
#define __MICROPY_INCLUDED_EFR32_PERIPHERALS_PINS_H__

typedef struct {
    mp_obj_base_t base;
    const uint8_t port : 4;
    const uint8_t number : 4;
    const uint8_t *const function_list;
} mcu_pin_obj_t;

extern const mp_obj_type_t mcu_pin_type;

#define PIN(p_port, p_number, p_funtion_list) \
    {                                         \
        {&mcu_pin_type},                      \
        .port = p_port,                   \
        .number = p_number,               \
        .function_list = p_funtion_list   \
    }

#ifdef GPIO_PA0_EXISTS
extern const mcu_pin_obj_t pin_PA0;
#endif

#ifdef GPIO_PA1_EXISTS
extern const mcu_pin_obj_t pin_PA1;
#endif

#ifdef GPIO_PA2_EXISTS
extern const mcu_pin_obj_t pin_PA2;
#endif

#ifdef GPIO_PA3_EXISTS
extern const mcu_pin_obj_t pin_PA3;
#endif

#ifdef GPIO_PA4_EXISTS
extern const mcu_pin_obj_t pin_PA4;
#endif

#ifdef GPIO_PA5_EXISTS
extern const mcu_pin_obj_t pin_PA5;
#endif

#ifdef GPIO_PA6_EXISTS
extern const mcu_pin_obj_t pin_PA6;
#endif

#ifdef GPIO_PA7_EXISTS
extern const mcu_pin_obj_t pin_PA7;
#endif

#ifdef GPIO_PA8_EXISTS
extern const mcu_pin_obj_t pin_PA8;
#endif

#ifdef GPIO_PB0_EXISTS
extern const mcu_pin_obj_t pin_PB0;
#endif

#ifdef GPIO_PB1_EXISTS
extern const mcu_pin_obj_t pin_PB1;
#endif

#ifdef GPIO_PB2_EXISTS
extern const mcu_pin_obj_t pin_PB2;
#endif

#ifdef GPIO_PB3_EXISTS
extern const mcu_pin_obj_t pin_PB3;
#endif

#ifdef GPIO_PB4_EXISTS
extern const mcu_pin_obj_t pin_PB4;
#endif

#ifdef GPIO_PB5_EXISTS
extern const mcu_pin_obj_t pin_PB5;
#endif

#ifdef GPIO_PC0_EXISTS
extern const mcu_pin_obj_t pin_PC0;
#endif

#ifdef GPIO_PC1_EXISTS
extern const mcu_pin_obj_t pin_PC1;
#endif

#ifdef GPIO_PC2_EXISTS
extern const mcu_pin_obj_t pin_PC2;
#endif

#ifdef GPIO_PC3_EXISTS
extern const mcu_pin_obj_t pin_PC3;
#endif

#ifdef GPIO_PC4_EXISTS
extern const mcu_pin_obj_t pin_PC4;
#endif

#ifdef GPIO_PC5_EXISTS
extern const mcu_pin_obj_t pin_PC5;
#endif

#ifdef GPIO_PC6_EXISTS
extern const mcu_pin_obj_t pin_PC6;
#endif

#ifdef GPIO_PC7_EXISTS
extern const mcu_pin_obj_t pin_PC7;
#endif

#ifdef GPIO_PC8_EXISTS
extern const mcu_pin_obj_t pin_PC8;
#endif

#ifdef GPIO_PC9_EXISTS
extern const mcu_pin_obj_t pin_PC9;
#endif

#ifdef GPIO_PD0_EXISTS
extern const mcu_pin_obj_t pin_PD0;
#endif

#ifdef GPIO_PD1_EXISTS
extern const mcu_pin_obj_t pin_PD1;
#endif

#ifdef GPIO_PD2_EXISTS
extern const mcu_pin_obj_t pin_PD2;
#endif

#ifdef GPIO_PD3_EXISTS
extern const mcu_pin_obj_t pin_PD3;
#endif

#ifdef GPIO_PD4_EXISTS
extern const mcu_pin_obj_t pin_PD4;
#endif

#ifdef GPIO_PD5_EXISTS
extern const mcu_pin_obj_t pin_PD5;
#endif

#endif // __MICROPY_INCLUDED_EFR32_PERIPHERALS_PINS_H__
