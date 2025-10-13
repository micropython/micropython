/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Damien P. George
 * Copyright (c) 2023-2024 Infineon Technologies AG
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// std includes
#include "stdio.h"

// micropython includes
#include "extmod/modtime.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"

// MTB includes
#include "mtb_hal.h"
#include "cybsp.h"

mtb_hal_timer_t psoc_edge_timer;

#define TIMER_HW CYBSP_GENERAL_PURPOSE_TIMER_HW
#define TIMER_NUM CYBSP_GENERAL_PURPOSE_TIMER_NUM
static cy_stc_tcpwm_counter_config_t timer_config;
mtb_hal_timer_configurator_t timer_hal_config;

void time_init(void) {
    cy_rslt_t rslt;
    timer_config = CYBSP_GENERAL_PURPOSE_TIMER_config;
    timer_hal_config = CYBSP_GENERAL_PURPOSE_TIMER_hal_config;
    psoc_edge_timer.tcpwm.base = TIMER_HW;
    psoc_edge_timer.tcpwm.cntnum = TIMER_NUM;

    rslt = Cy_TCPWM_Counter_Init(TIMER_HW, TIMER_NUM, &timer_config);

    if (rslt == CY_RSLT_SUCCESS) {
        rslt = mtb_hal_timer_setup(&psoc_edge_timer, &timer_hal_config, NULL);
    }

    if (rslt == CY_RSLT_SUCCESS) {
        rslt = mtb_hal_timer_start(&psoc_edge_timer);
    }
}

void time_deinit(void) {
    mtb_hal_timer_stop(&psoc_edge_timer);
}
