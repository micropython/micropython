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
#ifndef MICROPY_INCLUDED_STM32_TIMER_H
#define MICROPY_INCLUDED_STM32_TIMER_H

#define MAX_NO_OF_T_INTS  16
#define TIM_A_INTS 8

extern TIM_HandleTypeDef TIM5_Handle; 
extern TIM_HandleTypeDef TIM7_Handle;

typedef void (*intTab)(void);

#define NANOSEC_DIVIDER 100     // sets the lowest reasonable timer resolution for the used MCU

#define DELAY_CONFIG(p_device) \
{\
    p_device->CR1 &= ~(0x01); \
    p_device->EGR |= 0x01; \
    p_device->CR1 |= 0x08; \
}


#define DELAY_SET(p_device, value) {\
                                    p_device->ARR = value;\
                                    p_device->SR = 0;\
                                    }

#define DELAY_WAIT(p_device)    {\
                                p_device->CR1 |= 0x01;\
                                while(!(p_device->SR & 0x01));\
                                }



int add1msInt(intTab function); //1ms interrupt
void del1mInt(uint8_t index); //1ms interrupt

int addTimAInt(intTab function); //Tim A interrupts
void delTimAInt(uint8_t index); //Tim A interrupts                           
 
void timer_tim5_init(void); //servo motor -- do we need it for tiger ?
void timer_tim7_init(void); //1 ms interrupt
void timer_tim10_init(void); //Timer Delay

extern const mp_obj_type_t pyb_timer_type; //timer type

void timer_init0(void); //used in main.c
TIM_HandleTypeDef *timer_tim6_init(uint freq);//used in adc.c/dac.c(pyboard,not Tiger) -- do we need it?
void timer_deinit(void); //used in main.c
uint32_t timer_get_source_freq(uint32_t tim_id);//used in led.c(pyboard,not Tiger) -- do we need led.c?
void timer_irq_handler(uint tim_id); //used in stm32_it.c

TIM_HandleTypeDef *pyb_timer_get_handle(mp_obj_t timer); //for ADC/DAC from Pyboard

#endif // MICROPY_INCLUDED_STM32_TIMER_H
