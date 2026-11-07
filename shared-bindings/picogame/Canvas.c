// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "shared-module/picogame/pg_compat.h"
#include "shared-bindings/picogame/Canvas.h"
#include "shared-bindings/picogame/Bitmap.h"
#if CIRCUITPY_FONTIO
#include "shared-bindings/fontio/BuiltinFont.h"
#endif
#include "shared-module/picogame/Canvas.h"

//| class Canvas:
//|     """A RAM drawing surface composited as a `Scene` layer. Draw primitives
//|     into it; only redrawn areas repaint. Colors come from
//|     :py:func:`rgb565`."""
//|
//|     def __init__(
//|         self,
//|         width: int,
//|         height: int,
//|         *,
//|         transparent: Optional[int] = None,
//|         buffer: Optional[WriteableBuffer] = None,
//|     ) -> None:
//|         """:param int width: surface width in pixels
//|         :param int height: surface height in pixels
//|         :param int transparent: color that is skipped when the canvas is
//|             composited; `None` makes every pixel opaque
//|         :param ~circuitpython_typing.WriteableBuffer buffer: optional caller-owned
//|             pixel buffer of at least ``width * height * 2`` bytes (for example a
//|             ``bytearray`` or a writable ``memoryview``). The canvas draws into it
//|             instead of allocating its own."""
//|         ...
//|
static mp_obj_t picogame_canvas_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_width, ARG_height, ARG_transparent, ARG_buffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_transparent, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_buffer, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t w = mp_arg_validate_int_range(args[ARG_width].u_int, 1, 1024, MP_QSTR_width);
    mp_int_t h = mp_arg_validate_int_range(args[ARG_height].u_int, 1, 1024, MP_QSTR_height);

    picogame_canvas_obj_t *self = mp_obj_malloc(picogame_canvas_obj_t, type);
    self->w = w;
    self->h = h;
    self->x = 0;
    self->y = 0;
    if (args[ARG_buffer].u_obj != mp_const_none) {
        // external buffer (e.g. an arena slice) - draw into it, don't allocate/own it
        mp_buffer_info_t bi;
        mp_get_buffer_raise(args[ARG_buffer].u_obj, &bi, MP_BUFFER_RW);
        if (bi.len < (size_t)w * h * 2) {
            mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
        }
        if ((uintptr_t)bi.buf & 1) {    // odd byte address -> uint16 pixel stores fault on Cortex-M0+
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Buffer must be a multiple of %d bytes"), 2);
        }
        self->data = bi.buf;
        self->data_obj = args[ARG_buffer].u_obj;   // keep the backing object alive (GC-traced)
    } else {
        // Pure pixel data, no Python pointers -> exempt from the conservative GC scan
        // (shorter gc.collect() pauses; a 150x60 canvas is 18 KB the mark phase can skip).
        self->data = m_malloc_without_collect((size_t)w * h * sizeof(uint16_t));
        self->data_obj = MP_OBJ_NULL;
    }
    if (args[ARG_transparent].u_obj != mp_const_none) {
        self->transparent = mp_obj_get_int(args[ARG_transparent].u_obj);
        self->has_transparent = true;
    } else {
        self->transparent = 0;
        self->has_transparent = false;
    }
    // clear() marks the surface dirty and reads the accumulator, so reset it on both sides.
    picogame_canvas_dirty_reset(self);
    picogame_canvas_clear(self, self->has_transparent ? self->transparent : 0);
    picogame_canvas_dirty_reset(self);
    return MP_OBJ_FROM_PTR(self);
}

static picogame_canvas_obj_t *cv_self(mp_obj_t o) {
    return MP_OBJ_TO_PTR(o);
}

// Shared int-arg unpacker for the plain drawing trampolines below: every one of them is
// "self + N ints -> void", and the inlined per-wrapper mp_obj_get_int runs cost ~70-120 B
// each at -Os. One loop here + a tiny per-wrapper stub keeps the flash cost per method at
// ~2 calls. n comes from the VAR_BETWEEN exact arity, so v[] is always fully written.
// Call it in its own statement: the order a call's arguments are evaluated in is unspecified,
// so v[] read beside cv_args() in one argument list may be read before it is written.
static picogame_canvas_obj_t *cv_args(const mp_obj_t *a, size_t n, int *v) {
    for (size_t i = 1; i < n; i++) {
        v[i - 1] = mp_obj_get_int(a[i]);
    }
    return cv_self(a[0]);
}

//|     def clear(self, color: int) -> None:
//|         """Fill the whole surface with ``color``."""
//|         ...
//|
static mp_obj_t canvas_clear(mp_obj_t self_in, mp_obj_t color) {
    picogame_canvas_clear(cv_self(self_in), mp_obj_get_int(color));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(canvas_clear_obj, canvas_clear);

//|
//|     def pixel(self, x: int, y: int, color: int) -> None:
//|         """Set a single pixel."""
//|         ...
//|
static mp_obj_t canvas_pixel(size_t n, const mp_obj_t *a) {
    int v[3];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_pixel(cv, v[0], v[1], v[2]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_pixel_obj, 4, 4, canvas_pixel);

//|
//|     def fill_rect(self, x: int, y: int, w: int, h: int, color: int) -> None:
//|         """Fill an axis-aligned rectangle."""
//|         ...
//|
static mp_obj_t canvas_fill_rect(size_t n, const mp_obj_t *a) {
    int v[5];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_fill_rect(cv, v[0], v[1], v[2], v[3], v[4]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_fill_rect_obj, 6, 6, canvas_fill_rect);

//|
//|     def blit(
//|         self,
//|         bitmap: Bitmap,
//|         x: int,
//|         y: int,
//|         frame: int = 0,
//|         flip_x: bool = False,
//|         flip_y: bool = False,
//|     ) -> None:
//|         """Copy one frame of ``bitmap``, selected by ``frame``, into the canvas
//|         at ``(x, y)``, honoring the bitmap's transparent color."""
//|         ...
//|
static mp_obj_t canvas_blit(size_t n, const mp_obj_t *a) {
    picogame_bitmap_obj_t *bm = MP_OBJ_TO_PTR(
        mp_arg_validate_type(a[1], &picogame_bitmap_type, MP_QSTR_bitmap));
    // Same domain as Sprite.frame (uint8): a negative frame would survive the blitter's
    // wrap (C % keeps the sign) and read before the sheet data.
    int frame = (n > 4) ? (int)mp_arg_validate_int_range(mp_obj_get_int(a[4]), 0, 255, MP_QSTR_frame) : 0;
    bool fx = (n > 5) ? mp_obj_is_true(a[5]) : false;
    bool fy = (n > 6) ? mp_obj_is_true(a[6]) : false;
    picogame_canvas_blit(cv_self(a[0]), bm, mp_obj_get_int(a[2]), mp_obj_get_int(a[3]), frame, fx, fy);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_blit_obj, 4, 7, canvas_blit);

//|     def mode7(
//|         self,
//|         texture: Bitmap,
//|         horizon: int,
//|         y_off: int,
//|         z: int,
//|         rx0: int,
//|         ry0: int,
//|         rsx: int,
//|         rsy: int,
//|         cam_x: int,
//|         cam_y: int,
//|     ) -> None:
//|         """Fill rows below ``horizon`` with a perspective projection of ``texture``.
//|         The ``picogame_mode7`` helper computes the camera terms from an angle,
//|         position and field of view.
//|
//|         :param Bitmap texture: texture whose width and height are powers of two
//|         :param int horizon: first canvas row to fill
//|         :param int y_off: vertical offset of the projection, in rows
//|         :param int z: camera height (signed 16.16 fixed-point)
//|         :param int rx0: ray x at the left column (signed 16.16 fixed-point)
//|         :param int ry0: ray y at the left column (signed 16.16 fixed-point)
//|         :param int rsx: per-column ray step x (signed 16.16 fixed-point)
//|         :param int rsy: per-column ray step y (signed 16.16 fixed-point)
//|         :param int cam_x: camera x position (signed 16.16 fixed-point)
//|         :param int cam_y: camera y position (signed 16.16 fixed-point)"""
//|         ...
//|
static mp_obj_t canvas_mode7(size_t n, const mp_obj_t *a) {
    picogame_bitmap_obj_t *tex = MP_OBJ_TO_PTR(
        mp_arg_validate_type(a[1], &picogame_bitmap_type, MP_QSTR_texture));
    picogame_canvas_mode7(cv_self(a[0]), tex,
        mp_obj_get_int(a[2]), mp_obj_get_int(a[3]), mp_obj_get_int(a[4]),
        mp_obj_get_int(a[5]), mp_obj_get_int(a[6]), mp_obj_get_int(a[7]),
        mp_obj_get_int(a[8]), mp_obj_get_int(a[9]), mp_obj_get_int(a[10]));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_mode7_obj, 11, 11, canvas_mode7);

//|     def fill_triangles(
//|         self,
//|         verts: ReadableBuffer,
//|         colors: ReadableBuffer,
//|         n: int,
//|         x_off: int = 0,
//|         y_off: int = 0,
//|     ) -> None:
//|         """Fill a batch of triangles in one call, which is faster than repeated
//|         :py:meth:`fill_triangle` calls for many triangles.
//|
//|         :param ~circuitpython_typing.ReadableBuffer verts: ``6 * n`` ``int16``
//|             values: x0, y0, x1, y1, x2, y2 per triangle
//|         :param ~circuitpython_typing.ReadableBuffer colors: ``n`` ``uint16`` colors,
//|             one per triangle
//|         :param int n: number of triangles
//|         :param int x_off: added to every x before clipping
//|         :param int y_off: added to every y before clipping
//|
//|         The offsets translate the whole batch, so one screen-space batch can be
//|         replayed into each `StripDraw` view by passing the negated view origin
//|         (``x_off=-vx, y_off=-vy``); triangles outside the view are skipped."""
//|         ...
//|
static mp_obj_t canvas_fill_triangles(size_t na, const mp_obj_t *a) {
    picogame_canvas_obj_t *cv = cv_self(a[0]);
    mp_buffer_info_t vi, ci;
    mp_get_buffer_raise(a[1], &vi, MP_BUFFER_READ);
    mp_get_buffer_raise(a[2], &ci, MP_BUFFER_READ);
    int n = mp_obj_get_int(a[3]);
    int xo = na > 4 ? mp_obj_get_int(a[4]) : 0;
    int yo = na > 5 ? mp_obj_get_int(a[5]) : 0;
    const int16_t *v = vi.buf;
    const uint16_t *col = ci.buf;
    int cap_v = (int)(vi.len / 12);        // 6 int16 = 12 bytes per triangle
    int cap_c = (int)(ci.len >> 1);
    if (n > cap_v) {
        n = cap_v;
    }
    if (n > cap_c) {
        n = cap_c;
    }
    picogame_fill_triangle_batch(cv, v, col, n, xo, yo);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_fill_triangles_obj, 4, 6, canvas_fill_triangles);

//|     def vspans(
//|         self,
//|         x0s: ReadableBuffer,
//|         x1s: ReadableBuffer,
//|         tops: ReadableBuffer,
//|         bots: ReadableBuffer,
//|         colors: ReadableBuffer,
//|         n: int,
//|         x_off: int = 0,
//|         y_off: int = 0,
//|     ) -> None:
//|         """Fill a batch of vertical color spans in one call. Span ``i`` covers
//|         columns ``x0s[i]`` through ``x1s[i]`` (exclusive) and rows ``tops[i]``
//|         through ``bots[i]`` (exclusive) in color ``colors[i]``.
//|
//|         :param ~circuitpython_typing.ReadableBuffer x0s: ``n`` ``uint16`` left edges
//|         :param ~circuitpython_typing.ReadableBuffer x1s: ``n`` ``uint16`` exclusive
//|             right edges
//|         :param ~circuitpython_typing.ReadableBuffer tops: ``n`` ``uint16`` top rows
//|         :param ~circuitpython_typing.ReadableBuffer bots: ``n`` ``uint16`` exclusive
//|             bottom rows
//|         :param ~circuitpython_typing.ReadableBuffer colors: ``n`` ``uint16`` colors
//|         :param int n: number of spans
//|         :param int x_off: added to every x before clipping
//|         :param int y_off: added to every y before clipping
//|
//|         The offsets translate the whole batch, so one screen-space batch (for
//|         example `raycast` wall runs) can be replayed into each `StripDraw` view by
//|         passing the negated view origin; spans outside the view are skipped."""
//|         ...
//|
static mp_obj_t canvas_vspans(size_t na, const mp_obj_t *a) {
    picogame_canvas_obj_t *cv = cv_self(a[0]);
    // five equal-shape uint16 arrays in a row: fetch + shortest-length fold in one loop
    mp_buffer_info_t bi5[5];
    size_t cap = (size_t)-1;
    for (int i = 0; i < 5; i++) {
        mp_get_buffer_raise(a[1 + i], &bi5[i], MP_BUFFER_READ);
        if (bi5[i].len < cap) {
            cap = bi5[i].len;
        }
    }
    int n = mp_obj_get_int(a[6]);
    int xo = na > 7 ? mp_obj_get_int(a[7]) : 0;
    int yo = na > 8 ? mp_obj_get_int(a[8]) : 0;
    if (n > (int)(cap >> 1)) {
        n = (int)(cap >> 1);                     // never read past the shortest array
    }
    const uint16_t *x0s = bi5[0].buf;
    const uint16_t *x1s = bi5[1].buf;
    const uint16_t *tops = bi5[2].buf;
    const uint16_t *bots = bi5[3].buf;
    const uint16_t *cols = bi5[4].buf;
    int cw = cv->w, ch = cv->h;
    for (int i = 0; i < n; i++) {
        int t = tops[i] + yo, b = bots[i] + yo;
        if (b <= 0 || t >= ch || b <= t) {
            continue;                      // span outside this band
        }
        int x0 = x0s[i] + xo, x1 = x1s[i] + xo;
        if (x1 <= 0 || x0 >= cw || x1 <= x0) {
            continue;
        }
        picogame_canvas_fill_rect(cv, x0, t, x1 - x0, b - t, cols[i]);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_vspans_obj, 7, 9, canvas_vspans);

//|     def road(
//|         self,
//|         ri0: int,
//|         tab: ReadableBuffer,
//|         rl: ReadableBuffer,
//|         rr: ReadableBuffer,
//|         d05_q8: int,
//|         d07_q8: int,
//|         colors: ReadableBuffer,
//|     ) -> None:
//|         """Draw one racing-road strip from precomputed tables. A low-level
//|         interface used by the ``picogame_road`` helper, which builds the tables.
//|
//|         :param int ri0: road-table row of this surface's row 0; negative values
//|             are sky rows
//|         :param ~circuitpython_typing.ReadableBuffer tab: ``int16`` rows of
//|             ``edge_w``, ``dash_hw``, ``wb05_q8``, ``wb07_q8``, ``flags``
//|         :param ~circuitpython_typing.ReadableBuffer rl: ``int16`` per-row left
//|             edges, as computed by :py:func:`road_edges`
//|         :param ~circuitpython_typing.ReadableBuffer rr: ``int16`` per-row right
//|             edges
//|         :param int d05_q8: scroll phase (Q8 fixed-point)
//|         :param int d07_q8: scroll phase (Q8 fixed-point)
//|         :param ~circuitpython_typing.ReadableBuffer colors: six ``uint16`` colors,
//|             in order: sky, road a, road b, rumble a, rumble b, dash"""
//|         ...
//|
static mp_obj_t canvas_road(size_t n, const mp_obj_t *a) {
    mp_buffer_info_t tabi, rli, rri, coli;
    mp_get_buffer_raise(a[2], &tabi, MP_BUFFER_READ);
    mp_get_buffer_raise(a[3], &rli, MP_BUFFER_READ);
    mp_get_buffer_raise(a[4], &rri, MP_BUFFER_READ);
    mp_get_buffer_raise(a[7], &coli, MP_BUFFER_READ);
    int ntab = (int)(tabi.len / (5 * 2));
    int nedge = (int)(rli.len < rri.len ? rli.len : rri.len) / 2;
    if (nedge < ntab) {
        ntab = nedge;                                // never read past the shorter per-frame arrays
    }
    if (ntab <= 0 || coli.len < 6 * 2) {
        return mp_const_none;
    }
    picogame_canvas_road(cv_self(a[0]), mp_obj_get_int(a[1]),
        (const int16_t *)tabi.buf, ntab, (const int16_t *)rli.buf, (const int16_t *)rri.buf,
        mp_obj_get_int(a[5]), mp_obj_get_int(a[6]), (const uint16_t *)coli.buf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_road_obj, 8, 8, canvas_road);

//|     def rect(self, x: int, y: int, w: int, h: int, color: int) -> None:
//|         """Draw a one-pixel rectangle outline."""
//|         ...
//|
static mp_obj_t canvas_rect(size_t n, const mp_obj_t *a) {
    int v[5];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_rect(cv, v[0], v[1], v[2], v[3], v[4]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_rect_obj, 6, 6, canvas_rect);

//|
//|     def line(self, x0: int, y0: int, x1: int, y1: int, color: int) -> None:
//|         """Draw a one-pixel line between two points."""
//|         ...
//|
static mp_obj_t canvas_line(size_t n, const mp_obj_t *a) {
    int v[5];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_line(cv, v[0], v[1], v[2], v[3], v[4]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_line_obj, 6, 6, canvas_line);

//|
//|     def fill_circle(self, cx: int, cy: int, r: int, color: int) -> None:
//|         """Fill a circle of radius ``r`` centered on ``(cx, cy)``."""
//|         ...
//|
static mp_obj_t canvas_fill_circle(size_t n, const mp_obj_t *a) {
    int v[4];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_fill_circle(cv, v[0], v[1], v[2], v[3]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_fill_circle_obj, 5, 5, canvas_fill_circle);

//|
//|     def circle(self, cx: int, cy: int, r: int, color: int) -> None:
//|         """Draw a one-pixel circle outline."""
//|         ...
//|
static mp_obj_t canvas_circle(size_t n, const mp_obj_t *a) {
    int v[4];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_circle(cv, v[0], v[1], v[2], v[3]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_circle_obj, 5, 5, canvas_circle);

//|
//|     def ring(self, cx: int, cy: int, r: int, thickness: int, color: int) -> None:
//|         """Draw a circle outline ``thickness`` pixels wide, grown inwards from radius ``r``."""
//|         ...
//|
static mp_obj_t canvas_ring(size_t n, const mp_obj_t *a) {
    int v[5];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_ring(cv, v[0], v[1], v[2], v[3], v[4]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_ring_obj, 6, 6, canvas_ring);

//|
//|     def triangle(
//|         self, x0: int, y0: int, x1: int, y1: int, x2: int, y2: int, color: int
//|     ) -> None:
//|         """Draw a one-pixel triangle outline through the three points."""
//|         ...
//|
static mp_obj_t canvas_triangle(size_t n, const mp_obj_t *a) {
    int v[7];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_triangle(cv, v[0], v[1], v[2], v[3], v[4], v[5], v[6]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_triangle_obj, 8, 8, canvas_triangle);

//|
//|     def fill_triangle(
//|         self, x0: int, y0: int, x1: int, y1: int, x2: int, y2: int, color: int
//|     ) -> None:
//|         """Fill a triangle. See :py:meth:`fill_triangles` to submit a whole batch
//|         in one call."""
//|         ...
//|
static mp_obj_t canvas_fill_triangle(size_t n, const mp_obj_t *a) {
    int v[7];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_fill_triangle(cv, v[0], v[1], v[2], v[3], v[4], v[5], v[6]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_fill_triangle_obj, 8, 8, canvas_fill_triangle);

//|
//|     def ellipse(self, cx: int, cy: int, rx: int, ry: int, color: int) -> None:
//|         """Draw a one-pixel ellipse outline with radii ``rx``/``ry``."""
//|         ...
//|
static mp_obj_t canvas_ellipse(size_t n, const mp_obj_t *a) {
    int v[5];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_ellipse(cv, v[0], v[1], v[2], v[3], v[4]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_ellipse_obj, 6, 6, canvas_ellipse);

//|
//|     def fill_ellipse(self, cx: int, cy: int, rx: int, ry: int, color: int) -> None:
//|         """Fill an ellipse with radii ``rx``/``ry``."""
//|         ...
//|
static mp_obj_t canvas_fill_ellipse(size_t n, const mp_obj_t *a) {
    int v[5];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_fill_ellipse(cv, v[0], v[1], v[2], v[3], v[4]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_fill_ellipse_obj, 6, 6, canvas_fill_ellipse);

//|
//|     def fill_round_rect(self, x: int, y: int, w: int, h: int, r: int, color: int) -> None:
//|         """Fill a rectangle with corners rounded to radius ``r``."""
//|         ...
//|
static mp_obj_t canvas_fill_round_rect(size_t n, const mp_obj_t *a) {
    int v[6];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_fill_round_rect(cv, v[0], v[1], v[2], v[3], v[4], v[5]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_fill_round_rect_obj, 7, 7, canvas_fill_round_rect);

//|
//|     def frame3d(self, x: int, y: int, w: int, h: int, light: int, dark: int) -> None:
//|         """Draw a one-pixel bevelled frame: top and left edges in ``light``, bottom and
//|         right in ``dark``, giving a raised look (swap the two for a sunken one)."""
//|         ...
//|
static mp_obj_t canvas_frame3d(size_t n, const mp_obj_t *a) {
    int v[6];
    picogame_canvas_obj_t *cv = cv_args(a, n, v);
    picogame_canvas_frame3d(cv, v[0], v[1], v[2], v[3], v[4], v[5]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_frame3d_obj, 7, 7, canvas_frame3d);

//|
//|     def text(
//|         self, x: int, y: int, s: str, fg: int, font: fontio.BuiltinFont, bg: int | None = None
//|     ) -> None:
//|         """Draw ``s`` into the surface, rasterizing each glyph from ``font`` as it
//|         is drawn; no memory is retained between calls. Only ASCII characters are
//|         supported. If ``bg`` is given the glyph background is filled with it,
//|         otherwise it is transparent. Inside a `StripDraw` callback the view is a
//|         Canvas, so ``view.text(...)`` draws text directly into the frame.
//|         Raises :py:class:`NotImplementedError` in builds without `fontio`."""
//|         ...
//|
static mp_obj_t canvas_text(size_t n, const mp_obj_t *a) {
    #if CIRCUITPY_FONTIO
    const char *s = mp_obj_str_get_str(a[3]);
    mp_int_t fg = mp_obj_get_int(a[4]);
    const void *font = MP_OBJ_TO_PTR(mp_arg_validate_type(a[5], &fontio_builtinfont_type, MP_QSTR_font));
    bool has_bg = (n >= 7) && (a[6] != mp_const_none);
    uint16_t bg = has_bg ? (uint16_t)mp_obj_get_int(a[6]) : 0;
    picogame_canvas_text(cv_self(a[0]), mp_obj_get_int(a[1]), mp_obj_get_int(a[2]),
        s, (uint16_t)fg, bg, has_bg, font);
    return mp_const_none;
    #else
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Operation or feature not supported"));
    #endif
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canvas_text_obj, 6, 7, canvas_text);

//|     def move(self, x: int, y: int) -> None:
//|         """Move the surface's top-left corner to (x, y) and mark both the old and new area dirty."""
//|         ...
//|
static mp_obj_t canvas_move(mp_obj_t self_in, mp_obj_t x_in, mp_obj_t y_in) {
    picogame_canvas_obj_t *self = cv_self(self_in);
    int nx = mp_obj_get_int(x_in), ny = mp_obj_get_int(y_in);
    if (nx == self->x && ny == self->y) {
        return mp_const_none;                        // unchanged -> avoid an avoidable repaint
    }
    // dirty old + new extents so the move repaints both
    picogame_canvas_dirty_union(self, self->x, self->y, self->x + self->w, self->y + self->h);
    self->x = nx;
    self->y = ny;
    picogame_canvas_dirty_union(self, self->x, self->y, self->x + self->w, self->y + self->h);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(canvas_move_obj, canvas_move);

//|
//|     x: int
//|     """Horizontal position of the canvas top-left corner. (read-only)
//|
//|     Set with :py:meth:`move`."""
//|     y: int
//|     """Vertical position of the canvas top-left corner. (read-only)
//|
//|     Set with :py:meth:`move`."""
//|     width: int
//|     """Surface width in pixels. (read-only)"""
//|     height: int
//|     """Surface height in pixels. (read-only)"""
//|
//|
static mp_obj_t canvas_get_x(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(cv_self(self_in)->x);
}
static MP_DEFINE_CONST_FUN_OBJ_1(canvas_get_x_obj, canvas_get_x);
MP_PROPERTY_GETTER(canvas_x_obj, (mp_obj_t)&canvas_get_x_obj);

static mp_obj_t canvas_get_y(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(cv_self(self_in)->y);
}
static MP_DEFINE_CONST_FUN_OBJ_1(canvas_get_y_obj, canvas_get_y);
MP_PROPERTY_GETTER(canvas_y_obj, (mp_obj_t)&canvas_get_y_obj);

static mp_obj_t canvas_get_width(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(cv_self(self_in)->w);
}
static MP_DEFINE_CONST_FUN_OBJ_1(canvas_get_width_obj, canvas_get_width);
MP_PROPERTY_GETTER(canvas_width_obj, (mp_obj_t)&canvas_get_width_obj);

static mp_obj_t canvas_get_height(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(cv_self(self_in)->h);
}
static MP_DEFINE_CONST_FUN_OBJ_1(canvas_get_height_obj, canvas_get_height);
MP_PROPERTY_GETTER(canvas_height_obj, (mp_obj_t)&canvas_get_height_obj);

static const mp_rom_map_elem_t picogame_canvas_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&canvas_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&canvas_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&canvas_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&canvas_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&canvas_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&canvas_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_rect), MP_ROM_PTR(&canvas_fill_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&canvas_blit_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode7), MP_ROM_PTR(&canvas_mode7_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_triangles), MP_ROM_PTR(&canvas_fill_triangles_obj) },
    { MP_ROM_QSTR(MP_QSTR_vspans), MP_ROM_PTR(&canvas_vspans_obj) },
    { MP_ROM_QSTR(MP_QSTR_road), MP_ROM_PTR(&canvas_road_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&canvas_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&canvas_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_circle), MP_ROM_PTR(&canvas_fill_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&canvas_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_ring), MP_ROM_PTR(&canvas_ring_obj) },
    { MP_ROM_QSTR(MP_QSTR_triangle), MP_ROM_PTR(&canvas_triangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_triangle), MP_ROM_PTR(&canvas_fill_triangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_ellipse), MP_ROM_PTR(&canvas_ellipse_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_ellipse), MP_ROM_PTR(&canvas_fill_ellipse_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_round_rect), MP_ROM_PTR(&canvas_fill_round_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame3d), MP_ROM_PTR(&canvas_frame3d_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&canvas_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_move), MP_ROM_PTR(&canvas_move_obj) },
};
static MP_DEFINE_CONST_DICT(picogame_canvas_locals_dict, picogame_canvas_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picogame_canvas_type,
    MP_QSTR_Canvas,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, picogame_canvas_make_new,
    locals_dict, &picogame_canvas_locals_dict
    );
