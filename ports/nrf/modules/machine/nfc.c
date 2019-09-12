/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Glenn Ruben Bakke
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

#include "py/obj.h"

#if MICROPY_PY_MACHINE_NFC

#include "nfc.h"
#include "nfc_t2t_lib.h"
#include "nrfx_nfct.h"

void nfc_callback(void          * p_context,
                  enum            nfc_t2t_event event,
                  const uint8_t * p_data,
                  size_t          data_len) {
    // For now empty.
}

nrfx_err_t nfc_platform_setup(void) {
    NRF_CLOCK->TASKS_HFCLKSTART = 1UL;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
        ;
    }
    return NRFX_SUCCESS;
}

void nfc_platform_event_handler(nrfx_nfct_evt_t const *event)
{
    switch (event->evt_id) {
       case NRFX_NFCT_EVT_FIELD_DETECTED:
           nrfx_nfct_state_force(NRFX_NFCT_STATE_ACTIVATED);
           break;
       case NRFX_NFCT_EVT_FIELD_LOST:
           break;
       default:
           break;
    }
}

typedef struct {
    mp_obj_base_t base;
} machine_nfc_obj_t;

/// \method __str__()
/// Return a string describing the NFC object.
STATIC void machine_nfc_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    machine_nfc_obj_t *self = o;
    (void)self;

    mp_printf(print, "NFC");
}

STATIC mp_obj_t machine_nfc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) { 
    machine_nfc_obj_t *self = m_new_obj(machine_nfc_obj_t); 

    self->base.type = &machine_nfc_type;

    mp_int_t res = nfc_t2t_setup(nfc_callback, NULL);

    if (res == 0) {
        return MP_OBJ_FROM_PTR(self);
    } else {
        m_free(self);
    }

    return mp_const_none;
} 

/// \method start()
/// Start NFC emulation.
mp_obj_t machine_nfc_start(mp_obj_t self_in) {
    machine_nfc_obj_t *self = self_in;
    (void)self;

    mp_int_t res = nfc_t2t_emulation_start();

    return mp_obj_new_int(res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_nfc_start_obj, machine_nfc_start);

/// \method stop()
/// Stop NFC emulation.
mp_obj_t machine_nfc_stop(mp_obj_t self_in) {
    machine_nfc_obj_t *self = self_in;
    (void)self;

    mp_int_t res = nfc_t2t_emulation_stop();

    return mp_obj_new_int(res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_nfc_stop_obj, machine_nfc_stop);

/// \method payload_raw_set(bytes)
/// Set RAW payload data.
STATIC mp_obj_t machine_nfc_payload_raw_set(mp_obj_t self_in, mp_obj_t buf_in) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    mp_int_t res = nfc_t2t_payload_raw_set(bufinfo.buf, bufinfo.len);

    return mp_obj_new_int(res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_machine_nfc_payload_raw_set_obj, machine_nfc_payload_raw_set);

STATIC const mp_rom_map_elem_t machine_nfc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&mp_machine_nfc_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&mp_machine_nfc_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_payload_raw_set), MP_ROM_PTR(&mp_machine_nfc_payload_raw_set_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_nfc_locals_dict, machine_nfc_locals_dict_table);

const mp_obj_type_t machine_nfc_type = {
    { &mp_type_type },
    .name = MP_QSTR_NFC,
    .make_new = machine_nfc_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_nfc_locals_dict,
    .print = machine_nfc_print
};

#endif // MICROPY_PY_MACHINE_NFC
