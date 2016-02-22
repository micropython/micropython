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
#include "os_support.h"

#define PWM_1       PC_0 
#define PWM_2       PC_1 
#define PWM_3       PC_2 
#define PWM_4       PC_3 



pwmout_t pwm_led[4];
PinName  pwm_led_pin[4] =  {PWM_1, PWM_2, PWM_3, PWM_4};
float period[8] = {1.0/523, 1.0/587, 1.0/659, 1.0/698, 1.0/784, 1.0/880, 1.0/988, 1.0/1047};

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
    

        pwmout_init(&pwm_led[3], pwm_led_pin[3]);



    while (1) {
 
      for(i=0; i<8; i++){
            pwmout_period(&pwm_led[3], period[i]);            
            pwmout_pulsewidth(&pwm_led[3], period[i]/2);
            Mdelay(1000);
      }

      
//        wait_ms(20);
//        RtlMsleepOS(25);
		pwm_delay();
    }
}

