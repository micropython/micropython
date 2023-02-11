/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
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

#include "hal_data.h"
#include "ra_config.h"
#include "ra_gpio.h"
#include "ra_int.h"
#include "ra_utils.h"
#include "ra_sci.h"

#if !defined(RA_PRI_UART)
#define RA_PRI_UART (1)
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-parameter"
// #pragma GCC diagnostic ignored "-Wconversion"
// #pragma GCC diagnostic ignored "-Wshift-negative-value"
// #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
// #pragma GCC diagnostic ignored "-Wsequence-point"
// #pragma GCC diagnostic ignored "-Wunused-function"
#endif

enum
{
    #if defined(VECTOR_NUMBER_SCI0_RXI)
    SCI0_IDX,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_RXI)
    SCI1_IDX,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_RXI)
    SCI2_IDX,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_RXI)
    SCI3_IDX,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_RXI)
    SCI4_IDX,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_RXI)
    SCI5_IDX,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_RXI)
    SCI6_IDX,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_RXI)
    SCI7_IDX,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_RXI)
    SCI8_IDX,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_RXI)
    SCI9_IDX,
    #endif
    SCI_IDX_MAX,
};

static const IRQn_Type idx_to_rxirq[] = {
    #if defined(VECTOR_NUMBER_SCI0_RXI)
    VECTOR_NUMBER_SCI0_RXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_RXI)
    VECTOR_NUMBER_SCI1_RXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_RXI)
    VECTOR_NUMBER_SCI2_RXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_RXI)
    VECTOR_NUMBER_SCI3_RXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_RXI)
    VECTOR_NUMBER_SCI4_RXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_RXI)
    VECTOR_NUMBER_SCI5_RXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_RXI)
    VECTOR_NUMBER_SCI6_RXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_RXI)
    VECTOR_NUMBER_SCI7_RXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_RXI)
    VECTOR_NUMBER_SCI8_RXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_RXI)
    VECTOR_NUMBER_SCI9_RXI,
    #endif
};

static const IRQn_Type idx_to_txirq[] = {
    #if defined(VECTOR_NUMBER_SCI0_TXI)
    VECTOR_NUMBER_SCI0_TXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_TXI)
    VECTOR_NUMBER_SCI1_TXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_TXI)
    VECTOR_NUMBER_SCI2_TXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_TXI)
    VECTOR_NUMBER_SCI3_TXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_TXI)
    VECTOR_NUMBER_SCI4_TXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_TXI)
    VECTOR_NUMBER_SCI5_TXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_TXI)
    VECTOR_NUMBER_SCI6_TXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_TXI)
    VECTOR_NUMBER_SCI7_TXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_TXI)
    VECTOR_NUMBER_SCI8_TXI,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_TXI)
    VECTOR_NUMBER_SCI9_TXI,
    #endif
};

static const IRQn_Type idx_to_teirq[] = {
    #if defined(VECTOR_NUMBER_SCI0_TEI)
    VECTOR_NUMBER_SCI0_TEI,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_TEI)
    VECTOR_NUMBER_SCI1_TEI,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_TEI)
    VECTOR_NUMBER_SCI2_TEI,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_TEI)
    VECTOR_NUMBER_SCI3_TEI,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_TEI)
    VECTOR_NUMBER_SCI4_TEI,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_TEI)
    VECTOR_NUMBER_SCI5_TEI,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_TEI)
    VECTOR_NUMBER_SCI6_TEI,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_TEI)
    VECTOR_NUMBER_SCI7_TEI,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_TEI)
    VECTOR_NUMBER_SCI8_TEI,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_TEI)
    VECTOR_NUMBER_SCI9_TEI,
    #endif
};

static const IRQn_Type idx_to_erirq[] = {
    #if defined(VECTOR_NUMBER_SCI0_ERI)
    VECTOR_NUMBER_SCI0_ERI,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_ERI)
    VECTOR_NUMBER_SCI1_ERI,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_ERI)
    VECTOR_NUMBER_SCI2_ERI,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_ERI)
    VECTOR_NUMBER_SCI3_ERI,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_ERI)
    VECTOR_NUMBER_SCI4_ERI,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_ERI)
    VECTOR_NUMBER_SCI5_ERI,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_ERI)
    VECTOR_NUMBER_SCI6_ERI,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_ERI)
    VECTOR_NUMBER_SCI7_ERI,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_ERI)
    VECTOR_NUMBER_SCI8_ERI,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_ERI)
    VECTOR_NUMBER_SCI9_ERI,
    #endif
};

static uint32_t ch_to_idx[SCI_CH_MAX] = {
    #if defined(VECTOR_NUMBER_SCI0_RXI)
    SCI0_IDX,
    #else
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_RXI)
    SCI1_IDX,
    #else
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_RXI)
    SCI2_IDX,
    #else
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_RXI)
    SCI3_IDX,
    #else
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_RXI)
    SCI4_IDX,
    #else
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_RXI)
    SCI5_IDX,
    #else
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_RXI)
    SCI6_IDX,
    #else
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_RXI)
    SCI7_IDX,
    #else
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_RXI)
    SCI8_IDX,
    #else
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_RXI)
    SCI9_IDX,
    #else
    0,
    #endif
};

static R_SCI0_Type *sci_regs[] = {
    #if defined(VECTOR_NUMBER_SCI0_RXI)
    (R_SCI0_Type *)0x40070000,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_RXI)
    (R_SCI0_Type *)0x40070020,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_RXI)
    (R_SCI0_Type *)0x40070040,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_RXI)
    (R_SCI0_Type *)0x40070060,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_RXI)
    (R_SCI0_Type *)0x40070080,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_RXI)
    (R_SCI0_Type *)0x400700a0,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_RXI)
    (R_SCI0_Type *)0x400700c0,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_RXI)
    (R_SCI0_Type *)0x400700e0,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_RXI)
    (R_SCI0_Type *)0x40070100,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_RXI)
    (R_SCI0_Type *)0x40070120,
    #endif
};

static uint32_t sci_module_mask[] = {
    #if defined(VECTOR_NUMBER_SCI0_RXI)
    R_MSTP_MSTPCRB_MSTPB31_Msk,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_RXI)
    R_MSTP_MSTPCRB_MSTPB30_Msk,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_RXI)
    R_MSTP_MSTPCRB_MSTPB29_Msk,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_RXI)
    R_MSTP_MSTPCRB_MSTPB28_Msk,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_RXI)
    R_MSTP_MSTPCRB_MSTPB27_Msk,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_RXI)
    R_MSTP_MSTPCRB_MSTPB26_Msk,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_RXI)
    R_MSTP_MSTPCRB_MSTPB25_Msk,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_RXI)
    R_MSTP_MSTPCRB_MSTPB24_Msk,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_RXI)
    R_MSTP_MSTPCRB_MSTPB23_Msk,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_RXI)
    R_MSTP_MSTPCRB_MSTPB22_Msk,
    #endif
};

static const ra_af_pin_t ra_sci_tx_pins[] = {
    #if defined(RA4M1)

    { AF_SCI1, 0, P101 },
    { AF_SCI1, 0, P205 },
    { AF_SCI1, 0, P411 },

    { AF_SCI2, 1, P213 },
    { AF_SCI2, 1, P401 },
    { AF_SCI2, 1, P501 },

    { AF_SCI2, 2, P102 },
    { AF_SCI1, 2, P112 },
    { AF_SCI1, 2, P302 },

    { AF_SCI2, 9, P109 },
    { AF_SCI2, 9, P203 },
    { AF_SCI2, 9, P409 },
    { AF_SCI2, 9, P602 },

    #elif defined(RA4W1)
    { AF_SCI1, 0, P101 },

    { AF_SCI2, 1, P213 },

    { AF_SCI1, 4, P205 },

    { AF_SCI2, 9, P109 },

    #elif defined(RA6M1)

    { AF_SCI1, 0, P101 },
    { AF_SCI1, 0, P411 },

    { AF_SCI2, 1, P213 },

    { AF_SCI1, 2, P112 },
    { AF_SCI1, 2, P302 },

    { AF_SCI2, 3, P409 },

    { AF_SCI1, 4, P205 },

    { AF_SCI1, 8, P105 },

    { AF_SCI2, 9, P109 },
    { AF_SCI2, 9, P602 },

    #elif defined(RA6M2)

    { AF_SCI1, 0, P101 },
    { AF_SCI1, 0, P411 },

    { AF_SCI1, 1, P213 },
    { AF_SCI2, 1, P709 },

    { AF_SCI1, 2, P112 },
    { AF_SCI1, 2, P302 },

    { AF_SCI2, 3, P310 },
    { AF_SCI2, 3, P409 },

    { AF_SCI1, 4, P205 },
    { AF_SCI1, 4, P512 },

    { AF_SCI2, 5, P501 },

    { AF_SCI1, 6, P305 },
    { AF_SCI1, 6, P506 },

    { AF_SCI2, 7, P401 },
    { AF_SCI2, 7, P613 },

    { AF_SCI1, 8, P105 },

    { AF_SCI2, 9, P109 },
    { AF_SCI2, 9, P203 },
    { AF_SCI2, 9, P602 },

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define SCI_TX_PINS_SIZE sizeof(ra_sci_tx_pins) / sizeof(ra_af_pin_t)

static const ra_af_pin_t ra_sci_rx_pins[] = {
    #if defined(RA4M1)

    { AF_SCI1, 0, P100 },
    { AF_SCI1, 0, P104 },
    { AF_SCI1, 0, P206 },
    { AF_SCI1, 0, P410 },

    { AF_SCI2, 1, P212 },
    { AF_SCI2, 1, P402 },
    { AF_SCI2, 1, P502 },
    { AF_SCI2, 1, P708 },

    { AF_SCI1, 2, P301 },

    { AF_SCI2, 9, P110 },
    { AF_SCI2, 9, P202 },
    { AF_SCI2, 9, P408 },
    { AF_SCI2, 9, P601 },

    #elif defined(RA4W1)
    { AF_SCI1, 0, P100 },
    { AF_SCI1, 0, P104 },

    { AF_SCI2, 1, P212 },
    { AF_SCI2, 1, P402 },

    { AF_SCI1, 4, P206 },

    { AF_SCI2, 9, P110 },

    #elif defined(RA6M1)

    { AF_SCI1, 0, P100 },
    { AF_SCI1, 0, P410 },

    { AF_SCI2, 1, P212 },
    { AF_SCI2, 1, P708 },

    { AF_SCI1, 2, P113 },
    { AF_SCI1, 2, P301 },

    { AF_SCI2, 3, P408 },

    { AF_SCI1, 4, P206 },

    { AF_SCI1, 8, P104 },

    { AF_SCI2, 9, P110 },
    { AF_SCI2, 9, P601 },

    #elif defined(RA6M2)

    { AF_SCI1, 0, P100 },
    { AF_SCI1, 0, P410 },

    { AF_SCI1, 1, P212 },
    { AF_SCI2, 1, P708 },

    { AF_SCI1, 2, P113 },
    { AF_SCI1, 2, P301 },

    { AF_SCI2, 3, P309 },
    { AF_SCI2, 3, P408 },

    { AF_SCI1, 4, P206 },
    { AF_SCI1, 4, P511 },

    { AF_SCI2, 5, P502 },

    { AF_SCI1, 6, P304 },
    { AF_SCI1, 6, P505 },

    { AF_SCI2, 7, P402 },
    { AF_SCI2, 7, P614 },

    { AF_SCI1, 8, P104 },

    { AF_SCI2, 9, P110 },
    { AF_SCI2, 9, P202 },
    { AF_SCI2, 9, P601 },

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define SCI_RX_PINS_SIZE sizeof(ra_sci_rx_pins) / sizeof(ra_af_pin_t)

static const ra_af_pin_t ra_sci_cts_pins[] = {
    #if defined(RA4M1)

    { AF_SCI1, 0, P103 },
    { AF_SCI1, 0, P401 },
    { AF_SCI1, 0, P407 },
    { AF_SCI1, 0, P413 },

    { AF_SCI2, 1, P101 },
    { AF_SCI2, 1, P403 },
    { AF_SCI1, 1, P408 },
    { AF_SCI2, 1, P504 },

    { AF_SCI1, 2, P110 },
    { AF_SCI1, 2, P203 },

    { AF_SCI2, 9, P108 },
    { AF_SCI2, 9, P205 },
    { AF_SCI2, 9, P301 },
    { AF_SCI2, 9, P603 },

    #elif defined(RA4W1)
    { AF_SCI1, 0, P103 },

    { AF_SCI2, 1, P101 },

    { AF_SCI1, 4, P407 },

    { AF_SCI2, 9, P108 },
    { AF_SCI2, 9, P205 },

    #elif defined(RA6M1)

    { AF_SCI1, 0, P103 },
    { AF_SCI1, 0, P413 },

    { AF_SCI2, 1, P101 },

    { AF_SCI1, 2, P110 },

    { AF_SCI2, 3, P411 },

    { AF_SCI1, 4, P401 },
    { AF_SCI1, 4, P407 },

    { AF_SCI1, 8, P107 },

    { AF_SCI2, 9, P108 },
    { AF_SCI2, 9, P301 },

    #elif defined(RA6M2)

    { AF_SCI1, 0, P103 },
    { AF_SCI1, 0, P413 },

    { AF_SCI2, 1, P101 },
    { AF_SCI2, 1, P711 },

    { AF_SCI1, 2, P110 },
    { AF_SCI1, 2, P203 },

    { AF_SCI2, 3, P312 },
    { AF_SCI2, 3, P411 },

    { AF_SCI1, 4, P401 },
    { AF_SCI1, 4, P407 },

    { AF_SCI2, 5, P504 },

    { AF_SCI1, 6, P307 },
    { AF_SCI1, 6, P503 },

    { AF_SCI2, 7, P403 },
    { AF_SCI2, 7, P611 },

    { AF_SCI1, 8, P107 },

    { AF_SCI2, 9, P108 },
    { AF_SCI2, 9, P205 },
    { AF_SCI2, 9, P301 },
    { AF_SCI2, 9, P603 },

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define SCI_CTS_PINS_SIZE sizeof(ra_sci_cts_pins) / sizeof(ra_af_pin_t)

typedef struct _sci_fifo {
    volatile uint32_t tail, head, len, busy;
    uint8_t *bufp;
    uint32_t size;
} sci_fifo;

static uint32_t ra_sci_init_flag[] = {
    #if defined(VECTOR_NUMBER_SCI0_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_RXI)
    0,
    #endif
};
static SCI_CB sci_cb[] = {
    #if defined(VECTOR_NUMBER_SCI0_RXI)
    (SCI_CB)0,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_RXI)
    (SCI_CB)0,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_RXI)
    (SCI_CB)0,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_RXI)
    (SCI_CB)0,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_RXI)
    (SCI_CB)0,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_RXI)
    (SCI_CB)0,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_RXI)
    (SCI_CB)0,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_RXI)
    (SCI_CB)0,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_RXI)
    (SCI_CB)0,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_RXI)
    (SCI_CB)0,
    #endif
};
static uint8_t ch_9bit[] = {
    #if defined(VECTOR_NUMBER_SCI0_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_RXI)
    0,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_RXI)
    0,
    #endif
};
static uint8_t tx_buf[SCI_IDX_MAX][SCI_TX_BUF_SIZE];
static uint8_t rx_buf[SCI_IDX_MAX][SCI_RX_BUF_SIZE];
static volatile sci_fifo tx_fifo[SCI_IDX_MAX];
static volatile sci_fifo rx_fifo[SCI_IDX_MAX];
static uint32_t m_cts_pin[] = {
    #if defined(VECTOR_NUMBER_SCI0_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_RXI)
    PIN_END,
    #endif
};
static uint32_t m_rts_pin[] = {
    #if defined(VECTOR_NUMBER_SCI0_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI1_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI2_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI3_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI4_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI5_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI6_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI7_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI8_RXI)
    PIN_END,
    #endif
    #if defined(VECTOR_NUMBER_SCI9_RXI)
    PIN_END,
    #endif
};

static void delay_us(volatile unsigned int us) {
    us *= 48;
    while (us-- > 0) {
        ;
    }
}

bool ra_af_find_ch_af(ra_af_pin_t *af_pin, uint32_t size, uint32_t pin, uint32_t *ch, uint32_t *af) {
    bool find = false;
    uint32_t i;
    for (i = 0; i < size; i++) {
        if (af_pin->pin == pin) {
            find = true;
            *ch = af_pin->ch;
            *af = af_pin->af;
            break;
        }
        af_pin++;
    }
    return find;
}

static void ra_sci_tx_set_pin(uint32_t pin) {
    bool find = false;
    uint32_t ch;
    uint32_t af;
    find = ra_af_find_ch_af((ra_af_pin_t *)&ra_sci_tx_pins, SCI_TX_PINS_SIZE, pin, &ch, &af);
    if (find) {
        ra_gpio_config(pin, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_LOW_POWER, af);
    }
}

static void ra_sci_rx_set_pin(uint32_t pin) {
    bool find = false;
    uint32_t ch;
    uint32_t af;
    find = ra_af_find_ch_af((ra_af_pin_t *)&ra_sci_rx_pins, SCI_RX_PINS_SIZE, pin, &ch, &af);
    if (find) {
        ra_gpio_config(pin, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_LOW_POWER, af);
    }
}

static void ra_sci_cts_set_pin(uint32_t pin) {
    bool find = false;
    uint32_t ch;
    uint32_t af;
    find = ra_af_find_ch_af((ra_af_pin_t *)&ra_sci_cts_pins, SCI_CTS_PINS_SIZE, pin, &ch, &af);
    if (find) {
        ra_gpio_config(pin, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_LOW_POWER, af);
    }
}

static void ra_sci_module_start(uint32_t ch) {
    ra_mstpcrb_start(sci_module_mask[ch_to_idx[ch]]);
}

static void ra_sci_module_stop(uint32_t ch) {
    ra_mstpcrb_stop(sci_module_mask[ch_to_idx[ch]]);
}

void ra_sci_rx_set_callback(int ch, SCI_CB cb) {
    sci_cb[ch_to_idx[ch]] = cb;
}

static void ra_sci_irq_disable(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    R_BSP_IrqDisable(idx_to_rxirq[idx]);
    R_BSP_IrqDisable(idx_to_txirq[idx]);
    R_BSP_IrqDisable(idx_to_teirq[idx]);
    R_BSP_IrqDisable(idx_to_erirq[idx]);
    R_BSP_IrqStatusClear(idx_to_rxirq[idx]);
    R_BSP_IrqStatusClear(idx_to_txirq[idx]);
    R_BSP_IrqStatusClear(idx_to_teirq[idx]);
    R_BSP_IrqStatusClear(idx_to_erirq[idx]);
}

static void ra_sci_irq_enable(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    R_BSP_IrqEnable(idx_to_rxirq[idx]);
    R_BSP_IrqEnable(idx_to_txirq[idx]);
    R_BSP_IrqEnable(idx_to_teirq[idx]);
    R_BSP_IrqEnable(idx_to_erirq[idx]);
}

void ra_sci_rxirq_disable(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    R_BSP_IrqDisable(idx_to_rxirq[idx]);
}

void ra_sci_rxirq_enable(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    R_BSP_IrqEnable(idx_to_rxirq[idx]);
}

bool ra_sci_is_rxirq_enable(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    uint32_t _irq = (uint32_t)idx_to_rxirq[idx];
    return NVIC->ISER[(_irq >> 5UL)] == (uint32_t)(1UL << (_irq & 0x1FUL));
}

static void ra_sci_irq_priority(uint32_t ch, uint32_t ipl) {
    uint32_t idx = ch_to_idx[ch];
    R_BSP_IrqCfg(idx_to_rxirq[idx], ipl, (void *)NULL);
    R_BSP_IrqCfg(idx_to_txirq[idx], ipl, (void *)NULL);
    R_BSP_IrqCfg(idx_to_teirq[idx], ipl, (void *)NULL);
    R_BSP_IrqCfg(idx_to_erirq[idx], ipl, (void *)NULL);
}

static void ra_sci_isr_rx(uint32_t ch) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint32_t idx = ch_to_idx[ch];
    uint16_t d;
    if (ch_9bit[idx]) {
        d = (uint16_t)sci_regs[idx]->RDRHL;
    } else {
        d = (uint16_t)sci_regs[idx]->RDR;
    }
    if (sci_cb[idx]) {
        if ((*sci_cb[idx])(ch, (int)d)) {
            // goto ra_sci_isr_rx_exit;
        }
    }
    uint32_t size = rx_fifo[idx].size;
    sci_fifo *rxfifo = (sci_fifo *)&rx_fifo[idx];
    if (rxfifo->len < size) {
        uint32_t i = rxfifo->head;
        if (ch_9bit[idx]) {
            *(uint16_t *)(rxfifo->bufp + i) = (uint16_t)d;
            i += 2;
            rxfifo->len += 2;
        } else {
            *(rxfifo->bufp + i) = (uint8_t)d;
            i++;
            rxfifo->len++;
        }
        rxfifo->head = i % size;
        if (m_rts_pin[idx] != PIN_END) {
            if (rxfifo->len > (size - RA_SCI_FLOW_START_NUM)) {
                ra_gpio_write(m_rts_pin[idx], 1);
            }
        }
    }
// ra_sci_isr_rx_exit:
    R_BSP_IrqStatusClear(irq);
}

static void ra_sci_isr_er(uint32_t ch) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    R_BSP_IrqStatusClear(irq);
    uint32_t idx = ch_to_idx[ch];
    R_SCI0_Type *sci_reg = sci_regs[idx];
    sci_reg->RDR;
    while (0 != (sci_reg->SSR & 0x38)) {
        sci_reg->RDR;
        sci_reg->SSR = (uint8_t)((sci_reg->SSR & ~0x38) | 0xc0);
        if (0 != (sci_reg->SSR & 0x38)) {
            __asm__ __volatile__ ("nop");
        }
    }
}

static void ra_sci_isr_tx(uint32_t ch) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint32_t idx = ch_to_idx[ch];
    uint32_t size = tx_fifo[idx].size;
    sci_fifo *txfifo = (sci_fifo *)&tx_fifo[idx];
    if (txfifo->len != 0) {
        uint32_t i = txfifo->tail;
        if (ch_9bit[idx]) {
            sci_regs[idx]->TDRHL = *(uint16_t *)(txfifo->bufp + i);
            i += 2;
            txfifo->len -= 2;
        } else {
            sci_regs[idx]->TDR = (uint8_t)*(txfifo->bufp + i);
            i++;
            txfifo->len--;
        }
        txfifo->tail = i % size;
    } else {
        /* tx_fifo[idx].len == 0 */
        /* after transfer completed */
        uint8_t scr = sci_regs[idx]->SCR;
        scr &= (uint8_t) ~0x80; /* TIE disable */
        scr |= (uint8_t)0x04;  /* TEIE enable */
        sci_regs[idx]->SCR = scr;
    }
    R_BSP_IrqStatusClear(irq);
}

void ra_sci_isr_te(uint32_t ch) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint32_t idx = ch_to_idx[ch];
    tx_fifo[idx].busy = 0;
    sci_regs[idx]->SCR &= (uint8_t) ~0x84; /* TIE and TEIE disable */
    R_BSP_IrqStatusClear(irq);
}

int ra_sci_rx_ch(uint32_t ch) {
    uint16_t c;
    uint32_t idx = ch_to_idx[ch];
    uint32_t size = rx_fifo[idx].size;
    sci_fifo *rxfifo = (sci_fifo *)&rx_fifo[idx];
    if (rxfifo->len) {
        uint32_t state = ra_disable_irq();
        uint32_t i = rxfifo->tail;
        if (ch_9bit[idx]) {
            c = *(uint16_t *)(rxfifo->bufp + i);
            i += 2;
            rxfifo->len -= 2;
        } else {
            c = (uint16_t)*(rxfifo->bufp + i);
            i++;
            rxfifo->len--;
        }
        rxfifo->tail = i % size;
        if (m_rts_pin[idx] != PIN_END) {
            if (rxfifo->len <= (size - RA_SCI_FLOW_START_NUM)) {
                ra_gpio_write(m_rts_pin[idx], 0);
            }
        }
        ra_enable_irq(state);
    } else {
        c = 0;
    }
    return (int)c;
}

int ra_sci_rx_any(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    return (int)(rx_fifo[idx].len != 0);
}

void ra_sci_tx_ch(uint32_t ch, int c) {
    uint32_t idx = ch_to_idx[ch];
    uint32_t size = tx_fifo[idx].size;
    sci_fifo *txfifo = (sci_fifo *)&tx_fifo[idx];
    while (tx_fifo[idx].len == size) {
    }
    uint32_t state = ra_disable_irq();
    uint32_t i = tx_fifo[idx].head;
    if (ch_9bit[idx]) {
        *(uint16_t *)(txfifo->bufp + i) = (uint16_t)c;
        i += 2;
        txfifo->len += 2;
    } else {
        *(txfifo->bufp + i) = (uint8_t)c;
        i++;
        txfifo->len++;
    }
    txfifo->head = i % size;
    if (!txfifo->busy) {
        txfifo->busy = 1;
        uint8_t scr = sci_regs[idx]->SCR;
        if ((scr & 0xa0) != 0) {
            sci_regs[idx]->SCR &= ~0xa0;
        }
        sci_regs[idx]->SCR |= 0xa0; /* TIE and TE enable */
    }
    ra_enable_irq(state);
}

int ra_sci_tx_wait(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    return (int)(tx_fifo[idx].len != (tx_fifo[idx].size - 1));
}

int ra_sci_tx_busy(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    return (int)(tx_fifo[idx].busy);
}

int ra_sci_tx_bufsize(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    return (int)(tx_fifo[idx].size - 1);
}

void ra_sci_tx_break(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    R_SCI0_Type *sci_reg = sci_regs[idx];
    uint8_t scr = sci_reg->SCR;
    uint8_t smr = sci_reg->SMR;
    sci_reg->SCR = 0;
    while (sci_reg->SCR != 0) {
        ;
    }
    sci_reg->SMR_b.STOP = 1;
    sci_reg->SCR = scr;
    sci_reg->TDR = 0;
    while (sci_reg->SSR_b.TDRE == 0) {
        ;
    }
    sci_reg->SMR = smr;
    return;
}

void ra_sci_tx_str(uint32_t ch, uint8_t *p) {
    int c;
    uint32_t idx = ch_to_idx[ch];
    if (ch_9bit[idx]) {
        uint16_t *q = (uint16_t *)p;
        while ((c = *q++) != 0) {
            ra_sci_tx_ch(ch, (int)c);
        }
    } else {
        while ((c = (int)*p++) != 0) {
            ra_sci_tx_ch(ch, (int)c);
        }
    }
}

static void ra_sci_fifo_set(sci_fifo *fifo, uint8_t *bufp, uint32_t size) {
    fifo->head = 0;
    fifo->tail = 0;
    fifo->len = 0;
    fifo->busy = 0;
    fifo->bufp = bufp;
    fifo->size = size;
}

void ra_sci_txfifo_set(uint32_t ch, uint8_t *bufp, uint32_t size) {
    uint32_t idx = ch_to_idx[ch];
    sci_fifo *fifo = (sci_fifo *)&tx_fifo[idx];
    ra_sci_fifo_set(fifo, bufp, size);
}

void ra_sci_rxfifo_set(uint32_t ch, uint8_t *bufp, uint32_t size) {
    uint32_t idx = ch_to_idx[ch];
    sci_fifo *fifo = (sci_fifo *)&rx_fifo[idx];
    ra_sci_fifo_set(fifo, bufp, size);
}

static void ra_sci_fifo_init(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    ra_sci_txfifo_set(ch, (uint8_t *)&tx_buf[idx][0], SCI_TX_BUF_SIZE);
    ra_sci_rxfifo_set(ch, (uint8_t *)&rx_buf[idx][0], SCI_RX_BUF_SIZE);
}

void ra_sci_set_baud(uint32_t ch, uint32_t baud) {
    uint32_t idx = ch_to_idx[ch];
    R_SCI0_Type *sci_reg = sci_regs[idx];
    // Only works for 115200 bps
    // ToDo: support other bps
    if (baud == 0) {
        /* ABCS=1 */
        sci_reg->SMR_b.CKS = 0; /* PCLKA */
        sci_reg->BRR = (uint8_t)((int)PCLK / SCI_DEF_BAUD / 16 - 1);
    } else if (baud > 19200) {
        /* ABCS=1 */
        sci_reg->SMR_b.CKS = 0; /* PCLKA */
        sci_reg->BRR = (uint8_t)((int)PCLK / baud / 16 - 1);
    } else {
        /* ABCS=1 */
        sci_reg->SMR_b.CKS = 2; /* PCLKA/16 */
        sci_reg->BRR = (uint8_t)((int)PCLK / baud / 256 - 1);
    }
}

/*
 * bits: 7, 8, 9
 * parity: none:0, odd:1, even:2
 */
void ra_sci_init_with_flow(uint32_t ch, uint32_t tx_pin, uint32_t rx_pin, uint32_t baud, uint32_t bits, uint32_t parity, uint32_t stop, uint32_t flow, uint32_t cts_pin, uint32_t rts_pin) {
    uint8_t smr = 0;
    uint8_t scmr = (uint8_t)0xf2;
    uint32_t idx = ch_to_idx[ch];
    R_SCI0_Type *sci_reg = sci_regs[idx];
    if (ra_sci_init_flag[idx] == 0) {
        ra_sci_fifo_init(ch);
        sci_cb[idx] = 0;
        ra_sci_init_flag[idx]++;
    } else {
        ra_sci_init_flag[idx]++;
        return;
    }
    ra_sci_module_start(ch);
    ra_sci_tx_set_pin(tx_pin);
    ra_sci_rx_set_pin(rx_pin);
    if (flow) {
        if (cts_pin != (uint32_t)PIN_END) {
            m_cts_pin[idx] = cts_pin;
            ra_sci_cts_set_pin(cts_pin);
        }
        if (rts_pin != (uint32_t)PIN_END) {
            m_rts_pin[idx] = rts_pin;
            ra_gpio_config(rts_pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_LOW_POWER, 0);
            ra_gpio_write(rts_pin, 0);
        }
    }
    ra_sci_irq_disable(ch);
    ra_sci_irq_priority(ch, RA_PRI_UART);
    uint32_t state = ra_disable_irq();
    sci_reg->SCR = 0;
    while (sci_reg->SCR != 0) {
        ;
    }
    if (bits == 7) {
        smr |= (uint8_t)0x40;
    } else {
        smr &= (uint8_t) ~0x40;
    }
    if (parity != 0) {
        smr |= (uint8_t)0x20;
    } else {
        smr &= (uint8_t) ~0x20;
    }
    if (parity == 1) {
        smr |= (uint8_t)0x10;
    } else {
        smr &= (uint8_t) ~0x10;
    }
    if (stop == 2) {
        smr |= (uint8_t)0x80;
    } else {
        smr &= (uint8_t) ~0x80;
    }
    sci_reg->SMR = smr;
    if (bits == 9) {
        scmr &= (uint8_t) ~0x10;
        ch_9bit[idx] = 1;
    } else {
        scmr |= (uint8_t)0x10;
        ch_9bit[idx] = 0;
    }
    if (flow) {
        sci_reg->SPMR |= 0x01;
    }
    sci_reg->SCMR = scmr;
    sci_reg->SEMR = (uint8_t)0xc0;
    ra_sci_set_baud(ch, baud);
    delay_us(10);
    sci_reg->SCR = (uint8_t)0x50;
    ra_sci_irq_enable(ch);
    ra_enable_irq(state);
    if (!ra_sci_init_flag[idx]) {
        ra_sci_init_flag[idx] = true;
    }
}

void ra_sci_init(uint32_t ch, uint32_t tx_pin, uint32_t rx_pin, uint32_t baud, uint32_t bits, uint32_t parity, uint32_t stop, uint32_t flow) {
    ra_sci_init_with_flow(ch, tx_pin, rx_pin, baud, bits, parity, stop, flow, PIN_END, PIN_END);
}

void ra_sci_deinit(uint32_t ch) {
    uint32_t idx = ch_to_idx[ch];
    if (ra_sci_init_flag[idx] != 0) {
        ra_sci_init_flag[idx]--;
        if (ra_sci_init_flag[idx] == 0) {
            ra_sci_irq_disable(ch);
            ra_sci_module_stop(ch);
            sci_cb[idx] = 0;
        }
    }
}

/* rx interrupt */
void sci_uart_rxi_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint32_t ch = irq_to_ch[(uint32_t)irq];
    ra_sci_isr_rx(ch);
}

/* tx interrupt */
void sci_uart_txi_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint32_t ch = irq_to_ch[(uint32_t)irq];
    ra_sci_isr_tx(ch);
}

/* er interrupt */
void sci_uart_eri_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint32_t ch = irq_to_ch[(uint32_t)irq];
    ra_sci_isr_er(ch);
}

/* te interrupt */
void sci_uart_tei_isr(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    uint32_t ch = irq_to_ch[(uint32_t)irq];
    ra_sci_isr_te(ch);
}
