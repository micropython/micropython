/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Robert Foss, Daniel Busch
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

#include "py/mpconfig.h"
#if MICROPY_ESP8266_APA102

#include <stdio.h>
#include "c_types.h"
#include "eagle_soc.h"
#include "user_interface.h"
#include "espapa102.h"

#define NOP asm volatile(" nop \n\t")

static inline void _esp_apa102_send_byte(uint32_t clockPinMask, uint32_t dataPinMask, uint8_t byte) {
    for (uint32_t i = 0; i < 8; i++) {
        if (byte & 0x80) {
            // set data pin high
            GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, dataPinMask);
        } else {
            // set data pin low
            GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, dataPinMask);
        }

        // set clock pin high
        GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, clockPinMask);
        byte <<= 1;
        NOP;
        NOP;

        // set clock pin low
        GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, clockPinMask);
        NOP;
        NOP;
    }
}

static inline void _esp_apa102_send_colors(uint32_t clockPinMask, uint32_t dataPinMask, uint8_t *pixels, uint32_t numBytes) {
    for (uint32_t i = 0; i < numBytes / 4; i++) {
        _esp_apa102_send_byte(clockPinMask, dataPinMask, pixels[i * 4 + 3] | 0xE0);
        _esp_apa102_send_byte(clockPinMask, dataPinMask, pixels[i * 4 + 2]);
        _esp_apa102_send_byte(clockPinMask, dataPinMask, pixels[i * 4 + 1]);
        _esp_apa102_send_byte(clockPinMask, dataPinMask, pixels[i * 4]);
    }
}

static inline void _esp_apa102_start_frame(uint32_t clockPinMask, uint32_t dataPinMask) {
    for (uint32_t i = 0; i < 4; i++) {
        _esp_apa102_send_byte(clockPinMask, dataPinMask, 0x00);
    }
}

static inline void _esp_apa102_append_additionial_cycles(uint32_t clockPinMask, uint32_t dataPinMask, uint32_t numBytes) {
    GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, dataPinMask);

    // we need to write some more clock cycles, because each led
    // delays the data by one edge after inverting the clock
    for (uint32_t i = 0; i < numBytes / 8 + ((numBytes / 4) % 2); i++) {
        GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, clockPinMask);
        NOP;
        NOP;

        GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, clockPinMask);
        NOP;
        NOP;
    }
}

static inline void _esp_apa102_end_frame(uint32_t clockPinMask, uint32_t dataPinMask) {
    for (uint32_t i = 0; i < 4; i++) {
        _esp_apa102_send_byte(clockPinMask, dataPinMask, 0xFF);
    }
}

void esp_apa102_write(uint8_t clockPin, uint8_t dataPin, uint8_t *pixels, uint32_t numBytes) {
    uint32_t clockPinMask, dataPinMask;

    clockPinMask = 1 << clockPin;
    dataPinMask = 1 << dataPin;

    // start the frame
    _esp_apa102_start_frame(clockPinMask, dataPinMask);

    // write pixels
    _esp_apa102_send_colors(clockPinMask, dataPinMask, pixels, numBytes);

    // end the frame
    _esp_apa102_append_additionial_cycles(clockPinMask, dataPinMask, numBytes);
    _esp_apa102_end_frame(clockPinMask, dataPinMask);
}

#endif
