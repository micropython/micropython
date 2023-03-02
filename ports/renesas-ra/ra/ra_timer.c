/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021-2023 Renesas Electronics Corporation
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
#include "ra_int.h"
#include "ra_utils.h"
#include "ra_timer.h"

#define AGT_CH_SIZE 2

enum AGT_SOURCE {
    AGT_PCLKB = 0,
    AGT_PCLKB8,
    AGT_PCLKB2 = 3,
    AGT_AGTKCLK,
    AGT_AGT0UNDER,
    AGT_AGTSCLK
};

static R_AGTX0_AGT16_Type *agt_regs[AGT_CH_SIZE] = {
    (R_AGTX0_AGT16_Type *)R_AGTX0,
    (R_AGTX0_AGT16_Type *)R_AGTX1
};

static uint8_t ch_to_irq[AGT_CH_SIZE] = {
    #if defined(VECTOR_NUMBER_AGT0_INT)
    VECTOR_NUMBER_AGT0_INT,
    #else
    0,
    #endif
    #if defined(VECTOR_NUMBER_AGT1_INT)
    VECTOR_NUMBER_AGT1_INT,
    #else
    0,
    #endif
};
static float ra_agt_freq[AGT_CH_SIZE];
static volatile uint32_t ra_agt_counter[AGT_CH_SIZE];
static AGT_TIMER_CB ra_agt_timer_cb[AGT_CH_SIZE];
static void *ra_agt_timer_param[AGT_CH_SIZE];

void ra_agt_timer_set_callback(uint32_t ch, AGT_TIMER_CB cb, void *param) {
    ra_agt_timer_cb[ch] = cb;
    ra_agt_timer_param[ch] = param;
}

static void ra_agt_timer_chk_callback(uint32_t ch) {
    ra_agt_counter[ch] += 1;
    if (ra_agt_timer_cb[ch]) {
        (*ra_agt_timer_cb[ch])(ra_agt_timer_param[ch]);
    }
}

void ra_agt_timer_start(uint32_t ch) {
    agt_regs[ch]->CTRL.AGTCR_b.TSTART = 1; /* start counter */
}

void ra_agt_timer_stop(uint32_t ch) {
    agt_regs[ch]->CTRL.AGTCR_b.TSTART = 0; /* stop counter */
}

void ra_agt_timer_set_freq(uint32_t ch, float freq) {
    R_AGTX0_AGT16_Type *agt_reg = agt_regs[ch];
    uint8_t source = 0;
    uint16_t period = 0;
    uint8_t cks = 0;
    ra_agt_counter[ch] = 0;
    if (freq > (float)(PCLK / 2)) {
        return;
    } else if (freq > 1000.0) {
        source = AGT_PCLKB2;
        period = (uint16_t)((float)(PCLK / 2) / freq);
    } else if (freq > 1.0) {
        source = AGT_AGTSCLK;
        cks = 2;
        period = (uint16_t)((float)(32768 / 4) / freq);
    } else if (freq > 0.01) {
        source = AGT_AGTSCLK;
        period = (uint16_t)((float)(32768 / 128) / freq);
        cks = 7;
    } else {
        return;
    }
    ra_agt_freq[ch] = freq;
    agt_reg->CTRL.AGTCR_b.TSTART = 0;                // stop counter
    agt_reg->CTRL.AGTMR2 = cks;
    agt_reg->CTRL.AGTMR1 = (uint8_t)(source << 4);   // mode is timer mode
    agt_reg->AGT = (uint16_t)period;
}

float ra_agt_timer_get_freq(uint32_t ch) {
    return ra_agt_freq[ch];
}

void ra_agt_timer_init(uint32_t ch, float freq) {
    R_AGTX0_AGT16_Type *agt_reg = agt_regs[ch];
    if (ch == 0) {
        ra_mstpcrd_start(R_MSTP_MSTPCRD_MSTPD3_Msk);
    } else {
        ra_mstpcrd_start(R_MSTP_MSTPCRD_MSTPD2_Msk);
    }
    ra_agt_timer_set_freq(ch, freq);
    agt_reg->CTRL.AGTCR_b.TUNDF = 1;                 // underflow interrupt
    R_BSP_IrqCfgEnable((IRQn_Type const)ch_to_irq[ch], RA_PRI_TIM5, (void *)NULL);
}

void ra_agt_timer_deinit(uint32_t ch) {
    NVIC_DisableIRQ((IRQn_Type const)ch_to_irq[ch]);
    ra_agt_timer_stop(ch);
    ra_agt_timer_cb[ch] = (AGT_TIMER_CB)0;
    ra_agt_timer_param[ch] = (void *)0;
    if (ch == 0) {
        ra_mstpcrd_stop(R_MSTP_MSTPCRD_MSTPD3_Msk);
    } else {
        ra_mstpcrd_stop(R_MSTP_MSTPCRD_MSTPD2_Msk);
    }
}

__WEAK void agt_int_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint32_t ch = irq_to_ch[(uint32_t)irq];
    R_BSP_IrqStatusClear(irq);
    ra_agt_timer_chk_callback(ch);
}

extern uint32_t uwTick;

uint32_t mtick() {
    return uwTick;
}
