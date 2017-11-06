/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_SAMD51_PINS_H
#define MICROPY_INCLUDED_ATMEL_SAMD_SAMD51_PINS_H

#include "include/sam.h"

#include "common-hal/microcontroller/Pin.h"

void reset_pin(uint8_t pin);

#define MUX_C 2
#define MUX_D 3
#define MUX_E 4
#define MUX_F 5
#define PINMUX(pin, mux) ((((uint32_t) pin) << 16) | (mux))

#define NO_PIN PORT_BITS

// Pins in datasheet order.
#ifdef PIN_PB03
extern const mcu_pin_obj_t pin_PB03;
#endif
#ifdef PIN_PA00
extern const mcu_pin_obj_t pin_PA00;
#endif
#ifdef PIN_PA01
extern const mcu_pin_obj_t pin_PA01;
#endif
#ifdef PIN_PC00
extern const mcu_pin_obj_t pin_PC00;
#endif
#ifdef PIN_PC01
extern const mcu_pin_obj_t pin_PC01;
#endif
#ifdef PIN_PC02
extern const mcu_pin_obj_t pin_PC02;
#endif
#ifdef PIN_PC03
extern const mcu_pin_obj_t pin_PC03;
#endif
#ifdef PIN_PA02
extern const mcu_pin_obj_t pin_PA02;
#endif
#ifdef PIN_PA03
extern const mcu_pin_obj_t pin_PA03;
#endif
#ifdef PIN_PB04
extern const mcu_pin_obj_t pin_PB04;
#endif
#ifdef PIN_PB05
extern const mcu_pin_obj_t pin_PB05;
#endif
#ifdef PIN_PD00
extern const mcu_pin_obj_t pin_PD00;
#endif
#ifdef PIN_PD01
extern const mcu_pin_obj_t pin_PD01;
#endif
#ifdef PIN_PB06
extern const mcu_pin_obj_t pin_PB06;
#endif
#ifdef PIN_PB07
extern const mcu_pin_obj_t pin_PB07;
#endif
#ifdef PIN_PB08
extern const mcu_pin_obj_t pin_PB08;
#endif
#ifdef PIN_PB09
extern const mcu_pin_obj_t pin_PB09;
#endif
#ifdef PIN_PA04
extern const mcu_pin_obj_t pin_PA04;
#endif
#ifdef PIN_PA05
extern const mcu_pin_obj_t pin_PA05;
#endif
#ifdef PIN_PA06
extern const mcu_pin_obj_t pin_PA06;
#endif

// Second page
#ifdef PIN_PA07
extern const mcu_pin_obj_t pin_PA07;
#endif
#ifdef PIN_PC04
extern const mcu_pin_obj_t pin_PC04;
#endif
#ifdef PIN_PC05
extern const mcu_pin_obj_t pin_PC05;
#endif
#ifdef PIN_PC06
extern const mcu_pin_obj_t pin_PC06;
#endif
#ifdef PIN_PC07
extern const mcu_pin_obj_t pin_PC07;
#endif
#ifdef PIN_PA08
extern const mcu_pin_obj_t pin_PA08;
#endif
#ifdef PIN_PA09
extern const mcu_pin_obj_t pin_PA09;
#endif
#ifdef PIN_PA10
extern const mcu_pin_obj_t pin_PA10;
#endif
#ifdef PIN_PA11
extern const mcu_pin_obj_t pin_PA11;
#endif
#ifdef PIN_PB10
extern const mcu_pin_obj_t pin_PB10;
#endif
#ifdef PIN_PB11
extern const mcu_pin_obj_t pin_PB11;
#endif
#ifdef PIN_PB12
extern const mcu_pin_obj_t pin_PB12;
#endif
#ifdef PIN_PB13
extern const mcu_pin_obj_t pin_PB13;
#endif
#ifdef PIN_PB14
extern const mcu_pin_obj_t pin_PB14;
#endif
#ifdef PIN_PB15
extern const mcu_pin_obj_t pin_PB15;
#endif
#ifdef PIN_PD08
extern const mcu_pin_obj_t pin_PD08;
#endif
#ifdef PIN_PD09
extern const mcu_pin_obj_t pin_PD09;
#endif
#ifdef PIN_PD10
extern const mcu_pin_obj_t pin_PD10;
#endif
#ifdef PIN_PD11
extern const mcu_pin_obj_t pin_PD11;
#endif
#ifdef PIN_PD12
extern const mcu_pin_obj_t pin_PD12;
#endif
#ifdef PIN_PC10
extern const mcu_pin_obj_t pin_PC10;
#endif
#ifdef PIN_PC11
extern const mcu_pin_obj_t pin_PC11;
#endif
#ifdef PIN_PC12
extern const mcu_pin_obj_t pin_PC12;
#endif
#ifdef PIN_PC13
extern const mcu_pin_obj_t pin_PC13;
#endif
#ifdef PIN_PC14
extern const mcu_pin_obj_t pin_PC14;
#endif
#ifdef PIN_PC15
extern const mcu_pin_obj_t pin_PC15;
#endif
#ifdef PIN_PA12
extern const mcu_pin_obj_t pin_PA12;
#endif
#ifdef PIN_PA13
extern const mcu_pin_obj_t pin_PA13;
#endif

// Third page
#ifdef PIN_PA14
extern const mcu_pin_obj_t pin_PA14;
#endif
#ifdef PIN_PA15
extern const mcu_pin_obj_t pin_PA15;
#endif
#ifdef PIN_PA16
extern const mcu_pin_obj_t pin_PA16;
#endif
#ifdef PIN_PA17
extern const mcu_pin_obj_t pin_PA17;
#endif
#ifdef PIN_PA18
extern const mcu_pin_obj_t pin_PA18;
#endif
#ifdef PIN_PA19
extern const mcu_pin_obj_t pin_PA19;
#endif
#ifdef PIN_PC16
extern const mcu_pin_obj_t pin_PC16;
#endif
#ifdef PIN_PC17
extern const mcu_pin_obj_t pin_PC17;
#endif
#ifdef PIN_PC18
extern const mcu_pin_obj_t pin_PC18;
#endif
#ifdef PIN_PC19
extern const mcu_pin_obj_t pin_PC19;
#endif
#ifdef PIN_PC20
extern const mcu_pin_obj_t pin_PC20;
#endif
#ifdef PIN_PC21
extern const mcu_pin_obj_t pin_PC21;
#endif
#ifdef PIN_PC22
extern const mcu_pin_obj_t pin_PC22;
#endif
#ifdef PIN_PC23
extern const mcu_pin_obj_t pin_PC23;
#endif
#ifdef PIN_PD20
extern const mcu_pin_obj_t pin_PD20;
#endif
#ifdef PIN_PD21
extern const mcu_pin_obj_t pin_PD21;
#endif
#ifdef PIN_PB16
extern const mcu_pin_obj_t pin_PB16;
#endif
#ifdef PIN_PB17
extern const mcu_pin_obj_t pin_PB17;
#endif
#ifdef PIN_PB18
extern const mcu_pin_obj_t pin_PB18;
#endif
#ifdef PIN_PB19
extern const mcu_pin_obj_t pin_PB19;
#endif
#ifdef PIN_PB20
extern const mcu_pin_obj_t pin_PB20;
#endif
#ifdef PIN_PB21
extern const mcu_pin_obj_t pin_PB21;
#endif
#ifdef PIN_PA20
extern const mcu_pin_obj_t pin_PA20;
#endif
#ifdef PIN_PA21
extern const mcu_pin_obj_t pin_PA21;
#endif
#ifdef PIN_PA22
extern const mcu_pin_obj_t pin_PA22;
#endif
#ifdef PIN_PA23
extern const mcu_pin_obj_t pin_PA23;
#endif
#ifdef PIN_PA24
extern const mcu_pin_obj_t pin_PA24;
#endif
#ifdef PIN_PA25
extern const mcu_pin_obj_t pin_PA25;
#endif

// Fourth page
#ifdef PIN_PB22
extern const mcu_pin_obj_t pin_PB22;
#endif
#ifdef PIN_PB23
extern const mcu_pin_obj_t pin_PB23;
#endif
#ifdef PIN_PB24
extern const mcu_pin_obj_t pin_PB24;
#endif
#ifdef PIN_PB25
extern const mcu_pin_obj_t pin_PB25;
#endif
#ifdef PIN_PB26
extern const mcu_pin_obj_t pin_PB26;
#endif
#ifdef PIN_PB27
extern const mcu_pin_obj_t pin_PB27;
#endif
#ifdef PIN_PB28
extern const mcu_pin_obj_t pin_PB28;
#endif
#ifdef PIN_PB29
extern const mcu_pin_obj_t pin_PB29;
#endif
#ifdef PIN_PC24
extern const mcu_pin_obj_t pin_PC24;
#endif
#ifdef PIN_PC25
extern const mcu_pin_obj_t pin_PC25;
#endif
#ifdef PIN_PC26
extern const mcu_pin_obj_t pin_PC26;
#endif
#ifdef PIN_PC27
extern const mcu_pin_obj_t pin_PC27;
#endif
#ifdef PIN_PC28
extern const mcu_pin_obj_t pin_PC28;
#endif
#ifdef PIN_PA27
extern const mcu_pin_obj_t pin_PA27;
#endif
#ifdef PIN_PA30
extern const mcu_pin_obj_t pin_PA30;
#endif
#ifdef PIN_PA31
extern const mcu_pin_obj_t pin_PA31;
#endif
#ifdef PIN_PB30
extern const mcu_pin_obj_t pin_PB30;
#endif
#ifdef PIN_PB31
extern const mcu_pin_obj_t pin_PB31;
#endif
#ifdef PIN_PC30
extern const mcu_pin_obj_t pin_PC30;
#endif
#ifdef PIN_PC31
extern const mcu_pin_obj_t pin_PC31;
#endif
#ifdef PIN_PB00
extern const mcu_pin_obj_t pin_PB00;
#endif
#ifdef PIN_PB01
extern const mcu_pin_obj_t pin_PB01;
#endif
#ifdef PIN_PB02
extern const mcu_pin_obj_t pin_PB02;
#endif
#endif  // MICROPY_INCLUDED_ATMEL_SAMD_SAMD51_PINS_H
