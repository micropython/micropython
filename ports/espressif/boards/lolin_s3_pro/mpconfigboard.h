/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
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

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "LOLIN S3 PRO 16MB Flash 8MB PSRAM"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO38)

// #define MICROPY_HW_LED_STATUS (&pin_GPIO14)

#define DEFAULT_BUTTON (&pin_GPIO0)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO10)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO9)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO12)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO13)

#define DEFAULT_TF_CS (&pin_GPIO46)
#define DEFAULT_TS_CS (&pin_GPIO45)
#define DEFAULT_TFT_CS (&pin_GPIO48)
#define DEFAULT_TFT_DC (&pin_GPIO47)
#define DEFAULT_TFT_RST (&pin_GPIO21)
#define DEFAULT_TFT_LED (&pin_GPIO14)


#define DEFAULT_TX0 (&pin_GPIO43)
#define DEFAULT_RX0 (&pin_GPIO44)
