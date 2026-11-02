// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame.StripDraw: an immediate-mode draw layer that holds no pixel buffer.

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
#include "shared-bindings/picogame/StripDraw.h"
#include "shared-module/picogame/__init__.h"
#include "shared-module/picogame/Bitmap.h"
#include "shared-module/picogame/Sprite.h"

// ---------------------------------------------------------------------------
// StripDraw  (immediate-mode draw layer; struct in shared-module/picogame/__init__.h)
// ---------------------------------------------------------------------------

//| class StripDraw:
//|     """An immediate-mode draw layer that holds no pixel buffer. On each refresh,
//|     for every render strip overlapping its rectangle, ``callback(view, vx, vy,
//|     vw, vh)`` is called with a `Canvas` view of the live strip buffer, so the
//|     callback draws primitives directly into the frame. StripDraw layers always
//|     draw in screen coordinates and ignore the scene's view offset.
//|
//|     ``(vx, vy)`` is the view origin in screen coordinates and ``(vw, vh)`` is
//|     its size: to draw a screen point ``(sx, sy)``, draw at ``(sx - vx, sy - vy)``
//|     in ``view``. The view may span the full render-region width even when the
//|     layer is narrower (the layer's rectangle only limits which rows are drawn),
//|     so fill your own rectangle with :py:meth:`Canvas.fill_rect` rather than
//|     ``view.clear()``, which fills the whole width.
//|
//|     Moving or resizing the layer by assigning :py:attr:`x`, :py:attr:`y`,
//|     :py:attr:`width` or :py:attr:`height` can leave the old area stale; call
//|     :py:meth:`Scene.invalidate` afterwards for a clean repaint."""
//|
//|     def __init__(
//|         self,
//|         callback: Callable[[Canvas, int, int, int, int], None],
//|         x: int = 0,
//|         y: int = 0,
//|         width: int = 0,
//|         height: int = 0,
//|         *,
//|         always_dirty: bool = True,
//|     ) -> None:
//|         """:param callback: called for each overlapping render strip as
//|             ``callback(view, vx, vy, vw, vh)``
//|         :param int x: left edge of the layer's screen rectangle
//|         :param int y: top edge of the layer's screen rectangle
//|         :param int width: rectangle width in pixels
//|         :param int height: rectangle height in pixels
//|         :param bool always_dirty: when `True` the layer redraws every refresh;
//|             when `False`, call :py:meth:`invalidate` after its content changes"""
//|         ...
//|
static mp_obj_t picogame_stripdraw_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_callback, ARG_x, ARG_y, ARG_width, ARG_height, ARG_always_dirty };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_callback, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_width, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_height, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_always_dirty, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    picogame_stripdraw_obj_t *self = mp_obj_malloc(picogame_stripdraw_obj_t, type);
    self->callback = args[ARG_callback].u_obj;
    self->x = args[ARG_x].u_int;
    self->y = args[ARG_y].u_int;
    self->w = args[ARG_width].u_int;
    self->h = args[ARG_height].u_int;
    self->faulted = false;
    self->always_dirty = args[ARG_always_dirty].u_bool;
    picogame_dirty_reset(&self->dx1);        // render once on first refresh (even when always_dirty=False)
    picogame_dirty_union(&self->dx1, self->x, self->y, self->x + self->w, self->y + self->h);
    // A buffer-less Canvas reused as the per-strip drawing view: its `data` is
    // repointed at the live strip each blit, so no surface RAM is allocated here.
    picogame_canvas_obj_t *view = mp_obj_malloc(picogame_canvas_obj_t, &picogame_canvas_type);
    view->data = NULL;
    view->data_obj = MP_OBJ_NULL;
    view->w = 0;
    view->h = 0;
    view->x = 0;
    view->y = 0;
    view->transparent = 0;
    view->has_transparent = false;
    picogame_canvas_dirty_reset(view);
    self->view = MP_OBJ_FROM_PTR(view);
    return MP_OBJ_FROM_PTR(self);
}

//|
//|     x: int
//|     """Left edge of the layer's screen rectangle."""
//|     y: int
//|     """Top edge of the layer's screen rectangle."""
//|     width: int
//|     """Width of the layer's screen rectangle in pixels."""
//|     height: int
//|     """Height of the layer's screen rectangle in pixels."""
static mp_obj_t sd_get_x(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->x);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_get_x_obj, sd_get_x);
static mp_obj_t sd_set_x(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->x = mp_obj_get_int(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_set_x_obj, sd_set_x);
MP_PROPERTY_GETSET(sd_x_obj, (mp_obj_t)&sd_get_x_obj, (mp_obj_t)&sd_set_x_obj);

static mp_obj_t sd_get_y(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->y);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_get_y_obj, sd_get_y);
static mp_obj_t sd_set_y(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->y = mp_obj_get_int(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_set_y_obj, sd_set_y);
MP_PROPERTY_GETSET(sd_y_obj, (mp_obj_t)&sd_get_y_obj, (mp_obj_t)&sd_set_y_obj);

static mp_obj_t sd_get_width(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->w);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_get_width_obj, sd_get_width);
static mp_obj_t sd_set_width(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->w = mp_obj_get_int(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_set_width_obj, sd_set_width);
MP_PROPERTY_GETSET(sd_width_obj, (mp_obj_t)&sd_get_width_obj, (mp_obj_t)&sd_set_width_obj);

static mp_obj_t sd_get_height(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->h);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_get_height_obj, sd_get_height);
static mp_obj_t sd_set_height(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->h = mp_obj_get_int(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_set_height_obj, sd_set_height);
MP_PROPERTY_GETSET(sd_height_obj, (mp_obj_t)&sd_get_height_obj, (mp_obj_t)&sd_set_height_obj);

//|     always_dirty: bool
//|     """When `True` (the default) the rectangle repaints every frame, for animated
//|     content. When `False` the layer renders once initially and then repaints only
//|     after an :py:meth:`invalidate` call or when overlapped by another dirty
//|     layer; call :py:meth:`invalidate` after each content change."""
//|
static mp_obj_t sd_get_always_dirty(mp_obj_t self_in) {
    return mp_obj_new_bool(((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->always_dirty);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_get_always_dirty_obj, sd_get_always_dirty);
static mp_obj_t sd_set_always_dirty(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->always_dirty = mp_obj_is_true(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_set_always_dirty_obj, sd_set_always_dirty);
MP_PROPERTY_GETSET(sd_always_dirty_obj, (mp_obj_t)&sd_get_always_dirty_obj, (mp_obj_t)&sd_set_always_dirty_obj);

//|     def invalidate(self, x: int = 0, y: int = 0, w: int = 0, h: int = 0) -> None:
//|         """Mark the layer dirty so it repaints on the next refresh; only needed
//|         when ``always_dirty`` is `False`.
//|
//|         With no arguments the whole layer repaints. To repaint one region, pass
//|         all four values as a rectangle in the view-local coordinates the draw
//|         callback uses; passing only some of them raises :py:class:`ValueError`.
//|         Repeated calls accumulate, and the rectangle is clamped to the layer."""
//|         ...
//|
//|
static mp_obj_t sd_invalidate(size_t n_args, const mp_obj_t *args) {
    picogame_stripdraw_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args != 1 && n_args != 5) {
        // A partial rectangle is a bug, not a request for "everything".
        mp_arg_error_invalid(MP_QSTR_rect);
    }
    if (n_args >= 5) {                       // (x, y, w, h) in view-local coords -> clamped scene rect
        int x1 = self->x + mp_obj_get_int(args[1]);
        int y1 = self->y + mp_obj_get_int(args[2]);
        int x2 = x1 + mp_obj_get_int(args[3]);
        int y2 = y1 + mp_obj_get_int(args[4]);
        if (x1 < self->x) {
            x1 = self->x;
        }
        if (y1 < self->y) {
            y1 = self->y;
        }
        if (x2 > self->x + self->w) {
            x2 = self->x + self->w;
        }
        if (y2 > self->y + self->h) {
            y2 = self->y + self->h;
        }
        if (x2 > x1 && y2 > y1) {
            picogame_dirty_union(&self->dx1, x1, y1, x2, y2);
        }
    } else {                                 // whole layer
        picogame_dirty_union(&self->dx1, self->x, self->y, self->x + self->w, self->y + self->h);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sd_invalidate_obj, 1, 5, sd_invalidate);

static const mp_rom_map_elem_t picogame_stripdraw_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&sd_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&sd_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&sd_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&sd_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_always_dirty), MP_ROM_PTR(&sd_always_dirty_obj) },
    { MP_ROM_QSTR(MP_QSTR_invalidate), MP_ROM_PTR(&sd_invalidate_obj) },
};
static MP_DEFINE_CONST_DICT(picogame_stripdraw_locals_dict, picogame_stripdraw_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picogame_stripdraw_type,
    MP_QSTR_StripDraw,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, picogame_stripdraw_make_new,
    locals_dict, &picogame_stripdraw_locals_dict
    );
