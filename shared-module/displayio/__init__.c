
#include <string.h>
#include "shared-module/displayio/__init__.h"

#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/Display.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/Sprite.h"
#include "supervisor/usb.h"

primary_display_t displays[CIRCUITPY_DISPLAY_LIMIT];

void displayio_refresh_displays(void) {
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].display.base.type == NULL || displays[i].display.base.type == &mp_type_NoneType) {
            continue;
        }
        displayio_display_obj_t* display = &displays[i].display;

        if (!displayio_display_frame_queued(display)) {
            return;
        }
        if (displayio_display_refresh_queued(display)) {
            // We compute the pixels
            uint16_t x0 = 0;
            uint16_t y0 = 0;
            uint16_t x1 = display->width;
            uint16_t y1 = display->height;
            size_t index = 0;
            //size_t row_size = (x1 - x0);
            uint16_t buffer_size = 256;
            uint32_t buffer[buffer_size / 2];
            displayio_display_start_region_update(display, x0, y0, x1, y1);
            for (uint16_t y = y0; y < y1; ++y) {
                for (uint16_t x = x0; x < x1; ++x) {
                    uint16_t* pixel = &(((uint16_t*)buffer)[index]);
                    *pixel = 0;

                    if (display->current_group != NULL) {
                        displayio_group_get_pixel(display->current_group, x, y, pixel);
                    }

                    index += 1;
                    // The buffer is full, send it.
                    if (index >= buffer_size) {
                        if (!displayio_display_send_pixels(display, buffer, buffer_size / 2)) {
                            displayio_display_finish_region_update(display);
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
                return;
            }
            displayio_display_finish_region_update(display);
        }
        displayio_display_finish_refresh(display);
    }
}

uint32_t blinka_bitmap_data[32] = {
    0x00000011, 0x11000000,
    0x00000111, 0x53100000,
    0x00000111, 0x56110000,
    0x00000111, 0x11140000,
    0x00000111, 0x20002000,
    0x00000011, 0x13000000,
    0x00000001, 0x11200000,
    0x00000000, 0x11330000,
    0x00000000, 0x01122000,
    0x00001111, 0x44133000,
    0x00032323, 0x24112200,
    0x00111114, 0x44113300,
    0x00323232, 0x34112200,
    0x11111144, 0x44443300,
    0x11111111, 0x11144401,
    0x23232323, 0x21111110
};

displayio_bitmap_t blinka_bitmap = {
    .base = {.type = &displayio_bitmap_type },
    .width = 16,
    .height = 16,
    .data = blinka_bitmap_data,
    .stride = 2,
    .bits_per_value = 4,
    .x_shift = 3,
    .x_mask = 0x7,
    .bitmask = 0xf
};

uint32_t blinka_transparency[1] = {0x80000000};

// These colors are RGB 565 with the bytes swapped.
uint32_t blinka_colors[8] = {0x78890000, 0x9F86B8FC, 0xffff0D5A, 0x0000f501,
                             0x00000000, 0x00000000, 0x00000000, 0x00000000};

displayio_palette_t blinka_palette = {
    .base = {.type = &displayio_palette_type },
    .opaque = blinka_transparency,
    .colors = blinka_colors,
    .color_count = 16,
    .needs_refresh = false
};

displayio_sprite_t blinka_sprite = {
    .base = {.type = &displayio_sprite_type },
    .bitmap = &blinka_bitmap,
    .pixel_shader = &blinka_palette,
    .x = 0,
    .y = 0,
    .width = 16,
    .height = 16,
    .needs_refresh = false
};

mp_obj_t splash_children[1] = {
    &blinka_sprite,
};

displayio_group_t splash = {
    .base = {.type = &displayio_group_type },
    .x = 0,
    .y = 0,
    .scale = 2,
    .size = 1,
    .max_size = 1,
    .children = splash_children,
    .needs_refresh = true
};

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
        common_hal_displayio_display_show(display, &splash);
    }
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
        displays[i].display.base.type = &mp_type_NoneType;
    }
}
