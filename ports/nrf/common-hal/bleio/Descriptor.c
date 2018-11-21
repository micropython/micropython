/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
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

#include "common-hal/bleio/Descriptor.h"
#include "shared-bindings/bleio/UUID.h"

void common_hal_bleio_descriptor_construct(bleio_descriptor_obj_t *self, bleio_uuid_obj_t *uuid) {
    // TODO: set handle ???
    self->uuid = uuid;
}

void common_hal_bleio_descriptor_print(bleio_descriptor_obj_t *self, const mp_print_t *print) {
    mp_printf(print, "Descriptor(uuid=");
    common_hal_bleio_uuid_print(self->uuid, print);
    mp_printf(print, ", handle=%0x", self->handle);
}

mp_int_t common_hal_bleio_descriptor_get_handle(bleio_descriptor_obj_t *self) {
    return self->handle;
}

mp_obj_t common_hal_bleio_descriptor_get_uuid(bleio_descriptor_obj_t *self) {
    return MP_OBJ_FROM_PTR(self->uuid);
}
