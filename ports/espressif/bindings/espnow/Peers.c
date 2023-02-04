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

// TODO: Check for deinit

//| class Peers:
//|     """A class that provides peer managment functions. Sequence[Peer]."""
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
    const char *item_separator = ", ";
    if (!(MICROPY_PY_UJSON && kind == PRINT_JSON)) {
        kind = PRINT_REPR;
    } else {
        #if MICROPY_PY_UJSON_SEPARATORS
        item_separator = MP_PRINT_GET_EXT(print)->item_separator;
        #endif
    }
    mp_print_str(print, "[");
    for (size_t i = 0; i < list->len; i++) {
        if (i > 0) {
            mp_print_str(print, item_separator);
        }
        mp_obj_print_helper(print, list->items[i], kind);
    }
    mp_print_str(print, "]");
}

/******************************************************************************/
/* peers unary_op                                                             */

STATIC mp_obj_t espnow_peers_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_list_t *list = MP_OBJ_TO_PTR(self->list);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(list->len != 0);
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(list->len);
        #if MICROPY_PY_SYS_GETSIZEOF
        case MP_UNARY_OP_SIZEOF: {
            size_t sz = sizeof(*list) + sizeof(mp_obj_t) * list->alloc;
            return MP_OBJ_NEW_SMALL_INT(sz);
        }
        #endif
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

/******************************************************************************/
/* peers subscript                                                            */

STATIC mp_obj_t espnow_peers_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    if (value != MP_OBJ_SENTINEL) {
        return MP_OBJ_NULL; // op not supported
    }

    espnow_peers_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_list_t *list = MP_OBJ_TO_PTR(self->list);

    // load
    #if MICROPY_PY_BUILTINS_SLICE
    if (mp_obj_is_type(index, &mp_type_slice)) {
        mp_bound_slice_t slice;
        if (!mp_seq_get_fast_slice_indexes(list->len, index, &slice)) {
            return mp_seq_extract_slice(list->len, list->items, &slice);
        }

        mp_obj_list_t *res = MP_OBJ_TO_PTR(mp_obj_new_list(slice.stop - slice.start, NULL));
        mp_seq_copy(res->items, list->items + slice.start, res->len, mp_obj_t);
        return MP_OBJ_FROM_PTR(res);
    }
    #endif
    size_t index_val = mp_get_index(list->base.type, list->len, index, false);
    return list->items[index_val];
}

/******************************************************************************/
/* peers iterator                                                             */

typedef struct _espnow_peers_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t peers;
    size_t cur;
} espnow_peers_it_t;

STATIC mp_obj_t espnow_peers_it_iternext(mp_obj_t self_in) {
    espnow_peers_it_t *self = MP_OBJ_TO_PTR(self_in);
    espnow_peers_obj_t *peers = MP_OBJ_TO_PTR(self->peers);
    mp_obj_list_t *list = MP_OBJ_TO_PTR(peers->list);
    if (self->cur < list->len) {
        mp_obj_t o_out = list->items[self->cur];
        self->cur += 1;
        return o_out;
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

STATIC mp_obj_t espnow_peers_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(espnow_peers_it_t) <= sizeof(mp_obj_iter_buf_t));
    espnow_peers_it_t *o = (espnow_peers_it_t *)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = espnow_peers_it_iternext;
    o->peers = o_in;
    o->cur = 0;
    return MP_OBJ_FROM_PTR(o);
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
