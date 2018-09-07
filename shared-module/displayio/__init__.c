#include "shared-bindings/displayio/FourWire.h"

extern displayio_fourwire_obj_t board_display_obj;

void start_region_update(displayio_fourwire_obj_t* display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // TODO delegate between different display types
    displayio_fourwire_start_region_update(display, x0, y0, x1, y1);
}

void finish_region_update(displayio_fourwire_obj_t* display) {
    // TODO delegate between different display types
    displayio_fourwire_finish_region_update(display);
}

void finish_refresh(displayio_fourwire_obj_t* display) {
    // TODO delegate between different display types
    displayio_fourwire_finish_refresh(display);
}

bool frame_queued(displayio_fourwire_obj_t* display) {
    // TODO delegate between different display types
    return displayio_fourwire_frame_queued(display);
}

bool refresh_queued(displayio_fourwire_obj_t* display) {
    // TODO delegate between different display types
    return displayio_fourwire_refresh_queued(display);
}

bool send_pixels(displayio_fourwire_obj_t* display, uint32_t* pixels, uint32_t length) {
    // TODO delegate between different display types
    return displayio_fourwire_send_pixels(display, pixels, length);
}

void displayio_refresh_display(void) {
    displayio_fourwire_obj_t* display = &board_display_obj;

    if (!frame_queued(display)) {
        return;
    }
    if (refresh_queued(display)) {
        // We compute the pixels
        uint16_t x0 = 0;
        uint16_t y0 = 0;
        uint16_t x1 = display->width;
        uint16_t y1 = display->height;
        size_t index = 0;
        //size_t row_size = (x1 - x0);
        uint16_t buffer_size = 256;
        uint32_t buffer[buffer_size / 2];
        start_region_update(display, x0, y0, x1, y1);
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
                    if (!send_pixels(display, buffer, buffer_size / 2)) {
                        finish_region_update(display);
                        return;
                    }
                    index = 0;
                }
            }
        }
        // Send the remaining data.
        if (index && !send_pixels(display, buffer, index * 2)) {
            finish_region_update(display);
            return;
        }
        finish_region_update(display);
    }
    finish_refresh(display);
}
