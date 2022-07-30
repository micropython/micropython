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

#include "supervisor/board.h"
#include "background.h"
#include "mpconfigboard.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#include "shared-bindings/sharpdisplay/SharpMemoryFramebuffer.h"
#include "shared-module/displayio/__init__.h"

digitalio_digitalinout_obj_t extcomin;
digitalio_digitalinout_obj_t display_on;

uint32_t last_down_ticks_ms;

void board_init(void) {
    common_hal_digitalio_digitalinout_construct(&extcomin, &pin_P0_06);
    common_hal_digitalio_digitalinout_switch_to_output(&extcomin, true, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_never_reset(&extcomin);

    common_hal_digitalio_digitalinout_construct(&display_on, &pin_P0_07);
    common_hal_digitalio_digitalinout_switch_to_output(&display_on, true, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_never_reset(&display_on);

    sharpdisplay_framebuffer_obj_t *fb = &allocate_display_bus()->sharpdisplay;
    fb->base.type = &sharpdisplay_framebuffer_type;

    busio_spi_obj_t *spi = &fb->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_P0_26, &pin_P0_27, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    common_hal_sharpdisplay_framebuffer_construct(fb, spi, &pin_P0_05, 500000, 176, 176, true);

    primary_display_t *display = allocate_display();
    framebufferio_framebufferdisplay_obj_t *self = &display->framebuffer_display;
    self->base.type = &framebufferio_framebufferdisplay_type;
    common_hal_framebufferio_framebufferdisplay_construct(self, fb, 0, true);
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
    nrf_gpio_cfg_input(17, NRF_GPIO_PIN_PULLUP);
}

void board_deinit(void) {
    // common_hal_displayio_release_displays();
}

void board_background_task(void) {
    if (!nrf_gpio_pin_read(17)) {
        if (last_down_ticks_ms == 0) {
            last_down_ticks_ms = supervisor_ticks_ms32();
        }
    } else {
        last_down_ticks_ms = 0;
    }
    // If the button isn't pressed, then feed the watchdog.
    if (last_down_ticks_ms == 0) {
        NRF_WDT->RR[0] = 0x6E524635;
        return;
    }
    // if the button has been pressed less than 5 seconds, then feed the watchdog.
    uint32_t now = supervisor_ticks_ms32();
    if (now - last_down_ticks_ms < 5000) {
        NRF_WDT->RR[0] = 0x6E524635;
    }
    // Don't feed the watchdog so that it'll expire and kick us to the bootloader.
}
