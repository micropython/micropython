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

// #include "common-hal/is31fl3741/Is31fl3741.h"
#include "shared-module/is31fl3741/allocator.h"
#include "shared-bindings/is31fl3741/is31fl3741.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/util.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"
#include "shared-bindings/busio/I2C.h"

extern Protomatter_core *_PM_protoPtr;

uint8_t cur_page = 99;

void send_unlock(busio_i2c_obj_t *i2c, uint8_t addr) {
    uint8_t unlock[2] = { 0xFE, 0xC5 }; // unlock command
    uint8_t result = common_hal_busio_i2c_write(i2c, addr, unlock, 2, true);
    if (result != 0) {
        mp_printf(&mp_plat_print, "Unlock error %x\n", result);
    }
}

void set_page(busio_i2c_obj_t *i2c, uint8_t addr, uint8_t p) {
    if (p == cur_page) {
        return;
    }

    cur_page = p;
    send_unlock(i2c, addr);

    uint8_t page[2] = { 0xFD, 0x00 }; // page command
    page[1] = p;
    uint8_t result = common_hal_busio_i2c_write(i2c, addr, page, 2, true);
    if (result != 0) {
        mp_printf(&mp_plat_print, "Set Page error %x\n", result);
    }
}

void send_enable(busio_i2c_obj_t *i2c, uint8_t addr) {
    set_page(i2c, addr, 4);
    uint8_t enable[2] = { 0x00, 0x01 }; // enable command
    uint8_t result = common_hal_busio_i2c_write(i2c, addr, enable, 2, true);
    if (result != 0) {
        mp_printf(&mp_plat_print, "Enable error %x\n", result);
    }
}

void send_reset(busio_i2c_obj_t *i2c, uint8_t addr) {
    set_page(i2c, addr, 4);
    uint8_t rst[2] = { 0x3F, 0xAE }; // reset command
    uint8_t result = common_hal_busio_i2c_write(i2c, addr, rst, 2, true);
    if (result != 0) {
        mp_printf(&mp_plat_print, "reset error %x\n", result);
    }
}

void set_current(busio_i2c_obj_t *i2c, uint8_t addr, uint8_t current) {
    set_page(i2c, addr, 4);
    uint8_t gcur[2] = { 0x01, 0x00 }; // global current command
    gcur[1] = current;
    uint8_t result = common_hal_busio_i2c_write(i2c, addr, gcur, 2, true);
    if (result != 0) {
        mp_printf(&mp_plat_print, "set current error %x\n", result);
    }
}

uint8_t get_current(busio_i2c_obj_t *i2c, uint8_t addr) {
    set_page(i2c, addr, 4);
    uint8_t gcur = 0x01; // global current command

    uint8_t result = common_hal_busio_i2c_write(i2c, addr, &gcur, 1, true);
    if (result != 0) {
        mp_printf(&mp_plat_print, "get current error %x\n", result);
    }

    uint8_t data = 0;
    result = common_hal_busio_i2c_read(i2c, addr, &data, 1);
    if (result != 0) {
        mp_printf(&mp_plat_print, "get current error %x\n", result);
    }

    return data;
}


void set_led(busio_i2c_obj_t *i2c, uint8_t addr, uint16_t led, uint8_t level, uint8_t page) {
    uint8_t cmd[2] = { 0x00, 0x00 };

    if (led < 180) {
        set_page(i2c, addr, page);
        cmd[0] = (uint8_t)led;
    } else {
        set_page(i2c, addr, page + 1);
        cmd[0] = (uint8_t)(led - 180);
    }

    cmd[1] = level;

    uint8_t result = common_hal_busio_i2c_write(i2c, addr, cmd, 2, true);
    if (result != 0) {
        mp_printf(&mp_plat_print, "set led error %x\n", result);
    }
}

void drawPixel(busio_i2c_obj_t *i2c, uint8_t addr, int16_t x, int16_t y, uint32_t color) {
    uint8_t r = color >> 16 & 0xFF;
    uint8_t g = color >> 8 & 0xFF;
    uint8_t b = color & 0xFF;

    int16_t x1 = (x * 5 + y) * 3;
    uint16_t ridx = glassesmatrix_ledmap[x1 + 2];
    if (ridx != 65535) {
        uint16_t gidx = glassesmatrix_ledmap[x1 + 1];
        uint16_t bidx = glassesmatrix_ledmap[x1 + 0];
        set_led(i2c, addr, ridx, r, 0);
        set_led(i2c, addr, gidx, g, 0);
        set_led(i2c, addr, bidx, b, 0);
    }
}

void common_hal_is31fl3741_is31fl3741_construct(is31fl3741_is31fl3741_obj_t *self, int width, int height, mp_obj_t framebuffer, busio_i2c_obj_t *i2c, uint8_t addr) {
    self->width = width;
    self->height = height;

    self->bufsize = 4 * width * height;

    // Probe the bus to see if a device acknowledges the given address.
    if (!common_hal_busio_i2c_probe(i2c, addr)) {
        self->base.type = &mp_type_NoneType;
        mp_raise_ValueError_varg(translate("Unable to find I2C Display at %x"), addr);
    }

    self->i2c = i2c;
    self->device_address = addr;

    common_hal_is31fl3741_is31fl3741_reconstruct(self, framebuffer);

    common_hal_busio_i2c_try_lock(i2c);

    uint8_t command = 0xFC;
    common_hal_busio_i2c_write(i2c, addr, &command, 1, false);
    uint8_t data = 0;
    common_hal_busio_i2c_read(i2c, addr, &data, 1);

    send_reset(i2c, addr);
    send_enable(i2c, addr);
    set_current(i2c, addr, 0x08);

    // set scale to max for all
    for (int i; i < 351; i++) {
        set_led(i2c, addr, i, 0xFF, 2);
    }

    common_hal_busio_i2c_unlock(i2c);
}

void common_hal_is31fl3741_is31fl3741_reconstruct(is31fl3741_is31fl3741_obj_t *self, mp_obj_t framebuffer) {
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

/*
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
        common_hal_is31fl3741_timer_enable(self->timer);
        stat = _PM_begin(&self->protomatter);

        if (stat == PROTOMATTER_OK) {
            _PM_convert_565(&self->protomatter, self->bufinfo.buf, self->width);
            _PM_swapbuffer_maybe(&self->protomatter);
        }
    }

    if (stat != PROTOMATTER_OK) {
        common_hal_is31fl3741_is31fl3741_deinit(self);
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
*/
    self->paused = 0;
}

void common_hal_is31fl3741_is31fl3741_deinit(is31fl3741_is31fl3741_obj_t *self) {
    /*
    if (self->timer) {
        common_hal_is31fl3741_timer_free(self->timer);
        self->timer = 0;
    }

    if (_PM_protoPtr == &self->protomatter) {
        _PM_protoPtr = NULL;
    }

    if (self->protomatter.rgbPins) {
        _PM_deallocate(&self->protomatter);
    }
    memset(&self->protomatter, 0, sizeof(self->protomatter));

    // If it was supervisor-allocated, it is supervisor-freed and the pointer
    // is zeroed, otherwise the pointer is just zeroed
    _PM_free(self->bufinfo.buf);
    */
    self->base.type = NULL;

    // If a framebuffer was passed in to the constructor, NULL the reference
    // here so that it will become GC'able
    self->framebuffer = NULL;
}

void common_hal_is31fl3741_is31fl3741_set_paused(is31fl3741_is31fl3741_obj_t *self, bool paused) {
    self->paused = paused;
}

bool common_hal_is31fl3741_is31fl3741_get_paused(is31fl3741_is31fl3741_obj_t *self) {
    return self->paused;
}

void common_hal_is31fl3741_is31fl3741_set_global_current(is31fl3741_is31fl3741_obj_t *self, uint8_t current) {
    set_current(self->i2c, self->device_address, current);
}

uint8_t common_hal_is31fl3741_is31fl3741_get_global_current(is31fl3741_is31fl3741_obj_t *self) {
    return get_current(self->i2c, self->device_address);
}

void common_hal_is31fl3741_is31fl3741_refresh(is31fl3741_is31fl3741_obj_t *self, uint8_t *dirtyrows) {
    uint8_t dirty_row_flags = 0xFF;
    if (dirtyrows != 0) {
        dirty_row_flags = *dirtyrows;
    }

    if (!self->paused) {
        uint32_t *buffer = self->bufinfo.buf;
        for (int y = 0; y < 5; y++) {
            if ((dirty_row_flags >> y) & 0x1) {
                for (int x = 0; x < 18; x++) {
                    drawPixel(self->i2c, self->device_address, x, y, *buffer);
                    buffer++;
                }
            }
        }
    }
}

int common_hal_is31fl3741_is31fl3741_get_width(is31fl3741_is31fl3741_obj_t *self) {
    return self->width;
}

int common_hal_is31fl3741_is31fl3741_get_height(is31fl3741_is31fl3741_obj_t *self) {
    return self->height;
}

void *common_hal_is31fl3741_allocator_impl(size_t sz) {
    supervisor_allocation *allocation = allocate_memory(align32_size(sz), false, true);
    return allocation ? allocation->ptr : NULL;
}

void common_hal_is31fl3741_free_impl(void *ptr_in) {
    free_memory(allocation_from_ptr(ptr_in));
}
