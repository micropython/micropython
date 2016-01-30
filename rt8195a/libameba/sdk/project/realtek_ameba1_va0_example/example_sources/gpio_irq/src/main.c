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
#include "gpio_irq_api.h"   // mbed
#include "diag.h"
#include "main.h"

#define GPIO_LED_PIN        PC_5
#define GPIO_IRQ_PIN        PC_4

int led_ctrl;
gpio_t gpio_led;

extern u32 ConfigDebugWarn;

void gpio_demo_irq_handler (uint32_t id, gpio_irq_event event)
{
    gpio_t *gpio_led;

    DBG_GPIO_WARN("%s==>\n", __FUNCTION__);
    gpio_led = (gpio_t *)id;

    led_ctrl = !led_ctrl;
    gpio_write(gpio_led, led_ctrl);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
    gpio_irq_t gpio_btn;

    // Init LED control pin
    gpio_init(&gpio_led, GPIO_LED_PIN);
    gpio_dir(&gpio_led, PIN_OUTPUT);    // Direction: Output
    gpio_mode(&gpio_led, PullNone);     // No pull

    // Initial Push Button pin as interrupt source
    gpio_irq_init(&gpio_btn, GPIO_IRQ_PIN, gpio_demo_irq_handler, (uint32_t)(&gpio_led));
    gpio_irq_set(&gpio_btn, IRQ_FALL, 1);   // Falling Edge Trigger
    gpio_irq_enable(&gpio_btn);

    led_ctrl = 1;
    gpio_write(&gpio_led, led_ctrl);

    while(1);
}

