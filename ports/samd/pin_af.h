/*
 * This file is part of the MicroPython project, http://micropython.org/
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
 *
 * This file initialises provides and checks pin capabilities as required
 * for USART, I2C, SPI, PWM, ADC
 *
 */

#if defined(MCU_SAMD21)

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t pin_id;
    char *name;
    uint8_t eic;
    uint8_t adc0;
    uint8_t sercom1;
    uint8_t sercom2;
    uint8_t tcc1;
    uint8_t tcc2;
} machine_pin_obj_t;

#define ALT_FCT_TC        4
#define ALT_FCT_TCC1      4
#define ALT_FCT_TCC2      5

#elif defined(MCU_SAMD51)

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t pin_id;
    char *name;
    uint8_t eic;
    uint8_t adc0;
    uint8_t adc1;
    uint8_t sercom1;
    uint8_t sercom2;
    uint8_t tc;
    uint8_t tcc1;
    uint8_t tcc2;
} machine_pin_obj_t;

#define ALT_FCT_TC        4
#define ALT_FCT_TCC1      5
#define ALT_FCT_TCC2      6

#endif

typedef struct _sercom_pad_config_t {
    uint8_t alt_fct;
    uint8_t pad_nr;
} sercom_pad_config_t;

typedef struct _adc_config_t {
    uint8_t device;
    uint8_t channel;
} adc_config_t;

typedef struct _pwm_config_t {
    uint8_t alt_fct;
    uint8_t device_channel;
} pwm_config_t;

#define ALT_FCT_EIC       0
#define ALT_FCT_ADC       1
#define ALT_FCT_DAC       1
#define ALT_FCT_SERCOM1   2
#define ALT_FCT_SERCOM2   3

extern const machine_pin_obj_t pin_af_table[];

sercom_pad_config_t get_sercom_config(int pin_id, uint8_t sercom);
adc_config_t get_adc_config(int pin_id, int32_t flag);
pwm_config_t get_pwm_config(int pin_id, int wanted_dev, uint8_t used_dev[]);
const machine_pin_obj_t *get_pin_obj_ptr(int pin_id);
const char *pin_name(int id);
const machine_pin_obj_t *pin_find(mp_obj_t pin);
