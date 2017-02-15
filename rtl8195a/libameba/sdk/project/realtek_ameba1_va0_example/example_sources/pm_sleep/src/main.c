/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "gpio_api.h"   // mbed
#include "gpio_irq_api.h"   // mbed
#include "sleep_ex_api.h"
#include "sys_api.h"
#include "diag.h"
#include "main.h"

#define GPIO_LED_PIN        PC_5
#define GPIO_IRQ_PIN        PC_4

int led_ctrl = 0;
gpio_t gpio_led;

int put_to_sleep = 0;

void gpio_demo_irq_handler (uint32_t id, gpio_irq_event event)
{
    gpio_t *gpio_led;

    gpio_led = (gpio_t *)id;

    if (led_ctrl == 1) {
        led_ctrl = 0;
        gpio_write(gpio_led, led_ctrl);
        put_to_sleep = 1;
    } else {
        led_ctrl = 1;
        gpio_write(gpio_led, led_ctrl);
    }
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
    gpio_irq_t gpio_btn;

    DBG_INFO_MSG_OFF(_DBG_GPIO_);

    // Init LED control pin
    gpio_init(&gpio_led, GPIO_LED_PIN);
    gpio_dir(&gpio_led, PIN_OUTPUT);    // Direction: Output
    gpio_mode(&gpio_led, PullNone);     // No pull

    // Initial Push Button pin as interrupt source
    gpio_irq_init(&gpio_btn, GPIO_IRQ_PIN, gpio_demo_irq_handler, (uint32_t)(&gpio_led));
    gpio_irq_set(&gpio_btn, IRQ_FALL, 1);
    gpio_irq_enable(&gpio_btn);

    led_ctrl = 1;
    gpio_write(&gpio_led, led_ctrl);
    DBG_8195A("Push button to enter sleep\r\n");

    put_to_sleep = 0;
    while(1) {
        if (put_to_sleep) {
            DBG_8195A("Sleep 8s or push button to resume system...\r\n");
            sys_log_uart_off();
            sleep_ex(SLP_GPIO | SLEEP_WAKEUP_BY_STIMER, 8000); // sleep_ex can't be put in irq handler
            sys_log_uart_on();
            DBG_8195A("System resume\r\n");

            put_to_sleep = 0;
            led_ctrl = 1;
            gpio_write(&gpio_led, led_ctrl);
        }
    }
}
