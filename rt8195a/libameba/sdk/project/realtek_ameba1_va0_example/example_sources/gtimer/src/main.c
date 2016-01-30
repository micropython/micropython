/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "gpio_api.h"   // mbed
#include "timer_api.h"
#include "main.h"

#define GPIO_LED_PIN1       PC_0
#define GPIO_LED_PIN2       PC_1

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
//int main_app(IN u16 argc, IN u8 *argv[])

gtimer_t my_timer1;
gtimer_t my_timer2;
gpio_t gpio_led1;
gpio_t gpio_led2;
volatile uint32_t time2_expired=0;

void timer1_timeout_handler(uint32_t id)
{
    gpio_t *gpio_led = (gpio_t *)id;

    gpio_write(gpio_led, !gpio_read(gpio_led));    
}

void timer2_timeout_handler(uint32_t id)
{
    time2_expired = 1;
}

void main(void)
{
    // Init LED control pin
    gpio_init(&gpio_led1, GPIO_LED_PIN1);
    gpio_dir(&gpio_led1, PIN_OUTPUT);    // Direction: Output
    gpio_mode(&gpio_led1, PullNone);     // No pull

    gpio_init(&gpio_led2, GPIO_LED_PIN2);
    gpio_dir(&gpio_led2, PIN_OUTPUT);    // Direction: Output
    gpio_mode(&gpio_led2, PullNone);     // No pull

    // Initial a periodical timer
    gtimer_init(&my_timer1, TIMER0);
    gtimer_start_periodical(&my_timer1, 1000000, (void*)timer1_timeout_handler, (uint32_t)&gpio_led1);

    // Initial a one-shout timer and re-trigger it in while loop
    gtimer_init(&my_timer2, TIMER1);
    time2_expired = 0;
    gtimer_start_one_shout(&my_timer2, 500000, (void*)timer2_timeout_handler, NULL);
    
    while(1){
        if (time2_expired) {
            gpio_write(&gpio_led2, !gpio_read(&gpio_led2));
            time2_expired = 0;
            gtimer_start_one_shout(&my_timer2, 500000, (void*)timer2_timeout_handler, NULL);
        }
    }
}

