/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "boards/board.h"

#include "shared-bindings/displayio/FourWire.h"

#include "tick.h"

displayio_fourwire_obj_t board_display_obj;

uint8_t display_init_sequence[] = {
    0x01, 0, // SWRESET
    0x11, 0, // SLPOUT
    0xb1, 3, 0x01, 0x2C, 0x2D, // _FRMCTR1
    0xb2, 3, 0x01, 0x2C, 0x2D, //
    0xb3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,
    0xb4, 1, 0x07, // _INVCTR line inversion
    0xc0, 3, 0xa2, 0x02, 0x84, // _PWCTR1 GVDD = 4.7V, 1.0uA
    0xc1, 1, 0xc5, // _PWCTR2 VGH=14.7V, VGL=-7.35V
    0xc2, 2, 0x0a, 0x00, // _PWCTR3 Opamp current small, Boost frequency
    0xc3, 2, 0x8a, 0x2a,
    0xc4, 2, 0x8a, 0xee,
    0xc5, 1, 0x0e, // _VMCTR1 VCOMH = 4V, VOML = -1.1V
    0x2a, 0, // _INVOFF
    0x36, 1, 0x18, // _MADCTL bottom to top refresh
    // 1 clk cycle nonoverlap, 2 cycle gate rise, 3 sycle osc equalie,
    // fix on VTL
    0x3a, 1, 0x05, // COLMOD - 16bit color
    0xe0, 16, 0x02, 0x1c, 0x07, 0x12, // _GMCTRP1 Gamma
              0x37, 0x32, 0x29, 0x2d,
              0x29, 0x25, 0x2B, 0x39,
              0x00, 0x01, 0x03, 0x10,
    0xe1, 16, 0x03, 0x1d, 0x07, 0x06, // _GMCTRN1
              0x2E, 0x2C, 0x29, 0x2D,
              0x2E, 0x2E, 0x37, 0x3F,
              0x00, 0x00, 0x02, 0x10,
    0x2a, 3, 0x02, 0x00, 0x81, // _CASET XSTART = 2, XEND = 129
    0x2b, 3, 0x02, 0x00, 0x81, // _RASET XSTART = 2, XEND = 129
    0x13, 0, // _NORON
    0x29, 0, // _DISPON
};

void board_init(void) {
    board_display_obj.base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(&board_display_obj,
        &pin_PB23, &pin_PB22, &pin_PA28, &pin_PA01, &pin_PA27,
        128, 128, 2, 0, 16, 0x2a, 0x2b, 0x2c);

    uint32_t i = 0;
    common_hal_displayio_fourwire_begin_transaction(&board_display_obj);
    while (i < sizeof(display_init_sequence)) {
        uint8_t *cmd = display_init_sequence + i;
        uint8_t data_size = *(cmd + 1);
        uint8_t *data = cmd + 2;
        common_hal_displayio_fourwire_send(&board_display_obj, true, cmd, 1);
        common_hal_displayio_fourwire_send(&board_display_obj, false, data, data_size);
        if (*cmd == 0x01) {
            uint64_t start = ticks_ms;
            while (ticks_ms - start < 120) {}
        } else if (*cmd == 0x11) {
            uint64_t start = ticks_ms;
            while (ticks_ms - start < 500) {}
        } else {
            uint64_t start = ticks_ms;
            while (ticks_ms - start < 10) {}
        }
        i += 2 + data_size;

    }
    common_hal_displayio_fourwire_end_transaction(&board_display_obj);
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
    common_hal_displayio_fourwire_show(&board_display_obj, NULL);
}
