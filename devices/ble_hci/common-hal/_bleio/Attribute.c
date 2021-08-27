/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#include "py/runtime.h"

#include "shared-bindings/_bleio/Attribute.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"


bleio_uuid_obj_t *bleio_attribute_get_uuid(mp_obj_t *attribute) {
    if (mp_obj_is_type(attribute, &bleio_characteristic_type)) {
        bleio_characteristic_obj_t *characteristic = MP_OBJ_TO_PTR(attribute);
        return characteristic->uuid;
    }
    if (mp_obj_is_type(attribute, &bleio_descriptor_type)) {
        bleio_descriptor_obj_t *descriptor = MP_OBJ_TO_PTR(attribute);
        return descriptor->uuid;
    }
    if (mp_obj_is_type(attribute, &bleio_service_type)) {
        bleio_service_obj_t *service = MP_OBJ_TO_PTR(attribute);
        return service->uuid;
    }
    mp_raise_RuntimeError(translate("Invalid BLE attribute"));
}
