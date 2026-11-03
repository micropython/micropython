// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "shared-bindings/hardwarekey/__init__.h"
#include "shared-bindings/hardwarekey/HardwareKey.h"

//| class HardwareKey:
//|     """A key held in a hardware key store, usable but not readable.
//|
//|     This class cannot be instantiated. Every hardware key slot the board has
//|     is exposed as a fixed `HardwareKey` in :mod:`board` -- for example
//|     ``board.EFUSE_KEY0`` -- just like pins. A slot with no key burned into it
//|     still has a `HardwareKey` object; its `purpose` is `hardwarekey.Purpose.UNUSED`.
//|
//|     Compute a MAC with a key by passing it to `hmac.new()` in place of a
//|     ``bytes`` key, or turn a Digital Signature key into a
//|     :py:class:`cryptography.hazmat.primitives.asymmetric.rsa.DigitalSignatureKey`
//|     with :py:func:`cryptography.hazmat.primitives.asymmetric.rsa.load_digital_signature_key()`.
//|
//|     On espressif the slots are the eFuse key blocks (``BLOCK_KEY0`` -
//|     ``BLOCK_KEY5``); a slot is usable only if its block was burned with
//|     purpose ``HMAC_UP`` or ``HMAC_DOWN_DIGITAL_SIGNATURE``."""
//|

static void hardwarekey_hardwarekey_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->name != MP_QSTRnull) {
        mp_printf(print, "<HardwareKey %q>", self->name);
    } else {
        mp_printf(print, "<HardwareKey slot %d>", (int)self->key_slot);
    }
}

//|     def __bool__(self) -> bool:
//|         """``False`` if `purpose` is `hardwarekey.Purpose.UNUSED`, ``True`` otherwise.
//|         This lets you write ``if key:`` to check whether a slot is usable."""
//|         ...
static mp_obj_t hardwarekey_hardwarekey_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(common_hal_hardwarekey_hardwarekey_get_purpose(self) != HARDWAREKEY_PURPOSE_UNUSED);
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

//|     key_slot: int
//|     """The port-defined key identifier this handle is bound to. On espressif,
//|     the eFuse key block index. (read-only)"""
static mp_obj_t hardwarekey_hardwarekey_get_key_slot(mp_obj_t self_in) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_hardwarekey_hardwarekey_get_key_slot(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hardwarekey_hardwarekey_get_key_slot_obj, hardwarekey_hardwarekey_get_key_slot);
MP_PROPERTY_GETTER(hardwarekey_hardwarekey_key_slot_obj, (mp_obj_t)&hardwarekey_hardwarekey_get_key_slot_obj);

//|     purpose: Purpose
//|     """What this key slot is provisioned for -- `hardwarekey.Purpose.HMAC_UP`,
//|     `hardwarekey.Purpose.HMAC_DOWN_DIGITAL_SIGNATURE`, or `hardwarekey.Purpose.UNUSED`.
//|     (read-only)"""
static mp_obj_t hardwarekey_hardwarekey_get_purpose(mp_obj_t self_in) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return cp_enum_find(&hardwarekey_purpose_type, common_hal_hardwarekey_hardwarekey_get_purpose(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hardwarekey_hardwarekey_get_purpose_obj, hardwarekey_hardwarekey_get_purpose);
MP_PROPERTY_GETTER(hardwarekey_hardwarekey_purpose_obj, (mp_obj_t)&hardwarekey_hardwarekey_get_purpose_obj);

//|     exportable: bool
//|     """Whether the raw key bytes can ever leave the hardware. Always
//|     informational.
//|
//|     On espressif this is ``False`` once the key block's ``RD_DIS`` eFuse
//|     bit is set (which ``espefuse.py`` does by default). It is meant for
//|     manufacturing-time self-test code to confirm a key block was burned as
//|     expected. (read-only)"""
//|
static mp_obj_t hardwarekey_hardwarekey_get_exportable(mp_obj_t self_in) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_hardwarekey_hardwarekey_get_exportable(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hardwarekey_hardwarekey_get_exportable_obj, hardwarekey_hardwarekey_get_exportable);
MP_PROPERTY_GETTER(hardwarekey_hardwarekey_exportable_obj, (mp_obj_t)&hardwarekey_hardwarekey_get_exportable_obj);

static const mp_rom_map_elem_t hardwarekey_hardwarekey_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_key_slot), MP_ROM_PTR(&hardwarekey_hardwarekey_key_slot_obj) },
    { MP_ROM_QSTR(MP_QSTR_purpose), MP_ROM_PTR(&hardwarekey_hardwarekey_purpose_obj) },
    { MP_ROM_QSTR(MP_QSTR_exportable), MP_ROM_PTR(&hardwarekey_hardwarekey_exportable_obj) },
};
static MP_DEFINE_CONST_DICT(hardwarekey_hardwarekey_locals_dict, hardwarekey_hardwarekey_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    hardwarekey_hardwarekey_type,
    MP_QSTR_HardwareKey,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    print, hardwarekey_hardwarekey_print,
    unary_op, hardwarekey_hardwarekey_unary_op,
    locals_dict, &hardwarekey_hardwarekey_locals_dict
    );
