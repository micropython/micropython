/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include "common-hal/rgbmatrix/RGBMatrix.h"
#include "shared-module/rgbmatrix/allocator.h"
#include "shared-bindings/rgbmatrix/RGBMatrix.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/util.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"

extern Protomatter_core *_PM_protoPtr;

void common_hal_rgbmatrix_rgbmatrix_construct(rgbmatrix_rgbmatrix_obj_t *self, int width, int bit_depth, uint8_t rgb_count, uint8_t *rgb_pins, uint8_t addr_count, uint8_t *addr_pins, uint8_t clock_pin, uint8_t latch_pin, uint8_t oe_pin, bool doublebuffer, mp_obj_t framebuffer, int8_t tile, bool serpentine, void *timer) {
    self->width = width;
    self->bit_depth = bit_depth;
    self->rgb_count = rgb_count;
    memcpy(self->rgb_pins, rgb_pins, rgb_count);
    self->addr_count = addr_count;
    memcpy(self->addr_pins, addr_pins, addr_count);
    self->clock_pin = clock_pin;
    self->oe_pin = oe_pin;
    self->latch_pin = latch_pin;
    self->doublebuffer = doublebuffer;
    self->tile = tile;
    self->serpentine = serpentine;

    self->timer = timer ? timer : common_hal_rgbmatrix_timer_allocate(self);
    if (self->timer == NULL) {
        mp_raise_ValueError(translate("No timer available"));
    }

    self->width = width;
    self->bufsize = 2 * width * common_hal_rgbmatrix_rgbmatrix_get_height(self);

    common_hal_rgbmatrix_rgbmatrix_reconstruct(self, framebuffer);
}

void common_hal_rgbmatrix_rgbmatrix_reconstruct(rgbmatrix_rgbmatrix_obj_t *self, mp_obj_t framebuffer) {
    self->paused = 1;

    common_hal_rgbmatrix_timer_disable(self->timer);
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
        common_hal_rgbmatrix_free_impl(self->bufinfo.buf);
        common_hal_rgbmatrix_free_impl(self->protomatter.rgbPins);
        common_hal_rgbmatrix_free_impl(self->protomatter.addr);
        common_hal_rgbmatrix_free_impl(self->protomatter.screenData);

        self->framebuffer = NULL;
        self->bufinfo.buf = common_hal_rgbmatrix_allocator_impl(self->bufsize);
        self->bufinfo.len = self->bufsize;
        self->bufinfo.typecode = 'H' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW;
    }

    memset(&self->protomatter, 0, sizeof(self->protomatter));
    ProtomatterStatus stat = _PM_init(&self->protomatter,
        self->width, self->bit_depth,
        self->rgb_count / 6, self->rgb_pins,
        self->addr_count, self->addr_pins,
        self->clock_pin, self->latch_pin, self->oe_pin,
        self->doublebuffer, self->serpentine ? -self->tile : self->tile,
        self->timer);

    if (stat == PROTOMATTER_OK) {
        _PM_protoPtr = &self->protomatter;
        common_hal_rgbmatrix_timer_enable(self->timer);
        stat = _PM_begin(&self->protomatter);

        if (stat == PROTOMATTER_OK) {
            _PM_convert_565(&self->protomatter, self->bufinfo.buf, self->width);
            _PM_swapbuffer_maybe(&self->protomatter);
        }
    }

    if (stat != PROTOMATTER_OK) {
        common_hal_rgbmatrix_rgbmatrix_deinit(self);
        switch (stat) {
            case PROTOMATTER_ERR_PINS:
                mp_raise_ValueError(translate("Invalid pin"));
                break;
            case PROTOMATTER_ERR_ARG:
                mp_raise_ValueError(translate("Invalid argument"));
                break;
            case PROTOMATTER_ERR_MALLOC:
                mp_raise_msg(&mp_type_MemoryError, NULL);
                break;
            default:
                mp_raise_msg_varg(&mp_type_RuntimeError,
                    translate("Internal error #%d"), (int)stat);
                break;
        }
    }

    self->paused = 0;
}

STATIC void free_pin(uint8_t *pin) {
    if (*pin != COMMON_HAL_MCU_NO_PIN) {
        common_hal_mcu_pin_reset_number(*pin);
    }
    *pin = COMMON_HAL_MCU_NO_PIN;
}

STATIC void free_pin_seq(uint8_t *seq, int count) {
    for (int i = 0; i < count; i++) {
        free_pin(&seq[i]);
    }
}

void common_hal_rgbmatrix_rgbmatrix_deinit(rgbmatrix_rgbmatrix_obj_t *self) {
    if (self->timer) {
        common_hal_rgbmatrix_timer_free(self->timer);
        self->timer = 0;
    }

    if (_PM_protoPtr == &self->protomatter) {
        _PM_protoPtr = NULL;
    }

    free_pin_seq(self->rgb_pins, self->rgb_count);
    free_pin_seq(self->addr_pins, self->addr_count);
    free_pin(&self->clock_pin);
    free_pin(&self->latch_pin);
    free_pin(&self->oe_pin);

    if (self->protomatter.rgbPins) {
        _PM_deallocate(&self->protomatter);
    }
    memset(&self->protomatter, 0, sizeof(self->protomatter));

    // If it was supervisor-allocated, it is supervisor-freed and the pointer
    // is zeroed, otherwise the pointer is just zeroed
    _PM_free(self->bufinfo.buf);
    self->base.type = NULL;

    // If a framebuffer was passed in to the constructor, NULL the reference
    // here so that it will become GC'able
    self->framebuffer = NULL;
}

void rgbmatrix_rgbmatrix_collect_ptrs(rgbmatrix_rgbmatrix_obj_t *self) {
    gc_collect_ptr(self->framebuffer);
}

void common_hal_rgbmatrix_rgbmatrix_set_paused(rgbmatrix_rgbmatrix_obj_t *self, bool paused) {
    if (paused && !self->paused) {
        _PM_stop(&self->protomatter);
    } else if (!paused && self->paused) {
        _PM_resume(&self->protomatter);
        _PM_convert_565(&self->protomatter, self->bufinfo.buf, self->width);
        _PM_swapbuffer_maybe(&self->protomatter);
    }
    self->paused = paused;
}

bool common_hal_rgbmatrix_rgbmatrix_get_paused(rgbmatrix_rgbmatrix_obj_t *self) {
    return self->paused;
}

void common_hal_rgbmatrix_rgbmatrix_refresh(rgbmatrix_rgbmatrix_obj_t *self) {
    if (!self->paused) {
        _PM_convert_565(&self->protomatter, self->bufinfo.buf, self->width);
        _PM_swapbuffer_maybe(&self->protomatter);
    }
}

int common_hal_rgbmatrix_rgbmatrix_get_width(rgbmatrix_rgbmatrix_obj_t *self) {
    return self->width;
}

int common_hal_rgbmatrix_rgbmatrix_get_height(rgbmatrix_rgbmatrix_obj_t *self) {
    int computed_height = (self->rgb_count / 3) * (1 << (self->addr_count)) * self->tile;
    return computed_height;
}

void *common_hal_rgbmatrix_allocator_impl(size_t sz) {
    supervisor_allocation *allocation = allocate_memory(align32_size(sz), false, true);
    return allocation ? allocation->ptr : NULL;
}

void common_hal_rgbmatrix_free_impl(void *ptr_in) {
    free_memory(allocation_from_ptr(ptr_in));
}
