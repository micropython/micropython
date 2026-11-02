// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame.Triangles: a retained screen-space triangle batch rasterised in C.

#include "py/runtime.h"
#include "shared-module/picogame/pg_compat.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-bindings/picogame/__init__.h"
#include "shared-bindings/picogame/Bitmap.h"
#include "shared-bindings/picogame/Sprite.h"
#if CIRCUITPY_PICOGAME_FAST_DISPLAY
#include "shared-bindings/picogame/Display.h"   // fast DMA backend; absent on portable ports
#include "shared-module/picogame/Display.h"        // its struct (pg_get_display unwraps the wrapper)
#endif
#include "shared-bindings/picogame/Scene.h"
#include "shared-bindings/picogame/Tilemap.h"
#include "shared-bindings/picogame/Particles.h"
#include "shared-bindings/picogame/Canvas.h"
#include "shared-bindings/picogame/Framebuffer.h"
#include "shared-bindings/picogame/Triangles.h"
#include "shared-module/picogame/__init__.h"
#include "shared-module/picogame/Bitmap.h"
#include "shared-module/picogame/Sprite.h"

//| class Triangles:
//|     """A retained triangle batch drawn as a `Scene` layer. The batch is
//|     rasterized directly into each render strip; no per-strip Python runs and no
//|     pixel buffer is held. Triangles layers always draw in screen coordinates and
//|     ignore the scene's view offset.
//|
//|     For each frame: project points with :py:func:`project`, write triangles
//|     into the buffers in back-to-front order, set :py:attr:`count`, then call
//|     :py:meth:`Scene.refresh`."""
//|
//|     def __init__(self, verts: ReadableBuffer, colors: ReadableBuffer) -> None:
//|         """:param ~circuitpython_typing.ReadableBuffer verts: caller-owned buffer of
//|             ``int16`` values, six per triangle: x0, y0, x1, y1, x2, y2. Refill it in
//|             place each frame.
//|         :param ~circuitpython_typing.ReadableBuffer colors: caller-owned buffer of
//|             ``uint16`` colors, one per triangle"""
//|         ...
//|
static mp_obj_t picogame_triangles_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    picogame_triangles_obj_t *self = mp_obj_malloc(picogame_triangles_obj_t, type);
    mp_buffer_info_t vi, ci;
    mp_get_buffer_raise(all_args[0], &vi, MP_BUFFER_READ);
    mp_get_buffer_raise(all_args[1], &ci, MP_BUFFER_READ);
    self->verts_obj = all_args[0];
    self->colors_obj = all_args[1];
    self->verts = (const int16_t *)vi.buf;
    self->colors = (const uint16_t *)ci.buf;
    size_t cap_v = vi.len / 12;                    // 6 int16 = 12 bytes per triangle
    size_t cap_c = ci.len >> 1;
    self->cap = (uint16_t)(cap_v < cap_c ? cap_v : cap_c);
    self->count = 0;
    picogame_dirty_reset(&self->dx1);
    return MP_OBJ_FROM_PTR(self);
}

//|     count: int
//|     """How many triangles of the batch draw on the next refresh, clamped to the
//|     buffer capacity. Assigning marks the layer dirty for a full repaint."""
//|
//|
static mp_obj_t tri_get_count(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_triangles_obj_t *)MP_OBJ_TO_PTR(self_in))->count);
}
static MP_DEFINE_CONST_FUN_OBJ_1(tri_get_count_obj, tri_get_count);
static mp_obj_t tri_set_count(mp_obj_t self_in, mp_obj_t v) {
    picogame_triangles_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int n = mp_obj_get_int(v);
    if (n < 0) {
        n = 0;
    }
    if (n > self->cap) {
        n = self->cap;
    }
    self->count = (uint16_t)n;
    picogame_dirty_union(&self->dx1, 0, 0, 32767, 32767);   // clipped to the play rect later
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tri_set_count_obj, tri_set_count);
MP_PROPERTY_GETSET(tri_count_obj, (mp_obj_t)&tri_get_count_obj, (mp_obj_t)&tri_set_count_obj);

static const mp_rom_map_elem_t picogame_triangles_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&tri_count_obj) },
};
static MP_DEFINE_CONST_DICT(picogame_triangles_locals_dict, picogame_triangles_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picogame_triangles_type,
    MP_QSTR_Triangles,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, picogame_triangles_make_new,
    locals_dict, &picogame_triangles_locals_dict
    );
