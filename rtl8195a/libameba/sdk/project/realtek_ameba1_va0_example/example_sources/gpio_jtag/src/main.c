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
#include "sys_api.h"    // for sys_jtag_off()
#include "main.h"

#define GPIO_JTAG_ENABLE_PIN    PC_4
#define GPIO_LED_PIN            PE_0

void main(void)
{
    int i;
    gpio_t gpio_jtag_enable;
    gpio_t gpio_led;

    gpio_init(&gpio_jtag_enable, GPIO_JTAG_ENABLE_PIN);
    gpio_dir(&gpio_jtag_enable, PIN_INPUT);
    gpio_mode(&gpio_jtag_enable, PullUp);

    if (gpio_read(&gpio_jtag_enable) == 0)
    {
        // JTAG enable pin is disabled
        sys_jtag_off();
        printf("jtag off\r\n");

        // Now you can use jtag pin for other gpio usage
        // ex. use PE_0 to blink led
        gpio_init(&gpio_led, GPIO_LED_PIN);
        gpio_dir(&gpio_led, PIN_OUTPUT);    // Direction: Output
        gpio_mode(&gpio_led, PullNone);     // No pull

        while(1)
        {
            gpio_write(&gpio_led, 1);
            for (i=0; i<10000000; i++) asm(" nop"); // simple delay
            gpio_write(&gpio_led, 0);
            for (i=0; i<10000000; i++) asm(" nop"); // simple delay
        }
    }
    else
    {
        // JTAG enable pin is enabled
        printf("jtag on\r\n");
    }

    for (;;);
}

