// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame.Sprite: a positioned, animatable instance of a Bitmap.

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
#include "shared-module/picogame/__init__.h"
#include "shared-module/picogame/Bitmap.h"
#include "shared-module/picogame/Sprite.h"

// ---------------------------------------------------------------------------
// Sprite
// ---------------------------------------------------------------------------

// Int pixel/scale -> 24.8 fixed-point. Shift through unsigned so a wild coordinate (e.g. 100_000_000,
// in 32-bit mp_int range but not after <<8) wraps modularly instead of hitting signed-overflow UB.
// Costs nothing over a plain shift; for any real on-screen coordinate the result is identical.
static int32_t pg_int_to_fp8(mp_int_t v) {
    return (int32_t)((uint32_t)v << 8);
}

//| class Sprite:
//|     """A positioned, animatable instance of a :py:class:`Bitmap`."""
//|
//|     def __init__(
//|         self,
//|         bitmap: Bitmap,
//|         x: int = 0,
//|         y: int = 0,
//|         *,
//|         frame: int = 0,
//|         visible: bool = True,
//|         flip_x: bool = False,
//|         flip_y: bool = False,
//|     ) -> None:
//|         """Place ``bitmap`` with its top-left corner at ``(x, y)`` in scene
//|         coordinates, showing the frame selected by ``frame``. Scene coordinates
//|         follow the view offset, so the sprite moves with a scrolling world.
//|
//|         ``visible=False`` creates the sprite without drawing it, for example to
//|         pre-allocate a pool of sprites up front. ``flip_x`` and ``flip_y`` mirror
//|         the frame at draw time."""
//|         ...
//|
static mp_obj_t picogame_sprite_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_bitmap, ARG_x, ARG_y, ARG_frame, ARG_visible, ARG_flip_x, ARG_flip_y };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_frame, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_visible, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_flip_x, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_flip_y, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t bitmap_obj = mp_arg_validate_type(args[ARG_bitmap].u_obj, &picogame_bitmap_type, MP_QSTR_bitmap);

    picogame_sprite_obj_t *self = mp_obj_malloc(picogame_sprite_obj_t, type);
    self->bitmap = MP_OBJ_TO_PTR(bitmap_obj);
    self->x = pg_int_to_fp8(args[ARG_x].u_int);   // pixel -> 24.8 fixed-point (overflow-clamped)
    self->y = pg_int_to_fp8(args[ARG_y].u_int);
    self->frame = args[ARG_frame].u_int;
    self->flags = (args[ARG_visible].u_bool ? PICOGAME_SPR_VISIBLE : 0)
        | (args[ARG_flip_x].u_bool ? PICOGAME_SPR_FLIP_X : 0)
        | (args[ARG_flip_y].u_bool ? PICOGAME_SPR_FLIP_Y : 0);
    self->anchor_x = 0;   // default pivot = top-left (0, 0)
    self->anchor_y = 0;
    self->scale = 256;    // 8.8 fixed-point: 256 = 1.0x (no scaling)
    self->angle = 0;      // no rotation -> axis-aligned fast path
    self->flash_color = 0;
    self->dither = 0;
    self->data = mp_const_none;
    self->seq = 0;            // dirty-rect change counter (mp_obj_malloc zeroes anyway; explicit)
    return MP_OBJ_FROM_PTR(self);
}

static void set_flag(picogame_sprite_obj_t *self, uint8_t flag, bool on) {
    if (on) {
        self->flags |= flag;
    } else {
        self->flags &= ~flag;
    }
}

// shadow / flash / dither are mutually exclusive (one blit effect at a time); setting one ON
// clears the others, so "the last effect you set wins". Turning one OFF clears ONLY its own flag,
// so clearing an effect you never enabled (e.g. spr.flash = 0) can't wipe a different active one.
#define PICOGAME_SPR_FX_MASK (PICOGAME_SPR_SHADOW | PICOGAME_SPR_FLASH | PICOGAME_SPR_DITHER | PICOGAME_SPR_TINT)
static void set_effect(picogame_sprite_obj_t *self, uint8_t flag, bool on) {
    if (on) {
        self->flags = (uint8_t)((self->flags & ~PICOGAME_SPR_FX_MASK) | flag);
    } else {
        self->flags &= (uint8_t) ~flag;
    }
}

// Round a float to 24.8 fixed-point (shared by the position/scale/anchor setters so the soft-float
// round sequence is emitted once, not per call site).
static int32_t pg_round_fp8(mp_float_t f) {
    return (int32_t)(f * 256 + (f >= 0 ? (mp_float_t)0.5 : (mp_float_t)-0.5));
}


// Accept an int or float and store as 24.8 fixed-point (rounded). Integer fast path avoids
// software float on RP2040 - game code sets x/y (via move/setters) every frame, usually with ints.
static int32_t obj_to_fp(mp_obj_t o) {
    if (mp_obj_is_int(o)) {
        return pg_int_to_fp8(mp_obj_get_int(o));
    }
    mp_float_t f = mp_obj_get_float(o);
    return pg_round_fp8(f);
}

//|
//|     frame: int
//|     """Which frame of the bitmap's atlas to draw, starting at 0. Stepping this
//|     animates the sprite."""
//|     visible: bool
//|     """`False` hides the sprite. It stays in the scene and the area under it
//|     repaints."""
//|     flip_x: bool
//|     """Mirror the frame horizontally at draw time."""
//|     flip_y: bool
//|     """Mirror the frame vertically at draw time."""
//|     x: int
//|     """Horizontal pixel position in scene coordinates. Setting accepts a float
//|     for sub-pixel placement; reading returns the floored pixel."""
//|     y: int
//|     """Vertical pixel position in scene coordinates. Setting accepts a float
//|     for sub-pixel placement; reading returns the floored pixel."""
//|     fx: float
//|     """Horizontal sub-pixel position, for example ``sprite.fx += 2.4``."""
//|     fy: float
//|     """Vertical sub-pixel position, for example ``sprite.fy += 2.4``."""
// FLASH NOTE (property objects vs. a single `attr` handler): Sprite (and the other property-dense
// types) expose each attribute as its own getter/setter + MP_PROPERTY object below - the standard
// CircuitPython shared-bindings idiom (~423 such uses across CP). Collapsing these onto ONE `attr`
// load/store function per type (a qstr switch, like py/objcomplex.c's complex_attr for .real/.imag)
// would save ~3-4 KB of flash across the whole binding layer (Sprite alone ~1.5-2.5 KB). It is a
// valid MicroPython mechanism and would NOT break the .pyi stubs (those are generated from the //|
// comments, not the C property objects). We deliberately KEEP property objects: they are the
// idiomatic, most readable shared-bindings form and keep every type's definition uniform. Revisit
// only if flash becomes critical (then convert Sprite first - the densest cluster - not every type).
static mp_obj_t sprite_get_x(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->x >> 8);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_x_obj, sprite_get_x);
static mp_obj_t sprite_set_x(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->x = obj_to_fp(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_x_obj, sprite_set_x);
MP_PROPERTY_GETSET(sprite_x_obj, (mp_obj_t)&sprite_get_x_obj, (mp_obj_t)&sprite_set_x_obj);

static mp_obj_t sprite_get_y(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->y >> 8);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_y_obj, sprite_get_y);
static mp_obj_t sprite_set_y(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->y = obj_to_fp(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_y_obj, sprite_set_y);
MP_PROPERTY_GETSET(sprite_y_obj, (mp_obj_t)&sprite_get_y_obj, (mp_obj_t)&sprite_set_y_obj);

static mp_obj_t sprite_get_fx(mp_obj_t self_in) {
    return mp_obj_new_float(((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->x * (mp_float_t)(1.0 / 256.0));
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_fx_obj, sprite_get_fx);
// set_fx is byte-identical to set_x (both store obj_to_fp(v) into ->x) -> reuse set_x's fun obj.
MP_PROPERTY_GETSET(sprite_fx_obj, (mp_obj_t)&sprite_get_fx_obj, (mp_obj_t)&sprite_set_x_obj);

static mp_obj_t sprite_get_fy(mp_obj_t self_in) {
    return mp_obj_new_float(((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->y * (mp_float_t)(1.0 / 256.0));
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_fy_obj, sprite_get_fy);
// set_fy is byte-identical to set_y -> reuse set_y's fun obj.
MP_PROPERTY_GETSET(sprite_fy_obj, (mp_obj_t)&sprite_get_fy_obj, (mp_obj_t)&sprite_set_y_obj);

static mp_obj_t sprite_get_frame(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->frame);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_frame_obj, sprite_get_frame);
static mp_obj_t sprite_set_frame(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->frame = mp_obj_get_int(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_frame_obj, sprite_set_frame);
MP_PROPERTY_GETSET(sprite_frame_obj, (mp_obj_t)&sprite_get_frame_obj, (mp_obj_t)&sprite_set_frame_obj);

static mp_obj_t sprite_get_visible(mp_obj_t self_in) {
    return mp_obj_new_bool((((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->flags & PICOGAME_SPR_VISIBLE) != 0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_visible_obj, sprite_get_visible);
static mp_obj_t sprite_set_visible(mp_obj_t self_in, mp_obj_t v) {
    set_flag(MP_OBJ_TO_PTR(self_in), PICOGAME_SPR_VISIBLE, mp_obj_is_true(v));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_visible_obj, sprite_set_visible);
MP_PROPERTY_GETSET(sprite_visible_obj, (mp_obj_t)&sprite_get_visible_obj, (mp_obj_t)&sprite_set_visible_obj);

static mp_obj_t sprite_get_flip_x(mp_obj_t self_in) {
    return mp_obj_new_bool((((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->flags & PICOGAME_SPR_FLIP_X) != 0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_flip_x_obj, sprite_get_flip_x);
static mp_obj_t sprite_set_flip_x(mp_obj_t self_in, mp_obj_t v) {
    set_flag(MP_OBJ_TO_PTR(self_in), PICOGAME_SPR_FLIP_X, mp_obj_is_true(v));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_flip_x_obj, sprite_set_flip_x);
MP_PROPERTY_GETSET(sprite_flip_x_obj, (mp_obj_t)&sprite_get_flip_x_obj, (mp_obj_t)&sprite_set_flip_x_obj);

static mp_obj_t sprite_get_flip_y(mp_obj_t self_in) {
    return mp_obj_new_bool((((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->flags & PICOGAME_SPR_FLIP_Y) != 0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_flip_y_obj, sprite_get_flip_y);
static mp_obj_t sprite_set_flip_y(mp_obj_t self_in, mp_obj_t v) {
    set_flag(MP_OBJ_TO_PTR(self_in), PICOGAME_SPR_FLIP_Y, mp_obj_is_true(v));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_flip_y_obj, sprite_set_flip_y);
MP_PROPERTY_GETSET(sprite_flip_y_obj, (mp_obj_t)&sprite_get_flip_y_obj, (mp_obj_t)&sprite_set_flip_y_obj);

//|     scale: float
//|     """Uniform draw scale using nearest-neighbor sampling. ``1.0`` is native size;
//|     fractional values are allowed. The anchor point stays put."""
static mp_obj_t sprite_get_scale(mp_obj_t self_in) {
    return mp_obj_new_float(((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->scale * (mp_float_t)(1.0 / 256.0));
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_scale_obj, sprite_get_scale);
static mp_obj_t sprite_set_scale(mp_obj_t self_in, mp_obj_t v) {
    int q;
    if (mp_obj_is_int(v)) {                          // int fast path (no software float on RP2040)
        q = pg_int_to_fp8(mp_obj_get_int(v));        // overflow-clamped <<8
    } else {
        mp_float_t f = mp_obj_get_float(v);
        q = pg_round_fp8(f);
    }
    if (q < 1) {
        q = 1;
    }
    if (q > 65535) {
        q = 65535;
    }
    picogame_sprite_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->scale = (uint16_t)q;
    self->xf_valid = 0;                              // affine cache depends on scale
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_scale_obj, sprite_set_scale);
MP_PROPERTY_GETSET(sprite_scale_obj, (mp_obj_t)&sprite_get_scale_obj, (mp_obj_t)&sprite_set_scale_obj);

//|     angle: float
//|     """Rotation in degrees about the anchor; ``0`` is unrotated. Values are stored
//|     as whole degrees. Rotation uses nearest-neighbor sampling."""
static mp_obj_t sprite_get_angle(mp_obj_t self_in) {
    return mp_obj_new_float((mp_float_t)((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->angle);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_angle_obj, sprite_get_angle);
static mp_obj_t sprite_set_angle(mp_obj_t self_in, mp_obj_t v) {
    int a;
    if (mp_obj_is_int(v)) {                          // int fast path (no software float on RP2040)
        a = mp_obj_get_int(v);
    } else {
        mp_float_t f = mp_obj_get_float(v);
        a = (int)(f + (f >= 0 ? (mp_float_t)0.5 : (mp_float_t)-0.5));
    }
    a %= 360;
    picogame_sprite_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->angle = (int16_t)a;
    self->xf_valid = 0;                              // affine cache depends on angle
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_angle_obj, sprite_set_angle);
MP_PROPERTY_GETSET(sprite_angle_obj, (mp_obj_t)&sprite_get_angle_obj, (mp_obj_t)&sprite_set_angle_obj);

//|     shadow: bool
//|     """Draw opaque pixels by darkening the destination instead of writing color,
//|     producing a shadow silhouette or a dimming overlay. Mutually exclusive with
//|     `flash`, `dither` and `tint`."""
static mp_obj_t sprite_get_shadow(mp_obj_t self_in) {
    return mp_obj_new_bool((((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->flags & PICOGAME_SPR_SHADOW) != 0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_shadow_obj, sprite_get_shadow);
static mp_obj_t sprite_set_shadow(mp_obj_t self_in, mp_obj_t v) {
    set_effect(MP_OBJ_TO_PTR(self_in), PICOGAME_SPR_SHADOW, mp_obj_is_true(v));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_shadow_obj, sprite_set_shadow);
MP_PROPERTY_GETSET(sprite_shadow_obj, (mp_obj_t)&sprite_get_shadow_obj, (mp_obj_t)&sprite_set_shadow_obj);

//|     flash: int
//|     """Draw opaque pixels as one solid color instead of their own, for example as
//|     a brief hit flash. Set to a color from :py:func:`rgb565` to enable, ``0`` to
//|     disable. Because ``0`` disables the effect, pure black cannot be the flash
//|     color; use a near-black color instead. Mutually exclusive with `shadow`,
//|     `dither` and `tint`."""
static mp_obj_t sprite_get_flash(mp_obj_t self_in) {
    picogame_sprite_obj_t *s = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT((s->flags & PICOGAME_SPR_FLASH) ? s->flash_color : 0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_flash_obj, sprite_get_flash);
static mp_obj_t sprite_set_flash(mp_obj_t self_in, mp_obj_t v) {
    picogame_sprite_obj_t *s = MP_OBJ_TO_PTR(self_in);
    // Falsy (None / False / 0) turns flash OFF. A non-zero colour enables it. (You can't flash
    // pure black - use a near-black colour if you ever need that; off is the common case.)
    if (!mp_obj_is_true(v)) {
        set_effect(s, PICOGAME_SPR_FLASH, false);
    } else {
        s->flash_color = (uint16_t)mp_obj_get_int(v);
        set_effect(s, PICOGAME_SPR_FLASH, true);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_flash_obj, sprite_set_flash);
MP_PROPERTY_GETSET(sprite_flash_obj, (mp_obj_t)&sprite_get_flash_obj, (mp_obj_t)&sprite_set_flash_obj);

//|     dither: int
//|     """Approximate transparency with an ordered (Bayer) dither pattern; there is
//|     no alpha blending. ``0`` is opaque (off), ``8`` is about half transparent and
//|     ``16`` is invisible. Mutually exclusive with `shadow`, `flash` and `tint`."""
static mp_obj_t sprite_get_dither(mp_obj_t self_in) {
    picogame_sprite_obj_t *s = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT((s->flags & PICOGAME_SPR_DITHER) ? s->dither : 0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_dither_obj, sprite_get_dither);
static mp_obj_t sprite_set_dither(mp_obj_t self_in, mp_obj_t v) {
    picogame_sprite_obj_t *s = MP_OBJ_TO_PTR(self_in);
    int lv = mp_obj_get_int(v);
    if (lv < 0) {
        lv = 0;
    }
    if (lv > 16) {
        lv = 16;
    }
    s->dither = (uint8_t)lv;
    set_effect(s, PICOGAME_SPR_DITHER, lv > 0);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_dither_obj, sprite_set_dither);
MP_PROPERTY_GETSET(sprite_dither_obj, (mp_obj_t)&sprite_get_dither_obj, (mp_obj_t)&sprite_set_dither_obj);

//|     tint: int
//|     """Multiply opaque pixels by a color from :py:func:`rgb565`, preserving the
//|     sprite's shading (unlike `flash`, which replaces it). ``0`` disables, so pure
//|     black cannot be the tint color. Mutually exclusive with `shadow`, `flash`
//|     and `dither`."""
static mp_obj_t sprite_get_tint(mp_obj_t self_in) {
    picogame_sprite_obj_t *s = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT((s->flags & PICOGAME_SPR_TINT) ? s->flash_color : 0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_tint_obj, sprite_get_tint);
static mp_obj_t sprite_set_tint(mp_obj_t self_in, mp_obj_t v) {
    picogame_sprite_obj_t *s = MP_OBJ_TO_PTR(self_in);
    if (!mp_obj_is_true(v)) {
        set_effect(s, PICOGAME_SPR_TINT, false);
    } else {
        s->flash_color = (uint16_t)mp_obj_get_int(v);   // shared colour field with flash
        set_effect(s, PICOGAME_SPR_TINT, true);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_tint_obj, sprite_set_tint);
MP_PROPERTY_GETSET(sprite_tint_obj, (mp_obj_t)&sprite_get_tint_obj, (mp_obj_t)&sprite_set_tint_obj);

//|     transpose: bool
//|     """Transpose the frame by swapping its x and y axes. Combined with `flip_x`
//|     and `flip_y` this yields all 8 orthogonal orientations. Applies only at
//|     ``scale == 1.0`` and ``angle == 0``; for rotation combined with scaling use
//|     `angle`. The drawn footprint swaps width and height."""
static mp_obj_t sprite_get_transpose(mp_obj_t self_in) {
    return mp_obj_new_bool((((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->flags & PICOGAME_SPR_TRANSPOSE) != 0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_transpose_obj, sprite_get_transpose);
static mp_obj_t sprite_set_transpose(mp_obj_t self_in, mp_obj_t v) {
    set_flag(MP_OBJ_TO_PTR(self_in), PICOGAME_SPR_TRANSPOSE, mp_obj_is_true(v));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_transpose_obj, sprite_set_transpose);
MP_PROPERTY_GETSET(sprite_transpose_obj, (mp_obj_t)&sprite_get_transpose_obj, (mp_obj_t)&sprite_set_transpose_obj);

//|     data: Any
//|     """Arbitrary per-sprite user payload for game state (default None)."""
static mp_obj_t sprite_get_data(mp_obj_t self_in) {
    return ((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->data;
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_data_obj, sprite_get_data);
static mp_obj_t sprite_set_data(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->data = v;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_data_obj, sprite_set_data);
MP_PROPERTY_GETSET(sprite_data_obj, (mp_obj_t)&sprite_get_data_obj, (mp_obj_t)&sprite_set_data_obj);

//|     bitmap: Bitmap
//|     """The sprite's source bitmap. Assigning a new one swaps the graphics and may
//|     change the sprite's size; the scene repaints both the old and new bounds on
//|     the next refresh."""
static mp_obj_t sprite_get_bitmap(mp_obj_t self_in) {
    picogame_sprite_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return self->bitmap != NULL ? MP_OBJ_FROM_PTR(self->bitmap) : mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_bitmap_obj, sprite_get_bitmap);
static mp_obj_t sprite_set_bitmap(mp_obj_t self_in, mp_obj_t v) {
    picogame_sprite_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t bm = mp_arg_validate_type(v, &picogame_bitmap_type, MP_QSTR_bitmap);
    self->bitmap = MP_OBJ_TO_PTR(bm);
    self->xf_valid = 0;                              // affine cache depends on bitmap dims
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_bitmap_obj, sprite_set_bitmap);
MP_PROPERTY_GETSET(sprite_bitmap_obj, (mp_obj_t)&sprite_get_bitmap_obj, (mp_obj_t)&sprite_set_bitmap_obj);

//|     anchor: Tuple[float, float]
//|     """Pivot as fractions of the bitmap size: ``(0, 0)`` = top-left (default),
//|     ``(0.5, 0.5)`` = center, ``(0.5, 1.0)`` = bottom-center. ``x``/``y`` then
//|     refer to this point, so rotating frames or swapping to a different size
//|     stays aligned. Stored in 1/256 steps."""
//|
static mp_obj_t sprite_get_anchor(mp_obj_t self_in) {
    picogame_sprite_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t t[2] = {
        mp_obj_new_float(self->anchor_x * (mp_float_t)(1.0 / 256.0)),
        mp_obj_new_float(self->anchor_y * (mp_float_t)(1.0 / 256.0)),
    };
    return mp_obj_new_tuple(2, t);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_get_anchor_obj, sprite_get_anchor);
static int anchor_to_fp(mp_obj_t o) {
    mp_float_t f = mp_obj_get_float(o);
    int v = pg_round_fp8(f);
    return v < 0 ? 0 : (v > 256 ? 256 : v);
}
static mp_obj_t sprite_set_anchor(mp_obj_t self_in, mp_obj_t v) {
    picogame_sprite_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t len;
    mp_obj_t *items;
    mp_obj_get_array(v, &len, &items);
    mp_arg_validate_length(len, 2, MP_QSTR_anchor);
    self->anchor_x = anchor_to_fp(items[0]);
    self->anchor_y = anchor_to_fp(items[1]);
    self->xf_valid = 0;                              // affine cache depends on the pivot
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sprite_set_anchor_obj, sprite_set_anchor);
MP_PROPERTY_GETSET(sprite_anchor_obj, (mp_obj_t)&sprite_get_anchor_obj, (mp_obj_t)&sprite_set_anchor_obj);

//|     def move(self, x: float, y: float) -> None:
//|         """Set the sprite position. Accepts floats for sub-pixel placement."""
//|         ...
//|
static mp_obj_t sprite_move(mp_obj_t self_in, mp_obj_t x_in, mp_obj_t y_in) {
    picogame_sprite_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->x = obj_to_fp(x_in);
    self->y = obj_to_fp(y_in);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(sprite_move_obj, sprite_move);

//|     def touch(self) -> None:
//|         """Force this sprite to repaint on the next :py:meth:`Scene.refresh` even
//|         though none of its tracked properties (position, frame, scale, angle,
//|         bitmap) changed. Call it after mutating the bitmap's backing buffer in
//|         place, which the dirty-region tracking cannot otherwise detect."""
//|         ...
//|
static mp_obj_t sprite_touch(mp_obj_t self_in) {
    ((picogame_sprite_obj_t *)MP_OBJ_TO_PTR(self_in))->seq++;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(sprite_touch_obj, sprite_touch);

// Fill (x1,y1,x2,y2) from a Sprite (its drawn aabb), a (x,y) point, or a (x1,y1,x2,y2) rect.
static void pg_obj_to_box(mp_obj_t o, int *x1, int *y1, int *x2, int *y2) {
    if (mp_obj_is_type(o, &picogame_sprite_type)) {
        picogame_sprite_aabb(MP_OBJ_TO_PTR(o), x1, y1, x2, y2);
        return;
    }
    if (mp_obj_is_type(o, &mp_type_tuple) || mp_obj_is_type(o, &mp_type_list)) {
        size_t len;
        mp_obj_t *items;
        mp_obj_get_array(o, &len, &items);
        if (len == 2) {                       // a point -> a zero-size box
            *x1 = *x2 = mp_obj_get_int(items[0]);
            *y1 = *y2 = mp_obj_get_int(items[1]);
            return;
        }
        if (len == 4) {                       // a rect
            *x1 = mp_obj_get_int(items[0]);
            *y1 = mp_obj_get_int(items[1]);
            *x2 = mp_obj_get_int(items[2]);
            *y2 = mp_obj_get_int(items[3]);
            return;
        }
    }
    mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("%q must be of type %q or %q, not %q"),
        MP_QSTR_other, MP_QSTR_Sprite, MP_QSTR_tuple, mp_obj_get_type(o)->name);
}

//|     def overlaps(
//|         self,
//|         other: Union[Sprite, Tuple[int, int], Tuple[int, int, int, int]],
//|         inset: int = 0,
//|     ) -> bool:
//|         """Return `True` if this sprite's drawn rectangle overlaps ``other``. Bounds
//|         are inclusive, so touching edges count as an overlap. ``other`` may be
//|         another `Sprite`, a point ``(x, y)`` or a rectangle ``(x1, y1, x2, y2)``.
//|         The rectangle accounts for anchor, scale and rotation. ``inset`` shrinks
//|         this sprite's rectangle by that many pixels on each side."""
//|         ...
//|
static mp_obj_t sprite_overlaps(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_other, ARG_inset };
    static const mp_arg_t allowed[] = {
        { MP_QSTR_other, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_inset, MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed), allowed, args);
    int ax1, ay1, ax2, ay2, bx1, by1, bx2, by2;
    picogame_sprite_aabb(MP_OBJ_TO_PTR(pos_args[0]), &ax1, &ay1, &ax2, &ay2);
    pg_obj_to_box(args[ARG_other].u_obj, &bx1, &by1, &bx2, &by2);
    int in = args[ARG_inset].u_int;                        // inset shrinks the CALLER's box (kw or positional)
    bool hit = ((ax1 + in) <= bx2) && ((ax2 - in) >= bx1) &&
        ((ay1 + in) <= by2) && ((ay2 - in) >= by1);
    return mp_obj_new_bool(hit);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(sprite_overlaps_obj, 2, sprite_overlaps);

//|     def near(self, other: Union[Sprite, Tuple[int, int]], r: int) -> bool:
//|         """Return `True` when the distance between centers is less than ``r``
//|         pixels. ``other`` may be a `Sprite` or a point ``(x, y)``. Centers come
//|         from the drawn rectangle, so the test accounts for the anchor."""
//|         ...
//|
//|
static mp_obj_t sprite_near(mp_obj_t self_in, mp_obj_t other_in, mp_obj_t r_in) {
    int ax1, ay1, ax2, ay2;
    picogame_sprite_aabb(MP_OBJ_TO_PTR(self_in), &ax1, &ay1, &ax2, &ay2);
    int acx = (ax1 + ax2) / 2, acy = (ay1 + ay2) / 2, bcx, bcy;
    if (mp_obj_is_type(other_in, &picogame_sprite_type)) {
        int bx1, by1, bx2, by2;
        picogame_sprite_aabb(MP_OBJ_TO_PTR(other_in), &bx1, &by1, &bx2, &by2);
        bcx = (bx1 + bx2) / 2;
        bcy = (by1 + by2) / 2;
    } else {
        size_t len;
        mp_obj_t *items;
        mp_obj_get_array(other_in, &len, &items);
        if (len != 2) {
            mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("%q must be of type %q or %q, not %q"),
                MP_QSTR_other, MP_QSTR_Sprite, MP_QSTR_tuple, mp_obj_get_type(other_in)->name);
        }
        bcx = mp_obj_get_int(items[0]);
        bcy = mp_obj_get_int(items[1]);
    }
    mp_int_t r = mp_obj_get_int(r_in), dx = acx - bcx, dy = acy - bcy;
    return mp_obj_new_bool(dx * dx + dy * dy < r * r);
}
static MP_DEFINE_CONST_FUN_OBJ_3(sprite_near_obj, sprite_near);

static const mp_rom_map_elem_t picogame_sprite_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&sprite_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&sprite_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_fx), MP_ROM_PTR(&sprite_fx_obj) },
    { MP_ROM_QSTR(MP_QSTR_fy), MP_ROM_PTR(&sprite_fy_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame), MP_ROM_PTR(&sprite_frame_obj) },
    { MP_ROM_QSTR(MP_QSTR_visible), MP_ROM_PTR(&sprite_visible_obj) },
    { MP_ROM_QSTR(MP_QSTR_flip_x), MP_ROM_PTR(&sprite_flip_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_flip_y), MP_ROM_PTR(&sprite_flip_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_scale), MP_ROM_PTR(&sprite_scale_obj) },
    { MP_ROM_QSTR(MP_QSTR_angle), MP_ROM_PTR(&sprite_angle_obj) },
    { MP_ROM_QSTR(MP_QSTR_shadow), MP_ROM_PTR(&sprite_shadow_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash), MP_ROM_PTR(&sprite_flash_obj) },
    { MP_ROM_QSTR(MP_QSTR_dither), MP_ROM_PTR(&sprite_dither_obj) },
    { MP_ROM_QSTR(MP_QSTR_tint), MP_ROM_PTR(&sprite_tint_obj) },
    { MP_ROM_QSTR(MP_QSTR_transpose), MP_ROM_PTR(&sprite_transpose_obj) },
    { MP_ROM_QSTR(MP_QSTR_data), MP_ROM_PTR(&sprite_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&sprite_bitmap_obj) },
    { MP_ROM_QSTR(MP_QSTR_anchor), MP_ROM_PTR(&sprite_anchor_obj) },
    { MP_ROM_QSTR(MP_QSTR_overlaps), MP_ROM_PTR(&sprite_overlaps_obj) },
    { MP_ROM_QSTR(MP_QSTR_near), MP_ROM_PTR(&sprite_near_obj) },
    { MP_ROM_QSTR(MP_QSTR_move), MP_ROM_PTR(&sprite_move_obj) },
    { MP_ROM_QSTR(MP_QSTR_touch), MP_ROM_PTR(&sprite_touch_obj) },
};
static MP_DEFINE_CONST_DICT(picogame_sprite_locals_dict, picogame_sprite_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picogame_sprite_type,
    MP_QSTR_Sprite,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, picogame_sprite_make_new,
    locals_dict, &picogame_sprite_locals_dict
    );
