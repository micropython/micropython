/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mark Komus
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

#include <string.h>

#include "py/gc.h"
#include "py/obj.h"
#include "py/objarray.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-module/is31fl3741/allocator.h"
#include "shared-bindings/is31fl3741/IS31FL3741.h"
#include "shared-bindings/is31fl3741/FrameBuffer.h"
#include "shared-bindings/util.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"
#include "shared-bindings/busio/I2C.h"

void common_hal_is31fl3741_FrameBuffer_construct(is31fl3741_FrameBuffer_obj_t *self, int width, int height, mp_obj_t framebuffer, is31fl3741_IS31FL3741_obj_t *is31, mp_obj_t mapping) {
    self->width = width;
    self->height = height;

    self->bufsize = sizeof(uint32_t) * width * height;

    self->is31fl3741 = is31;

    common_hal_busio_i2c_never_reset(self->is31fl3741->i2c);
    // Our object is statically allocated off the heap so make sure the bus object lives to the end
    // of the heap as well.
    gc_never_free(self->is31fl3741->i2c);
    gc_never_free(self->is31fl3741);

    mp_obj_t *items;
    size_t len;
    mp_obj_tuple_get(mapping, &len, &items);

    if (len != (size_t)(self->scale_width * self->scale_height * 3)) {
        mp_raise_ValueError(translate("LED mappings must match display size"));
    }

    self->mapping = common_hal_is31fl3741_allocator_impl(sizeof(uint16_t) * len);
    for (size_t i = 0; i < len; i++) {
        mp_int_t value = mp_obj_get_int(items[i]);
        // We only store up to 16 bits
        if (value > 0xFFFF) {
            value = 0xFFFF;
        }
        self->mapping[i] = (uint16_t)value;
    }

    common_hal_is31fl3741_FrameBuffer_reconstruct(self, framebuffer);
}

void common_hal_is31fl3741_FrameBuffer_reconstruct(is31fl3741_FrameBuffer_obj_t *self, mp_obj_t framebuffer) {
    self->paused = 1;

    if (framebuffer) {
        self->framebuffer = framebuffer;
        mp_get_buffer_raise(self->framebuffer, &self->bufinfo, MP_BUFFER_READ);
        if (mp_get_buffer(self->framebuffer, &self->bufinfo, MP_BUFFER_RW)) {
            self->bufinfo.typecode = 'H' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW;
        } else {
            self->bufinfo.typecode = 'H';
        }

        // verify that the matrix is big enough
        mp_get_index(mp_obj_get_type(self->framebuffer), self->bufinfo.len, MP_OBJ_NEW_SMALL_INT(self->bufsize - 1), false);
    } else {
        common_hal_is31fl3741_free_impl(self->bufinfo.buf);

        self->framebuffer = NULL;
        self->bufinfo.buf = common_hal_is31fl3741_allocator_impl(self->bufsize);
        self->bufinfo.len = self->bufsize;
        self->bufinfo.typecode = 'H' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW;
    }

    common_hal_is31fl3741_begin_transaction(self->is31fl3741);
    common_hal_is31fl3741_send_reset(self->is31fl3741);
    common_hal_is31fl3741_set_current(self->is31fl3741, 0xFE);

    // set scale (brightness) to max for all LEDs
    for (int i = 0; i < 351; i++) {
        common_hal_is31fl3741_set_led(self->is31fl3741, i, 0xFF, 2);
    }

    common_hal_is31fl3741_send_enable(self->is31fl3741);
    common_hal_is31fl3741_end_transaction(self->is31fl3741);

    self->paused = 0;
}

void common_hal_is31fl3741_FrameBuffer_deinit(is31fl3741_FrameBuffer_obj_t *self) {
    common_hal_is31fl3741_end_transaction(self->is31fl3741); // in case we still had a lock

    common_hal_is31fl3741_IS31FL3741_deinit(self->is31fl3741);

    if (self->mapping != 0) {
        common_hal_is31fl3741_free_impl(self->mapping);
        self->mapping = 0;
    }

    self->base.type = NULL;

    // If a framebuffer was passed in to the constructor, NULL the reference
    // here so that it will become GC'able
    self->framebuffer = NULL;
}

void common_hal_is31fl3741_FrameBuffer_set_paused(is31fl3741_FrameBuffer_obj_t *self, bool paused) {
    self->paused = paused;
}

bool common_hal_is31fl3741_FrameBuffer_get_paused(is31fl3741_FrameBuffer_obj_t *self) {
    return self->paused;
}

void common_hal_is31fl3741_FrameBuffer_refresh(is31fl3741_FrameBuffer_obj_t *self, uint8_t *dirtyrows) {
    if (!self->paused) {
        common_hal_is31fl3741_begin_transaction(self->is31fl3741);

        uint8_t dirty_row_flags = 0xFF; // only supports 8 rows gotta fix

        if (self->scale) {
            // Based on the Arduino IS31FL3741 driver code
            // dirtyrows flag current not implemented for scaled displays
            uint32_t *buffer = self->bufinfo.buf;

            for (int x = 0; x < self->scale_width; x++) {
                uint32_t *ptr = &buffer[x * 3]; // Entry along top scan line w/x offset
                for (int y = 0; y < self->scale_height; y++) {
                    uint16_t rsum = 0, gsum = 0, bsum = 0;
                    // Inner x/y loops are row-major on purpose (less pointer math)
                    for (uint8_t yy = 0; yy < 3; yy++) {
                        for (uint8_t xx = 0; xx < 3; xx++) {
                            uint32_t rgb = ptr[xx];
                            rsum += rgb >> 16 & 0xFF;
                            gsum += (rgb >> 8) & 0xFF;
                            bsum += rgb & 0xFF;
                        }
                        ptr += self->width; // Advance one scan line
                    }
                    rsum = rsum / 9;
                    gsum = gsum / 9;
                    bsum = bsum / 9;
                    uint32_t color = 0;
                    if (self->auto_gamma) {
                        color = (IS31GammaTable[rsum] << 16) +
                            (IS31GammaTable[gsum] << 8) +
                            IS31GammaTable[bsum];
                    } else {
                        color = (rsum << 16) + (gsum << 8) + bsum;
                    }
                    common_hal_is31fl3741_draw_pixel(self->is31fl3741, x, y, color, self->mapping);
                }
            }
        } else {
            uint32_t *buffer = self->bufinfo.buf;
            for (int y = 0; y < self->height; y++) {
                if ((dirtyrows != 0) && ((y % 8) == 0)) {
                    dirty_row_flags = *dirtyrows++;
                }

                if ((dirty_row_flags >> (y % 8)) & 0x1) {
                    uint32_t color = 0;
                    if (self->auto_gamma) {
                        color = IS31GammaTable[((*buffer) >> 16 & 0xFF)] +
                            IS31GammaTable[((*buffer) >> 8 & 0xFF)] +
                            IS31GammaTable[((*buffer) & 0xFF)];
                    } else {
                        color = *buffer;
                    }

                    for (int x = 0; x < self->width; x++) {
                        common_hal_is31fl3741_draw_pixel(self->is31fl3741, x, y, color, self->mapping);
                        buffer++;
                    }
                }
            }
        }
        common_hal_is31fl3741_end_transaction(self->is31fl3741);
    }
}

int common_hal_is31fl3741_FrameBuffer_get_width(is31fl3741_FrameBuffer_obj_t *self) {
    return self->width;
}

int common_hal_is31fl3741_FrameBuffer_get_height(is31fl3741_FrameBuffer_obj_t *self) {
    return self->height;
}

void *common_hal_is31fl3741_allocator_impl(size_t sz) {
    supervisor_allocation *allocation = allocate_memory(align32_size(sz), false, true);
    return allocation ? allocation->ptr : NULL;
}

void common_hal_is31fl3741_free_impl(void *ptr_in) {
    free_memory(allocation_from_ptr(ptr_in));
}

void is31fl3741_FrameBuffer_collect_ptrs(is31fl3741_FrameBuffer_obj_t *self) {
    gc_collect_ptr(self->framebuffer);
    gc_collect_ptr(self->mapping);
}
