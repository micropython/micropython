/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#ifndef MICROPY_INCLUDED_PSOC_EDGE_CLK_H
#define MICROPY_INCLUDED_PSOC_EDGE_CLK_H

/* Forward declare pclk_div_obj_t */
typedef struct _pclk_div_obj_t pclk_div_obj_t;

typedef uint32_t pclk_mmio_slave_num_t;

void pclk_div_slave_init(en_clk_dst_t clk_dst, pclk_mmio_slave_num_t clk_slave_num);
void pclk_div_slave_deinit(en_clk_dst_t clk_dst, pclk_mmio_slave_num_t clk_slave_num);

uint32_t pclk_div_get_input_freq(en_clk_dst_t clk_dst);
pclk_div_obj_t *pclk_div_init(en_clk_dst_t clk_dst, uint32_t divider, uint8_t divider_frac);
void pclk_div_deinit(pclk_div_obj_t *clk);

#endif // MICROPY_INCLUDED_PSOC_EDGE_CLK_H
