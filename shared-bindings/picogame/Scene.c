// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include <string.h>
#include "shared-module/picogame/pg_compat.h"
#include "py/objtuple.h"
#include "py/objlist.h"
#include "shared-bindings/picogame/Scene.h"
#include "shared-bindings/picogame/__init__.h"
#include "shared-bindings/picogame/Sprite.h"
#include "shared-bindings/picogame/Tilemap.h"
#include "shared-bindings/picogame/Particles.h"
#include "shared-bindings/picogame/Canvas.h"
#include "shared-bindings/picogame/Framebuffer.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#if CIRCUITPY_PICOGAME_FAST_DISPLAY
#include "shared-bindings/picogame/Display.h"
#endif
#include "shared-module/picogame/Scene.h"
#include "shared-module/picogame/__init__.h"
#include "shared-module/picogame/Tilemap.h"
#include "shared-module/picogame/Particles.h"
#include "shared-module/picogame/Canvas.h"
#if CIRCUITPY_PICOGAME_FAST_DISPLAY
#include "shared-module/picogame/Display.h"
#endif

#define SCENE_INIT_CAP 8
#define PICOGAME_MAX_DIRTY_RECTS 6   // separate regions repainted per refresh

// Classify the `display` arg into a render backend and return the object Scene should
// store: a fast picogame.Display (DMA), a picogame.Framebuffer (RAM scanout buffer, when
// built in), or a plain busdisplay (portable bus.send). Sets *fast / *fb_target
// accordingly; for a busdisplay it accepts a SUBCLASS (e.g. adafruit_st7789.ST7789) by
// casting to its native base and returns that. Raises TypeError otherwise. This is where
// the flag-gated type checks live, so the constructor body stays clean.
static mp_obj_t scene_resolve_target(mp_obj_t disp, bool *fast, bool *fb_target) {
    *fast = false;
    *fb_target = false;
    #if CIRCUITPY_PICOGAME_FAST_DISPLAY
    {
        // Accept subclasses too (same as the busdisplay path below): resolve to the
        // native base object, which the C renderer can safely cast.
        mp_obj_t ndisp = mp_obj_cast_to_native_base(disp, &picogame_display_type);
        if (ndisp != MP_OBJ_NULL && mp_obj_is_type(ndisp, &picogame_display_type)) {
            *fast = true;
            return ndisp;
        }
    }
    #endif
    #if CIRCUITPY_PICOGAME_FRAMEBUFFER
    {
        mp_obj_t nfb = mp_obj_cast_to_native_base(disp, &picogame_framebuffer_type);
        if (nfb != MP_OBJ_NULL && mp_obj_is_type(nfb, &picogame_framebuffer_type)) {
            *fb_target = true;   // refresh() composites dirty rects straight into its RAM buffer
            return nfb;
        }
    }
    #endif
    // Plain busdisplay: accept a subclass by casting to its native base; the portable
    // renderer treats self->display as a busdisplay_busdisplay_obj_t directly.
    mp_obj_t native = mp_obj_cast_to_native_base(disp, &busdisplay_busdisplay_type);
    // MP_OBJ_NULL for a non-BusDisplay: validate the ORIGINAL object (NULL's type is unreadable).
    if (native == MP_OBJ_NULL || !mp_obj_is_type(native, &busdisplay_busdisplay_type)) {
        mp_arg_validate_type(disp, &busdisplay_busdisplay_type, MP_QSTR_display);
    }
    return native;
}

//| class Scene:
//|     """A retained-mode scene with dirty-rectangle rendering for a `Display`,
//|     :py:class:`~busdisplay.BusDisplay` or `Framebuffer` target. Add layers once
//|     (insertion order is bottom to top), mutate them each frame, then call
//|     :py:meth:`refresh`; only the regions reported changed are repainted."""
//|
//|     def __init__(
//|         self,
//|         display: Union[Display, busdisplay.BusDisplay, Framebuffer],
//|         buffer_a: Optional[WriteableBuffer] = None,
//|         buffer_b: Optional[WriteableBuffer] = None,
//|         *,
//|         background: int = 0,
//|         top: int = 0,
//|         bottom: int = 0,
//|         left: int = 0,
//|         right: int = 0,
//|     ) -> None:
//|         """:param display: the render target
//|         :param ~circuitpython_typing.WriteableBuffer buffer_a: a strip buffer,
//|             typically ``display.width * STRIP_H * 2`` bytes. Its size sets the
//|             strip height: each strip is ``size // (display.width * 2)`` rows. Two
//|             buffers let the next strip be composited while the previous one is
//|             being sent. On a `Framebuffer` target there are no strips and the
//|             buffers are unused; both default to `None`.
//|         :param ~circuitpython_typing.WriteableBuffer buffer_b: the second strip
//|             buffer, sized like ``buffer_a``
//|         :param int background: color that exposed areas are cleared to
//|         :param int top: rows at the top edge the scene never renders into
//|         :param int bottom: rows at the bottom edge the scene never renders into
//|         :param int left: columns at the left edge the scene never renders into
//|         :param int right: columns at the right edge the scene never renders into
//|
//|         The four border insets reserve screen edges for content the application
//|         draws itself, for example with :py:func:`render`; the scene renders only
//|         the inner rectangle and never repaints the border."""
//|         ...
//|
static mp_obj_t picogame_scene_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_display, ARG_buffer_a, ARG_buffer_b, ARG_background,
           ARG_top, ARG_bottom, ARG_left, ARG_right };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_display, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_buffer_a, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_buffer_b, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_background, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_top, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },      // reserved border insets:
        { MP_QSTR_bottom, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },   // the scene renders only the
        { MP_QSTR_left, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },     // inner play rect; the app
        { MP_QSTR_right, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },    // owns the border around it
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Resolve the render backend (fast Display / Framebuffer / busdisplay) - all the
    // flag-gated type checks are concentrated in scene_resolve_target().
    bool fast, fb_target;
    mp_obj_t disp = scene_resolve_target(args[ARG_display].u_obj, &fast, &fb_target);

    // The framebuffer path composites directly into the target, so it needs no strip
    // buffers; the SPI/DMA paths do. Validate them only when they'll be used. (When
    // CIRCUITPY_PICOGAME_FRAMEBUFFER is off, fb_target is always false -> always validated,
    // as before.)
    if (!fb_target) {
        mp_buffer_info_t tmp;
        mp_get_buffer_raise(args[ARG_buffer_a].u_obj, &tmp, MP_BUFFER_WRITE);
        mp_get_buffer_raise(args[ARG_buffer_b].u_obj, &tmp, MP_BUFFER_WRITE);
    }

    picogame_scene_obj_t *self = mp_obj_malloc(picogame_scene_obj_t, type);
    self->display = disp;
    self->fast = fast;
    #if CIRCUITPY_PICOGAME_FRAMEBUFFER
    self->fb_target = fb_target;
    #endif
    self->buf_a = args[ARG_buffer_a].u_obj;
    self->buf_b = args[ARG_buffer_b].u_obj;
    self->background = args[ARG_background].u_int;
    self->count = 0;
    self->cap = SCENE_INIT_CAP;
    self->items = m_new(mp_obj_t, SCENE_INIT_CAP);
    self->kinds = m_new(uint8_t, SCENE_INIT_CAP);
    self->snap = m_new(picogame_snapshot_t, SCENE_INIT_CAP);
    self->cleared = false;
    self->ox = 0;
    self->oy = 0;
    self->top = args[ARG_top].u_int;
    self->bottom = args[ARG_bottom].u_int;
    self->left = args[ARG_left].u_int;
    self->right = args[ARG_right].u_int;
    mp_obj_t zeros[4] = {
        MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_NEW_SMALL_INT(0),
        MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_NEW_SMALL_INT(0),
    };
    self->dirty_rect = mp_obj_new_list(4, zeros);   // reused every refresh
    return MP_OBJ_FROM_PTR(self);
}

// On a full repaint, sync sprite snapshots to current and drain the layer
// dirties (tilemap/particles/canvas) so they don't re-report a stale region.
static void snapshot_sync(picogame_scene_obj_t *self) {
    int a, b, c, d;
    for (uint16_t i = 0; i < self->count; i++) {
        uint8_t kind = self->kinds[i] & PICOGAME_KIND_MASK;
        if (kind == PICOGAME_KIND_TILEMAP) {
            picogame_tilemap_take_dirty(MP_OBJ_TO_PTR(self->items[i]), &a, &b, &c, &d);
        } else if (kind == PICOGAME_KIND_PARTICLES) {
            picogame_particles_take_dirty(MP_OBJ_TO_PTR(self->items[i]), &a, &b, &c, &d);
        } else if (kind == PICOGAME_KIND_CANVAS) {
            picogame_canvas_take_dirty(MP_OBJ_TO_PTR(self->items[i]), &a, &b, &c, &d);
        } else if (kind == PICOGAME_KIND_STRIPDRAW) {
            // Immediate-mode layer: no retained state to snapshot/drain.
        } else if (kind == PICOGAME_KIND_TRIANGLES) {
            // Screen-space batch: drain the count-set dirty so it doesn't re-report.
            int e, f, g, hh;
            picogame_dirty_take(&((picogame_triangles_obj_t *)MP_OBJ_TO_PTR(self->items[i]))->dx1,
                &e, &f, &g, &hh);
        } else {
            picogame_sprite_obj_t *s = MP_OBJ_TO_PTR(self->items[i]);
            picogame_bitmap_obj_t *bm = s->bitmap;
            int ax1, ay1, ax2, ay2;
            picogame_sprite_aabb(s, &ax1, &ay1, &ax2, &ay2);
            self->snap[i].x = ax1;
            self->snap[i].y = ay1;
            self->snap[i].w = ax2 - ax1;
            self->snap[i].h = ay2 - ay1;
            self->snap[i].bitmap = (void *)bm;
            self->snap[i].frame = s->frame;
            self->snap[i].flags = s->flags;
            self->snap[i].scale = s->scale;
            self->snap[i].angle = s->angle;
            self->snap[i].seq = s->seq;
            self->snap[i].dither = s->dither;
            self->snap[i].flash_color = s->flash_color;
        }
    }
}

//|
//|     def add(
//|         self, item: Union[Sprite, Tilemap, Canvas, Particles, StripDraw, Triangles], *, fixed: bool = False
//|     ) -> Union[Sprite, Tilemap, Canvas, Particles, StripDraw, Triangles]:
//|         """Add a layer of any kind, drawn starting with the next refresh; insertion
//|         order is bottom to top. Returns the added item.
//|
//|         ``fixed=True`` pins the item to the screen so it ignores the view offset
//|         set by :py:meth:`set_view`, for example for a HUD over a scrolling world.
//|         `StripDraw` and `Triangles` layers always draw in screen coordinates and
//|         are unaffected by both ``fixed`` and the view offset."""
//|         ...
//|
static void scene_add_one(picogame_scene_obj_t *self, mp_obj_t item_in, bool fixed) {
    uint8_t kind;
    kind = picogame_kind_of(item_in);
    if (fixed) {
        kind |= PICOGAME_KIND_FIXED;
    }
    if (self->count >= self->cap) {
        if (self->cap >= 0x8000) {                 // next doubling overflows uint16_t -> m_renew(0) shrink
            m_malloc_fail((size_t)self->cap * 2);   // next doubling overflows the count type
        }
        uint16_t new_cap = self->cap * 2;
        self->items = m_renew(mp_obj_t, self->items, self->cap, new_cap);
        self->kinds = m_renew(uint8_t, self->kinds, self->cap, new_cap);
        self->snap = m_renew(picogame_snapshot_t, self->snap, self->cap, new_cap);
        self->cap = new_cap;
    }
    self->items[self->count] = item_in;
    self->kinds[self->count] = kind;
    // Snapshot starts all-zero ("invisible", nothing diffed against garbage - m_renew doesn't
    // zero), so a newly added layer is detected as changed and drawn on the next refresh.
    memset(&self->snap[self->count], 0, sizeof(picogame_snapshot_t));
    self->count++;
    // Honour "drawn on the next refresh" for EVERY kind: the zeroed snapshot covers sprites,
    // but a re-add()ed Canvas/Tilemap whose dirty flag was already consumed would otherwise
    // stay invisible until some other change. add() is a cold path -> force a full repaint.
    self->cleared = false;
}

static mp_obj_t picogame_scene_add(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_item, ARG_fixed };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_item, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_fixed, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    scene_add_one(MP_OBJ_TO_PTR(pos_args[0]), args[ARG_item].u_obj, args[ARG_fixed].u_bool);
    return args[ARG_item].u_obj;   // constructive: return the added item for `x = scene.add(...)`
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_scene_add_obj, 2, picogame_scene_add);

//|     def add_all(self, items: Iterable[Union[Sprite, Tilemap, Canvas, Particles, StripDraw, Triangles]]) -> None:
//|         """Add several layers at once, bottom to top in iteration order."""
//|         ...
//|
static mp_obj_t picogame_scene_add_all(mp_obj_t self_in, mp_obj_t iterable) {
    picogame_scene_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t iter = mp_getiter(iterable, NULL);
    mp_obj_t item;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        scene_add_one(self, item, false);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(picogame_scene_add_all_obj, picogame_scene_add_all);

//|     def remove(self, item: Union[Sprite, Tilemap, Canvas, Particles, StripDraw, Triangles]) -> None:
//|         """Remove a previously added item; the draw order of the rest is unchanged.
//|         The next refresh repaints the scene, so the item leaves no ghost. The item
//|         itself is untouched and may be added again later. Raises
//|         :py:class:`ValueError` if the item is not in the scene."""
//|         ...
//|
static mp_obj_t picogame_scene_remove(mp_obj_t self_in, mp_obj_t item_in) {
    picogame_scene_obj_t *self = MP_OBJ_TO_PTR(self_in);
    for (uint16_t i = 0; i < self->count; i++) {
        if (self->items[i] != item_in) {
            continue;
        }
        self->count--;
        for (uint16_t j = i; j < self->count; j++) {   // keep draw order: shift the tail down
            self->items[j] = self->items[j + 1];
            self->kinds[j] = self->kinds[j + 1];
            self->snap[j] = self->snap[j + 1];
        }
        self->items[self->count] = mp_const_none;   // release the GC reference
        self->cleared = false;   // full repaint next refresh: background covers where it was
        return mp_const_none;
    }
    mp_arg_error_invalid(MP_QSTR_item);
}
static MP_DEFINE_CONST_FUN_OBJ_2(picogame_scene_remove_obj, picogame_scene_remove);

//|     def invalidate(self) -> None:
//|         """Force the scene's whole render area to repaint on the next refresh."""
//|         ...
//|
static mp_obj_t picogame_scene_invalidate(mp_obj_t self_in) {
    ((picogame_scene_obj_t *)MP_OBJ_TO_PTR(self_in))->cleared = false;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(picogame_scene_invalidate_obj, picogame_scene_invalidate);

//|     def set_view(self, ox: int, oy: int) -> None:
//|         """Set the screen position ``(ox, oy)`` of scene coordinate ``(0, 0)``:
//|         a scene point ``(x, y)`` is drawn at ``(x + ox, y + oy)``. Use a constant
//|         offset to center a small scene, or update it each frame to scroll, which
//|         repaints the whole render area."""
//|         ...
//|
static mp_obj_t picogame_scene_set_view(mp_obj_t self_in, mp_obj_t ox_in, mp_obj_t oy_in) {
    picogame_scene_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ox = mp_obj_get_int(ox_in);
    int oy = mp_obj_get_int(oy_in);
    if (ox != self->ox || oy != self->oy) {
        self->ox = ox;
        self->oy = oy;
        self->cleared = false;   // the whole view shifted -> full repaint next refresh
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(picogame_scene_set_view_obj, picogame_scene_set_view);

//|     view: Tuple[int, int]
//|     """The current view offset ``(ox, oy)`` as set by :py:meth:`set_view`. (read-only)"""
static mp_obj_t picogame_scene_get_view(mp_obj_t self_in) {
    picogame_scene_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t t[2] = { MP_OBJ_NEW_SMALL_INT(self->ox), MP_OBJ_NEW_SMALL_INT(self->oy) };
    return mp_obj_new_tuple(2, t);
}
static MP_DEFINE_CONST_FUN_OBJ_1(picogame_scene_get_view_obj, picogame_scene_get_view);
MP_PROPERTY_GETTER(picogame_scene_view_obj, (mp_obj_t)&picogame_scene_get_view_obj);

//|     display: Union[Display, busdisplay.BusDisplay, Framebuffer]
//|     """The render target this Scene was built with. (read-only)"""
//|
static mp_obj_t picogame_scene_get_display(mp_obj_t self_in) {
    return ((picogame_scene_obj_t *)MP_OBJ_TO_PTR(self_in))->display;
}
static MP_DEFINE_CONST_FUN_OBJ_1(picogame_scene_get_display_obj, picogame_scene_get_display);
MP_PROPERTY_GETTER(picogame_scene_display_obj, (mp_obj_t)&picogame_scene_get_display_obj);

// Compute the frame's dirty rectangles for a WxH target, clipped to the play rect
// [left, w-right) x [top, h-bottom). On the first refresh (or after invalidate) this is
// a single full-screen rect + a snapshot sync; afterwards it diffs against the previous
// frame. Returns the rect count written to `rects` (0 = nothing to repaint).
// Backend-agnostic, so the SPI/fast and framebuffer paths share the exact dirty logic.
static int scene_collect_dirty_rects(picogame_scene_obj_t *self, int w, int h, picogame_rect_t *rects) {
    int nr;
    if (self->cleared) {
        nr = picogame_scene_compute_dirty_rects(self->items, self->kinds, self->snap,
            self->count, w, h, self->ox, self->oy, rects, PICOGAME_MAX_DIRTY_RECTS);
        if (nr == 0) {
            return 0;
        }
    } else {
        rects[0].x1 = 0;
        rects[0].y1 = 0;
        rects[0].x2 = w;
        rects[0].y2 = h;
        nr = 1;
        snapshot_sync(self);
        // NOTE: cleared flips to true only after the RENDER completes (see the callers) -
        // the snapshots are already advanced here, so an exception mid-render would
        // otherwise leave a partially painted frame that no later refresh repairs.
    }

    // Clip every dirty rect to the play rect [left, w-right) x [top, h-bottom); the
    // reserved border is the app's, so the scene never paints into it. Drop empty rects.
    int pa_x1 = self->left;
    int pa_x2 = w - self->right;
    int pa_y1 = self->top;
    int pa_y2 = h - self->bottom;
    int kept = 0;
    for (int i = 0; i < nr; i++) {
        int rx1 = rects[i].x1 < pa_x1 ? pa_x1 : rects[i].x1;
        int rx2 = rects[i].x2 > pa_x2 ? pa_x2 : rects[i].x2;
        int ry1 = rects[i].y1 < pa_y1 ? pa_y1 : rects[i].y1;
        int ry2 = rects[i].y2 > pa_y2 ? pa_y2 : rects[i].y2;
        if (rx1 >= rx2 || ry1 >= ry2) {
            continue;
        }
        rects[kept].x1 = rx1;
        rects[kept].y1 = ry1;
        rects[kept].x2 = rx2;
        rects[kept].y2 = ry2;
        kept++;
    }
    return kept;
}

// Store the bounding union of `nr` rects into the reusable dirty_rect list (no per-frame
// tuple allocation) and return it. Shared by both refresh backends.
static mp_obj_t scene_store_dirty(picogame_scene_obj_t *self, const picogame_rect_t *rects, int nr, int w, int h) {
    int ux1 = w, uy1 = h, ux2 = 0, uy2 = 0;
    for (int i = 0; i < nr; i++) {
        if (rects[i].x1 < ux1) {
            ux1 = rects[i].x1;
        }
        if (rects[i].y1 < uy1) {
            uy1 = rects[i].y1;
        }
        if (rects[i].x2 > ux2) {
            ux2 = rects[i].x2;
        }
        if (rects[i].y2 > uy2) {
            uy2 = rects[i].y2;
        }
    }
    mp_obj_list_store(self->dirty_rect, MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_NEW_SMALL_INT(ux1));
    mp_obj_list_store(self->dirty_rect, MP_OBJ_NEW_SMALL_INT(1), MP_OBJ_NEW_SMALL_INT(uy1));
    mp_obj_list_store(self->dirty_rect, MP_OBJ_NEW_SMALL_INT(2), MP_OBJ_NEW_SMALL_INT(ux2));
    mp_obj_list_store(self->dirty_rect, MP_OBJ_NEW_SMALL_INT(3), MP_OBJ_NEW_SMALL_INT(uy2));
    return self->dirty_rect;
}

#if CIRCUITPY_PICOGAME_FRAMEBUFFER
// Retained-mode refresh against a RAM framebuffer target (scanout-buffer platforms:
// WASM playground, desktop sim, FruitJam DVI/HSTX). Same dirty-rect logic as the SPI
// path (via the shared helpers), but each rect is composited straight into the target
// with picogame_render_framebuffer - no strip buffers, no bus transaction. A latched
// StripDraw BaseException is re-raised (no bus to close). Self-contained here so
// picogame_scene_refresh() keeps its original SPI/fast shape.
static mp_obj_t scene_refresh_fb(picogame_scene_obj_t *self) {
    picogame_framebuffer_obj_t *fbt = MP_OBJ_TO_PTR(self->display);
    int w = fbt->width;
    int h = fbt->height;

    // Emulated invert (pg.invert on a Framebuffer) just toggled -> recomposite the WHOLE frame so the
    // negative flip covers the whole screen, not only this frame's dirty rects (like a panel INVON).
    if (picogame_fb_take_invert_dirty()) {
        self->cleared = false;
    }

    picogame_rect_t rects[PICOGAME_MAX_DIRTY_RECTS];
    int nr = scene_collect_dirty_rects(self, w, h, rects);
    if (nr == 0) {
        return mp_const_none;
    }

    // Snapshots are already advanced; stay in the needs-full-repaint state until the
    // render completes, so a BaseException mid-render (Ctrl-C in a StripDraw) leaves a
    // scene whose NEXT refresh repaints everything instead of keeping a torn frame.
    self->cleared = false;
    for (int i = 0; i < nr; i++) {
        mp_obj_t exc = picogame_render_framebuffer(fbt->fb, fbt->width, fbt->height, fbt->fmt,
            fbt->scratch, fbt->scratch_rows,
            self->items, self->kinds, self->count,
            rects[i].x1, rects[i].y1, rects[i].x2, rects[i].y2,
            self->background, self->ox, self->oy);
        if (exc != MP_OBJ_NULL) {
            nlr_raise(MP_OBJ_TO_PTR(exc));   // cleared stays false -> full repaint next refresh
        }
    }
    self->cleared = true;
    return scene_store_dirty(self, rects, nr, w, h);
}
#endif // CIRCUITPY_PICOGAME_FRAMEBUFFER

//|     def refresh(self) -> Optional[list]:
//|         """Repaint the regions changed by the scene's layers since the previous
//|         refresh. Returns the bounding dirty rectangle as a list
//|         ``[x1, y1, x2, y2]`` with exclusive ``x2``/``y2``, or `None` if nothing
//|         changed. The returned list object is reused: read it before the next
//|         ``refresh()`` call."""
//|         ...
//|
//|
static mp_obj_t picogame_scene_refresh(mp_obj_t self_in) {
    picogame_scene_obj_t *self = MP_OBJ_TO_PTR(self_in);

    #if CIRCUITPY_PICOGAME_FRAMEBUFFER
    if (self->fb_target) {
        return scene_refresh_fb(self);
    }
    #endif

    // Resolve the underlying busdisplay from either backend.
    busdisplay_busdisplay_obj_t *bd;
    #if CIRCUITPY_PICOGAME_FAST_DISPLAY
    picogame_display_obj_t *disp = NULL;
    if (self->fast) {
        disp = MP_OBJ_TO_PTR(self->display);
        bd = disp->display;
    } else
    #endif
    {
        bd = MP_OBJ_TO_PTR(self->display);
    }
    int w = bd->core.width;
    int h = bd->core.height;

    picogame_rect_t rects[PICOGAME_MAX_DIRTY_RECTS];
    int nr = scene_collect_dirty_rects(self, w, h, rects);
    if (nr == 0) {
        return mp_const_none;
    }

    mp_buffer_info_t a, b;
    mp_get_buffer_raise(self->buf_a, &a, MP_BUFFER_WRITE);
    mp_get_buffer_raise(self->buf_b, &b, MP_BUFFER_WRITE);
    #if CIRCUITPY_PICOGAME_FAST_DISPLAY
    size_t buf_pixels = (a.len < b.len ? a.len : b.len) / 2;  // fast path double-buffers
    #endif

    // Render each dirty rect independently; return their bounding union (kept for
    // the existing "dirty WxH" debug prints).
    // Snapshots are already advanced; stay in the needs-full-repaint state until the
    // render completes, so a BaseException mid-render (Ctrl-C in a StripDraw) leaves a
    // scene whose NEXT refresh repaints everything instead of keeping a torn frame.
    self->cleared = false;
    for (int i = 0; i < nr; i++) {
        #if CIRCUITPY_PICOGAME_FAST_DISPLAY
        if (self->fast) {
            common_hal_picogame_display_render(disp, self->items, self->kinds, self->count,
                (uint16_t *)a.buf, (uint16_t *)b.buf, buf_pixels,
                rects[i].x1, rects[i].y1, rects[i].x2, rects[i].y2,
                self->background, self->ox, self->oy);
        } else
        #endif
        {
            // Portable single-buffer bus.send path (any CircuitPython port).
            picogame_render_region(bd, self->items, self->kinds, self->count,
                (uint16_t *)a.buf, a.len / 2,
                rects[i].x1, rects[i].y1, rects[i].x2, rects[i].y2,
                self->background, self->ox, self->oy);
        }
    }
    self->cleared = true;

    return scene_store_dirty(self, rects, nr, w, h);
}
static MP_DEFINE_CONST_FUN_OBJ_1(picogame_scene_refresh_obj, picogame_scene_refresh);

static const mp_rom_map_elem_t picogame_scene_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_add), MP_ROM_PTR(&picogame_scene_add_obj) },
    { MP_ROM_QSTR(MP_QSTR_add_all), MP_ROM_PTR(&picogame_scene_add_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&picogame_scene_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh), MP_ROM_PTR(&picogame_scene_refresh_obj) },
    { MP_ROM_QSTR(MP_QSTR_invalidate), MP_ROM_PTR(&picogame_scene_invalidate_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_view), MP_ROM_PTR(&picogame_scene_set_view_obj) },
    { MP_ROM_QSTR(MP_QSTR_view), MP_ROM_PTR(&picogame_scene_view_obj) },
    { MP_ROM_QSTR(MP_QSTR_display), MP_ROM_PTR(&picogame_scene_display_obj) },
};
static MP_DEFINE_CONST_DICT(picogame_scene_locals_dict, picogame_scene_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picogame_scene_type,
    MP_QSTR_Scene,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, picogame_scene_make_new,
    locals_dict, &picogame_scene_locals_dict
    );
