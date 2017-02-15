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
#include "main.h"

#define GPIO_LED_PIN       PC_5
#define GPIO_PUSHBT_PIN    PC_4

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
//int main_app(IN u16 argc, IN u8 *argv[])
void main(void)
{
    gpio_t gpio_led;
    gpio_t gpio_btn;

    // Init LED control pin
    gpio_init(&gpio_led, GPIO_LED_PIN);
    gpio_dir(&gpio_led, PIN_OUTPUT);    // Direction: Output
    gpio_mode(&gpio_led, PullNone);     // No pull

    // Initial Push Button pin
    gpio_init(&gpio_btn, GPIO_PUSHBT_PIN);
    gpio_dir(&gpio_btn, PIN_INPUT);     // Direction: Input
    gpio_mode(&gpio_btn, PullUp);       // Pull-High

    while(1){
        if (gpio_read(&gpio_btn)) {
            // turn off LED
            gpio_write(&gpio_led, 0);
        }
        else {
            // turn on LED
            gpio_write(&gpio_led, 1);
        }
    }
}

