/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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
#ifndef MICROPY_INCLUDED_CC3200_MODS_PYBPIN_H
#define MICROPY_INCLUDED_CC3200_MODS_PYBPIN_H

enum {
    PORT_A0 = GPIOA0_BASE,
    PORT_A1 = GPIOA1_BASE,
    PORT_A2 = GPIOA2_BASE,
    PORT_A3 = GPIOA3_BASE,
};

enum {
    PIN_FN_UART = 0,
    PIN_FN_SPI,
    PIN_FN_I2S,
    PIN_FN_I2C,
    PIN_FN_TIM,
    PIN_FN_SD,
    PIN_FN_ADC,
};

enum {
    PIN_TYPE_UART_TX = 0,
    PIN_TYPE_UART_RX,
    PIN_TYPE_UART_RTS,
    PIN_TYPE_UART_CTS,
};

enum {
    PIN_TYPE_SPI_CLK = 0,
    PIN_TYPE_SPI_MOSI,
    PIN_TYPE_SPI_MISO,
    PIN_TYPE_SPI_CS0,
};

enum {
    PIN_TYPE_I2S_CLK = 0,
    PIN_TYPE_I2S_FS,
    PIN_TYPE_I2S_DAT0,
    PIN_TYPE_I2S_DAT1,
};

enum {
    PIN_TYPE_I2C_SDA = 0,
    PIN_TYPE_I2C_SCL,
};

enum {
    PIN_TYPE_TIM_PWM = 0,
};

enum {
    PIN_TYPE_SD_CLK = 0,
    PIN_TYPE_SD_CMD,
    PIN_TYPE_SD_DAT0,
};

enum {
    PIN_TYPE_ADC_CH0 = 0,
    PIN_TYPE_ADC_CH1,
    PIN_TYPE_ADC_CH2,
    PIN_TYPE_ADC_CH3,
};

typedef struct {
  qstr name;
  int8_t  idx;
  uint8_t fn;
  uint8_t unit;
  uint8_t type;
} pin_af_t;

typedef struct {
    const mp_obj_base_t base;
    const qstr          name;
    const uint32_t      port;
    const pin_af_t      *af_list;
    uint16_t            pull;
    const uint8_t       bit;
    const uint8_t       pin_num;
    int8_t              af;
    uint8_t             strength;
    uint8_t             mode;        // this is now a combination of type and mode
    const uint8_t       num_afs;     // 255 AFs
    uint8_t             value;
    uint8_t             used;
    uint8_t             irq_trigger;
    uint8_t             irq_flags;
} pin_obj_t;

extern const mp_obj_type_t pin_type;

typedef struct {
    const char *name;
    const pin_obj_t *pin;
} pin_named_pin_t;

typedef struct {
    mp_obj_base_t base;
    qstr name;
    const pin_named_pin_t *named_pins;
} pin_named_pins_obj_t;

extern const mp_obj_type_t pin_board_pins_obj_type;
extern const mp_obj_dict_t pin_board_pins_locals_dict;

void pin_init0(void);
void pin_config(pin_obj_t *self, int af, uint mode, uint type, int value, uint strength);
pin_obj_t *pin_find(mp_obj_t user_obj);
void pin_assign_pins_af (mp_obj_t *pins, uint32_t n_pins, uint32_t pull, uint32_t fn, uint32_t unit);
uint8_t pin_find_peripheral_unit (const mp_obj_t pin, uint8_t fn, uint8_t type);
uint8_t pin_find_peripheral_type (const mp_obj_t pin, uint8_t fn, uint8_t unit);
int8_t pin_find_af_index (const pin_obj_t* pin, uint8_t fn, uint8_t unit, uint8_t type);;

#endif // MICROPY_INCLUDED_CC3200_MODS_PYBPIN_H
