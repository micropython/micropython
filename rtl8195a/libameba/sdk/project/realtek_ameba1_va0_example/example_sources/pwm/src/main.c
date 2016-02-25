/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "pwmout_api.h"   // mbed
#include "main.h"

#define PWM_1       PC_0 
#define PWM_2       PC_1 
#define PWM_3       PC_2 
#define PWM_4       PC_3 
#define PWM_PERIOD  20000
#define USE_FLOAT   0

#if USE_FLOAT
#define PWM_STEP    (1.0/20.0)
float pwms[4]={0.0, 0.25, 0.5, 0.75};
float steps[4]={PWM_STEP, PWM_STEP, PWM_STEP, PWM_STEP};
#else
#define PWM_STEP    (PWM_PERIOD/20)
int pwms[4]={0, PWM_PERIOD/4, PWM_PERIOD/2, PWM_PERIOD/4*3};
int steps[4]={PWM_STEP,PWM_STEP,PWM_STEP,PWM_STEP};
#endif

pwmout_t pwm_led[4];
PinName  pwm_led_pin[4] =  {PWM_1, PWM_2, PWM_3, PWM_4};

extern void RtlMsleepOS(u32 ms);

void pwm_delay(void)
{
	for(int i=0;i<1000000;i++)
		asm(" nop");
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
//int main_app(IN u16 argc, IN u8 *argv[])
void main(void)
{
    int i;
    
    for (i=0; i<4; i++) {
        pwmout_init(&pwm_led[i], pwm_led_pin[i]);
        pwmout_period_us(&pwm_led[i], PWM_PERIOD);
    }

    while (1) {
#if USE_FLOAT
        for (i=0; i<4; i++) {
            pwmout_write(&pwm_led[i], pwms[i]);

            pwms[i] += steps[i];
            if (pwms[i] >= 1.0) {
                steps[i] = -PWM_STEP;
                pwms[i] = 1.0;
            }

            if (pwms[i] <= 0.0) {
                steps[i] = PWM_STEP;
                pwms[i] = 0.0;
            }
        }
#else        
        for (i=0; i<4; i++) {
            pwmout_pulsewidth_us(&pwm_led[i], pwms[i]);

            pwms[i] += steps[i];
            if (pwms[i] >= PWM_PERIOD) {
                steps[i] = -PWM_STEP;
                pwms[i] = PWM_PERIOD;
            }

            if (pwms[i] <= 0) {
                steps[i] = PWM_STEP;
                pwms[i] = 0;
            }
        }
#endif        
//        wait_ms(20);
//        RtlMsleepOS(25);
		pwm_delay();
    }
}

