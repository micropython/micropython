
#include <string.h>
#include "shared-module/displayio/__init__.h"

#include "lib/utils/interrupt_char.h"
#include "py/reload.h"
#include "py/runtime.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/Display.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/displayio/Palette.h"
#include "supervisor/shared/autoreload.h"
#include "supervisor/shared/display.h"
#include "supervisor/memory.h"
#include "supervisor/usb.h"

primary_display_t displays[CIRCUITPY_DISPLAY_LIMIT];

static inline void swap(uint16_t* a, uint16_t* b) {
    uint16_t temp = *a;
    *a = *b;
    *b = temp;
}

bool refreshing_displays = false;

void displayio_refresh_displays(void) {
    if (mp_hal_is_interrupted()) {
        return;
    }
    // Somehow reloads from the sdcard are being lost. So, cheat and reraise.
    // But don't re-raise if already pending.
    if (reload_requested && MP_STATE_VM(mp_pending_exception) == MP_OBJ_NULL) {
        mp_raise_reload_exception();
        return;
    }

    if (refreshing_displays) {
        return;
    }
    refreshing_displays = true;
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].display.base.type == NULL || displays[i].display.base.type == &mp_type_NoneType) {
            continue;
        }
        displayio_display_obj_t* display = &displays[i].display;
        displayio_display_update_backlight(display);

        if (!displayio_display_frame_queued(display)) {
            refreshing_displays = false;
            return;
        }
        if (displayio_display_refresh_queued(display)) {
            // We compute the pixels. r and c are row and column to match the display memory
            // structure. x and y match location within the groups.
            uint16_t c0 = 0;
            uint16_t r0 = 0;
            uint16_t c1 = display->width;
            uint16_t r1 = display->height;
            if (display->transpose_xy) {
                swap(&c1, &r1);
            }
            // Someone else is holding the bus used to talk to the display,
            // so skip update for now.
            if (!displayio_display_start_region_update(display, c0, r0, c1, r1)) {
                return;
            }

            uint16_t x0 = 0;
            uint16_t x1 = display->width - 1;
            uint16_t startx = 0;
            int8_t dx = 1;
            if (display->mirror_x) {
                dx = -1;
                startx = x1;
            }
            uint16_t y0 = 0;
            uint16_t y1 = display->height - 1;
            uint16_t starty = 0;
            int8_t dy = 1;
            if (display->mirror_y) {
                dy = -1;
                starty = y1;
            }

            bool transpose = false;
            if (display->transpose_xy) {
                transpose = true;
                int8_t temp_dx = dx;
                dx = dy;
                dy = temp_dx;

                swap(&starty, &startx);
                swap(&x0, &y0);
                swap(&x1, &y1);
            }

            size_t index = 0;
            uint16_t buffer_size = 256;
            uint32_t buffer[buffer_size / 2];
            for (uint16_t y = starty; y0 <= y && y <= y1; y += dy) {
                for (uint16_t x = startx; x0 <= x && x <= x1; x += dx) {
                    uint16_t* pixel = &(((uint16_t*)buffer)[index]);
                    *pixel = 0;

                    if (display->current_group != NULL) {
                        if (transpose) {
                            displayio_group_get_pixel(display->current_group, y, x, pixel);
                        } else {
                            displayio_group_get_pixel(display->current_group, x, y, pixel);
                        }
                    }

                    index += 1;
                    // The buffer is full, send it.
                    if (index >= buffer_size) {
                        if (!displayio_display_send_pixels(display, buffer, buffer_size / 2) || reload_requested) {
                            displayio_display_finish_region_update(display);
                            refreshing_displays = false;
                            return;
                        }
                        // TODO(tannewt): Make refresh displays faster so we don't starve other
                        // background tasks.
                        usb_background();
                        index = 0;
                    }
                }
            }
            // Send the remaining data.
            if (index && !displayio_display_send_pixels(display, buffer, index * 2)) {
                displayio_display_finish_region_update(display);
                refreshing_displays = false;
                return;
            }
            displayio_display_finish_region_update(display);
        }
        displayio_display_finish_refresh(display);
    }
    refreshing_displays = false;
}

void common_hal_displayio_release_displays(void) {
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        mp_const_obj_t bus_type = displays[i].fourwire_bus.base.type;
        if (bus_type == NULL) {
            continue;
        } else if (bus_type == &displayio_fourwire_type) {
            common_hal_displayio_fourwire_deinit(&displays[i].fourwire_bus);
        } else if (bus_type == &displayio_parallelbus_type) {
            common_hal_displayio_parallelbus_deinit(&displays[i].parallel_bus);
        }
        displays[i].fourwire_bus.base.type = &mp_type_NoneType;
    }
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        release_display(&displays[i].display);
        displays[i].display.base.type = &mp_type_NoneType;
    }

    supervisor_stop_terminal();
}

void reset_displays(void) {
    #if CIRCUITPY_DISPLAYIO
    // The SPI buses used by FourWires may be allocated on the heap so we need to move them inline.
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].fourwire_bus.base.type != &displayio_fourwire_type) {
            continue;
        }
        displayio_fourwire_obj_t* fourwire = &displays[i].fourwire_bus;
        if (((uint32_t) fourwire->bus) < ((uint32_t) &displays) ||
            ((uint32_t) fourwire->bus) > ((uint32_t) &displays + CIRCUITPY_DISPLAY_LIMIT)) {
            busio_spi_obj_t* original_spi = fourwire->bus;
            if (original_spi == board_spi()) {
                continue;
            }
            memcpy(&fourwire->inline_bus, original_spi, sizeof(busio_spi_obj_t));
            fourwire->bus = &fourwire->inline_bus;
            // Check for other displays that use the same spi bus and swap them too.
            for (uint8_t j = i + 1; j < CIRCUITPY_DISPLAY_LIMIT; j++) {
                if (displays[i].fourwire_bus.bus == original_spi) {
                    displays[i].fourwire_bus.bus = &fourwire->inline_bus;
                }
            }
        }
    }

    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].display.base.type == NULL) {
            continue;
        }
        displayio_display_obj_t* display = &displays[i].display;
        display->auto_brightness = true;
        common_hal_displayio_display_show(display, &circuitpython_splash);
    }
    #endif
}
