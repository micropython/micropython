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
#ifndef MICROPY_INCLUDED_STM32_PIN_REMAP_H
#define MICROPY_INCLUDED_STM32_PIN_REMAP_H

#include "py/obj.h"

// stm32f1 Pin remap value
#define REMAP_VAL_NONE   (0) /* no remap    */
#define REMAP_VAL_PART1  (1) /* remap part1 */
#define REMAP_VAL_PART2  (2) /* remap part2 */
#define REMAP_VAL_FULL   (3) /* full remap  */

/* bits struct is:
    0000 0000
    x --------- bit7:     0=AFIO_MAPR, 1=AFIO_MAPR2
     xx ------- bit[6:5]: config mask
       x xxxx - bit[4:0]:  offset of remap bit (form right to left) in range[0, 31]
*/
#define MAPR_IDX  (0<<7)
#define MAPR2_IDX (1<<7)

#define MAPR_BIT_NUM1 (1<<5)
#define MAPR_BIT_NUM2 (3<<5)

#define REMAP_PIN_SPI1         (MAPR_IDX | MAPR_BIT_NUM1 | 0)
#define REMAP_PIN_I2C1         (MAPR_IDX | MAPR_BIT_NUM1 | 1)
#define REMAP_PIN_USART1       (MAPR_IDX | MAPR_BIT_NUM1 | 2)
#define REMAP_PIN_USART2       (MAPR_IDX | MAPR_BIT_NUM1 | 3)
#define REMAP_PIN_USART3       (MAPR_IDX | MAPR_BIT_NUM2 | 4)
#define REMAP_PIN_TIM1         (MAPR_IDX | MAPR_BIT_NUM2 | 6)
#define REMAP_PIN_TIM2         (MAPR_IDX | MAPR_BIT_NUM2 | 8)
#define REMAP_PIN_TIM3         (MAPR_IDX | MAPR_BIT_NUM2 | 10)
#define REMAP_PIN_TIM4         (MAPR_IDX | MAPR_BIT_NUM1 | 12)
#define REMAP_PIN_CAN          (MAPR_IDX | MAPR_BIT_NUM2 | 13)
#define REMAP_PIN_PD01         (MAPR_IDX | MAPR_BIT_NUM1 | 15)
#define REMAP_PIN_TIM5CH_4     (MAPR_IDX | MAPR_BIT_NUM1 | 16)
#define REMAP_PIN_ADC1_ETRGINJ (MAPR_IDX | MAPR_BIT_NUM1 | 17)
#define REMAP_PIN_ADC1_ETRGREG (MAPR_IDX | MAPR_BIT_NUM1 | 18)
#define REMAP_PIN_ADC2_ETRGINJ (MAPR_IDX | MAPR_BIT_NUM1 | 19)
#define REMAP_PIN_ADC2_ETRGREG (MAPR_IDX | MAPR_BIT_NUM1 | 20)

#ifdef STM32F103xG
#define REMAP_PIN_TIM9         (MAPR2_IDX | MAPR_BIT_NUM1 | 5)
#define REMAP_PIN_TIM10        (MAPR2_IDX | MAPR_BIT_NUM1 | 6)
#define REMAP_PIN_TIM11        (MAPR2_IDX | MAPR_BIT_NUM1 | 7)
#define REMAP_PIN_TIM13        (MAPR2_IDX | MAPR_BIT_NUM1 | 8)
#define REMAP_PIN_TIM14        (MAPR2_IDX | MAPR_BIT_NUM1 | 9)
#define REMAP_PIN_FSMC_NADV    (MAPR2_IDX | MAPR_BIT_NUM1 | 10)
#endif

extern const mp_obj_type_t pin_remap_type;


void pin_remap_init0(void);
uint32_t pin_remap_config(uint8_t periph, int8_t remap);

#endif // MICROPY_INCLUDED_STM32_PIN_MAPPER_H
