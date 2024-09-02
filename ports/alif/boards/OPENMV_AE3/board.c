/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include "py/mphal.h"
#include "ospi_flash.h"

#if 0
// TODO: currently does not work, need to wait for next hardware revision
// ISSI IS25WP256D octal flash.
const ospi_flash_settings_t ospi_flash_settings = {
    .jedec_id = 0x195b9d, // manuf=0x9d, type=0x5b=1.8V, density=0x19=256Mb
    .freq_mhz = 80000000,
    .is_quad = true,
    .is_oct = false,
    .is_ddr = true,
    .read_id_dummy_cycles = 0,
    .read_dummy_cycles = 16,
    .read_command = 0xfd, // octal DDR read
    .write_command = 0xc2, // octal DDR write
};
#endif

void board_startup(void) {
    // Switch the USB multiplexer to use the Alif USB port.
    mp_hal_pin_output(pin_USB_MUX);
    mp_hal_pin_high(pin_USB_MUX);
}
