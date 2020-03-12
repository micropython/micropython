/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
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


#include "tick.h"
#include "supervisor/usb.h"
#include "lib/utils/interrupt_char.h"
#include "lib/mp-readline/readline.h"
#include "stm32f4xx_hal.h"

#include "py/mpconfig.h"

#include "common-hal/microcontroller/Pin.h"

STATIC void init_usb_vbus_sense(void) {

#if (BOARD_NO_VBUS_SENSE)
    // Disable VBUS sensing
    #ifdef USB_OTG_GCCFG_VBDEN
        USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_VBDEN;
    #else
        USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
        USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_VBUSBSEN;
        USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_VBUSASEN;
    #endif
#else
    // Enable VBUS hardware sensing
    #ifdef USB_OTG_GCCFG_VBDEN
        USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBDEN;
    #else
        USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_NOVBUSSENS;
        USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBUSBSEN; // B Device sense
    #endif
#endif
}

void init_usb_hardware(void) {
    //TODO: if future chips overload this with options, move to peripherals management. 

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /**USB_OTG_FS GPIO Configuration
    PA10     ------> USB_OTG_FS_ID
    PA11     ------> USB_OTG_FS_DM
    PA12     ------> USB_OTG_FS_DP 
    */
    __HAL_RCC_GPIOA_CLK_ENABLE();

      /* Configure DM DP Pins */
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    never_reset_pin_number(0, 11);
    never_reset_pin_number(0, 12);

    /* Configure VBUS Pin */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    never_reset_pin_number(0, 9);

    /* This for ID line debug */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    never_reset_pin_number(0, 10);

#ifdef STM32F412Zx
    /* Configure POWER_SWITCH IO pin (F412 ONLY)*/
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    never_reset_pin_number(0, 8);
#endif
    
    /* Peripheral clock enable */
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    init_usb_vbus_sense();
}
