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
    // mp_printf(&mp_plat_print, "Set Page %x -> %x\n", page[0], page[1]);
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
    mp_printf(&mp_plat_print, "CH IS construct: enable\n");
}

void send_reset(busio_i2c_obj_t *i2c, uint8_t addr) {
    set_page(i2c, addr, 4);
    uint8_t rst[2] = { 0x3F, 0xAE }; // reset command
    uint8_t result = common_hal_busio_i2c_write(i2c, addr, rst, 2, true);
    if (result != 0) {
        mp_printf(&mp_plat_print, "reset error %x\n", result);
    }
    mp_printf(&mp_plat_print, "CH IS construct: reset\n");
}

void set_current(busio_i2c_obj_t *i2c, uint8_t addr, uint8_t cur) {
    set_page(i2c, addr, 4);
    uint8_t gcur[2] = { 0x01, 0x00 }; // global current command
    gcur[1] = cur;
    uint8_t result = common_hal_busio_i2c_write(i2c, addr, gcur, 2, true);
    if (result != 0) {
        mp_printf(&mp_plat_print, "set current error %x\n", result);
    }
    mp_printf(&mp_plat_print, "CH IS construct: set global current\n");
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
    // mp_printf(&mp_plat_print, "CH IS construct: set led %x -> %x\n", led, level);
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
        // mp_printf(&mp_plat_print, "drawPixel: x %d y %d r %02x g %02x b %02x ri %d gi %d bi %d\n", x, y, r, g, b, ridx, gidx, bidx);
    } else {
        // mp_printf(&mp_plat_print, "drawPixel: x %d y %d r %02x g %02x b %02x OOB\n", x, y, r, g, b);
    }

}




void common_hal_is31fl3741_is31fl3741_construct(is31fl3741_is31fl3741_obj_t *self, int width, int height, mp_obj_t framebuffer, busio_i2c_obj_t *i2c, uint8_t addr) {
    mp_printf(&mp_plat_print, "CH IS construct %x\n", addr);
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
    mp_printf(&mp_plat_print, "CH IS construct device %x\n", data);

    send_reset(i2c, addr);
    send_enable(i2c, addr);
    set_current(i2c, addr, 0x08);

    // set scale to max for all
    for (int i; i < 351; i++) {
        set_led(i2c, addr, i, 0xFF, 2);
    }

    // set_led(i2c, addr, 0x09, 0xA1, 0);
    // set_led(i2c, addr, 309, 0xA1, 0);
    // set_led(i2c, addr, 0x09, 0xCC, 2);

    // set_led(i2c, addr, 0x19, 0xA2, 0);
    // set_led(i2c, addr, 0x19, 0x02, 2);

    // set_led(i2c, addr, 0x2A, 0xA3, 1);
    // set_led(i2c, addr, 0x29, 0xEE, 3);

    common_hal_busio_i2c_unlock(i2c);


}

void common_hal_is31fl3741_is31fl3741_reconstruct(is31fl3741_is31fl3741_obj_t *self, mp_obj_t framebuffer) {
    self->paused = 1;

    if (framebuffer) {
        mp_printf(&mp_plat_print, "CH IS reconstruct framebuffer %x\n", MP_OBJ_TO_PTR(framebuffer));
        self->framebuffer = framebuffer;
        mp_get_buffer_raise(self->framebuffer, &self->bufinfo, MP_BUFFER_READ);
        if (mp_get_buffer(self->framebuffer, &self->bufinfo, MP_BUFFER_RW)) {
            self->bufinfo.typecode = 'H' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW;
        } else {
            self->bufinfo.typecode = 'H';
        }
        mp_printf(&mp_plat_print, "CH IS reconstruct self->bufinfo is %x\n", self->bufinfo.buf);
        // verify that the matrix is big enough
        mp_get_index(mp_obj_get_type(self->framebuffer), self->bufinfo.len, MP_OBJ_NEW_SMALL_INT(self->bufsize - 1), false);
    } else {
        mp_printf(&mp_plat_print, "CH IS reconstruct NO framebuffer\n");
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
    mp_printf(&mp_plat_print, "IS CH Deinit\n");
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
    mp_printf(&mp_plat_print, "CH IS set paused\n");
    /*if (paused && !self->paused) {
        _PM_stop(&self->protomatter);
    } else if (!paused && self->paused) {
        _PM_resume(&self->protomatter);
        _PM_convert_565(&self->protomatter, self->bufinfo.buf, self->width);
        _PM_swapbuffer_maybe(&self->protomatter);
    }*/
    self->paused = paused;
}

bool common_hal_is31fl3741_is31fl3741_get_paused(is31fl3741_is31fl3741_obj_t *self) {
    mp_printf(&mp_plat_print, "CH IS get paused\n");
    return self->paused;
}

void common_hal_is31fl3741_is31fl3741_refresh(is31fl3741_is31fl3741_obj_t *self) {
    // mp_printf(&mp_plat_print, "CH IS refresh len %x addr %x\n", self->bufinfo.len, self->bufinfo.buf);

    if (!self->paused) {
        uint32_t *buffer = self->bufinfo.buf;
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 18; x++) {
                drawPixel(self->i2c, self->device_address, x, y, *buffer);
                // mp_printf(&mp_plat_print, "%06x ", *buffer);
                buffer++;
            }
            // mp_printf(&mp_plat_print, "\n");
        }
        //
        // _PM_convert_565(&self->protomatter, self->bufinfo.buf, self->width);
        // _PM_swapbuffer_maybe(&self->protomatter);
    }
}

int common_hal_is31fl3741_is31fl3741_get_width(is31fl3741_is31fl3741_obj_t *self) {
    mp_printf(&mp_plat_print, "CH IS get width\n");
    return self->width;
}

int common_hal_is31fl3741_is31fl3741_get_height(is31fl3741_is31fl3741_obj_t *self) {
    mp_printf(&mp_plat_print, "CH IS get height\n");
    return self->height;
}

void *common_hal_is31fl3741_allocator_impl(size_t sz) {
    mp_printf(&mp_plat_print, "CH IS allocator\n");
    supervisor_allocation *allocation = allocate_memory(align32_size(sz), false, true);
    return allocation ? allocation->ptr : NULL;
}

void common_hal_is31fl3741_free_impl(void *ptr_in) {
    mp_printf(&mp_plat_print, "CH IS free\n");
    free_memory(allocation_from_ptr(ptr_in));
}
