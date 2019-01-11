/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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
#include "mpconfigboard.h"
#include "hal/include/hal_gpio.h"

#include "shared-bindings/displayio/FourWire.h"
#include "shared-module/displayio/mipi_constants.h"

#include "tick.h"

displayio_fourwire_obj_t board_display_obj;

#define DELAY 0x80

uint8_t display_init_sequence[] = {
    0xEF, 3, 0x03, 0x80, 0x02,
    0xCF, 3, 0x00, 0xC1, 0x30,
    0xED, 4, 0x64, 0x03, 0x12, 0x81,
    0xE8, 3, 0x85, 0x00, 0x78,
    0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    0xF7, 1, 0x20,
    0xEA, 2, 0x00, 0x00,
    0xc0, 1, 0x23,             // Power control VRH[5:0]
    0xc1, 1, 0x10,             // Power control SAP[2:0];BT[3:0]
    0xc5, 2, 0x3e, 0x28,       // VCM control
    0xc7, 1, 0x86,             // VCM control2
    0x36, 1, 0x38,             // Memory Access Control
    0x37, 1, 0x00,             // Vertical scroll zero
    0x3a, 1, 0x55,             // COLMOD: Pixel Format Set
    0xb1, 2, 0x00, 0x18,       // Frame Rate Control (In Normal Mode/Full Colors)
    0xb6, 3, 0x08, 0x82, 0x27, // Display Function Control
    0xF2, 1, 0x00,                         // 3Gamma Function Disable
    0x26, 1, 0x01,             // Gamma curve selected
    0xe0, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
      0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    0xe1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
      0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
    0x11, DELAY, 120,                // Exit Sleep
    0x29, DELAY, 120, // Display on
};


void board_init(void) {
    board_display_obj.base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(&board_display_obj,
        &pin_PA13, // Clock
        &pin_PA12, // Data
        &pin_PB09, // Command or data
        &pin_PB06, // Chip select
        &pin_PA00, // Reset
        320, // Width
        240, // Height
        0, // column start
        0, // row start
        16, // Color depth
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START); // Write memory command

    uint32_t i = 0;
    common_hal_displayio_fourwire_begin_transaction(&board_display_obj);
    while (i < sizeof(display_init_sequence)) {
        uint8_t *cmd = display_init_sequence + i;
        uint8_t data_size = *(cmd + 1);
        bool delay = (data_size & DELAY) != 0;
        data_size &= ~DELAY;
        uint8_t *data = cmd + 2;
        common_hal_displayio_fourwire_send(&board_display_obj, true, cmd, 1);
        common_hal_displayio_fourwire_send(&board_display_obj, false, data, data_size);
        if (delay) {
            data_size++;
            uint16_t delay_length_ms = *(cmd + 1 + data_size);
            if (delay_length_ms == 255) {
                delay_length_ms = 500;
            }
            uint64_t start = ticks_ms;
            while (ticks_ms - start < delay_length_ms) {}
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
