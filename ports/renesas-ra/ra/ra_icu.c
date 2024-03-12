/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"
#include "ra_config.h"
#include "ra_gpio.h"
#include "ra_int.h"
#include "ra_timer.h"
#include "ra_icu.h"

#define DEFAULT_BOUNCE_PERIOD (200) /* 200ms */

#if !defined(RA_PRI_EXTIT)
#define RA_PRI_EXTINT (14)
#endif

static R_ICU_Type *icu_reg = (R_ICU_Type *)0x40006000;

enum
{
    #if defined(VECTOR_NUMBER_ICU_IRQ0)
    IRQ0_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ1)
    IRQ1_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ2)
    IRQ2_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ3)
    IRQ3_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ4)
    IRQ4_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ5)
    IRQ5_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ6)
    IRQ6_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ7)
    IRQ7_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ8)
    IRQ8_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ9)
    IRQ9_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ10)
    IRQ10_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ11)
    IRQ11_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ12)
    IRQ12_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ13)
    IRQ13_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ14)
    IRQ14_IDX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ15)
    IRQ15_IDX,
    #endif
    IRQ_IDX_MAX,
};

static const ra_icu_pin_t ra_irq_pins[] = {
    #if defined(RA4W1)

    #if defined(VECTOR_NUMBER_ICU_IRQ0)
    { 0, P105 },
    { 0, P206 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ1)
    { 1, P101 },
    { 1, P104 },
    { 1, P205 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ2)
    { 2, P100 },
    { 2, P213 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ3)
    { 3, P004 },
    { 3, P110 },
    { 3, P212 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ4)
    { 4, P111 },
    { 4, P402 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ6)
    { 6, P409 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ7)
    { 7, P015 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ9)
    { 9, P414 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ11)
    { 11, P501 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ14)
    { 14, P010 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ15)
    { 15, P011 },
    #endif

    #elif defined(RA4M1)

    #if defined(VECTOR_NUMBER_ICU_IRQ0)
    { 0, P105 },
    { 0, P206 },
    { 0, P400 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ1)
    { 1, P101 },
    { 1, P104 },
    { 1, P205 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ2)
    { 2, P002 },
    { 2, P100 },
    { 2, P213 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ3)
    { 3, P004 },
    { 3, P110 },
    { 3, P212 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ4)
    { 4, P111 },
    { 4, P411 },
    { 4, P402 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ5)
    { 5, P302 },
    { 5, P410 },
    { 5, P401 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ6)
    { 6, P000 },
    { 6, P301 },
    { 6, P409 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ7)
    { 7, P001 },
    { 7, P015 },
    { 7, P408 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ8)
    { 8, P305 },
    { 8, P415 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ9)
    { 9, P304 },
    { 9, P414 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ10)
    { 10, P005 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ11)
    { 11, P501 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ12)
    { 12, P502 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ14)
    { 14, P505 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ15)
    { 15, P011 },
    #endif

    #elif defined(RA6M1)

    #if defined(VECTOR_NUMBER_ICU_IRQ0)
    { 0, P105 },
    { 0, P206 }, /* DS */
    { 0, P400 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ1)
    { 1, P101 },
    { 1, P104 },
    { 1, P205 }, /* DS */
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ2)
    { 2, P100 },
    { 2, P213 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ3)
    { 3, P110 },
    { 3, P212 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ4)
    { 4, P111 },
    { 4, P402 }, /* DS */
    { 4, P411 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ5)
    { 5, P302 },
    { 5, P401 }, /* DS */
    { 5, P410 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ6)
    { 6, P000 }, /* DS */
    { 6, P301 },
    { 6, P409 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ7)
    { 7, P001 }, /* DS */
    { 7, P408 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ8)
    { 8, P002 }, /* DS */
    { 8, P305 },
    { 8, P415 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ9)
    { 9, P004 }, /*  DS */
    { 9, P304 },
    { 9, P414 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ10)
    { 10, P005 }, /* DS */
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ11)
    { 11, P006 }, /* DS */
    { 11, P501 },
    { 11, P708 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ12)
    { 12, P008 }, /* DS */
    { 12, P502 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ13)
    { 13, P015 },
    #endif

    #elif defined(RA6M2)

    #if defined(VECTOR_NUMBER_ICU_IRQ0)
    { 0, P105 },
    { 0, P206 }, /* DS */
    { 0, P400 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ1)
    { 1, P101 },
    { 1, P104 },
    { 1, P205 }, /* DS */
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ2)
    { 2, P100 },
    { 2, P213 },
    { 2, P203 }, /* DS */
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ3)
    { 3, P110 },
    { 3, P212 },
    { 3, P202 }, /* DS */
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ4)
    { 4, P111 },
    { 4, P402 }, /* DS */
    { 4, P411 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ5)
    { 5, P302 },
    { 5, P401 }, /* DS */
    { 5, P410 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ6)
    { 6, P000 }, /* DS */
    { 6, P301 },
    { 6, P409 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ7)
    { 7, P001 }, /* DS */
    { 7, P408 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ8)
    { 8, P002 }, /* DS */
    { 8, P305 },
    { 8, P415 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ9)
    { 9, P004 }, /*  DS */
    { 9, P304 },
    { 9, P414 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ10)
    { 10, P005 }, /* DS */
    { 10, P709 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ11)
    { 11, P006 }, /* DS */
    { 11, P501 },
    { 11, P708 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ12)
    { 12, P008 }, /* DS */
    { 12, P502 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ13)
    { 13, P009 }, /* DS */
    { 13, P015 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ14)
    { 14, P505 },
    { 14, P512 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ15)
    { 15, P506 },
    { 15, P511 },
    #endif

    #elif defined(RA6M3)

    #if defined(VECTOR_NUMBER_ICU_IRQ0)
    { 0, P105 },
    { 0, P206 }, /* DS */
    { 0, P400 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ1)
    { 1, P101 },
    { 1, P104 },
    { 1, P205 }, /* DS */
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ2)
    { 2, P100 },
    { 2, P213 },
    { 2, P203 }, /* DS */
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ3)
    { 3, P110 },
    { 3, P212 },
    { 3, P202 }, /* DS */
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ4)
    { 4, P111 },
    { 4, P402 }, /* DS */
    { 4, P411 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ5)
    { 5, P302 },
    { 5, P401 }, /* DS */
    { 5, P410 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ6)
    { 6, P000 }, /* DS */
    { 6, P301 },
    { 6, P409 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ7)
    { 7, P001 }, /* DS */
    { 7, P408 },
    { 7, P706 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ8)
    { 8, P002 }, /* DS */
    { 8, P305 },
    { 8, P415 },
    { 8, P707 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ9)
    { 9, P004 }, /*  DS */
    { 9, P304 },
    { 9, P414 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ10)
    { 10, P005 }, /* DS */
    { 10, P709 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ11)
    { 11, P006 }, /* DS */
    { 11, P501 },
    { 11, P708 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ12)
    { 12, P008 }, /* DS */
    { 12, P502 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ13)
    { 13, P009 }, /* DS */
    { 13, P015 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ14)
    { 14, P505 },
    { 14, P512 },
    { 14, P010 }, /* DS */
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ15)
    { 15, P506 },
    { 15, P511 },
    #endif

    #elif defined(RA6M5)

    #if defined(VECTOR_NUMBER_ICU_IRQ0)
    { 0, P105 },
    { 0, P206 }, /* DS */
    { 0, P400 },
    { 0, P806 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ1)
    { 1, P101 },
    { 1, P104 },
    { 1, P205 }, /* DS */
    { 1, P804 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ2)
    { 2, P100 },
    { 2, P203 }, /* DS */
    { 2, P213 },
    { 2, P803 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ3)
    { 3, P110 },
    { 3, P202 }, /* DS */
    { 3, P212 },
    { 3, P802 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ4)
    { 4, P111 },
    { 4, P402 }, /* DS */
    { 4, P411 },
    { 4, PA10 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ5)
    { 5, P302 },
    { 5, P401 }, /* DS */
    { 5, P410 },
    { 5, PA09 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ6)
    { 6, P000 }, /* DS */
    { 6, P301 },
    { 6, P409 },
    { 6, PA08 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ7)
    { 7, P001 }, /* DS */
    { 7, P408 },
    { 7, P706 },
    { 7, P615 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ8)
    { 8, P002 }, /* DS */
    { 8, P305 },
    { 8, P415 },
    { 8, P707 },
    { 8, P905 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ9)
    { 9, P004 }, /*  DS */
    { 9, P304 },
    { 9, P414 },
    { 9, P906 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ10)
    { 10, P005 }, /* DS */
    { 10, P709 },
    { 10, P907 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ11)
    { 11, P006 }, /* DS */
    { 11, P501 },
    { 11, P708 },
    { 11, P908 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ12)
    { 12, P008 }, /* DS */
    { 12, P502 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ13)
    { 13, P009 }, /* DS */
    { 13, P015 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ14)
    { 14, P010 },
    { 14, P403 }, /* DS */
    { 14, P505 },
    { 14, P512 },
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ15)
    { 15, P404 }, /* DS */
    { 15, P506 },
    { 15, P511 },
    #endif

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define ICU_PINS_SIZE (sizeof(ra_irq_pins) / sizeof(ra_icu_pin_t))

static const uint32_t idx_to_irq_vec[] = {
    #if defined(VECTOR_NUMBER_ICU_IRQ0)
    VECTOR_NUMBER_ICU_IRQ0,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ1)
    VECTOR_NUMBER_ICU_IRQ1,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ2)
    VECTOR_NUMBER_ICU_IRQ2,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ3)
    VECTOR_NUMBER_ICU_IRQ3,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ4)
    VECTOR_NUMBER_ICU_IRQ4,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ5)
    VECTOR_NUMBER_ICU_IRQ5,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ6)
    VECTOR_NUMBER_ICU_IRQ6,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ7)
    VECTOR_NUMBER_ICU_IRQ7,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ8)
    VECTOR_NUMBER_ICU_IRQ8,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ9)
    VECTOR_NUMBER_ICU_IRQ9,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ10)
    VECTOR_NUMBER_ICU_IRQ10,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ11)
    VECTOR_NUMBER_ICU_IRQ11,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ12)
    VECTOR_NUMBER_ICU_IRQ12,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ13)
    VECTOR_NUMBER_ICU_IRQ13,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ14)
    VECTOR_NUMBER_ICU_IRQ14,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ15)
    VECTOR_NUMBER_ICU_IRQ15,
    #endif
};

static const uint8_t irq_no_to_idx[] = {
    #if defined(VECTOR_NUMBER_ICU_IRQ0)
    IRQ0_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ1)
    IRQ1_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ2)
    IRQ2_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ3)
    IRQ3_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ4)
    IRQ4_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ5)
    IRQ5_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ6)
    IRQ6_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ7)
    IRQ7_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ8)
    IRQ8_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ9)
    IRQ9_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ10)
    IRQ10_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ11)
    IRQ11_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ12)
    IRQ12_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ13)
    IRQ13_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ14)
    IRQ14_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ15)
    IRQ15_IDX,
    #else
    IRQ_IDX_MAX,
    #endif
};

static const uint8_t irq_no_to_irq_vec[] = {
    #if defined(VECTOR_NUMBER_ICU_IRQ0)
    VECTOR_NUMBER_ICU_IRQ0,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ1)
    VECTOR_NUMBER_ICU_IRQ1,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ2)
    VECTOR_NUMBER_ICU_IRQ2,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ3)
    VECTOR_NUMBER_ICU_IRQ3,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ4)
    VECTOR_NUMBER_ICU_IRQ4,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ5)
    VECTOR_NUMBER_ICU_IRQ5,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ6)
    VECTOR_NUMBER_ICU_IRQ6,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ7)
    VECTOR_NUMBER_ICU_IRQ7,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ8)
    VECTOR_NUMBER_ICU_IRQ8,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ9)
    VECTOR_NUMBER_ICU_IRQ9,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ10)
    VECTOR_NUMBER_ICU_IRQ10,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ11)
    VECTOR_NUMBER_ICU_IRQ11,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ12)
    VECTOR_NUMBER_ICU_IRQ12,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ13)
    VECTOR_NUMBER_ICU_IRQ13,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ14)
    VECTOR_NUMBER_ICU_IRQ14,
    #else
    VECTOR_NUMBER_NONE,
    #endif
    #if defined(VECTOR_NUMBER_ICU_IRQ15)
    VECTOR_NUMBER_ICU_IRQ15,
    #else
    VECTOR_NUMBER_NONE,
    #endif
};
static ICU_CB icu_cbs[IRQ_IDX_MAX];
static void *icu_params[IRQ_IDX_MAX];
static bool bounce_flag[IRQ_IDX_MAX];
static uint32_t bounce_period[IRQ_IDX_MAX];
static uint32_t bounce_start[IRQ_IDX_MAX];

bool ra_icu_find_irq_no(uint32_t pin, uint8_t *irq_no) {
    ra_icu_pin_t *icu_pin = (ra_icu_pin_t *)&ra_irq_pins;
    uint32_t size = (uint32_t)ICU_PINS_SIZE;
    bool find = false;
    uint32_t i;
    for (i = 0; i < size; i++) {
        if (icu_pin->pin == pin) {
            find = true;
            *irq_no = icu_pin->irq_no;
            break;
        }
        icu_pin++;
    }
    return find;
}

static void ra_icu_irq_no(uint8_t irq_no, bool enable) {
    uint8_t irq_vec = irq_no_to_irq_vec[irq_no];
    if (irq_vec != VECTOR_NUMBER_NONE) {
        if (enable) {
            R_BSP_IrqCfg((IRQn_Type const)irq_vec, (uint32_t)RA_PRI_EXTINT, (void *)NULL);
            R_BSP_IrqEnable((IRQn_Type const)irq_vec);
        } else {
            R_BSP_IrqDisable((IRQn_Type const)irq_vec);
        }
    }
}

void ra_icu_set_pin(uint32_t pin, bool irq_enable, bool pullup) {
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    uint32_t pfs = _PXXPFS(port, bit);
    pwpr_unprotect();
    pfs &= ~PMR_MASK; /* GPIO */
    pfs &= ~PDR_MASK; /* input */
    if (irq_enable) {
        pfs |= ISEL_MASK; /* set pullup */
    } else {
        pfs &= ~ISEL_MASK; /* clear pullup */
    }
    if (pullup) {
        pfs |= PCR_MASK; /* set pullup */
    } else {
        pfs &= ~PCR_MASK; /* clear pullup */
    }
    _PXXPFS(port, bit) = pfs;
    pwpr_protect();
}

static void ra_icu_pin(uint32_t pin, bool enable) {
    bool find = false;
    uint8_t irq_no;
    find = ra_icu_find_irq_no(pin, &irq_no);
    if (find) {
        ra_icu_irq_no(irq_no, enable);
    }
}

void ra_icu_enable_irq_no(uint8_t irq_no) {
    ra_icu_irq_no(irq_no, true);
}

void ra_icu_disable_irq_no(uint8_t irq_no) {
    ra_icu_irq_no(irq_no, false);
}

void ra_icu_enable_pin(uint32_t pin) {
    ra_icu_pin(pin, true);
}

void ra_icu_disable_pin(uint32_t pin) {
    ra_icu_pin(pin, false);
}

void ra_icu_priority_irq_no(uint8_t irq_no, uint32_t ipl) {
    uint8_t irq_vec = (uint8_t)idx_to_irq_vec[irq_no];
    R_BSP_IrqCfg((IRQn_Type const)irq_vec, ipl, (void *)NULL);
}

void ra_icu_priority_pin(uint32_t pin, uint32_t ipl) {
    bool find = false;
    uint8_t irq_no;
    find = ra_icu_find_irq_no(pin, &irq_no);
    if (find) {
        ra_icu_priority_irq_no(irq_no, ipl);
    }
}

void ra_icu_set_callback(uint8_t irq_no, ICU_CB func, void *param) {
    uint8_t idx = irq_no_to_idx[irq_no];
    if (idx != IRQ_IDX_MAX) {
        icu_cbs[idx] = func;
        icu_params[idx] = param;
    }
}

static void ra_icu_callback(uint8_t irq_no) {
    uint8_t idx = irq_no_to_idx[irq_no];
    if (idx != IRQ_IDX_MAX) {
        if (bounce_flag[idx]) {
            if ((mtick() - bounce_start[idx]) > bounce_period[idx]) {
                bounce_flag[idx] = false;
            } else {
                return;
            }
        }
        if (icu_cbs[idx] != NULL) {
            if (!bounce_flag[idx]) {
                bounce_start[idx] = mtick();
                bounce_flag[idx] = true;
                (*icu_cbs[idx])(icu_params[idx]);
            }
        }
    }
}

/*
 * pin: cpu pin
 * irq_no: IRQ number
 * cond: 0: falling, 1: rising, 2: both edge, 3 low level
 */
void ra_icu_trigger_irq_no(uint8_t irq_no, uint32_t cond) {
    icu_reg->IRQCR_b[irq_no].IRQMD = (uint8_t)cond;
}

void ra_icu_trigger_pin(uint32_t pin, uint32_t cond) {
    bool find = false;
    uint8_t irq_no;
    find = ra_icu_find_irq_no(pin, &irq_no);
    if (find) {
        ra_icu_trigger_irq_no(irq_no, cond);
    }
}

void ra_icu_set_bounce(uint8_t irq_no, uint32_t bounce) {
    uint8_t idx = irq_no_to_idx[irq_no];
    if (idx != IRQ_IDX_MAX) {
        bounce_period[idx] = bounce;
    }
}

static void ra_icu_bounce_init(void) {
    uint32_t idx;
    for (idx = 0; idx < IRQ_IDX_MAX; idx++) {
        bounce_flag[idx] = false;
        bounce_period[idx] = DEFAULT_BOUNCE_PERIOD;
    }
}

void ra_icu_init(void) {
    ra_icu_bounce_init();
}

void ra_icu_deinit(void) {
    uint32_t idx;
    for (idx = 0; idx < IRQ_IDX_MAX; idx++) {
        R_BSP_IrqDisable(idx_to_irq_vec[idx]);
    }
}

void ra_icu_swint(uint8_t irq_no) {
    // RA MCU doesn't support STM32 EXTINT SWINT
    // just call callback function
    uint8_t idx = irq_no_to_idx[irq_no];
    if (icu_cbs[idx] != NULL) {
        (*icu_cbs[idx])(icu_params[idx]);
    }
}

__WEAK void r_icu_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint32_t irq_no = (uint32_t)irq_to_ch[(uint32_t)irq];
    R_BSP_IrqStatusClear(irq);
    ra_icu_callback(irq_no);
}
