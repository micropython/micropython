/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2022 Rakesh Peter
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
#include "pin.h"
#include "debug.h"

// Ref: CH32FV2x_V3x Reference Manual - Chapter 10 - GPIO and Alternate Function (GPIO/AFIO)

#define mode(pin) ((pin->pin) < 8) ? (((pin->gpio->CFGLR)  >> ((pin->pin) * 4)) & 3) : (((pin->gpio->CFGHR)  >> ((pin->pin) * 4)) & 3)
#define cnf(pin)  ((pin->pin) < 8) ? (((pin->gpio->CFGLR)  >> (((pin->pin) * 4 + 2))) & 3) : (((pin->gpio->CFGHR)  >> (((pin->pin) * 4) + 2)) & 3)

void gpio_clock_enable(const pin_obj_t *pin) {
    uint32_t rcc_map[]  = {0x4, 0x8, 0x10, 0x20, 0x40};    
    RCC->APB2PCENR |= rcc_map[pin->port];
}

uint8_t pin_read(const pin_obj_t *pin) {        
    return ((uint8_t)(pin->gpio->INDR & pin->pin_mask) == 0) ? 0 : 1;
}

void pin_write(const pin_obj_t *pin, bool val) {            
    if (val) 
        pin->gpio->BSHR = pin->pin_mask;
    else
        pin->gpio->BCR = pin->pin_mask;    
}

// Returns the pin mode. This value returned by this macro should be one of:
// GPIO_MODE_AIN, GPIO_MODE_IN_FLOATING, GPIO_MODE_IPD, GPIO_MODE_IPU, 
// GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD, GPIO_MODE_AF_PP, GPIO_MODE_AF_OD.
uint32_t pin_get_mode(const pin_obj_t *pin) {
    
    uint8_t mode = mode(pin);
    uint8_t cnf = cnf(pin);

    if (mode == 0) {
        if      (cnf == 0)  return GPIO_Mode_AIN;
        else if (cnf == 1)  return GPIO_Mode_IN_FLOATING;
        else if (cnf == 2)  return GPIO_Mode_IPD; // TODO: Handle Input Pin Pull State
    }
    else {
        if      (cnf == 0)  return GPIO_Mode_Out_PP; 
        else if (cnf == 1)  return GPIO_Mode_Out_OD;        
        else if (cnf == 2)  return GPIO_Mode_AF_PP;
        else if (cnf == 3)  return GPIO_Mode_AF_OD;        
    }

    return 0xFFFF;
}
