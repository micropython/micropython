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

// deep sleep can only be waked up by GPIOB_1 and GTimer
#define GPIO_WAKE_PIN       PB_1

// NOTICE: The pull condition may differnet on your board
PinName pull_down_list[] = {
    PA_0, PA_1, PA_2, PA_3, PA_4, PA_5, PA_6, PA_7,
    PB_0,             PB_3, PB_4, PB_5, PB_6, PB_7,
    PC_0, PC_1, PC_2, PC_3, PC_4, PC_5, PC_6, PC_7, PC_8, PC_9,
    PD_0, PD_1, PD_2, PD_3, PD_4, PD_5, PD_6, PD_7, PD_8, PD_9,
    PE_0, PE_1, PE_2, PE_3, PE_4, PE_5, PE_6, PE_7, PE_8, PE_9, PE_A,
          PF_1, PF_2, PF_3, PF_4, PF_5
};

// NOTICE: The pull condition may differnet on your board
PinName pull_up_list[] = {
                PB_2,
    PF_0,
    PG_0, PG_1, PG_2, PG_3, PG_4, PG_5, PG_6, PG_7,
    PH_0, PH_1, PH_2, PH_3, PH_4, PH_5, PH_6, PH_7,
    PI_0, PI_1, PI_2, PI_3, PI_4, PI_5, PI_6, PI_7,
    PJ_0, PJ_1, PJ_2, PJ_3, PJ_4, PJ_5, PJ_6,
    PK_0, PK_1, PK_2, PK_3, PK_4, PK_5, PK_6
};

void gpio_pull_control()
{
    int i;
    gpio_t gpio_obj;

    for (i=0; i < sizeof(pull_down_list) / sizeof(pull_down_list[0]); i++) {
        gpio_init(&gpio_obj, pull_down_list[i]);
        gpio_dir(&gpio_obj, PIN_INPUT);
        gpio_mode(&gpio_obj, PullDown);
    }

    for (i=0; i < sizeof(pull_up_list) / sizeof(pull_up_list[0]); i++) {
        gpio_init(&gpio_obj, pull_up_list[i]);
        gpio_dir(&gpio_obj, PIN_INPUT);
        gpio_mode(&gpio_obj, PullUp);
    }
}

void gpio_demo_irq_handler (uint32_t id, gpio_irq_event event)
{
    gpio_t *gpio_led;
    gpio_led = (gpio_t *)id;

    printf("Enter deep sleep...Wait 10s or give rising edge at PB_1 to wakeup system.\r\n\r\n");

    // turn off led
    gpio_write(gpio_led, 0);

    // turn off log uart
    sys_log_uart_off();

    // initialize wakeup pin at PB_1
    gpio_t gpio_wake;
    gpio_init(&gpio_wake, GPIO_WAKE_PIN);
    gpio_dir(&gpio_wake, PIN_INPUT);
    gpio_mode(&gpio_wake, PullDown);

    // Please note that the pull control is different in different board
    // This example is a sample code for RTL Ameba Dev Board
    gpio_pull_control();

    // enter deep sleep
    deepsleep_ex(DSLEEP_WAKEUP_BY_GPIO | DSLEEP_WAKEUP_BY_TIMER, 10000);
}

void main(void)
{
    gpio_t gpio_led;
    gpio_irq_t gpio_btn;

    // Init LED control pin
    gpio_init(&gpio_led, GPIO_LED_PIN);
    gpio_dir(&gpio_led, PIN_OUTPUT);    // Direction: Output
    gpio_mode(&gpio_led, PullNone);     // No pull

    // Initial Push Button pin as interrupt source
    gpio_irq_init(&gpio_btn, GPIO_IRQ_PIN, gpio_demo_irq_handler, (uint32_t)(&gpio_led));
    gpio_irq_set(&gpio_btn, IRQ_FALL, 1);   // Falling Edge Trigger
    gpio_irq_enable(&gpio_btn);

    // led on means system is in run mode
    gpio_write(&gpio_led, 1);
    printf("\r\nPush button at PC_4 to enter deep sleep\r\n");

    while(1);
}
