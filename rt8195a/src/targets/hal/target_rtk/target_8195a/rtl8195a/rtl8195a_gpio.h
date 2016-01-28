/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTL8195A_GPIO_H_
#define _RTL8195A_GPIO_H_

#include "hal_api.h"
#include "hal_gpio.h"

#define GPIO_PORTA_DR           0x00        // data register
#define GPIO_PORTA_DDR          0x04        // data direction
#define GPIO_PORTA_CTRL         0x08        // data source control, we should keep it as default: data source from software

#define GPIO_PORTB_DR           0x0c        // data register
#define GPIO_PORTB_DDR          0x10        // data direction
#define GPIO_PORTB_CTRL         0x14        // data source control, we should keep it as default: data source from software

#define GPIO_PORTC_DR           0x18        // data register
#define GPIO_PORTC_DDR          0x1c        // data direction
#define GPIO_PORTC_CTRL         0x20        // data source control, we should keep it as default: data source from software

//1 Only the PORTA can be configured to generate interrupts
#define GPIO_INT_EN             0x30        // Interrupt enable register
#define GPIO_INT_MASK           0x34        // Interrupt mask
#define GPIO_INT_TYPE           0x38        // Interrupt type(level/edge) register
#define GPIO_INT_POLARITY       0x3C        // Interrupt polarity(Active low/high) register
#define GPIO_INT_STATUS         0x40        // Interrupt status
#define GPIO_INT_RAWSTATUS      0x44        // Interrupt status without mask
#define GPIO_DEBOUNCE           0x48        // Interrupt signal debounce
#define GPIO_PORTA_EOI          0x4c        // Clear interrupt

#define GPIO_EXT_PORTA          0x50        // GPIO IN read or OUT read back
#define GPIO_EXT_PORTB          0x54        // GPIO IN read or OUT read back
#define GPIO_EXT_PORTC          0x58        // GPIO IN read or OUT read back

#define GPIO_INT_SYNC           0x60        // Is level-sensitive interrupt being sync sith PCLK

enum {
    HAL_GPIO_HIGHZ      = 0,
    HAL_GPIO_PULL_LOW   = 1,
    HAL_GPIO_PULL_HIGH  = 2
};

extern u32
HAL_GPIO_IrqHandler_8195a(
    IN VOID *pData
);

extern u32
HAL_GPIO_MbedIrqHandler_8195a(
    IN VOID *pData
);

extern u32
HAL_GPIO_MbedIrqHandler_8195a(
    IN VOID *pData
);

HAL_Status 
HAL_GPIO_IntCtrl_8195a(
    HAL_GPIO_PIN  *GPIO_Pin, 
    u32 En
);

HAL_Status 
HAL_GPIO_Init_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

HAL_Status 
HAL_GPIO_DeInit_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

HAL_GPIO_PIN_STATE 
HAL_GPIO_ReadPin_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

HAL_Status 
HAL_GPIO_WritePin_8195a(
    HAL_GPIO_PIN  *GPIO_Pin,
    HAL_GPIO_PIN_STATE Pin_State
);

HAL_Status
HAL_GPIO_RegIrq_8195a(
    IN PIRQ_HANDLE pIrqHandle
);

HAL_Status
HAL_GPIO_UnRegIrq_8195a(
    IN PIRQ_HANDLE pIrqHandle
);

HAL_Status
HAL_GPIO_UserRegIrq_8195a(
    HAL_GPIO_PIN  *GPIO_Pin,
    VOID *IrqHandler,
    VOID *IrqData
);

HAL_Status
HAL_GPIO_UserUnRegIrq_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

HAL_Status
HAL_GPIO_MaskIrq_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

HAL_Status
HAL_GPIO_UnMaskIrq_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

HAL_Status
HAL_GPIO_IntDebounce_8195a(
    HAL_GPIO_PIN  *GPIO_Pin,
    u8 Enable
);

u32
HAL_GPIO_GetIPPinName_8195a(
    u32 chip_pin
);

HAL_Status
HAL_GPIO_PullCtrl_8195a(
    u32 chip_pin,
    u8 pull_type
);

#endif  // end of "#define _RTL8195A_GPIO_H_"

