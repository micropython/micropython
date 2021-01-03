/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Artur Pacholec
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

#ifndef MICROPY_INCLUDED_MIMXRT10XX_MIMXRT1062_PERIPHERALS_MIMXRT1011_PERIPH_H
#define MICROPY_INCLUDED_MIMXRT10XX_MIMXRT1062_PERIPHERALS_MIMXRT1011_PERIPH_H

extern LPI2C_Type *mcu_i2c_banks[4];

extern const mcu_periph_obj_t mcu_i2c_sda_list[9];
extern const mcu_periph_obj_t mcu_i2c_scl_list[9];

extern LPSPI_Type *mcu_spi_banks[4];

extern const mcu_periph_obj_t mcu_spi_sck_list[8];
extern const mcu_periph_obj_t mcu_spi_mosi_list[8];
extern const mcu_periph_obj_t mcu_spi_miso_list[8];

extern LPUART_Type *mcu_uart_banks[8];

extern const mcu_periph_obj_t mcu_uart_rx_list[18];
extern const mcu_periph_obj_t mcu_uart_tx_list[18];
extern const mcu_periph_obj_t mcu_uart_rts_list[9];
extern const mcu_periph_obj_t mcu_uart_cts_list[9];

extern const mcu_pwm_obj_t mcu_pwm_list[67];

#endif // MICROPY_INCLUDED_MIMXRT10XX_MIMXRT1062_PERIPHERALS_MIMXRT1011_PERIPH_H
