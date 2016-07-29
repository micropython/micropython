/* esp/clocks.h
 *
 * ESP8266 internal clock values
 *
 * At the moment there's not a lot known about varying clock speeds
 * apart from doubling the CPU clock. It may be possible to set clock
 * domains differently somehow.
 *
 * Part of esp-open-rtos
 * Copyright (C) 2015 Superhouse Automation Pty Ltd
 * BSD Licensed as described in the file LICENSE
 */
#ifndef _ESP_CLOCKS_H
#define _ESP_CLOCKS_H

/* CPU clock, can be overclocked to 160MHz via a dport register setting */
#define CPU_CLK_FREQ 80*1000000

/* Main peripheral clock

   This is also the master frequency for the UART and the TIMER module
   (before divisors applied to either.)
 */
#ifndef APB_CLK_FREQ
#define APB_CLK_FREQ CPU_CLK_FREQ
#endif

#endif
