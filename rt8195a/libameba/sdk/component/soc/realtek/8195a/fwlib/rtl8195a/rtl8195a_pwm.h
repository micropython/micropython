/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTL8195A_PWM_H_
#define _RTL8195A_PWM_H_

extern void
HAL_Pwm_SetDuty_8195a(
    HAL_PWM_ADAPTER *pPwmAdapt,
    u32 period,
    u32 pulse_width
);

extern HAL_Status 
HAL_Pwm_Init_8195a(
    HAL_PWM_ADAPTER *pPwmAdapt
);

extern void 
HAL_Pwm_Enable_8195a(
    HAL_PWM_ADAPTER *pPwmAdapt
);

extern void 
HAL_Pwm_Disable_8195a(
    HAL_PWM_ADAPTER *pPwmAdapt
);


#endif
