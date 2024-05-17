// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Radomir Dopieralski
//
// SPDX-License-Identifier: MIT

#include "Layer.h"
#include "Text.h"
#include "__init__.h"
#include "shared-bindings/_stage/Layer.h"
#include "shared-bindings/_stage/Text.h"


void render_stage(
    uint16_t x0, uint16_t y0,
    uint16_t x1, uint16_t y1,
    int16_t vx, int16_t vy,
    mp_obj_t *layers, size_t layers_size,
    uint16_t *buffer, size_t buffer_size,
    busdisplay_busdisplay_obj_t *display,
    uint8_t scale, uint16_t background) {


    displayio_area_t area;
    area.x1 = x0 * scale;
    area.y1 = y0 * scale;
    area.x2 = x1 * scale;
    area.y2 = y1 * scale;
    displayio_display_bus_set_region_to_update(&display->bus, &display->core, &area);

    while (!displayio_display_bus_begin_transaction(&display->bus)) {
        RUN_BACKGROUND_TASKS;
    }
    display->bus.send(display->bus.bus, DISPLAY_COMMAND,
        CHIP_SELECT_TOGGLE_EVERY_BYTE,
        &display->write_ram_command, 1);
    size_t index = 0;
    for (int16_t y = y0 + vy; y < y1 + vy; ++y) {
        for (uint8_t yscale = 0; yscale < scale; ++yscale) {
            for (int16_t x = x0 + vx; x < x1 + vx; ++x) {
                uint16_t c = TRANSPARENT;
                for (size_t layer = 0; layer < layers_size; ++layer) {
                    layer_obj_t *obj = MP_OBJ_TO_PTR(layers[layer]);
                    if (obj->base.type == &mp_type_layer) {
                        c = get_layer_pixel(obj, x, y);
                    } else if (obj->base.type == &mp_type_text) {
                        c = get_text_pixel((text_obj_t *)obj, x, y);
                    }
                    if (c != TRANSPARENT) {
                        break;
                    }
                }
                if (c == TRANSPARENT) {
                    c = background;
                }
                for (uint8_t xscale = 0; xscale < scale; ++xscale) {
                    buffer[index] = c;
                    index += 1;
                    // The buffer is full, send it.
                    if (index >= buffer_size) {
                        display->bus.send(display->bus.bus, DISPLAY_DATA,
                            CHIP_SELECT_UNTOUCHED,
                            ((uint8_t *)buffer), buffer_size * 2);
                        index = 0;
                    }
                }
            }
        }
    }
    // Send the remaining data.
    if (index) {
        display->bus.send(display->bus.bus, DISPLAY_DATA,
            CHIP_SELECT_UNTOUCHED,
            ((uint8_t *)buffer), index * 2);
    }

    displayio_display_bus_end_transaction(&display->bus);
}
