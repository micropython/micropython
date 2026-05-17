/*
 * This file is part of the OpenMV project, https://openmv.io.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
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
 *
 * NINA-W10 driver BSP.
 */
#ifndef MICROPY_INCLUDED_DRIVERS_NINAW10_NINA_BSP_H
#define MICROPY_INCLUDED_DRIVERS_NINAW10_NINA_BSP_H

int nina_bsp_init(void);
int nina_bsp_deinit(void);
int nina_bsp_atomic_enter(void);
int nina_bsp_atomic_exit(void);
int nina_bsp_read_irq(void);
int nina_bsp_spi_slave_select(uint32_t timeout);
int nina_bsp_spi_slave_deselect(void);
int nina_bsp_spi_transfer(const uint8_t *tx_buf, uint8_t *rx_buf, uint32_t size);

#endif // MICROPY_INCLUDED_DRIVERS_NINAW10_NINA_BSP_H
