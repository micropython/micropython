
#include "shared-module/displayio/__init__.h"

#include "shared-bindings/displayio/Display.h"

primary_display_t primary_display;

void displayio_refresh_display(void) {
    displayio_display_obj_t* display = &primary_display.display;

    if (!displayio_display_frame_queued(display)) {
        return;
    }
    if (displayio_display_refresh_queued(display)) {
        PORT->Group[1].DIRSET.reg = 1 << 22;

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

                PORT->Group[1].OUTTGL.reg = 1 << 22;

                //if (index == 0) {
                    if (display->current_group != NULL) {
                        displayio_group_get_pixel(display->current_group, x, y, pixel);
                    }
                // } else {
                //     *pixel = (((uint16_t*)buffer)[0]);
                // }


                PORT->Group[1].OUTTGL.reg = 1 << 22;

                index += 1;
                // The buffer is full, send it.
                if (index >= buffer_size) {
                    if (!displayio_display_send_pixels(display, buffer, buffer_size / 2)) {
                        displayio_display_finish_region_update(display);
                        return;
                    }
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

void reset_primary_display(void) {
    common_hal_displayio_display_show(&primary_display.display, NULL);
}
