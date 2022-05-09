/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * This file initialises provides and checks pin capabilities as required
 * for USART, I2C, SPI, PWM, ADC
 *
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Robert Hammelrath
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

typedef struct _sercom_pad_config_t {
    uint8_t alt_fct;
    uint8_t pad_nr;
} sercom_pad_config_t;

typedef struct _adc_config_t {
    uint8_t device;
    uint8_t channel;
} adc_config_t;

#define ALT_FCT_ADC0      1
#define ALT_FCT_SERCOM1   2
#define ALT_FCT_SERCOM2   3
#define ALT_FCT_T2C       4
#define ALT_FCT_TC        5

sercom_pad_config_t is_sercom_n(int pin_id, uint8_t sercom);
adc_config_t is_adc(int pin_id, int32_t flag);
uint8_t is_eic(int pin_id);

void pin_cap_set_mux(int pin, uint8_t mux);
