/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Peter van der Burg
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
 * Used by machine_pin.c. Holds Board/MCU specific Pin allocations.
 */

#include "modmachine.h"
#include "pins.h"

// Ensure Declaration in 'pins.h' reflects # of Pins defined here.
const machine_pin_obj_t machine_pin_obj[] = {
    // EXT1
    {{&machine_pin_type}, PIN_PB00}, // PIN3_ADC(+)
    {{&machine_pin_type}, PIN_PB01}, // PIN4_ADC(-)
    {{&machine_pin_type}, PIN_PB06}, // PIN5_GPIO
    {{&machine_pin_type}, PIN_PB07}, // PIN6_GPIO
    {{&machine_pin_type}, PIN_PB02}, // PIN7_PWM(+)
    {{&machine_pin_type}, PIN_PB03}, // PIN8_PWM(-)
    {{&machine_pin_type}, PIN_PB04}, // PIN9_IRQ/GPIO
    {{&machine_pin_type}, PIN_PB05}, // PIN10_SPI_SS_B/GPIO
    {{&machine_pin_type}, PIN_PA08}, // PIN11_TWI_SDA
    {{&machine_pin_type}, PIN_PA09}, // PIN12_TWI_SCL
    {{&machine_pin_type}, PIN_PB09}, // PIN13_UART_RX
    {{&machine_pin_type}, PIN_PB08}, // PIN14_UART_TX
    {{&machine_pin_type}, PIN_PA05}, // PIN15_SPI_SS_A
    {{&machine_pin_type}, PIN_PA06}, // PIN16_SPI_MOSI
    {{&machine_pin_type}, PIN_PA04}, // PIN17_SPI_MISO
    {{&machine_pin_type}, PIN_PA07}, // PIN18_SPI_SCK

    // EXT2
    {{&machine_pin_type}, PIN_PA10}, // PIN3_ADC(+)
    {{&machine_pin_type}, PIN_PA11}, // PIN4_ADC(-)
    {{&machine_pin_type}, PIN_PA20}, // PIN5_GPIO
    {{&machine_pin_type}, PIN_PA21}, // PIN6_GPIO
    {{&machine_pin_type}, PIN_PB12}, // PIN7_PWM(+)
    {{&machine_pin_type}, PIN_PB13}, // PIN8_PWM(-)
    {{&machine_pin_type}, PIN_PB14}, // PIN9_IRQ/GPIO
    {{&machine_pin_type}, PIN_PB15}, // PIN10_SPI_SS_B/GPIO
    {{NULL}, -1}, // PIN_PA08/ PIN11_TWI_SDA already defined
    {{NULL}, -1}, // PIN_PA09/ PIN12_TWI_SCL already defined
    {{&machine_pin_type}, PIN_PB11}, // PIN13_UART_RX
    {{&machine_pin_type}, PIN_PB10}, // PIN14_UART_TX
    {{&machine_pin_type}, PIN_PA17}, // PIN15_SPI_SS_A
    {{&machine_pin_type}, PIN_PA18}, // PIN16_SPI_MOSI
    {{&machine_pin_type}, PIN_PA16}, // PIN17_SPI_MISO
    {{&machine_pin_type}, PIN_PA19}, // PIN18_SPI_SCK

    // EXT3
    {{&machine_pin_type}, PIN_PA02}, // PIN3_ADC(+)
    {{&machine_pin_type}, PIN_PA03}, // PIN4_ADC(-)
    {{NULL}, -1}, // PIN_PB30/ PIN5_GPIO already defined
    {{&machine_pin_type}, PIN_PA15}, // PIN6_GPIO; USER_BUTTON
    {{&machine_pin_type}, PIN_PA12}, // PIN7_PWM(+)
    {{&machine_pin_type}, PIN_PA13}, // PIN8_PWM(-)
    {{&machine_pin_type}, PIN_PA28}, // PIN9_IRQ/GPIO
    {{&machine_pin_type}, PIN_PA27}, // PIN10_SPI_SS_B/GPIO
    {{NULL}, -1}, // PIN_PA08/ PIN11_TWI_SDA already defined
    {{NULL}, -1}, // PIN_PA09/ PIN12_TWI_SCL already defined
    {{NULL}, -1}, // PIN_PB11/ PIN13_UART_RX already defined
    {{NULL}, -1}, // PIN_PB10/ PIN14_UART_TX already defined
    {{&machine_pin_type}, PIN_PB17}, // PIN15_SPI_SS_A
    {{&machine_pin_type}, PIN_PB22}, // PIN16_SPI_MOSI
    {{&machine_pin_type}, PIN_PB16}, // PIN17_SPI_MISO
    {{&machine_pin_type}, PIN_PB23}, // PIN18_SPI_SCK
};

const machine_led_obj_t machine_led_obj[] = {
    {{&machine_led_type}, PIN_PB30}, // USER_LED
};
