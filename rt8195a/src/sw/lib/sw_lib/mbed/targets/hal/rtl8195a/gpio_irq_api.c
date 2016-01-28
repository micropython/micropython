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

//static uint32_t channel_ids[32] = {0};

//static gpio_irq_handler irq_handler;

#if CONFIG_GPIO_EN
#include "gpio_irq_api.h"

int gpio_irq_init(gpio_irq_t *obj, PinName pin, gpio_irq_handler handler, uint32_t id) 
{

    if (pin == NC) return -1;

    obj->pin = pin;
    obj->hal_pin.pin_name = HAL_GPIO_GetPinName((u32)pin);; // get the IP pin name
    obj->hal_pin.pin_mode = INT_FALLING;    // default use Falling trigger
    HAL_GPIO_Irq_Init(&obj->hal_pin);
    HAL_GPIO_UserRegIrq(&obj->hal_pin, (VOID*) handler, (VOID*) id);

    return 0;
}

void gpio_irq_free(gpio_irq_t *obj) 
{
    HAL_GPIO_UserUnRegIrq(&obj->hal_pin);
    HAL_GPIO_DeInit(&obj->hal_pin);
}

void gpio_irq_set(gpio_irq_t *obj, gpio_irq_event event, uint32_t enable) 
{
    switch(event) {
        case IRQ_RISE:
            obj->hal_pin.pin_mode = INT_RISING;
            break;

        case IRQ_FALL:
            obj->hal_pin.pin_mode = INT_FALLING;
            break;

        case IRQ_NONE:
            // ?
            break;
            
        default:
            break;
    }
    HAL_GPIO_Irq_Init(&obj->hal_pin);

    HAL_GPIO_IntCtrl(&obj->hal_pin, enable);
}

void gpio_irq_enable(gpio_irq_t *obj) 
{
    HAL_GPIO_UnMaskIrq(&obj->hal_pin);
}

void gpio_irq_disable(gpio_irq_t *obj) 
{
    HAL_GPIO_MaskIrq(&obj->hal_pin);
}

#endif
