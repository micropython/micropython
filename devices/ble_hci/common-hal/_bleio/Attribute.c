// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
    mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid BLE attribute"));
}
