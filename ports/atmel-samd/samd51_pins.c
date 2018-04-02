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

#include "shared-bindings/microcontroller/Pin.h"

#include "samd51_pins.h"

#define SERCOM(sercom_index, p_pad)     \
{ \
  .index = sercom_index,   \
  .pad = p_pad \
}

#define NO_SERCOM \
{ \
  .index = 0x3f, \
  .pad = 0 \
}

#define TCC(p_index, p_wave_output) \
{ \
  .index = p_index, \
  .is_tc = false, \
  .wave_output = p_wave_output \
}

#define TC(p_index, p_wave_output) \
{ \
  .index = p_index, \
  .is_tc = true, \
  .wave_output = p_wave_output \
}

#define NO_TIMER TCC(0xff, 0)

#define TOUCH(y_line) \
    .has_touch = true, \
    .touch_y_line = y_line,

#define NO_TOUCH \
    .has_touch = false,

#define EXTINT_CHANNEL(channel) \
    .has_extint = true, \
    .extint_channel = channel,

#define NO_EXTINT \
    .has_extint = false,

#define ADC_INPUT(input) (input)

#define NO_ADC 0xff

// This macro is used to simplify pin definition in boards/<board>/pins.c
#define PIN(p_name, p_extint, p_adc0, p_adc1, p_touch, \
            p_primary_sercom, p_secondary_sercom, \
            p_primary_timer, p_secondary_timer, p_tertiary_timer) \
const mcu_pin_obj_t pin_## p_name = { \
    { &mcu_pin_type }, \
    .name = MP_QSTR_## p_name, \
    .pin = (PIN_## p_name), \
    p_extint \
    p_touch \
    .adc_input = {p_adc0, p_adc1}, \
    .timer = {p_primary_timer, p_secondary_timer, p_tertiary_timer}, \
    .sercom = {p_primary_sercom, p_secondary_sercom}, \
}

// Pins in datasheet order.
// NOTE(tannewt): TC wave out 0 is commented out because the first channel is
// used to vary the 16 bit timer's frequency.
#ifdef PIN_PB03
PIN(PB03, EXTINT_CHANNEL(3), ADC_INPUT(15), NO_ADC,
    TOUCH(21),
    NO_SERCOM,
    SERCOM(5, 1),
    #ifdef TC6
    TC(6, 1),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PA00
PIN(PA00, EXTINT_CHANNEL(0), NO_ADC, NO_ADC,
    NO_TOUCH,
    NO_SERCOM,
    SERCOM(1, 0),
    TC(2, 0),
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PA01
PIN(PA01, EXTINT_CHANNEL(1), NO_ADC, NO_ADC,
    NO_TOUCH,
    NO_SERCOM,
    SERCOM(1, 1),
    TC(2, 0),
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC00
PIN(PC00, EXTINT_CHANNEL(0), NO_ADC, ADC_INPUT(10),
    NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC01
PIN(PC01, EXTINT_CHANNEL(1), NO_ADC, ADC_INPUT(11),
    NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC02
PIN(PC02, EXTINT_CHANNEL(2), NO_ADC, ADC_INPUT(4),
    NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC03
PIN(PC03, EXTINT_CHANNEL(3), NO_ADC, ADC_INPUT(5),
    NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PA02
PIN(PA02, EXTINT_CHANNEL(2), ADC_INPUT(0), NO_ADC,
    NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PA03
PIN(PA03, EXTINT_CHANNEL(3), ADC_INPUT(1), NO_ADC,
    TOUCH(0),
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB04
PIN(PB04, EXTINT_CHANNEL(4), NO_ADC, ADC_INPUT(6),
    #ifdef PINMUX_PB04B_ADC0_PTCXY22
    TOUCH(22),
    #else
    NO_TOUCH,
    #endif
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB05
PIN(PB05, EXTINT_CHANNEL(5), NO_ADC, ADC_INPUT(7),
    #ifdef PINMUX_PB05B_ADC0_PTCXY23
    TOUCH(23),
    #else
    NO_TOUCH,
    #endif
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PD00
PIN(PD00, EXTINT_CHANNEL(0), NO_ADC, ADC_INPUT(14), NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PD01
PIN(PD01, EXTINT_CHANNEL(1), NO_ADC, ADC_INPUT(15), NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB06
PIN(PB06, EXTINT_CHANNEL(6), NO_ADC, ADC_INPUT(8),
    #ifdef PINMUX_PB06B_ADC0_PTCXY24
    TOUCH(24),
    #else
    NO_TOUCH,
    #endif
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB07
PIN(PB07, EXTINT_CHANNEL(7), NO_ADC, ADC_INPUT(9),
    #ifdef PINMUX_PB07B_ADC0_PTCXY25
    TOUCH(25),
    #else
    NO_TOUCH,
    #endif
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB08
PIN(PB08, EXTINT_CHANNEL(8), ADC_INPUT(2), ADC_INPUT(0), TOUCH(1),
    NO_SERCOM,
    SERCOM(4, 0),
    #ifdef TC4
    TC(4, 0),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB09
PIN(PB09, EXTINT_CHANNEL(9), ADC_INPUT(3), ADC_INPUT(1), TOUCH(2),
    NO_SERCOM,
    SERCOM(4, 1),
    #ifdef TC4
    TC(4, 1),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PA04
PIN(PA04, EXTINT_CHANNEL(4), ADC_INPUT(4), NO_ADC, TOUCH(3),
    NO_SERCOM,
    SERCOM(0, 0),
    TC(0, 0),
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PA05
PIN(PA05, EXTINT_CHANNEL(5), ADC_INPUT(5), NO_ADC, NO_TOUCH,
    NO_SERCOM,
    SERCOM(0, 1),
    TC(0, 1),
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PA06
PIN(PA06, EXTINT_CHANNEL(6), ADC_INPUT(6), NO_ADC, TOUCH(4),
    NO_SERCOM,
    SERCOM(0, 2),
    TC(1, 0),
    NO_TIMER,
    NO_TIMER);
#endif

// Second page
#ifdef PIN_PA07
PIN(PA07, EXTINT_CHANNEL(7), ADC_INPUT(7), NO_ADC, TOUCH(5),
    NO_SERCOM,
    SERCOM(0, 3),
    TC(1, 1),
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC04
PIN(PC04, EXTINT_CHANNEL(4), NO_ADC, NO_ADC, NO_TOUCH,
    #ifdef SERCOM6
    SERCOM(6, 0),
    #else
    NO_SERCOM,
    #endif
    NO_SERCOM,
    NO_TIMER,
    TCC(0, 0),
    NO_TIMER);
#endif
#ifdef PIN_PC05
PIN(PC05, EXTINT_CHANNEL(5), NO_ADC, NO_ADC, NO_TOUCH,
    #ifdef SERCOM6
    SERCOM(6, 1),
    #else
    NO_SERCOM,
    #endif
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC06
PIN(PC06, EXTINT_CHANNEL(6), NO_ADC, NO_ADC, NO_TOUCH,
    #ifdef SERCOM6
    SERCOM(6, 2),
    #else
    NO_SERCOM,
    #endif
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC07
PIN(PC07, EXTINT_CHANNEL(7), NO_ADC, NO_ADC, NO_TOUCH,
    #ifdef SERCOM6
    SERCOM(6, 3),
    #else
    NO_SERCOM,
    #endif
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PA08
PIN(PA08, NO_EXTINT, ADC_INPUT(8), ADC_INPUT(2), TOUCH(6),
    SERCOM(0, 0),
    SERCOM(2, 1),
    TC(0, 0),
    TCC(0, 0),
    TCC(1, 4));
#endif
#ifdef PIN_PA09
PIN(PA09, EXTINT_CHANNEL(9), ADC_INPUT(9), ADC_INPUT(3), TOUCH(7),
    SERCOM(0, 1),
    SERCOM(2, 0),
    TC(0, 1),
    TCC(0, 1),
    TCC(1, 5));
#endif
#ifdef PIN_PA10
PIN(PA10, EXTINT_CHANNEL(10), ADC_INPUT(10), NO_ADC, TOUCH(8),
    SERCOM(0, 2),
    SERCOM(2, 2),
    TC(1, 0),
    TCC(0, 2),
    TCC(1, 6));
#endif
#ifdef PIN_PA11
PIN(PA11, EXTINT_CHANNEL(11), ADC_INPUT(11), NO_ADC, TOUCH(9),
    SERCOM(0, 3),
    SERCOM(2, 3),
    TC(1, 0),
    TCC(0, 3),
    TCC(1, 7));
#endif
#ifdef PIN_PB10
PIN(PB10, EXTINT_CHANNEL(10), NO_ADC, NO_ADC, NO_TOUCH,
    NO_SERCOM,
    SERCOM(4, 2),
    #ifdef TC5
    TC(5, 0),
    #else
    NO_TIMER,
    #endif
    TCC(0, 4),
    TCC(1, 0));
#endif
#ifdef PIN_PB11
PIN(PB11, EXTINT_CHANNEL(11), NO_ADC, NO_ADC, NO_TOUCH,
    NO_SERCOM,
    SERCOM(4, 3),
    #ifdef TC5
    TC(5, 1),
    #else
    NO_TIMER,
    #endif
    TCC(0, 5),
    TCC(1, 1));
#endif
#ifdef PIN_PB12
PIN(PB12, EXTINT_CHANNEL(12), NO_ADC, NO_ADC,
    #ifdef PINMUX_PB12B_ADC0_PTCXY26
    TOUCH(26),
    #else
    NO_TOUCH,
    #endif
    SERCOM(4, 0),
    NO_SERCOM,
    #ifdef TC4
    TC(4, 0),
    #else
    NO_TIMER,
    #endif
    #ifdef TCC3
    TCC(3, 0),
    #else
    NO_TIMER,
    #endif
    TCC(0, 0));
#endif
#ifdef PIN_PB13
PIN(PB13, EXTINT_CHANNEL(13), NO_ADC, NO_ADC,
    #ifdef PINMUX_PB13B_ADC0_PTCXY27
    TOUCH(27),
    #else
    NO_TOUCH,
    #endif
    SERCOM(4, 1),
    NO_SERCOM,
    #ifdef TC4
    TC(4, 1),
    #else
    NO_TIMER,
    #endif
    #ifdef TCC3
    TCC(3, 1),
    #else
    NO_TIMER,
    #endif
    TCC(0, 1));
#endif
#ifdef PIN_PB14
PIN(PB14, EXTINT_CHANNEL(14), NO_ADC, NO_ADC,
    #ifdef PINMUX_PB14B_ADC0_PTCXY28
    TOUCH(28),
    #else
    NO_TOUCH,
    #endif
    SERCOM(4, 2),
    NO_SERCOM,
    #ifdef TC5
    TC(5, 0),
    #else
    NO_TIMER,
    #endif
    #ifdef TCC4
    TCC(4, 0),
    #else
    NO_TIMER,
    #endif
    TCC(0, 2));
#endif
#ifdef PIN_PB15
PIN(PB15, EXTINT_CHANNEL(15), NO_ADC, NO_ADC,
    #ifdef PINMUX_PB15B_ADC0_PTCXY29
    TOUCH(28),
    #else
    NO_TOUCH,
    #endif
    SERCOM(4, 3),
    NO_SERCOM,
    #ifdef TC5
    TC(5, 1),
    #else
    NO_TIMER,
    #endif
    #ifdef TCC4
    TCC(4, 1),
    #else
    NO_TIMER,
    #endif
    TCC(0, 3));
#endif
#ifdef PIN_PD08
PIN(PD08, EXTINT_CHANNEL(3), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM7
    SERCOM(7, 0),
    #else
    NO_SERCOM,
    #endif
    #ifdef SERCOM6
    SERCOM(6, 1),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(0, 1),
    NO_TIMER);
#endif
#ifdef PIN_PD09
PIN(PD09, EXTINT_CHANNEL(4), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM7
    SERCOM(7, 1),
    #else
    NO_SERCOM,
    #endif
    #ifdef SERCOM6
    SERCOM(6, 0),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(0, 2),
    NO_TIMER);
#endif
#ifdef PIN_PD10
PIN(PD10, EXTINT_CHANNEL(5), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM7
    SERCOM(7, 2),
    #else
    NO_SERCOM,
    #endif
    #ifdef SERCOM6
    SERCOM(6, 2),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(0, 3),
    NO_TIMER);
#endif
#ifdef PIN_PD11
PIN(PD11, EXTINT_CHANNEL(6), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM7
    SERCOM(7, 3),
    #else
    NO_SERCOM,
    #endif
    #ifdef SERCOM6
    SERCOM(6, 3),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(0, 4),
    NO_TIMER);
#endif
#ifdef PIN_PD12
PIN(PD12, EXTINT_CHANNEL(7), NO_ADC, NO_ADC,
    NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    TCC(0, 5),
    NO_TIMER);
#endif
#ifdef PIN_PC10
PIN(PC10, EXTINT_CHANNEL(10), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM6
    SERCOM(6, 2),
    #else
    NO_SERCOM,
    #endif
    #ifdef SERCOM7
    SERCOM(7, 2),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(0, 0),
    TCC(1, 4));
#endif
#ifdef PIN_PC11
PIN(PC11, EXTINT_CHANNEL(11), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM6
    SERCOM(6, 3),
    #else
    NO_SERCOM,
    #endif
    #ifdef SERCOM7
    SERCOM(7, 3),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(0, 1),
    TCC(1, 5));
#endif
#ifdef PIN_PC12
PIN(PC12, EXTINT_CHANNEL(12), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM7
    SERCOM(7, 0),
    #else
    NO_SERCOM,
    #endif
    #ifdef SERCOM6
    SERCOM(6, 1),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(0, 2),
    TCC(1, 6));
#endif
#ifdef PIN_PC13
PIN(PC13, EXTINT_CHANNEL(13), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM7
    SERCOM(7, 1),
    #else
    NO_SERCOM,
    #endif
    #ifdef SERCOM6
    SERCOM(6, 0),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(0, 3),
    TCC(1, 7));
#endif
#ifdef PIN_PC14
PIN(PC14, EXTINT_CHANNEL(14), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM7
    SERCOM(7, 2),
    #else
    NO_SERCOM,
    #endif
    #ifdef SERCOM6
    SERCOM(6, 2),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(0, 4),
    TCC(1, 0));
#endif
#ifdef PIN_PC15
PIN(PC15, EXTINT_CHANNEL(15), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM7
    SERCOM(7, 3),
    #else
    NO_SERCOM,
    #endif
    #ifdef SERCOM6
    SERCOM(6, 3),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(0, 5),
    TCC(1, 1));
#endif
#ifdef PIN_PA12
PIN(PA12, EXTINT_CHANNEL(12), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(2, 0),
    SERCOM(4, 1),
    TC(2, 0),
    TCC(0, 6),
    TCC(1, 2));
#endif
#ifdef PIN_PA13
PIN(PA13, EXTINT_CHANNEL(13), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(2, 1),
    SERCOM(4, 0),
    TC(2, 1),
    TCC(0, 7),
    TCC(1, 3));
#endif

// Third page
#ifdef PIN_PA14
PIN(PA14, EXTINT_CHANNEL(14), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(2, 2),
    SERCOM(4, 2),
    TC(3, 0),
    TCC(2, 0),
    TCC(1, 2));
#endif
#ifdef PIN_PA15
PIN(PA15, EXTINT_CHANNEL(15), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(2, 3),
    SERCOM(4, 3),
    TC(3, 1),
    TCC(2, 1),
    TCC(1, 3));
#endif
#ifdef PIN_PA16
PIN(PA16, EXTINT_CHANNEL(0), NO_ADC, NO_ADC, TOUCH(10),
    SERCOM(1, 0),
    SERCOM(3, 1),
    TC(2, 0),
    TCC(1, 0),
    TCC(0, 4));
#endif
#ifdef PIN_PA17
PIN(PA17, EXTINT_CHANNEL(1), NO_ADC, NO_ADC, TOUCH(11),
    SERCOM(1, 1),
    SERCOM(3, 0),
    TC(2, 1),
    TCC(1, 1),
    TCC(0, 5));
#endif
#ifdef PIN_PA18
PIN(PA18, EXTINT_CHANNEL(2), NO_ADC, NO_ADC, TOUCH(12),
    SERCOM(1, 2),
    SERCOM(3, 2),
    TC(3, 0),
    TCC(1, 2),
    TCC(0, 6));
#endif
#ifdef PIN_PA19
PIN(PA19, EXTINT_CHANNEL(3), NO_ADC, NO_ADC, TOUCH(13),
    SERCOM(1, 3),
    SERCOM(3, 3),
    TC(3, 1),
    TCC(1, 3),
    TCC(0, 7));
#endif
#ifdef PIN_PC16
PIN(PC16, EXTINT_CHANNEL(0), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM6
    SERCOM(6, 0),
    #else
    NO_SERCOM,
    #endif
    SERCOM(0, 1),
    NO_TIMER,
    TCC(TCC0, 0),
    NO_TIMER);
#endif
#ifdef PIN_PC17
PIN(PC17, EXTINT_CHANNEL(1), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM6
    SERCOM(6, 1),
    #else
    NO_SERCOM,
    #endif
    SERCOM(0, 0),
    NO_TIMER,
    TCC(0, 1),
    NO_TIMER);
#endif
#ifdef PIN_PC18
PIN(PC18, EXTINT_CHANNEL(2), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM6
    SERCOM(6, 2),
    #else
    NO_SERCOM,
    #endif
    SERCOM(0, 2),
    NO_TIMER,
    TCC(0, 2),
    NO_TIMER);
#endif
#ifdef PIN_PC19
PIN(PC19, EXTINT_CHANNEL(3), NO_ADC, NO_ADC,
    NO_TOUCH,
    #ifdef SERCOM6
    SERCOM(6, 3),
    #else
    NO_SERCOM,
    #endif
    SERCOM(0, 3),
    NO_TIMER,
    TCC(0, 3),
    NO_TIMER);
#endif
#ifdef PIN_PC20
PIN(PC20, EXTINT_CHANNEL(4), NO_ADC, NO_ADC,
    NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    TCC(0, 4),
    NO_TIMER);
#endif
#ifdef PIN_PC21
PIN(PC21, EXTINT_CHANNEL(5), NO_ADC, NO_ADC,
    NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    TCC(0, 5),
    NO_TIMER);
#endif
#ifdef PIN_PC22
PIN(PC22, EXTINT_CHANNEL(6), NO_ADC, NO_ADC,
    NO_TOUCH,
    SERCOM(1, 0),
    SERCOM(3, 1),
    NO_TIMER,
    TCC(0, 6),
    NO_TIMER);
#endif
#ifdef PIN_PC23
PIN(PC23, EXTINT_CHANNEL(7), NO_ADC, NO_ADC,
    NO_TOUCH,
    SERCOM(1, 1),
    SERCOM(3, 0),
    NO_TIMER,
    TCC(0, 7),
    NO_TIMER);
#endif
#ifdef PIN_PD20
PIN(PD20, EXTINT_CHANNEL(10), NO_ADC, NO_ADC,
    NO_TOUCH,
    SERCOM(1, 2),
    SERCOM(3, 2),
    NO_TIMER,
    TCC(1, 0),
    NO_TIMER);
#endif
#ifdef PIN_PD21
PIN(PD21, EXTINT_CHANNEL(11), NO_ADC, NO_ADC,
    NO_TOUCH,
    SERCOM(1, 3),
    SERCOM(3, 3),
    NO_TIMER,
    TCC(1, 1),
    NO_TIMER);
#endif




#ifdef PIN_PB16
PIN(PB16, EXTINT_CHANNEL(0), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(5, 0),
    NO_SERCOM,
    #ifdef TC6
    TC(6, 0),
    #else
    NO_TIMER,
    #endif
    TCC(3, 0),
    TCC(0, 4));
#endif
#ifdef PIN_PB17
PIN(PB17, EXTINT_CHANNEL(1), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(5, 1),
    NO_SERCOM,
    #ifdef TC6
    TC(6, 1),
    #else
    NO_TIMER,
    #endif
    TCC(3, 1),
    TCC(0, 5));
#endif
#ifdef PIN_PB18
PIN(PB18, EXTINT_CHANNEL(2), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(5, 2),
    #ifdef SERCOM7
    SERCOM(7, 2),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(1, 0),
    NO_TIMER);
#endif
#ifdef PIN_PB19
PIN(PB19, EXTINT_CHANNEL(3), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(5, 3),
    #ifdef SERCOM7
    SERCOM(7, 3),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(1, 1),
    NO_TIMER);
#endif
#ifdef PIN_PB20
PIN(PB20, EXTINT_CHANNEL(4), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(5, 0),
    #ifdef SERCOM7
    SERCOM(7, 1),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(1, 2),
    NO_TIMER);
#endif
#ifdef PIN_PB21
PIN(PB21, EXTINT_CHANNEL(5), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(5, 1),
    #ifdef SERCOM7
    SERCOM(7, 0),
    #else
    NO_SERCOM,
    #endif
    NO_TIMER,
    TCC(1, 3),
    NO_TIMER);
#endif
#ifdef PIN_PA20
PIN(PA20, EXTINT_CHANNEL(4), NO_ADC, NO_ADC, TOUCH(14),
    SERCOM(5, 2),
    SERCOM(3, 2),
    #ifdef TC7
    TC(7, 0),
    #else
    NO_TIMER,
    #endif
    TCC(1, 4),
    TCC(0, 0));
#endif
#ifdef PIN_PA21
PIN(PA21, EXTINT_CHANNEL(5), NO_ADC, NO_ADC, TOUCH(15),
    SERCOM(5, 3),
    SERCOM(3, 3),
    #ifdef TC7
    TC(7, 1),
    #else
    NO_TIMER,
    #endif
    TCC(1, 5),
    TCC(0, 1));
#endif
#ifdef PIN_PA22
PIN(PA22, EXTINT_CHANNEL(6), NO_ADC, NO_ADC, TOUCH(16),
    SERCOM(3, 0),
    SERCOM(5, 1),
    #ifdef TC4
    TC(4, 0),
    #else
    NO_TIMER,
    #endif
    TCC(1, 6),
    TCC(0, 2));
#endif
#ifdef PIN_PA23
PIN(PA23, EXTINT_CHANNEL(7), NO_ADC, NO_ADC, TOUCH(17),
    SERCOM(3, 1),
    SERCOM(5, 0),
    #ifdef TC4
    TC(4, 1),
    #else
    NO_TIMER,
    #endif
    TCC(1, 7),
    TCC(0, 3));
#endif
#ifdef PIN_PA24
PIN(PA24, EXTINT_CHANNEL(8), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(3, 2),
    SERCOM(5, 2),
    #ifdef TC5
    TC(5, 0),
    #else
    NO_TIMER,
    #endif
    TCC(2, 2),
    NO_TIMER);
#endif
#ifdef PIN_PA25
PIN(PA25, EXTINT_CHANNEL(9), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(3, 3),
    SERCOM(5, 3),
    #ifdef TC5
    TC(5, 1),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_TIMER);
#endif

// Fourth page
#ifdef PIN_PB22
PIN(PB22, EXTINT_CHANNEL(6), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(1, 2),
    SERCOM(5, 2),
    #ifdef TC7
    TC(7, 0),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB23
PIN(PB23, EXTINT_CHANNEL(7), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(1, 3),
    SERCOM(5, 3),
    #ifdef TC7
    TC(7, 1),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB24
PIN(PB24, EXTINT_CHANNEL(8), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(0, 0),
    SERCOM(2, 1),
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB25
PIN(PB25, EXTINT_CHANNEL(9), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(0, 1),
    SERCOM(2, 0),
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB26
PIN(PB26, EXTINT_CHANNEL(12), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(2, 0),
    SERCOM(4, 1),
    NO_TIMER,
    TCC(TCC1, 2),
    NO_TIMER);
#endif
#ifdef PIN_PB27
PIN(PB27, EXTINT_CHANNEL(13), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(2, 1),
    SERCOM(4, 0),
    NO_TIMER,
    TCC(1, 3),
    NO_TIMER);
#endif
#ifdef PIN_PB28
PIN(PB28, EXTINT_CHANNEL(14), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(2, 2),
    SERCOM(4, 2),
    NO_TIMER,
    TCC(1, 4),
    NO_TIMER);
#endif
#ifdef PIN_PB29
PIN(PB29, EXTINT_CHANNEL(15), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(2, 3),
    SERCOM(4, 3),
    NO_TIMER,
    TCC(1, 5),
    NO_TIMER);
#endif
#ifdef PIN_PC24
PIN(PC24, EXTINT_CHANNEL(8), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(0, 2),
    SERCOM(2, 2),
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC25
PIN(PC25, EXTINT_CHANNEL(9), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(0, 3),
    SERCOM(2, 3),
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC26
PIN(PC26, EXTINT_CHANNEL(10), NO_ADC, NO_ADC, NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC27
PIN(PC27, EXTINT_CHANNEL(11), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(1, 0),
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC28
PIN(PC28, EXTINT_CHANNEL(12), NO_ADC, NO_ADC, NO_TOUCH,
    SERCOM(1, 1),
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PA27
PIN(PA27, EXTINT_CHANNEL(11), NO_ADC, NO_ADC, TOUCH(18),
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PA30
PIN(PA30, EXTINT_CHANNEL(14), NO_ADC, NO_ADC, TOUCH(19),
    #ifdef SERCOM7
    SERCOM(7, 2),
    #else
    NO_SERCOM,
    #endif
    SERCOM(1, 2),
    #ifdef TC6
    TC(6, 0),
    #else
    NO_TIMER,
    #endif
    TCC(2, 0),
    NO_TIMER);
#endif
#ifdef PIN_PA31
PIN(PA31, EXTINT_CHANNEL(15), NO_ADC, NO_ADC, NO_TOUCH,
    #ifdef SERCOM7
    SERCOM(7, 3),
    #else
    NO_SERCOM,
    #endif
    SERCOM(1, 3),
    #ifdef TC6
    TC(6, 1),
    #else
    NO_TIMER,
    #endif
    TCC(2, 1),
    NO_TIMER);
#endif
#ifdef PIN_PB30
PIN(PB30, EXTINT_CHANNEL(14), NO_ADC, NO_ADC, NO_TOUCH,
    #ifdef SERCOM7
    SERCOM(7, 0),
    #else
    NO_SERCOM,
    #endif
    SERCOM(5, 1),
    TC(0, 0),
    TCC(4, 0),
    TCC(0, 6));
#endif
#ifdef PIN_PB31
PIN(PB31, EXTINT_CHANNEL(15), NO_ADC, NO_ADC, NO_TOUCH,
    #ifdef SERCOM7
    SERCOM(7, 1),
    #else
    NO_SERCOM,
    #endif
    SERCOM(5, 0),
    TC(0, 1),
    TCC(4, 1),
    TCC(0, 7));
#endif
#ifdef PIN_PC30
PIN(PC30, EXTINT_CHANNEL(14), NO_ADC, ADC_INPUT(12), NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PC31
PIN(PC31, EXTINT_CHANNEL(15), NO_ADC, ADC_INPUT(13), NO_TOUCH,
    NO_SERCOM,
    NO_SERCOM,
    NO_TIMER,
    NO_TIMER,
    NO_TIMER);
#endif


#ifdef PIN_PB00
PIN(PB00, EXTINT_CHANNEL(0), ADC_INPUT(12), NO_ADC,
    #ifdef PINMUX_PB00B_ADC0_PTCXY30
    TOUCH(30),
    #else
    NO_TOUCH,
    #endif
    NO_SERCOM,
    SERCOM(5, 2),
    #ifdef TC7
    TC(7, 0),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB01
PIN(PB01, EXTINT_CHANNEL(1), ADC_INPUT(13), NO_ADC,
    #ifdef PINMUX_PB01B_ADC0_PTCXY31
    TOUCH(31),
    #else
    NO_TOUCH,
    #endif
    NO_SERCOM,
    SERCOM(5, 3),
    #ifdef TC7
    TC(7, 1),
    #else
    NO_TIMER,
    #endif
    NO_TIMER,
    NO_TIMER);
#endif
#ifdef PIN_PB02
PIN(PB02, EXTINT_CHANNEL(2), ADC_INPUT(14), NO_ADC, TOUCH(20),
    NO_SERCOM,
    SERCOM(5, 0),
    #ifdef TC6
    TC(6, 0),
    #else
    NO_TIMER,
    #endif
    TCC(2, 2),
    NO_TIMER);
#endif
