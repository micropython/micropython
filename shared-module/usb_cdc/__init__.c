/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
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

#include "shared-bindings/usb_cdc/__init__.h"

#include "genhdr/autogen_usb_descriptor.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objtuple.h"
#include "shared-bindings/usb_cdc/Serial.h"
#include "tusb.h"

supervisor_allocation* usb_cdc_allocation;

static usb_cdc_serial_obj_t serial_objs[CFG_TUD_CDC];

void usb_cdc_init(void) {

    serial_obj_ptrs *usb_cdc_serial_obj_t[CFG_TUD_CDC];

    for (size_t i = 0; i < CFG_TUD_CDC; i++) {
        serial_objs[i].base.type = &usb_cdc_serial_type;
        serial_objs[i].idx = i;
        serial_obj_ptrs[i] = &serial_objs[i];
    }

    serials_tuple->base.type = mp_obj_new_tuple(CFG_TUD_CDC, serials);

    repl->base.type =
    repl->idx = 0;    mp_map_lookup(&usb_cdc_module_globals.map, MP_ROM_QSTR(MP_QSTR_serial), MP_MAP_LOOKUP)->value = MP_OBJ_FROM_PTR(serials_tuple);
}
