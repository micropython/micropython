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
#include "pwmout_api.h"
//#include <rtl_lib.h>

#if DEVICE_PWMOUT
     
static const PinMap PinMap_PWM[] = {
    {PB_4,  RTL_PIN_PERI(PWM0, 0, S0), RTL_PIN_FUNC(PWM0, S0)},
    {PB_5,  RTL_PIN_PERI(PWM1, 1, S0), RTL_PIN_FUNC(PWM1, S0)},
    {PB_6,  RTL_PIN_PERI(PWM2, 2, S0), RTL_PIN_FUNC(PWM2, S0)},
    {PB_7,  RTL_PIN_PERI(PWM3, 3, S0), RTL_PIN_FUNC(PWM3, S0)},

    {PC_0,  RTL_PIN_PERI(PWM0, 0, S1), RTL_PIN_FUNC(PWM0, S1)},
    {PC_1,  RTL_PIN_PERI(PWM1, 1, S1), RTL_PIN_FUNC(PWM1, S1)},
    {PC_2,  RTL_PIN_PERI(PWM2, 2, S1), RTL_PIN_FUNC(PWM2, S1)},
    {PC_3,  RTL_PIN_PERI(PWM3, 3, S1), RTL_PIN_FUNC(PWM3, S1)},

    {PD_3,  RTL_PIN_PERI(PWM0, 0, S2), RTL_PIN_FUNC(PWM0, S2)},
    {PD_4,  RTL_PIN_PERI(PWM1, 1, S2), RTL_PIN_FUNC(PWM1, S2)},
    {PD_5,  RTL_PIN_PERI(PWM2, 2, S2), RTL_PIN_FUNC(PWM2, S2)},
    {PD_6,  RTL_PIN_PERI(PWM3, 3, S2), RTL_PIN_FUNC(PWM3, S2)},

    {PE_0,  RTL_PIN_PERI(PWM0, 0, S3), RTL_PIN_FUNC(PWM0, S3)},
    {PE_1,  RTL_PIN_PERI(PWM1, 1, S3), RTL_PIN_FUNC(PWM1, S3)},
    {PE_2,  RTL_PIN_PERI(PWM2, 2, S3), RTL_PIN_FUNC(PWM2, S3)},
    {PE_3,  RTL_PIN_PERI(PWM3, 3, S3), RTL_PIN_FUNC(PWM3, S3)},

    {NC,    NC,     0}
};
 
void pwmout_init(pwmout_t* obj, PinName pin) 
{
    uint32_t peripheral;
    u32 pwm_idx;
    u32 pin_sel;

    DBG_PWM_INFO("%s: Init PWM for pin(0x%x)\n", __FUNCTION__, pin);
    
    // Get the peripheral name from the pin and assign it to the object
    peripheral = pinmap_peripheral(pin, PinMap_PWM);

    if (unlikely(peripheral == NC)) {
        DBG_PWM_ERR("%s: Cannot find matched pwm for this pin(0x%x)\n", __FUNCTION__, pin);
        return;
    }

    pwm_idx = RTL_GET_PERI_IDX(peripheral);
    pin_sel = RTL_GET_PERI_SEL(peripheral);

    obj->pwm_idx = pwm_idx;
    obj->pin_sel = pin_sel;
    obj->period = 0;
    obj->pulse = 0;
    HAL_Pwm_Init(pwm_idx, pin_sel);
    pwmout_period_us(obj, 20000); // 20 ms per default
    HAL_Pwm_Enable(pwm_idx);
}

void pwmout_free(pwmout_t* obj) 
{
    HAL_Pwm_Disable(obj->pwm_idx);
}

void pwmout_write(pwmout_t* obj, float value) 
{
    if (value < (float)0.0) {
        value = 0.0;
    } 
    else if (value > (float)1.0) {
        value = 1.0;
    }

    obj->pulse = (uint32_t)((float)obj->period * value);
    HAL_Pwm_SetDuty(obj->pwm_idx, obj->period, obj->pulse);
}

float pwmout_read(pwmout_t* obj) 
{
    float value = 0;
    if (obj->period > 0) {
        value = (float)(obj->pulse) / (float)(obj->period);
    }
    return ((value > (float)1.0) ? (float)(1.0) : (value));
}

void pwmout_period(pwmout_t* obj, float seconds) 
{
    pwmout_period_us(obj, (int)(seconds * 1000000.0f));
}

void pwmout_period_ms(pwmout_t* obj, int ms) 
{
    pwmout_period_us(obj, (int)(ms * 1000));
}

void pwmout_period_us(pwmout_t* obj, int us) 
{
    float dc = pwmout_read(obj);

    obj->period = us;
    // Set duty cycle again
    pwmout_write(obj, dc);
}

void pwmout_pulsewidth(pwmout_t* obj, float seconds) 
{
    pwmout_pulsewidth_us(obj, (int)(seconds * 1000000.0f));
}

void pwmout_pulsewidth_ms(pwmout_t* obj, int ms) 
{
    pwmout_pulsewidth_us(obj, ms * 1000);
}

void pwmout_pulsewidth_us(pwmout_t* obj, int us) 
{
    float value = (float)us / (float)obj->period;
    pwmout_write(obj, value);
}

#endif
