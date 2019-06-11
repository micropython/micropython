
#include <string.h>
#include "shared-module/displayio/__init__.h"

#include "lib/utils/interrupt_char.h"
#include "py/gc.h"
#include "py/reload.h"
#include "py/runtime.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/Display.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-module/displayio/area.h"
#include "supervisor/shared/autoreload.h"
#include "supervisor/shared/display.h"
#include "supervisor/memory.h"
#include "supervisor/usb.h"

primary_display_t displays[CIRCUITPY_DISPLAY_LIMIT];

static inline void swap(int16_t* a, int16_t* b) {
    int16_t temp = *a;
    *a = *b;
    *b = temp;
}

// Check for recursive calls to displayio_refresh_displays.
bool refresh_displays_in_progress = false;

void displayio_refresh_displays(void) {
    if (mp_hal_is_interrupted()) {
        return;
    }
    if (reload_requested) {
        // Reload is about to happen, so don't redisplay.
        return;
    }

    if (refresh_displays_in_progress) {
        // Don't allow recursive calls to this routine.
        return;
    }

    refresh_displays_in_progress = true;

    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].display.base.type == NULL || displays[i].display.base.type == &mp_type_NoneType) {
            // Skip null display.
            continue;
        }
        displayio_display_obj_t* display = &displays[i].display;
        displayio_display_update_backlight(display);

        // Time to refresh at specified frame rate?
        if (!displayio_display_frame_queued(display)) {
            // Too soon. Try next display.
            continue;
        }
        if (displayio_display_refresh_queued(display)) {
            if (!displayio_display_begin_transaction(display)) {
                // Can't acquire display bus; skip updating this display. Try next display.
                continue;
            }
            displayio_display_end_transaction(display);

            displayio_area_t whole_screen = {
                .x1 = 0,
                .y1 = 0,
                .x2 = display->width,
                .y2 = display->height
            };
            if (display->transpose_xy) {
                swap(&whole_screen.x2, &whole_screen.y2);
            }

            uint16_t buffer_size = 512;

            uint16_t subrectangles = 1;
            uint16_t rows_per_buffer = displayio_area_height(&whole_screen);
            if (displayio_area_size(&whole_screen) > buffer_size) {
                rows_per_buffer = buffer_size / displayio_area_width(&whole_screen);
                subrectangles = displayio_area_height(&whole_screen) / rows_per_buffer;
                buffer_size = rows_per_buffer * displayio_area_width(&whole_screen);
            }
            uint32_t buffer[buffer_size / 2];

            for (uint16_t j = 0; j < subrectangles; j++) {
                displayio_area_t subrectangle = {
                    .x1 = 0,
                    .y1 = rows_per_buffer * j,
                    .x2 = displayio_area_width(&whole_screen),
                    .y2 = rows_per_buffer * (j + 1)
                };

                displayio_display_begin_transaction(display);
                displayio_display_set_region_to_update(display, subrectangle.x1, subrectangle.y1,
                                                                subrectangle.x2, subrectangle.y2);
                displayio_display_end_transaction(display);

                // Handle display mirroring and transpose.
                displayio_area_t transformed_subrectangle;
                displayio_buffer_transform_t transform;
                if (display->mirror_x) {
                    uint16_t width = displayio_area_width(&whole_screen);
                    transformed_subrectangle.x1 = width - subrectangle.x2;
                    transformed_subrectangle.x2 = width - subrectangle.x1;
                } else {
                    transformed_subrectangle.x1 = subrectangle.x1;
                    transformed_subrectangle.x2 = subrectangle.x2;
                }
                if (display->mirror_y != display->transpose_xy) {
                    uint16_t height = displayio_area_height(&whole_screen);
                    transformed_subrectangle.y1 = height - subrectangle.y2;
                    transformed_subrectangle.y2 = height - subrectangle.y1;
                } else {
                    transformed_subrectangle.y1 = subrectangle.y1;
                    transformed_subrectangle.y2 = subrectangle.y2;
                }
                transform.width = transformed_subrectangle.x2 - transformed_subrectangle.x1;
                transform.height = transformed_subrectangle.y2 - transformed_subrectangle.y1;
                if (display->transpose_xy) {
                    int16_t y1 = transformed_subrectangle.y1;
                    int16_t y2 = transformed_subrectangle.y2;
                    transformed_subrectangle.y1 = transformed_subrectangle.x1;
                    transformed_subrectangle.y2 = transformed_subrectangle.x2;
                    transformed_subrectangle.x1 = y1;
                    transformed_subrectangle.x2 = y2;
                }
                transform.transpose_xy = display->transpose_xy;
                transform.mirror_x = display->mirror_x;
                transform.mirror_y = display->mirror_y;
                transform.scale = 1;

                uint32_t mask[(buffer_size / 32) + 1];
                for (uint16_t k = 0; k < (buffer_size / 32) + 1; k++) {
                    mask[k] = 0x00000000;
                }
                bool full_coverage = displayio_group_get_area(display->current_group, &transform, &transformed_subrectangle, mask, buffer);
                if (!full_coverage) {
                    uint32_t index = 0;
                    uint32_t current_mask = 0;
                    for (int16_t y = subrectangle.y1; y < subrectangle.y2; y++) {
                        for (int16_t x = subrectangle.x1; x < subrectangle.x2; x++) {
                            if (index % 32 == 0) {
                                current_mask = mask[index / 32];
                            }
                            if ((current_mask & (1 << (index % 32))) == 0) {
                                ((uint16_t*) buffer)[index] = 0x0000;
                            }
                            index++;
                        }
                    }
                }

                if (!displayio_display_begin_transaction(display)) {
                    // Can't acquire display bus; skip the rest of the data. Try next display.
                    break;
                }
                displayio_display_send_pixels(display, buffer, buffer_size / 2);
                displayio_display_end_transaction(display);

                // TODO(tannewt): Make refresh displays faster so we don't starve other
                // background tasks.
                usb_background();
            }
        }
        displayio_display_finish_refresh(display);
    }

    // All done.
    refresh_displays_in_progress = false;
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
    // The SPI buses used by FourWires may be allocated on the heap so we need to move them inline.
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].fourwire_bus.base.type != &displayio_fourwire_type) {
            continue;
        }
        displayio_fourwire_obj_t* fourwire = &displays[i].fourwire_bus;
        if (((uint32_t) fourwire->bus) < ((uint32_t) &displays) ||
            ((uint32_t) fourwire->bus) > ((uint32_t) &displays + CIRCUITPY_DISPLAY_LIMIT)) {
            busio_spi_obj_t* original_spi = fourwire->bus;
            #if BOARD_SPI
                // We don't need to move original_spi if it is the board.SPI object because it is
                // statically allocated already. (Doing so would also make it impossible to reference in
                // a subsequent VM run.)
                if (original_spi == common_hal_board_get_spi()) {
                    continue;
                }
            #endif
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
}

void displayio_gc_collect(void) {
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].display.base.type == NULL) {
            continue;
        }

        // Alternatively, we could use gc_collect_root over the whole object,
        // but this is more precise, and is the only field that needs marking.
        gc_collect_ptr(displays[i].display.current_group);

    }
}

void displayio_area_shift(displayio_area_t* area, int16_t dx, int16_t dy) {
    area->x1 += dx;
    area->y1 += dy;
    area->x2 += dx;
    area->y2 += dy;
}

bool displayio_area_compute_overlap(const displayio_area_t* a,
                                    const displayio_area_t* b,
                                    displayio_area_t* overlap) {
    overlap->x1 = a->x1;
    if (b->x1 > overlap->x1) {
        overlap->x1 = b->x1;
    }
    overlap->x2 = a->x2;
    if (b->x2 < overlap->x2) {
        overlap->x2 = b->x2;
    }
    if (overlap->x1 > overlap->x2) {
        return false;
    }
    overlap->y1 = a->y1;
    if (b->y1 > overlap->y1) {
        overlap->y1 = b->y1;
    }
    overlap->y2 = a->y2;
    if (b->y2 < overlap->y2) {
        overlap->y2 = b->y2;
    }
    if (overlap->y1 > overlap->y2) {
        return false;
    }
    return true;
}

uint16_t displayio_area_width(const displayio_area_t* area) {
    return area->x2 - area->x1;
}

uint16_t displayio_area_height(const displayio_area_t* area) {
    return area->y2 - area->y1;
}

uint32_t displayio_area_size(const displayio_area_t* area) {
    return displayio_area_width(area) * displayio_area_height(area);
}

bool displayio_area_equal(const displayio_area_t* a, const displayio_area_t* b) {
    return a->x1 == b->x1 &&
           a->y1 == b->y1 &&
           a->x2 == b->x2 &&
           a->y2 == b->y2;
}
