/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
//#include "mbed_assert.h"
#include "objects.h"
#include "pinmap.h"
//#include "error.h"

/**
 * Configure pin enable and function
 */
void pin_function(PinName pin, int function) 
{
    // MBED_ASSERT(pin != (PinName)NC);
    //1 Our HAL API cannot support  to configure the pin function by this way
    /* the pin function (pin mux) is depends on each IP On/Off and priority, so we cannot
       set the pin function directly */
}

/**
 * Configure pin pull-up/pull-down
 */
void pin_mode(PinName pin, PinMode mode) 
{
//    MBED_ASSERT(pin != (PinName)NC);
    HAL_GPIO_PullCtrl((u32)pin, (u32)mode);

}
