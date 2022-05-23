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

#ifndef PORTS_RA_RA_RA_SCI_H_
#define PORTS_RA_RA_RA_SCI_H_

#include <stdint.h>

#define RA_SCI_FLOW_START_NUM   (16)

typedef int (*SCI_CB)(uint32_t ch, uint32_t d);

// static bool find_irq_to_ch(IRQn_Type *irq_tbl, IRQn_Type irq, uint32_t *ch);
// static void delay_us(volatile unsigned int us);
bool ra_af_find_ch_af(ra_af_pin_t *af_pin, uint32_t size, uint32_t pin, uint32_t *ch, uint32_t *af);
// static void ra_sci_tx_set_pin(uint32_t pin);
// static void ra_sci_rx_set_pin(uint32_t pin);
// static void ra_sci_cts_set_pin(uint32_t pin);
// static void ra_sci_module_start(uint32_t ch);
// static void ra_sci_module_stop(uint32_t ch);
void ra_sci_rx_set_callback(int ch, SCI_CB cb);
// static void ra_sci_irq_disable(uint32_t ch);
// static void ra_sci_irq_enable(uint32_t ch);
void ra_sci_rxirq_disable(uint32_t ch);
void ra_sci_rxirq_enable(uint32_t ch);
bool ra_sci_is_rxirq_enable(uint32_t ch);
// static void ra_sci_irq_priority(uint32_t ch, uint32_t ipl);
// static void ra_sci_isr_rx(uint32_t ch);
// static void ra_sci_isr_er(uint32_t ch);
// static void ra_sci_isr_tx(uint32_t ch);
void ra_sci_isr_te(uint32_t ch);
int ra_sci_rx_ch(uint32_t ch);
int ra_sci_rx_any(uint32_t ch);
void ra_sci_tx_ch(uint32_t ch, int c);
int ra_sci_tx_wait(uint32_t ch);
void ra_sci_tx_break(uint32_t ch);
void ra_sci_tx_str(uint32_t ch, uint8_t *p);
// static void ra_sci_fifo_set(sci_fifo *fifo, uint8_t *bufp, uint32_t size);
void ra_sci_txfifo_set(uint32_t ch, uint8_t *bufp, uint32_t size);
void ra_sci_rxfifo_set(uint32_t ch, uint8_t *bufp, uint32_t size);
// static void sci_fifo_init(uint32_t ch);
void ra_sci_set_baud(uint32_t ch, uint32_t baud);
void ra_sci_init_with_flow(uint32_t ch, uint32_t tx_pin, uint32_t rx_pin, uint32_t baud, uint32_t bits, uint32_t parity, uint32_t stop, uint32_t flow, uint32_t cts_pin, uint32_t rts_pin);
void ra_sci_init(uint32_t ch, uint32_t tx_pin, uint32_t rx_pin, uint32_t baud, uint32_t bits, uint32_t parity, uint32_t stop, uint32_t flow);
void ra_sci_deinit(uint32_t ch);
void sci_uart_rxi_isr(void);
void sci_uart_txi_isr(void);
void sci_uart_eri_isr(void);
void sci_uart_tei_isr(void);

#endif /* PORTS_RA_RA_RA_SCI_H_ */
