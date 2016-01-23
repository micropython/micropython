/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
#include "objects.h"
#include "pinmap.h"

#if CONFIG_GPIO_EN

#include "gpio_api.h"

void gpio_set_hal_pin_mode(gpio_t *obj)
{
    if (obj->direction == PIN_OUTPUT) {
        switch (obj->mode) {
            case PullNone:
            case PullDown:
            case PullUp:
                obj->hal_pin.pin_mode = DOUT_PUSH_PULL;
                break;

            case OpenDrain:
                obj->hal_pin.pin_mode = DOUT_OPEN_DRAIN;
                break;
                
            default:
                obj->hal_pin.pin_mode = DOUT_PUSH_PULL;
                break;
        }
    }
    else {
        switch (obj->mode) {
            case PullNone:
            case OpenDrain:
                obj->hal_pin.pin_mode = DIN_PULL_NONE;
                break;
                
            case PullDown:
                obj->hal_pin.pin_mode = DIN_PULL_LOW;
                break;
                
            case PullUp:
                obj->hal_pin.pin_mode = DIN_PULL_HIGH;
                break;
                
            default:
                obj->hal_pin.pin_mode = DIN_PULL_NONE;
                break;
        }
    }
}

uint32_t gpio_set(PinName pin) 
{
    u32 ip_pin;
    
    //MBED_ASSERT(pin != (PinName)NC);
    DBG_ASSERT(pin != (PinName)NC);
    pin_function(pin, 0);
    ip_pin = HAL_GPIO_GetPinName((u32)pin);
//    DBG_GPIO_INFO("%s chip_pin[0x%x]->ip_pin[0x%x]\n", __FUNCTION__, pin, ip_pin);
    
    return ip_pin;
}

void gpio_init(gpio_t *obj, PinName pin) 
{
    if (pin == (PinName)NC)
        return;

    obj->pin = pin;
    obj->mode = PullNone;
    obj->direction = PIN_INPUT;
    obj->hal_pin.pin_name = gpio_set(pin); // get the IP pin name
    obj->hal_pin.pin_mode = DIN_PULL_NONE;
    HAL_GPIO_Init(&obj->hal_pin);
}

void gpio_mode(gpio_t *obj, PinMode mode) 
{
    obj->mode = mode;
    gpio_set_hal_pin_mode(obj);    
    //DBG_GPIO_INFO("%s GPIO[0x%x], mode=%d\n", __FUNCTION__, obj->hal_pin.pin_name, obj->hal_pin.pin_mode);
    HAL_GPIO_Init(&obj->hal_pin);
}

void gpio_dir(gpio_t *obj, PinDirection direction) {
//    MBED_ASSERT(obj->pin != (PinName)NC);
    DBG_ASSERT(obj->pin != (PinName)NC);
    obj->direction = direction;
    gpio_set_hal_pin_mode(obj);
    //DBG_GPIO_INFO("%s GPIO[0x%x], mode=%d\n", __FUNCTION__, obj->hal_pin.pin_name, obj->hal_pin.pin_mode);
    HAL_GPIO_Init(&obj->hal_pin);
}

void gpio_write(gpio_t *obj, int value) 
{
//    MBED_ASSERT(obj->pin != (PinName)NC);
    DBG_ASSERT(obj->pin != (PinName)NC);
    HAL_GPIO_WritePin(&obj->hal_pin, value);
}

int gpio_read(gpio_t *obj) {
//    MBED_ASSERT(obj->pin != (PinName)NC);
    DBG_ASSERT(obj->pin != (PinName)NC);
    return HAL_GPIO_ReadPin(&obj->hal_pin);
}

#endif