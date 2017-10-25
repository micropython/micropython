/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/microcontroller/Processor.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"

#include "eagle_soc.h"
#include "ets_alt_task.h"
#include "etshal.h"
#include "osapi.h"
#include "xtirq.h"

#define ETS_LOOP_ITER_BIT (12)

void common_hal_mcu_delay_us(uint32_t delay) {
    os_delay_us(delay);
}

static uint16_t saved_interrupt_state;
void common_hal_mcu_disable_interrupts() {
    saved_interrupt_state = disable_irq();
    saved_interrupt_state = (saved_interrupt_state & ~(1 << ETS_LOOP_ITER_BIT)) | (ets_loop_iter_disable << ETS_LOOP_ITER_BIT);
    ets_loop_iter_disable = 1;
}

void common_hal_mcu_enable_interrupts() {
    ets_loop_iter_disable = (saved_interrupt_state >> ETS_LOOP_ITER_BIT) & 1;
    enable_irq(saved_interrupt_state & ~(1 << ETS_LOOP_ITER_BIT));
}

// The singleton microcontroller.Processor object, returned by microcontroller.cpu
// It currently only has properties, and no state.
mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

// This macro is used to simplify pin definition in boards/<board>/pins.c
#define PIN(p_name, p_gpio_number, p_gpio_function, p_peripheral) \
const mcu_pin_obj_t pin_## p_name = { \
    { &mcu_pin_type }, \
    .name = MP_QSTR_ ## p_name, \
    .gpio_number = p_gpio_number, \
    .gpio_function = p_gpio_function, \
    .peripheral = p_peripheral, \
}

// Using microcontroller names from the datasheet.
// https://cdn-shop.adafruit.com/datasheets/ESP8266_Specifications_English.pdf
// PIN(mcu name) // function notes | module name | huzzah name
PIN(TOUT, NO_GPIO, NO_GPIO, NO_GPIO); // adc | ADC | ADC
PIN(XPD_DCDC, 16, SPECIAL_CASE, SPECIAL_CASE); // gpio16 | GPIO16 | GPIO16
PIN(MTMS, 14, FUNC_GPIO14, PERIPHS_IO_MUX_MTMS_U); // gpio14 / hspi_clk / pwm2 | GPIO14 | GPIO14/SCK
PIN(MTDI, 12, FUNC_GPIO12, PERIPHS_IO_MUX_MTDI_U); // gpio12 / hspi_miso / pwm0 | GPIO12 | GPIO12/MISO
PIN(MTCK, 13, FUNC_GPIO13, PERIPHS_IO_MUX_MTCK_U); // gpio13 / hspi_mosi / U0cts | GPIO13 | GPIO13/MOSI
PIN(MTDO, 15, FUNC_GPIO15, PERIPHS_IO_MUX_MTDO_U); // gpio15 / hspi_cs / u0rts / pwm1 | GPIO15 | GPIO15
PIN(GPIO2, 2, FUNC_GPIO2, PERIPHS_IO_MUX_GPIO2_U); // U1txd | GPIO2 | GPIO2
PIN(GPIO0, 0, FUNC_GPIO0, PERIPHS_IO_MUX_GPIO0_U); // spi_Cs2 | GPIO0 | GPIO0
PIN(GPIO4, 4, FUNC_GPIO4, PERIPHS_IO_MUX_GPIO4_U); // pwm3 on mcu datasheet as vdd which must be wrong | GPIO4 | GPIO4/SDA
PIN(SD_DATA_2, 9, FUNC_GPIO9, PERIPHS_IO_MUX_SD_DATA2_U); // spihd / hspihd / gpio9 | GPIO9
PIN(SD_DATA_3, 10, FUNC_GPIO10, PERIPHS_IO_MUX_SD_DATA3_U); // spiwp / hspiwp / gpio10 | GPIO10
PIN(SD_CMD, NO_GPIO, NO_GPIO, PERIPHS_IO_MUX_SD_CMD_U); // spi_cs0 / gpio11 | CS0
PIN(SD_CLK, NO_GPIO, NO_GPIO, PERIPHS_IO_MUX_SD_CLK_U); // spi_clk / gpio6 | SCLK
PIN(SD_DATA_0, NO_GPIO, NO_GPIO, PERIPHS_IO_MUX_SD_DATA0_U); // spi_miso / gpio7 | MISO
PIN(SD_DATA_1, NO_GPIO, NO_GPIO, PERIPHS_IO_MUX_SD_DATA1_U); // spi_mosi / gpio8 / u1rxd | MOSI
PIN(DVDD, 5, FUNC_GPIO5, PERIPHS_IO_MUX_GPIO5_U); // gpio5 | GPIO5 | GPIO5/SCL
PIN(U0RXD, 3, FUNC_GPIO3, PERIPHS_IO_MUX_U0RXD_U); // gpio3 | RXD0 | RXD
PIN(U0TXD, 1, FUNC_GPIO1, PERIPHS_IO_MUX_U0TXD_U); // gpio1 / spi_cs1 | TXD0 | TXD

// This maps MCU pin names to pin objects.
STATIC const mp_rom_map_elem_t mcu_pin_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_TOUT), MP_ROM_PTR(&pin_TOUT) },
    { MP_ROM_QSTR(MP_QSTR_XPD_DCDC), MP_ROM_PTR(&pin_XPD_DCDC) },
    { MP_ROM_QSTR(MP_QSTR_MTMS), MP_ROM_PTR(&pin_MTMS) },
    { MP_ROM_QSTR(MP_QSTR_MTDI), MP_ROM_PTR(&pin_MTDI) },
    { MP_ROM_QSTR(MP_QSTR_MTCK), MP_ROM_PTR(&pin_MTCK) },
    { MP_ROM_QSTR(MP_QSTR_MTDO), MP_ROM_PTR(&pin_MTDO) },
    { MP_ROM_QSTR(MP_QSTR_GPIO2), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_GPIO0), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_GPIO4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_SD_DATA_2), MP_ROM_PTR(&pin_SD_DATA_2) },
    { MP_ROM_QSTR(MP_QSTR_SD_DATA_3), MP_ROM_PTR(&pin_SD_DATA_3) },
    { MP_ROM_QSTR(MP_QSTR_SD_CMD), MP_ROM_PTR(&pin_SD_CMD) },
    { MP_ROM_QSTR(MP_QSTR_SD_CLK), MP_ROM_PTR(&pin_SD_CLK) },
    { MP_ROM_QSTR(MP_QSTR_SD_DATA_0), MP_ROM_PTR(&pin_SD_DATA_0) },
    { MP_ROM_QSTR(MP_QSTR_SD_DATA_1), MP_ROM_PTR(&pin_SD_DATA_1) },
    { MP_ROM_QSTR(MP_QSTR_DVDD), MP_ROM_PTR(&pin_DVDD) },
    { MP_ROM_QSTR(MP_QSTR_U0RXD), MP_ROM_PTR(&pin_U0RXD) },
    { MP_ROM_QSTR(MP_QSTR_U0TXD), MP_ROM_PTR(&pin_U0TXD) },
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_global_dict_table);
