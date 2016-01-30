/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "gpio_irq_api.h"   // mbed
#include "gpio_irq_ex_api.h"
#include "diag.h"
#include "main.h"

#define GPIO_IRQ_LEVEL_PIN        PC_4
#define GPIO_SIGNAL_SOURCE        PC_5

gpio_irq_t gpio_level;
int current_level = IRQ_LOW;

void gpio_level_irq_handler (uint32_t id, gpio_irq_event event)
{
    uint32_t *level = (uint32_t *) id;

    // Disable level irq because the irq will keep triggered when it keeps in same level.
    gpio_irq_disable(&gpio_level);

    // make some software de-bounce here if the signal source is not stable.

    if (*level == IRQ_LOW )
    {
        printf("low level event\r\n");

        // Change to listen to high level event
        *level = IRQ_HIGH;
        gpio_irq_set(&gpio_level, IRQ_HIGH, 1);
        gpio_irq_enable(&gpio_level);
    }
    else if (*level == IRQ_HIGH)
    {
        printf("high level event\r\n");

        // Change to listen to low level event
        *level = IRQ_LOW;
        gpio_irq_set(&gpio_level, IRQ_LOW, 1);
        gpio_irq_enable(&gpio_level);
    }
}

void main(void)
{
    int i;

    // configure level trigger handler
    gpio_irq_init(&gpio_level, GPIO_IRQ_LEVEL_PIN, gpio_level_irq_handler, (uint32_t)(&current_level));
    gpio_irq_set(&gpio_level, IRQ_LOW, 1);
    gpio_irq_enable(&gpio_level);

    // configure gpio as signal source for high/low level trigger
    gpio_t gpio_src;
    gpio_init(&gpio_src, GPIO_SIGNAL_SOURCE);
    gpio_dir(&gpio_src, PIN_OUTPUT);    // Direction: Output
    gpio_mode(&gpio_src, PullNone);

    while(1) {
        gpio_write(&gpio_src, 1);
        for (i=0; i<20000000; i++) asm("nop");
        gpio_write(&gpio_src, 0);
        for (i=0; i<20000000; i++) asm("nop");
    }
}

