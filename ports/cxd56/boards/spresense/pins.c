/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#include "py/objtuple.h"

#include "shared-bindings/board/__init__.h"

STATIC const mp_rom_obj_tuple_t sdio_data_tuple = {
    {&mp_type_tuple},
    4,
    {
        MP_ROM_PTR(&pin_SDIO_DATA0),
        MP_ROM_PTR(&pin_SDIO_DATA1),
        MP_ROM_PTR(&pin_SDIO_DATA2),
        MP_ROM_PTR(&pin_SDIO_DATA3),
    }
};

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_UART2_RXD) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_UART2_TXD) },
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_HIF_IRQ_OUT) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_PWM3) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_SPI2_MOSI) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PWM1) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PWM0) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_SPI3_CS1_X) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_SPI2_MISO) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PWM2) },
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_SPI4_CS_X) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_SPI4_MOSI) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_SPI4_MISO) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_SPI4_SCK) },
    { MP_ROM_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_I2C0_BDT) },
    { MP_ROM_QSTR(MP_QSTR_D15), MP_ROM_PTR(&pin_I2C0_BCK) },
    { MP_ROM_QSTR(MP_QSTR_D16), MP_ROM_PTR(&pin_EMMC_DATA0) },
    { MP_ROM_QSTR(MP_QSTR_D17), MP_ROM_PTR(&pin_EMMC_DATA1) },
    { MP_ROM_QSTR(MP_QSTR_D18), MP_ROM_PTR(&pin_I2S0_DATA_OUT) },
    { MP_ROM_QSTR(MP_QSTR_D19), MP_ROM_PTR(&pin_I2S0_DATA_IN) },
    { MP_ROM_QSTR(MP_QSTR_D20), MP_ROM_PTR(&pin_EMMC_DATA2) },
    { MP_ROM_QSTR(MP_QSTR_D21), MP_ROM_PTR(&pin_EMMC_DATA3) },
    { MP_ROM_QSTR(MP_QSTR_D22), MP_ROM_PTR(&pin_SEN_IRQ_IN) },
    { MP_ROM_QSTR(MP_QSTR_D23), MP_ROM_PTR(&pin_EMMC_CLK) },
    { MP_ROM_QSTR(MP_QSTR_D24), MP_ROM_PTR(&pin_EMMC_CMD) },
    { MP_ROM_QSTR(MP_QSTR_D25), MP_ROM_PTR(&pin_I2S0_LRCK) },
    { MP_ROM_QSTR(MP_QSTR_D26), MP_ROM_PTR(&pin_I2S0_BCK) },
    { MP_ROM_QSTR(MP_QSTR_D27), MP_ROM_PTR(&pin_UART2_CTS) },
    { MP_ROM_QSTR(MP_QSTR_D28), MP_ROM_PTR(&pin_UART2_RTS) },
    { MP_ROM_QSTR(MP_QSTR_LED0), MP_ROM_PTR(&pin_I2S1_BCK) },
    { MP_ROM_QSTR(MP_QSTR_LED1), MP_ROM_PTR(&pin_I2S1_LRCK) },
    { MP_ROM_QSTR(MP_QSTR_LED2), MP_ROM_PTR(&pin_I2S1_DATA_IN) },
    { MP_ROM_QSTR(MP_QSTR_LED3), MP_ROM_PTR(&pin_I2S1_DATA_OUT) },
    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_LPADC0) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_LPADC1) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_LPADC2) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_LPADC3) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_HPADC0) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_HPADC1) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_I2C0_BDT) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_I2C0_BCK) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_SPI4_SCK) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_SPI4_MISO) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_SPI4_MOSI) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_UART2_RXD) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_UART2_TXD) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDIO_CLOCK), MP_ROM_PTR(&pin_SDIO_CLK) },
    { MP_ROM_QSTR(MP_QSTR_SDIO_COMMAND), MP_ROM_PTR(&pin_SDIO_CMD) },
    { MP_ROM_QSTR(MP_QSTR_SDIO_DATA), MP_ROM_PTR(&sdio_data_tuple) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
