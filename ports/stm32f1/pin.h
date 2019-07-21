/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_PIN_H
#define MICROPY_INCLUDED_STM32_PIN_H

// This file requires pin_defs_xxx.h (which has port specific enums and
// defines, so we include it here. It should never be included directly

#include MICROPY_PIN_DEFS_PORT_H
#include "py/obj.h"

// stm32f1 Pin特有功能映射定义
enum {
  MP_HAL_PIN_REMAP_NONE = 0, // 无重映射
  MP_HAL_PIN_REMAP_PART1,    // 部分映射1
  MP_HAL_PIN_REMAP_PART2,    // 部分映射2
  MP_HAL_PIN_REMAP_FULL,     // 全部映射
};

typedef struct {
  mp_obj_base_t base;
  qstr name;
  uint8_t idx;  // 指AF功能组， 例如AF1, AF2, AF3等
  uint8_t fn;   // 对应AF_FN_xxx 宏定义， 例如AF_FN_TIM, AF_FN_USART
  uint8_t unit; // 同一类外设的索引, 例如SPI2 的索引是2
  uint8_t remap_type;  // 对应MP_HAL_PIN_REMAP_xxx， 重映射值
  uint32_t remap_mask; // 重映射mask
  void *reg; // 对应的外设地址
} pin_af_obj_t;

typedef struct {
  mp_obj_base_t base;       // micropython object 基类
  qstr name;                // Pin 的名称
  uint32_t port : 4;        // Pin 的 Port
  uint32_t pin : 5;         // Pin 索引， 如PA11, 则索引是11
  uint32_t num_af : 4;      // 复用功能数组大小
  uint32_t adc_channel : 5; // Some ARM processors use 32 bits/PORT
  uint32_t adc_num : 3;     // 1 bit per ADC
  uint32_t pin_mask;        // 管脚Set Mask
  pin_gpio_t *gpio;
  const pin_af_obj_t *af; // 复用功能数组
} pin_obj_t;

extern const mp_obj_type_t pin_type;
extern const mp_obj_type_t pin_af_type;

// Include all of the individual pin objects
#include "genhdr/pins.h"

typedef struct {
  const char *name;
  const pin_obj_t *pin;
} pin_named_pin_t;

extern const pin_named_pin_t pin_board_pins[];
extern const pin_named_pin_t pin_cpu_pins[];

//extern pin_map_obj_t pin_map_obj;

typedef struct {
  mp_obj_base_t base;
  qstr name;
  const pin_named_pin_t *named_pins;
} pin_named_pins_obj_t;

extern const mp_obj_type_t pin_board_pins_obj_type;
extern const mp_obj_type_t pin_cpu_pins_obj_type;

extern const mp_obj_dict_t pin_cpu_pins_locals_dict;
extern const mp_obj_dict_t pin_board_pins_locals_dict;

MP_DECLARE_CONST_FUN_OBJ_KW(pin_init_obj);

void pin_init0(void);
uint32_t pin_get_mode(const pin_obj_t *pin);
uint32_t pin_get_pull(const pin_obj_t *pin);
uint32_t pin_get_af(const pin_obj_t *pin);
const pin_obj_t *pin_find(mp_obj_t user_obj);
const pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name);
const pin_af_obj_t *pin_find_af(const pin_obj_t *pin, uint8_t fn, uint8_t unit);
const pin_af_obj_t *pin_find_af_by_index(const pin_obj_t *pin, mp_uint_t af_idx);
const pin_af_obj_t *pin_find_af_by_name(const pin_obj_t *pin, const char *name);

#endif // MICROPY_INCLUDED_STM32_PIN_H
