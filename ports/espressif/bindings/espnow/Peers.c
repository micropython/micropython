// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 MicroDev
//
// SPDX-License-Identifier: MIT

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
static mp_obj_t espnow_peers_append(mp_obj_t self_in, mp_obj_t arg) {
    espnow_peer_obj_t *peer = MP_OBJ_TO_PTR(mp_arg_validate_type(arg, &espnow_peer_type, MP_QSTR_Peer));
    CHECK_ESP_RESULT(esp_now_add_peer(&peer->peer_info));
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_list_append(self->list, arg);
}
static MP_DEFINE_CONST_FUN_OBJ_2(espnow_peers_append_obj, espnow_peers_append);

//|     def remove(self, peer: Peer) -> None:
//|         """Remove peer.
//|
//|         :param Peer peer: The peer object to remove.
//|         """
//|         ...
//|
static mp_obj_t espnow_peers_remove(mp_obj_t self_in, mp_obj_t arg) {
    espnow_peer_obj_t *peer = MP_OBJ_TO_PTR(mp_arg_validate_type(arg, &espnow_peer_type, MP_QSTR_Peer));
    CHECK_ESP_RESULT(esp_now_del_peer(peer->peer_info.peer_addr));
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_list_remove(self->list, arg);
}
static MP_DEFINE_CONST_FUN_OBJ_2(espnow_peers_remove_obj, espnow_peers_remove);

static const mp_rom_map_elem_t espnow_peers_locals_dict_table[] = {
    // Peer management functions
    { MP_ROM_QSTR(MP_QSTR_append),  MP_ROM_PTR(&espnow_peers_append_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove),  MP_ROM_PTR(&espnow_peers_remove_obj) },
};
static MP_DEFINE_CONST_DICT(espnow_peers_locals_dict, espnow_peers_locals_dict_table);

/******************************************************************************/
/* peers print                                                                */

static void espnow_peers_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_TYPE_GET_SLOT(mp_obj_get_type(self->list), print)(print, self->list, kind);
}

/******************************************************************************/
/* peers unary_op                                                             */

static mp_obj_t espnow_peers_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_TYPE_GET_SLOT(mp_obj_get_type(self->list), unary_op)(op, self->list);
}

/******************************************************************************/
/* peers subscript                                                            */

static mp_obj_t espnow_peers_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    if (value != MP_OBJ_SENTINEL) {
        return MP_OBJ_NULL; // op not supported
    }
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_TYPE_GET_SLOT(mp_obj_get_type(self->list), subscr)(self->list, index, value);
}

/******************************************************************************/
/* peers iterator                                                             */

static mp_obj_t espnow_peers_getiter(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return ((mp_getiter_fun_t)MP_OBJ_TYPE_GET_SLOT(mp_obj_get_type(self->list), iter))(self->list, iter_buf);
}

espnow_peers_obj_t *espnow_peers_new(void) {
    espnow_peers_obj_t *self = mp_obj_malloc(espnow_peers_obj_t, &espnow_peers_type);
    self->list = mp_obj_new_list(0, NULL);
    return self;
}

MP_DEFINE_CONST_OBJ_TYPE(
    espnow_peers_type,
    MP_QSTR_Peers,
    MP_TYPE_FLAG_ITER_IS_GETITER,
    print, espnow_peers_print,
    locals_dict, &espnow_peers_locals_dict,
    unary_op, espnow_peers_unary_op,
    subscr, espnow_peers_subscr,
    iter, espnow_peers_getiter
    );
