// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame.Framebuffer: a RAM render target used in place of a BusDisplay.

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

#if CIRCUITPY_PICOGAME_FRAMEBUFFER
// ---------------------------------------------------------------------------
// Framebuffer  (a RAM render target used in place of a BusDisplay; scanout-buffer
// platforms - WASM playground, desktop sim, FruitJam DVI/HSTX)
// ---------------------------------------------------------------------------
//| class Framebuffer:
//|     """A RAM framebuffer render target that a `Scene` or :py:func:`render` can
//|     draw into instead of a :py:class:`~busdisplay.BusDisplay`, for example the
//|     scanout buffer of a :py:class:`picodvi.Framebuffer`.
//|
//|     Constructing it raises :py:class:`NotImplementedError` when
//|     :py:data:`FRAMEBUFFER_SUPPORTED` is `False`."""
//|
//|     def __init__(
//|         self,
//|         buffer: WriteableBuffer,
//|         width: int,
//|         height: int,
//|         *,
//|         native_rgb565: bool = False,
//|         rgb332: bool = False,
//|     ) -> None:
//|         """:param ~circuitpython_typing.WriteableBuffer buffer: caller-owned target
//|             buffer of at least ``width * height * 2`` bytes, or ``width * height``
//|             bytes with ``rgb332=True``
//|         :param int width: target width in pixels
//|         :param int height: target height in pixels
//|         :param bool native_rgb565: fill the buffer with native-endian RGB565, the
//|             format 16-bit scanout targets expect. By default the buffer holds
//|             transfer-order RGB565.
//|         :param bool rgb332: fill the buffer with 8-bit RGB332, the format of 8-bit
//|             scanout targets
//|
//|         ``native_rgb565`` and ``rgb332`` cannot both be true. Bitmaps, palettes and
//|         :py:func:`rgb565` values stay transfer-order RGB565 regardless of the
//|         output format."""
//|         ...
//|
//|     width: int
//|     """Target width in pixels. (read-only)"""
//|     height: int
//|     """Target height in pixels. (read-only)"""
//|
//|
// Static SRAM compose strip (see the scratch comment in make_new). 640*16*2 = 20 KB .bss,
// only on CIRCUITPY_PICOGAME_FRAMEBUFFER builds (fb boards have the SRAM to spare).
#define PICOGAME_FB_SCRATCH_MAX_W 640
static uint16_t picogame_fb_scratch_sram[PICOGAME_FB_SCRATCH_MAX_W * PICOGAME_FB_SCRATCH_H];

static mp_obj_t picogame_framebuffer_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_buffer, ARG_width, ARG_height, ARG_native_rgb565, ARG_rgb332 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_native_rgb565, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_rgb332, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_int_t width = mp_arg_validate_int_range(args[ARG_width].u_int, 1, 4096, MP_QSTR_width);
    mp_int_t height = mp_arg_validate_int_range(args[ARG_height].u_int, 1, 4096, MP_QSTR_height);
    if (args[ARG_native_rgb565].u_bool && args[ARG_rgb332].u_bool) {
        mp_arg_error_invalid(MP_QSTR_format);          // native_rgb565 and rgb332 are exclusive
    }
    bool rgb332 = args[ARG_rgb332].u_bool;

    mp_buffer_info_t bi;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bi, MP_BUFFER_WRITE);
    uint64_t need = (uint64_t)width * (uint64_t)height * (rgb332 ? 1u : 2u);
    if ((uint64_t)bi.len < need) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
    }

    picogame_framebuffer_obj_t *self = mp_obj_malloc(picogame_framebuffer_obj_t, type);
    self->buffer = args[ARG_buffer].u_obj;
    self->fb = (uint16_t *)bi.buf;
    self->width = width;
    self->height = height;
    self->fmt = rgb332 ? PICOGAME_FB_RGB332
        : (args[ARG_native_rgb565].u_bool ? PICOGAME_FB_NATIVE565 : PICOGAME_FB_WIRE565);
    // A LIVE scanout buffer (picodvi/HDMI) is read continuously, so picogame_render_framebuffer
    // composes each band into this PRIVATE strip and only memcpys the FINISHED band into the fb.
    // That serves BOTH targets: (a) native -> also byte-swap the strip so the fb never holds wire
    // (no pink); (b) wire -> no swap, but the off-screen compose still stops the beam from sampling
    // a half-composited region (background filled, sprite not yet drawn) = no sprite/HUD flicker.
    // Always allocated for the FB target; the WASM/sim path (read out after present, not live) just
    // pays a small strip + one memcpy. See PICOGAME_FB_SCRATCH_H.
    //
    // The scratch must be FAST memory: on a PSRAM-heap board (Fruit Jam) a heap bytearray
    // lands in external PSRAM and every compose write pays QSPI latency (measured 8.7 vs
    // 64+ MB/s SRAM; a full-res StripDraw frame ballooned refresh to ~30-38 ms). One static
    // SRAM strip serves every Framebuffer (compose is synchronous) up to 640 px wide; wider
    // targets fall back to the heap.
    self->scratch_buf = mp_const_none;
    self->scratch = NULL;
    self->scratch_rows = 0;
    {
        int rows = PICOGAME_FB_SCRATCH_H;
        if (rows > height) {
            rows = height;
        }
        if (width <= PICOGAME_FB_SCRATCH_MAX_W) {
            self->scratch = picogame_fb_scratch_sram;
        } else {
            mp_obj_t sb = mp_obj_new_bytearray_of_zeros((size_t)width * (size_t)rows * 2u);
            mp_buffer_info_t sbi;
            mp_get_buffer_raise(sb, &sbi, MP_BUFFER_WRITE);
            self->scratch_buf = sb;
            self->scratch = (uint16_t *)sbi.buf;
        }
        self->scratch_rows = rows;
    }
    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t picogame_framebuffer_get_width(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_framebuffer_obj_t *)MP_OBJ_TO_PTR(self_in))->width);
}
static MP_DEFINE_CONST_FUN_OBJ_1(picogame_framebuffer_get_width_obj, picogame_framebuffer_get_width);
MP_PROPERTY_GETTER(picogame_framebuffer_width_obj, (mp_obj_t)&picogame_framebuffer_get_width_obj);

static mp_obj_t picogame_framebuffer_get_height(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_framebuffer_obj_t *)MP_OBJ_TO_PTR(self_in))->height);
}
static MP_DEFINE_CONST_FUN_OBJ_1(picogame_framebuffer_get_height_obj, picogame_framebuffer_get_height);
MP_PROPERTY_GETTER(picogame_framebuffer_height_obj, (mp_obj_t)&picogame_framebuffer_get_height_obj);

static const mp_rom_map_elem_t picogame_framebuffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&picogame_framebuffer_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&picogame_framebuffer_height_obj) },
};
static MP_DEFINE_CONST_DICT(picogame_framebuffer_locals_dict, picogame_framebuffer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picogame_framebuffer_type,
    MP_QSTR_Framebuffer,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, picogame_framebuffer_make_new,
    locals_dict, &picogame_framebuffer_locals_dict
    );
#endif // CIRCUITPY_PICOGAME_FRAMEBUFFER
