
/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Mark Olsson <mark@markolsson.se>
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

#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

STATIC const mp_rom_map_elem_t mcu_pin_globals_table[] = {
// Row A
    { MP_ROM_QSTR(MP_QSTR_PE04), MP_ROM_PTR(&pin_PE04) },
    { MP_ROM_QSTR(MP_QSTR_PE03), MP_ROM_PTR(&pin_PE03) },
    { MP_ROM_QSTR(MP_QSTR_PE02), MP_ROM_PTR(&pin_PE02) },
    { MP_ROM_QSTR(MP_QSTR_PG14), MP_ROM_PTR(&pin_PG14) },
    { MP_ROM_QSTR(MP_QSTR_PE01), MP_ROM_PTR(&pin_PE01) },
    { MP_ROM_QSTR(MP_QSTR_PE00), MP_ROM_PTR(&pin_PE00) },
    { MP_ROM_QSTR(MP_QSTR_PB08), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_PB05), MP_ROM_PTR(&pin_PB05) },
    { MP_ROM_QSTR(MP_QSTR_PB04), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_PB03), MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_PD07), MP_ROM_PTR(&pin_PD07) },
    { MP_ROM_QSTR(MP_QSTR_PC12), MP_ROM_PTR(&pin_PC12) },
    { MP_ROM_QSTR(MP_QSTR_PA15), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_PA14), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_PA13), MP_ROM_PTR(&pin_PA13) },

// Row B
    { MP_ROM_QSTR(MP_QSTR_PE05), MP_ROM_PTR(&pin_PE05) },
    { MP_ROM_QSTR(MP_QSTR_PE06), MP_ROM_PTR(&pin_PE06) },
    { MP_ROM_QSTR(MP_QSTR_PG13), MP_ROM_PTR(&pin_PG13) },
    { MP_ROM_QSTR(MP_QSTR_PB09), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_PB07), MP_ROM_PTR(&pin_PB07) },
    { MP_ROM_QSTR(MP_QSTR_PB06), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_PG15), MP_ROM_PTR(&pin_PG15) },
    { MP_ROM_QSTR(MP_QSTR_PG11), MP_ROM_PTR(&pin_PG11) },
    { MP_ROM_QSTR(MP_QSTR_PJ13), MP_ROM_PTR(&pin_PJ13) },
    { MP_ROM_QSTR(MP_QSTR_PJ12), MP_ROM_PTR(&pin_PJ12) },
    { MP_ROM_QSTR(MP_QSTR_PD06), MP_ROM_PTR(&pin_PD06) },
    { MP_ROM_QSTR(MP_QSTR_PD00), MP_ROM_PTR(&pin_PD00) },
    { MP_ROM_QSTR(MP_QSTR_PC11), MP_ROM_PTR(&pin_PC11) },
    { MP_ROM_QSTR(MP_QSTR_PC10), MP_ROM_PTR(&pin_PC10) },
    { MP_ROM_QSTR(MP_QSTR_PA12), MP_ROM_PTR(&pin_PA12) },

// Row C
    { MP_ROM_QSTR(MP_QSTR_PI08), MP_ROM_PTR(&pin_PI08) },
    { MP_ROM_QSTR(MP_QSTR_PI04), MP_ROM_PTR(&pin_PI04) },
    { MP_ROM_QSTR(MP_QSTR_PK07), MP_ROM_PTR(&pin_PK07) },
    { MP_ROM_QSTR(MP_QSTR_PK06), MP_ROM_PTR(&pin_PK06) },
    { MP_ROM_QSTR(MP_QSTR_PK05), MP_ROM_PTR(&pin_PK05) },
    { MP_ROM_QSTR(MP_QSTR_PG12), MP_ROM_PTR(&pin_PG12) },
    { MP_ROM_QSTR(MP_QSTR_PG10), MP_ROM_PTR(&pin_PG10) },
    { MP_ROM_QSTR(MP_QSTR_PJ14), MP_ROM_PTR(&pin_PJ14) },
    { MP_ROM_QSTR(MP_QSTR_PD05), MP_ROM_PTR(&pin_PD05) },
    { MP_ROM_QSTR(MP_QSTR_PD03), MP_ROM_PTR(&pin_PD03) },
    { MP_ROM_QSTR(MP_QSTR_PD01), MP_ROM_PTR(&pin_PD01) },
    { MP_ROM_QSTR(MP_QSTR_PI03), MP_ROM_PTR(&pin_PI03) },
    { MP_ROM_QSTR(MP_QSTR_PI02), MP_ROM_PTR(&pin_PI02) },
    { MP_ROM_QSTR(MP_QSTR_PA11), MP_ROM_PTR(&pin_PA11) },

// Row D
    { MP_ROM_QSTR(MP_QSTR_PC13), MP_ROM_PTR(&pin_PC13) },
    { MP_ROM_QSTR(MP_QSTR_PF00), MP_ROM_PTR(&pin_PF00) },
    { MP_ROM_QSTR(MP_QSTR_PI05), MP_ROM_PTR(&pin_PI05) },
    { MP_ROM_QSTR(MP_QSTR_PI07), MP_ROM_PTR(&pin_PI07) },
    { MP_ROM_QSTR(MP_QSTR_PI10), MP_ROM_PTR(&pin_PI10) },
    { MP_ROM_QSTR(MP_QSTR_PI06), MP_ROM_PTR(&pin_PI06) },
    { MP_ROM_QSTR(MP_QSTR_PK04), MP_ROM_PTR(&pin_PK04) },
    { MP_ROM_QSTR(MP_QSTR_PK03), MP_ROM_PTR(&pin_PK03) },
    { MP_ROM_QSTR(MP_QSTR_PG09), MP_ROM_PTR(&pin_PG09) },
    { MP_ROM_QSTR(MP_QSTR_PJ15), MP_ROM_PTR(&pin_PJ15) },
    { MP_ROM_QSTR(MP_QSTR_PD04), MP_ROM_PTR(&pin_PD04) },
    { MP_ROM_QSTR(MP_QSTR_PD02), MP_ROM_PTR(&pin_PD02) },
    { MP_ROM_QSTR(MP_QSTR_PH15), MP_ROM_PTR(&pin_PH15) },
    { MP_ROM_QSTR(MP_QSTR_PI01), MP_ROM_PTR(&pin_PI01) },
    { MP_ROM_QSTR(MP_QSTR_PA10), MP_ROM_PTR(&pin_PA10) },

// Row E
    // { MP_ROM_QSTR(MP_QSTR_PC14), MP_ROM_PTR(&pin_PC14) }, // OSC32_IN
    { MP_ROM_QSTR(MP_QSTR_PF01), MP_ROM_PTR(&pin_PF01) },
    { MP_ROM_QSTR(MP_QSTR_PI12), MP_ROM_PTR(&pin_PI12) },
    { MP_ROM_QSTR(MP_QSTR_PI09), MP_ROM_PTR(&pin_PI09) },
    { MP_ROM_QSTR(MP_QSTR_PH13), MP_ROM_PTR(&pin_PH13) },
    { MP_ROM_QSTR(MP_QSTR_PH14), MP_ROM_PTR(&pin_PH14) },
    { MP_ROM_QSTR(MP_QSTR_PI00), MP_ROM_PTR(&pin_PI00) },
    { MP_ROM_QSTR(MP_QSTR_PA09), MP_ROM_PTR(&pin_PA09) },

// Row F
    // { MP_ROM_QSTR(MP_QSTR_PC15), MP_ROM_PTR(&pin_PC15) }, // OSC32_OUT
    { MP_ROM_QSTR(MP_QSTR_PK01), MP_ROM_PTR(&pin_PK01) },
    { MP_ROM_QSTR(MP_QSTR_PK02), MP_ROM_PTR(&pin_PK02) },
    { MP_ROM_QSTR(MP_QSTR_PC09), MP_ROM_PTR(&pin_PC09) },
    { MP_ROM_QSTR(MP_QSTR_PA08), MP_ROM_PTR(&pin_PA08) },

// Row G
    // { MP_ROM_QSTR(MP_QSTR_PH00), MP_ROM_PTR(&pin_PH00) }, // OSC_IN
    { MP_ROM_QSTR(MP_QSTR_PF02), MP_ROM_PTR(&pin_PF02) },
    { MP_ROM_QSTR(MP_QSTR_PI13), MP_ROM_PTR(&pin_PI13) },
    { MP_ROM_QSTR(MP_QSTR_PI15), MP_ROM_PTR(&pin_PI15) },
    { MP_ROM_QSTR(MP_QSTR_PJ11), MP_ROM_PTR(&pin_PJ11) },
    { MP_ROM_QSTR(MP_QSTR_PK00), MP_ROM_PTR(&pin_PK00) },
    { MP_ROM_QSTR(MP_QSTR_PC08), MP_ROM_PTR(&pin_PC08) },
    { MP_ROM_QSTR(MP_QSTR_PC07), MP_ROM_PTR(&pin_PC07) },

// Row H
    // { MP_ROM_QSTR(MP_QSTR_PH01), MP_ROM_PTR(&pin_PH01) }, // OSC_OUT
    { MP_ROM_QSTR(MP_QSTR_PF03), MP_ROM_PTR(&pin_PF03) },
    { MP_ROM_QSTR(MP_QSTR_PI14), MP_ROM_PTR(&pin_PI14) },
    { MP_ROM_QSTR(MP_QSTR_PH04), MP_ROM_PTR(&pin_PH04) },
    { MP_ROM_QSTR(MP_QSTR_PJ08), MP_ROM_PTR(&pin_PJ08) },
    { MP_ROM_QSTR(MP_QSTR_PJ10), MP_ROM_PTR(&pin_PJ10) },
    { MP_ROM_QSTR(MP_QSTR_PG08), MP_ROM_PTR(&pin_PG08) },
    { MP_ROM_QSTR(MP_QSTR_PC06), MP_ROM_PTR(&pin_PC06) },

// Row J
    { MP_ROM_QSTR(MP_QSTR_PF04), MP_ROM_PTR(&pin_PF04) },
    { MP_ROM_QSTR(MP_QSTR_PH05), MP_ROM_PTR(&pin_PH05) },
    { MP_ROM_QSTR(MP_QSTR_PH03), MP_ROM_PTR(&pin_PH03) },
    { MP_ROM_QSTR(MP_QSTR_PJ07), MP_ROM_PTR(&pin_PJ07) },
    { MP_ROM_QSTR(MP_QSTR_PJ09), MP_ROM_PTR(&pin_PJ09) },
    { MP_ROM_QSTR(MP_QSTR_PG07), MP_ROM_PTR(&pin_PG07) },
    { MP_ROM_QSTR(MP_QSTR_PG06), MP_ROM_PTR(&pin_PG06) },

// Row K
    { MP_ROM_QSTR(MP_QSTR_PF07), MP_ROM_PTR(&pin_PF07) },
    { MP_ROM_QSTR(MP_QSTR_PF06), MP_ROM_PTR(&pin_PF06) },
    { MP_ROM_QSTR(MP_QSTR_PF05), MP_ROM_PTR(&pin_PF05) },
    { MP_ROM_QSTR(MP_QSTR_PH02), MP_ROM_PTR(&pin_PH02) },
    { MP_ROM_QSTR(MP_QSTR_PJ06), MP_ROM_PTR(&pin_PJ06) },
    { MP_ROM_QSTR(MP_QSTR_PD15), MP_ROM_PTR(&pin_PD15) },
    { MP_ROM_QSTR(MP_QSTR_PB13), MP_ROM_PTR(&pin_PB13) },
    { MP_ROM_QSTR(MP_QSTR_PD10), MP_ROM_PTR(&pin_PD10) },

// Row L
    { MP_ROM_QSTR(MP_QSTR_PF10), MP_ROM_PTR(&pin_PF10) },
    { MP_ROM_QSTR(MP_QSTR_PF09), MP_ROM_PTR(&pin_PF09) },
    { MP_ROM_QSTR(MP_QSTR_PF08), MP_ROM_PTR(&pin_PF08) },
    { MP_ROM_QSTR(MP_QSTR_PC03), MP_ROM_PTR(&pin_PC03) },
    { MP_ROM_QSTR(MP_QSTR_PD14), MP_ROM_PTR(&pin_PD14) },
    { MP_ROM_QSTR(MP_QSTR_PB12), MP_ROM_PTR(&pin_PB12) },
    { MP_ROM_QSTR(MP_QSTR_PD09), MP_ROM_PTR(&pin_PD09) },
    { MP_ROM_QSTR(MP_QSTR_PD08), MP_ROM_PTR(&pin_PD08) },

// Row M
    { MP_ROM_QSTR(MP_QSTR_PC00), MP_ROM_PTR(&pin_PC00) },
    { MP_ROM_QSTR(MP_QSTR_PC01), MP_ROM_PTR(&pin_PC01) },
    { MP_ROM_QSTR(MP_QSTR_PC02), MP_ROM_PTR(&pin_PC02) },
    { MP_ROM_QSTR(MP_QSTR_PB02), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_PF12), MP_ROM_PTR(&pin_PF12) },
    { MP_ROM_QSTR(MP_QSTR_PG01), MP_ROM_PTR(&pin_PG01) },
    { MP_ROM_QSTR(MP_QSTR_PF15), MP_ROM_PTR(&pin_PF15) },
    { MP_ROM_QSTR(MP_QSTR_PJ04), MP_ROM_PTR(&pin_PJ04) },
    { MP_ROM_QSTR(MP_QSTR_PD12), MP_ROM_PTR(&pin_PD12) },
    { MP_ROM_QSTR(MP_QSTR_PD13), MP_ROM_PTR(&pin_PD13) },
    { MP_ROM_QSTR(MP_QSTR_PG03), MP_ROM_PTR(&pin_PG03) },
    { MP_ROM_QSTR(MP_QSTR_PG02), MP_ROM_PTR(&pin_PG02) },
    { MP_ROM_QSTR(MP_QSTR_PJ05), MP_ROM_PTR(&pin_PJ05) },
    { MP_ROM_QSTR(MP_QSTR_PH12), MP_ROM_PTR(&pin_PH12) },

// Row N
    { MP_ROM_QSTR(MP_QSTR_PA01), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_PA00), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_PA04), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_PC04), MP_ROM_PTR(&pin_PC04) },
    { MP_ROM_QSTR(MP_QSTR_PF13), MP_ROM_PTR(&pin_PF13) },
    { MP_ROM_QSTR(MP_QSTR_PG00), MP_ROM_PTR(&pin_PG00) },
    { MP_ROM_QSTR(MP_QSTR_PJ03), MP_ROM_PTR(&pin_PJ03) },
    { MP_ROM_QSTR(MP_QSTR_PE08), MP_ROM_PTR(&pin_PE08) },
    { MP_ROM_QSTR(MP_QSTR_PD11), MP_ROM_PTR(&pin_PD11) },
    { MP_ROM_QSTR(MP_QSTR_PG05), MP_ROM_PTR(&pin_PG05) },
    { MP_ROM_QSTR(MP_QSTR_PG04), MP_ROM_PTR(&pin_PG04) },
    { MP_ROM_QSTR(MP_QSTR_PH07), MP_ROM_PTR(&pin_PH07) },
    { MP_ROM_QSTR(MP_QSTR_PH09), MP_ROM_PTR(&pin_PH09) },
    { MP_ROM_QSTR(MP_QSTR_PH11), MP_ROM_PTR(&pin_PH11) },

// Row P
    { MP_ROM_QSTR(MP_QSTR_PA02), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_PA06), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_PA05), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_PC05), MP_ROM_PTR(&pin_PC05) },
    { MP_ROM_QSTR(MP_QSTR_PF14), MP_ROM_PTR(&pin_PF14) },
    { MP_ROM_QSTR(MP_QSTR_PJ02), MP_ROM_PTR(&pin_PJ02) },
    { MP_ROM_QSTR(MP_QSTR_PF11), MP_ROM_PTR(&pin_PF11) },
    { MP_ROM_QSTR(MP_QSTR_PE09), MP_ROM_PTR(&pin_PE09) },
    { MP_ROM_QSTR(MP_QSTR_PE11), MP_ROM_PTR(&pin_PE11) },
    { MP_ROM_QSTR(MP_QSTR_PE14), MP_ROM_PTR(&pin_PE14) },
    { MP_ROM_QSTR(MP_QSTR_PB10), MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_PH06), MP_ROM_PTR(&pin_PH06) },
    { MP_ROM_QSTR(MP_QSTR_PH08), MP_ROM_PTR(&pin_PH08) },
    { MP_ROM_QSTR(MP_QSTR_PH10), MP_ROM_PTR(&pin_PH10) },

// Row R
    { MP_ROM_QSTR(MP_QSTR_PA03), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_PA07), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_PB01), MP_ROM_PTR(&pin_PB01) },
    { MP_ROM_QSTR(MP_QSTR_PB00), MP_ROM_PTR(&pin_PB00) },
    { MP_ROM_QSTR(MP_QSTR_PJ00), MP_ROM_PTR(&pin_PJ00) },
    { MP_ROM_QSTR(MP_QSTR_PJ01), MP_ROM_PTR(&pin_PJ01) },
    { MP_ROM_QSTR(MP_QSTR_PE07), MP_ROM_PTR(&pin_PE07) },
    { MP_ROM_QSTR(MP_QSTR_PE10), MP_ROM_PTR(&pin_PE10) },
    { MP_ROM_QSTR(MP_QSTR_PE12), MP_ROM_PTR(&pin_PE12) },
    { MP_ROM_QSTR(MP_QSTR_PE15), MP_ROM_PTR(&pin_PE15) },
    { MP_ROM_QSTR(MP_QSTR_PE13), MP_ROM_PTR(&pin_PE13) },
    { MP_ROM_QSTR(MP_QSTR_PB11), MP_ROM_PTR(&pin_PB11) },
    { MP_ROM_QSTR(MP_QSTR_PB14), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_PB15), MP_ROM_PTR(&pin_PB15) },

};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_globals_table);
