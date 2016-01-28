/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
 

#include "rtl8195a.h"

#ifdef CONFIG_PWM_EN
#include "hal_pwm.h"
#include "hal_timer.h"

HAL_PWM_ADAPTER PWMPin[MAX_PWM_CTRL_PIN];
const u8 PWMTimerIdx[MAX_PWM_CTRL_PIN]= {3,4,5,6};  // the G-timer ID used for PWM pin 0~3

/**
  * @brief  Initializes and enable a PWM control pin.
  *
  * @param  pwm_id: the PWM pin index
  * @param  sel: pin mux selection
  *
  * @retval HAL_Status
  */
HAL_Status 
HAL_Pwm_Init(
    u32 pwm_id,
    u32 sel
)
{
    HAL_PWM_ADAPTER *pPwmAdapt;
    u32 timer_id;

    DBG_PWM_INFO("%s: Init PWM for PWM %d, Sel %d\n", __FUNCTION__, pwm_id, sel);

    if ((pwm_id >= MAX_PWM_CTRL_PIN) || (sel > 3)) {
        DBG_PWM_ERR ("HAL_Pwm_Init: Invalid PWM index(%d), sel(%d)\n", pwm_id, sel);
        return HAL_ERR_PARA;
    }
    
    pPwmAdapt = &PWMPin[pwm_id];
    pPwmAdapt->pwm_id = pwm_id;
    pPwmAdapt->sel = sel;
    timer_id = PWMTimerIdx[pwm_id];
    pPwmAdapt->gtimer_id = timer_id;
    
    return HAL_Pwm_Init_8195a (pPwmAdapt);
}


/**
  * @brief  Disable a PWM control pin.
  *
  * @param  pwm_id: the PWM pin index
  *
  * @retval None
  */
void 
HAL_Pwm_Enable(
    u32 pwm_id
)
{
    HAL_PWM_ADAPTER *pPwmAdapt;
        
    if (pwm_id >= MAX_PWM_CTRL_PIN) {
        DBG_PWM_ERR ("HAL_Pwm_Enable: Invalid PWM index(%d)\n", pwm_id);
        return;
    }
    pPwmAdapt = &PWMPin[pwm_id];
    
    HAL_Pwm_Enable_8195a(pPwmAdapt);
}


/**
  * @brief  Disable a PWM control pin.
  *
  * @param  pwm_id: the PWM pin index
  *
  * @retval None
  */
void 
HAL_Pwm_Disable(
    u32 pwm_id
)
{
    HAL_PWM_ADAPTER *pPwmAdapt;
        
    if (pwm_id >= MAX_PWM_CTRL_PIN) {
        DBG_PWM_ERR ("HAL_Pwm_Disable: Invalid PWM index(%d)\n", pwm_id);
        return;
    }
    pPwmAdapt = &PWMPin[pwm_id];
    
    HAL_Pwm_Disable_8195a(pPwmAdapt);
}

/**
  * @brief  Set the duty ratio of the PWM pin.
  *
  * @param  pwm_id: the PWM pin index
  * @param  period: the period time, in micro-second.
  * @param  pulse_width: the pulse width time, in micro-second.
  *
  * @retval None
  */
void
HAL_Pwm_SetDuty(
    u32 pwm_id,
    u32 period,
    u32 pulse_width
)
{
    HAL_PWM_ADAPTER *pPwmAdapt;

    if (pwm_id >= MAX_PWM_CTRL_PIN) {
        DBG_PWM_ERR ("HAL_Pwm_SetDuty: Invalid PWM index(%d)\n", pwm_id);
        return;
    }

//    DBG_PWM_INFO("%s: Period%d Pulse%d\n", __FUNCTION__, period, pulse_width);
    pPwmAdapt = &PWMPin[pwm_id];

    HAL_Pwm_SetDuty_8195a(pPwmAdapt, period, pulse_width);
}


#endif  // end of "#ifdef CONFIG_PWM_EN"
