/*******************************************************************************
* Copyright (c) 2014, Realtek Semiconductor Corp.
* All rights reserved.
*
* This module is a confidential and proprietary property of RealTek and
* possession or use of this module requires written permission of RealTek.
*******************************************************************************/
#ifndef MBED_EXT_TIMER_API_EXT_H
#define MBED_EXT_TIMER_API_EXT_H

#include "device.h"
//#include "rtl8195a.h"

typedef void (*gtimer_irq_handler)(uint32_t id);

typedef struct gtimer_s gtimer_t;
enum { 
    TIMER0 = 2,    // GTimer 2, share with us_tick(wait_ms()) functions
    TIMER1 = 3,    // GTimer 3, share with PWM_0
    TIMER2 = 4,    // GTimer 4, share with PWM_1
    TIMER3 = 5,    // GTimer 5, share with PWM_2
    TIMER4 = 6,    // GTimer 6, share with PWM_3
    TIMER5 = 7,    // share with the software-RTC

    GTIMER_MAX = 7    
};

void gtimer_init (gtimer_t *obj, uint32_t tid);
void gtimer_deinit (gtimer_t *obj);
uint32_t gtimer_read_tick (gtimer_t *obj);
uint64_t gtimer_read_us (gtimer_t *obj);
void gtimer_reload (gtimer_t *obj, uint32_t duration_us);
void gtimer_start (gtimer_t *obj);
void gtimer_start_one_shout (gtimer_t *obj, uint32_t duration_us, void* handler, uint32_t hid);
void gtimer_start_periodical (gtimer_t *obj, uint32_t duration_us, void* handler, uint32_t hid);
void gtimer_stop (gtimer_t *obj);

#endif
