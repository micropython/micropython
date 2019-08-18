 /*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include "py/obj.h"
#include "py/mphal.h"
#include "stm32f4/pins.h"

const mcu_pin_obj_t pin_PE02 = PIN(4, GPIOE, 2);
const mcu_pin_obj_t pin_PE03 = PIN(4, GPIOE, 3);
const mcu_pin_obj_t pin_PE04 = PIN(4, GPIOE, 4);
const mcu_pin_obj_t pin_PE05 = PIN(4, GPIOE, 5);
const mcu_pin_obj_t pin_PE06 = PIN(4, GPIOE, 6);

const mcu_pin_obj_t pin_PC13 = PIN(2, GPIOC, 13); //anti-tamp
const mcu_pin_obj_t pin_PC14 = PIN(2, GPIOC, 14); //OSC32_IN
const mcu_pin_obj_t pin_PC15 = PIN(2, GPIOC, 15); //OSC32_OUT

const mcu_pin_obj_t pin_PC00 = PIN(2, GPIOC, 0);
const mcu_pin_obj_t pin_PC01 = PIN(2, GPIOC, 1);
const mcu_pin_obj_t pin_PC02 = PIN(2, GPIOC, 2);
const mcu_pin_obj_t pin_PC03 = PIN(2, GPIOC, 3);

const mcu_pin_obj_t pin_PA00 = PIN(0, GPIOA, 0);
const mcu_pin_obj_t pin_PA01 = PIN(0, GPIOA, 1);
const mcu_pin_obj_t pin_PA02 = PIN(0, GPIOA, 2);
const mcu_pin_obj_t pin_PA03 = PIN(0, GPIOA, 3);
const mcu_pin_obj_t pin_PA04 = PIN(0, GPIOA, 4);
const mcu_pin_obj_t pin_PA05 = PIN(0, GPIOA, 5);
const mcu_pin_obj_t pin_PA06 = PIN(0, GPIOA, 6);
const mcu_pin_obj_t pin_PA07 = PIN(0, GPIOA, 7);

const mcu_pin_obj_t pin_PC04 = PIN(2, GPIOC, 4);
const mcu_pin_obj_t pin_PC05 = PIN(2, GPIOC, 5);

const mcu_pin_obj_t pin_PB00 = PIN(1, GPIOB, 0);
const mcu_pin_obj_t pin_PB01 = PIN(1, GPIOB, 1);
const mcu_pin_obj_t pin_PB02 = PIN(1, GPIOB, 2);

const mcu_pin_obj_t pin_PE07 = PIN(4, GPIOE, 7);
const mcu_pin_obj_t pin_PE08 = PIN(4, GPIOE, 8);
const mcu_pin_obj_t pin_PE09 = PIN(4, GPIOE, 9);
const mcu_pin_obj_t pin_PE10 = PIN(4, GPIOE, 10);
const mcu_pin_obj_t pin_PE11 = PIN(4, GPIOE, 11);
const mcu_pin_obj_t pin_PE12 = PIN(4, GPIOE, 12);
const mcu_pin_obj_t pin_PE13 = PIN(4, GPIOE, 13);
const mcu_pin_obj_t pin_PE14 = PIN(4, GPIOE, 14);
const mcu_pin_obj_t pin_PE15 = PIN(4, GPIOE, 15);

const mcu_pin_obj_t pin_PB10 = PIN(1, GPIOB, 10);
const mcu_pin_obj_t pin_PB12 = PIN(1, GPIOB, 12);
const mcu_pin_obj_t pin_PB13 = PIN(1, GPIOB, 13);
const mcu_pin_obj_t pin_PB14 = PIN(1, GPIOB, 14);
const mcu_pin_obj_t pin_PB15 = PIN(1, GPIOB, 15);

const mcu_pin_obj_t pin_PD08 = PIN(3, GPIOD, 8);
const mcu_pin_obj_t pin_PD09 = PIN(3, GPIOD, 9);
const mcu_pin_obj_t pin_PD10 = PIN(3, GPIOD, 10);
const mcu_pin_obj_t pin_PD11 = PIN(3, GPIOD, 11);
const mcu_pin_obj_t pin_PD12 = PIN(3, GPIOD, 12);
const mcu_pin_obj_t pin_PD13 = PIN(3, GPIOD, 13);
const mcu_pin_obj_t pin_PD14 = PIN(3, GPIOD, 14);
const mcu_pin_obj_t pin_PD15 = PIN(3, GPIOD, 15);

const mcu_pin_obj_t pin_PC06 = PIN(2, GPIOC, 6);
const mcu_pin_obj_t pin_PC07 = PIN(2, GPIOC, 7);
const mcu_pin_obj_t pin_PC08 = PIN(2, GPIOC, 8);
const mcu_pin_obj_t pin_PC09 = PIN(2, GPIOC, 9);

const mcu_pin_obj_t pin_PA08 = PIN(0, GPIOA, 8);
const mcu_pin_obj_t pin_PA09 = PIN(0, GPIOA, 9);
const mcu_pin_obj_t pin_PA10 = PIN(0, GPIOA, 10);
const mcu_pin_obj_t pin_PA11 = PIN(0, GPIOA, 11);
const mcu_pin_obj_t pin_PA12 = PIN(0, GPIOA, 12);
const mcu_pin_obj_t pin_PA13 = PIN(0, GPIOA, 13);
const mcu_pin_obj_t pin_PA14 = PIN(0, GPIOA, 14);
const mcu_pin_obj_t pin_PA15 = PIN(0, GPIOA, 15);

const mcu_pin_obj_t pin_PC10 = PIN(2, GPIOC, 10);
const mcu_pin_obj_t pin_PC11 = PIN(2, GPIOC, 11);
const mcu_pin_obj_t pin_PC12 = PIN(2, GPIOC, 12);

const mcu_pin_obj_t pin_PD00 = PIN(3, GPIOD, 0);
const mcu_pin_obj_t pin_PD01 = PIN(3, GPIOD, 1);
const mcu_pin_obj_t pin_PD02 = PIN(3, GPIOD, 2);
const mcu_pin_obj_t pin_PD03 = PIN(3, GPIOD, 3);
const mcu_pin_obj_t pin_PD04 = PIN(3, GPIOD, 4);
const mcu_pin_obj_t pin_PD05 = PIN(3, GPIOD, 5);
const mcu_pin_obj_t pin_PD06 = PIN(3, GPIOD, 6);
const mcu_pin_obj_t pin_PD07 = PIN(3, GPIOD, 7);

const mcu_pin_obj_t pin_PB03 = PIN(1, GPIOB, 3);
const mcu_pin_obj_t pin_PB04 = PIN(1, GPIOB, 4);
const mcu_pin_obj_t pin_PB05 = PIN(1, GPIOB, 5);
const mcu_pin_obj_t pin_PB06 = PIN(1, GPIOB, 6);
const mcu_pin_obj_t pin_PB07 = PIN(1, GPIOB, 7);
const mcu_pin_obj_t pin_PB08 = PIN(1, GPIOB, 8);
const mcu_pin_obj_t pin_PB09 = PIN(1, GPIOB, 9);

const mcu_pin_obj_t pin_PE00 = PIN(4, GPIOE, 0);
const mcu_pin_obj_t pin_PE01 = PIN(4, GPIOE, 1);