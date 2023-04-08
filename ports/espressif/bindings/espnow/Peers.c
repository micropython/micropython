/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 MicroDev
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
#include "py/objlist.h"
#include "py/runtime.h"

#include "bindings/espidf/__init__.h"

#include "bindings/espnow/Peer.h"
#include "bindings/espnow/Peers.h"

#include "esp_now.h"

//| class Peers:
//|     """Maintains a `list` of `Peer` internally and only exposes a subset of `list` methods."""
//|
//|     def __init__(self) -> None:
//|         """You cannot create an instance of `Peers`."""
//|         ...

//|     def append(self, peer: Peer) -> None:
//|         """Append peer.
//|
//|         :param Peer peer: The peer object to append.
//|         """
//|         ...
STATIC mp_obj_t espnow_peers_append(mp_obj_t self_in, mp_obj_t arg) {
    espnow_peer_obj_t *peer = MP_OBJ_TO_PTR(mp_arg_validate_type(arg, &espnow_peer_type, MP_QSTR_Peer));
    CHECK_ESP_RESULT(esp_now_add_peer(&peer->peer_info));
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_list_append(self->list, arg);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_peers_append_obj, espnow_peers_append);

//|     def remove(self, peer: Peer) -> None:
//|         """Remove peer.
//|
//|         :param Peer peer: The peer object to remove.
//|         """
//|         ...
//|
STATIC mp_obj_t espnow_peers_remove(mp_obj_t self_in, mp_obj_t arg) {
    espnow_peer_obj_t *peer = MP_OBJ_TO_PTR(mp_arg_validate_type(arg, &espnow_peer_type, MP_QSTR_Peer));
    CHECK_ESP_RESULT(esp_now_del_peer(peer->peer_info.peer_addr));
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_list_remove(self->list, arg);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_peers_remove_obj, espnow_peers_remove);

STATIC const mp_rom_map_elem_t espnow_peers_locals_dict_table[] = {
    // Peer management functions
    { MP_ROM_QSTR(MP_QSTR_append),  MP_ROM_PTR(&espnow_peers_append_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove),  MP_ROM_PTR(&espnow_peers_remove_obj) },
};
STATIC MP_DEFINE_CONST_DICT(espnow_peers_locals_dict, espnow_peers_locals_dict_table);

/******************************************************************************/
/* peers print                                                                */

STATIC void espnow_peers_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_list_t *list = MP_OBJ_TO_PTR(self->list);
    return list->base.type->print(print, self->list, kind);
}

/******************************************************************************/
/* peers unary_op                                                             */

STATIC mp_obj_t espnow_peers_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_list_t *list = MP_OBJ_TO_PTR(self->list);
    return list->base.type->ext->unary_op(op, self->list);
}

/******************************************************************************/
/* peers subscript                                                            */

STATIC mp_obj_t espnow_peers_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    if (value != MP_OBJ_SENTINEL) {
        return MP_OBJ_NULL; // op not supported
    }
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_list_t *list = MP_OBJ_TO_PTR(self->list);
    return list->base.type->ext->subscr(self->list, index, value);
}

/******************************************************************************/
/* peers iterator                                                             */

STATIC mp_obj_t espnow_peers_getiter(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_list_t *list = MP_OBJ_TO_PTR(self->list);
    return list->base.type->ext->getiter(self->list, iter_buf);
}

espnow_peers_obj_t *espnow_peers_new(void) {
    espnow_peers_obj_t *self = m_new_obj(espnow_peers_obj_t);
    self->base.type = &espnow_peers_type;
    self->list = mp_obj_new_list(0, NULL);
    return self;
}

const mp_obj_type_t espnow_peers_type = {
    { &mp_type_type },
    .name = MP_QSTR_Peers,
    .print = espnow_peers_print,
    .locals_dict = (mp_obj_t)&espnow_peers_locals_dict,
    .flags = MP_TYPE_FLAG_EXTENDED,
    MP_TYPE_EXTENDED_FIELDS(
        .unary_op = espnow_peers_unary_op,
        .subscr = espnow_peers_subscr,
        .getiter = espnow_peers_getiter,
        ),
};
