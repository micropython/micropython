/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 James Bowman for Excamera Labs
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

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "py/runtime.h"
#include "py/binary.h"

#include "shared-module/_eve/__init__.h"
#include "shared-bindings/_eve/__init__.h"

//| """Low-level BridgeTek EVE bindings
//|
//| The `_eve` module provides a class _EVE which
//| contains methods for constructing EVE command
//| buffers and appending basic graphics commands."""
//|

//| class _EVE:
//|

typedef struct _mp_obj__EVE_t {
    mp_obj_base_t base;
    common_hal__eve_t _eve;
} mp_obj__EVE_t;

STATIC const mp_obj_type_t _EVE_type;

#define EVEHAL(s) \
    (&((mp_obj__EVE_t *)mp_obj_cast_to_native_base((s), &_EVE_type))->_eve)

//|     def register(self, o: object) -> None:
//|         ...
//|
STATIC mp_obj_t _register(mp_obj_t self, mp_obj_t o) {
    common_hal__eve_t *eve = EVEHAL(self);
    mp_load_method(o, MP_QSTR_write, eve->dest);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(register_obj, _register);

//|     def flush(self) -> None:
//|         """Send any queued drawing commands directly to the hardware.
//|
//|         :param int width: The width of the grid in tiles, or 1 for sprites."""
//|         ...
//|
STATIC mp_obj_t _flush(mp_obj_t self) {
    common_hal__eve_flush(EVEHAL(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(flush_obj, _flush);

//|     def cc(self, b: ReadableBuffer) -> None:
//|         """Append bytes to the command FIFO.
//|
//|         :param ~_typing.ReadableBuffer b: The bytes to add"""
//|         ...
//|
STATIC mp_obj_t _cc(mp_obj_t self, mp_obj_t b) {
    mp_buffer_info_t buffer_info;
    mp_get_buffer_raise(b, &buffer_info, MP_BUFFER_READ);
    common_hal__eve_add(EVEHAL(self), buffer_info.len, buffer_info.buf);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc_obj, _cc);

// {

//|     def AlphaFunc(self, func: int, ref: int) -> None:
//|         """Set the alpha test function
//|
//|         :param int func: specifies the test function, one of ``NEVER``, ``LESS``, ``LEQUAL``, ``GREATER``, ``GEQUAL``, ``EQUAL``, ``NOTEQUAL``, or ``ALWAYS``. Range 0-7. The initial value is ALWAYS(7)
//|         :param int ref: specifies the reference value for the alpha test. Range 0-255. The initial value is 0
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _alphafunc(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    uint32_t func = mp_obj_get_int_truncated(a0);
    uint32_t ref = mp_obj_get_int_truncated(a1);
    common_hal__eve_AlphaFunc(EVEHAL(self), func, ref);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(alphafunc_obj, _alphafunc);

//|     def Begin(self, prim: int) -> None:
//|         """Begin drawing a graphics primitive
//|
//|         :param int prim: graphics primitive.
//|
//|         Valid primitives are ``BITMAPS``, ``POINTS``, ``LINES``, ``LINE_STRIP``, ``EDGE_STRIP_R``, ``EDGE_STRIP_L``, ``EDGE_STRIP_A``, ``EDGE_STRIP_B`` and ``RECTS``."""
//|         ...
//|

STATIC mp_obj_t _begin(mp_obj_t self, mp_obj_t a0) {
    uint32_t prim = mp_obj_get_int_truncated(a0);
    common_hal__eve_Begin(EVEHAL(self), prim);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(begin_obj, _begin);

//|     def BitmapExtFormat(self, format: int) -> None:
//|         """Set the bitmap format
//|
//|         :param int format: bitmap pixel format."""
//|         ...
//|

STATIC mp_obj_t _bitmapextformat(mp_obj_t self, mp_obj_t a0) {
    uint32_t fmt = mp_obj_get_int_truncated(a0);
    common_hal__eve_BitmapExtFormat(EVEHAL(self), fmt);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bitmapextformat_obj, _bitmapextformat);

//|     def BitmapHandle(self, handle: int) -> None:
//|         """Set the bitmap handle
//|
//|         :param int handle: bitmap handle. Range 0-31. The initial value is 0
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _bitmaphandle(mp_obj_t self, mp_obj_t a0) {
    uint32_t handle = mp_obj_get_int_truncated(a0);
    common_hal__eve_BitmapHandle(EVEHAL(self), handle);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bitmaphandle_obj, _bitmaphandle);

//|     def BitmapLayoutH(self, linestride: int, height: int) -> None:
//|         """Set the source bitmap memory format and layout for the current handle. high bits for large bitmaps
//|
//|         :param int linestride: high part of bitmap line stride, in bytes. Range 0-7
//|         :param int height: high part of bitmap height, in lines. Range 0-3"""
//|         ...
//|

STATIC mp_obj_t _bitmaplayouth(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    uint32_t linestride = mp_obj_get_int_truncated(a0);
    uint32_t height = mp_obj_get_int_truncated(a1);
    common_hal__eve_BitmapLayoutH(EVEHAL(self), linestride, height);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaplayouth_obj, _bitmaplayouth);

//|     def BitmapLayout(self, format: int, linestride: int, height: int) -> None:
//|         """Set the source bitmap memory format and layout for the current handle
//|
//|         :param int format: bitmap pixel format, or GLFORMAT to use BITMAP_EXT_FORMAT instead. Range 0-31
//|         :param int linestride: bitmap line stride, in bytes. Range 0-1023
//|         :param int height: bitmap height, in lines. Range 0-511"""
//|         ...
//|

STATIC mp_obj_t _bitmaplayout(size_t n_args, const mp_obj_t *args) {
    uint32_t format = mp_obj_get_int_truncated(args[1]);
    uint32_t linestride = mp_obj_get_int_truncated(args[2]);
    uint32_t height = mp_obj_get_int_truncated(args[3]);
    common_hal__eve_BitmapLayout(EVEHAL(args[0]), format, linestride, height);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitmaplayout_obj, 4, 4, _bitmaplayout);

//|     def BitmapSizeH(self, width: int, height: int) -> None:
//|         """Set the screen drawing of bitmaps for the current handle. high bits for large bitmaps
//|
//|         :param int width: high part of drawn bitmap width, in pixels. Range 0-3
//|         :param int height: high part of drawn bitmap height, in pixels. Range 0-3"""
//|         ...
//|

STATIC mp_obj_t _bitmapsizeh(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    uint32_t width = mp_obj_get_int_truncated(a0);
    uint32_t height = mp_obj_get_int_truncated(a1);
    common_hal__eve_BitmapSizeH(EVEHAL(self), width, height);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmapsizeh_obj, _bitmapsizeh);

//|     def BitmapSize(self, filter: int, wrapx: int, wrapy: int, width: int, height: int) -> None:
//|         """Set the screen drawing of bitmaps for the current handle
//|
//|         :param int filter: bitmap filtering mode, one of ``NEAREST`` or ``BILINEAR``. Range 0-1
//|         :param int wrapx: bitmap :math:`x` wrap mode, one of ``REPEAT`` or ``BORDER``. Range 0-1
//|         :param int wrapy: bitmap :math:`y` wrap mode, one of ``REPEAT`` or ``BORDER``. Range 0-1
//|         :param int width: drawn bitmap width, in pixels. Range 0-511
//|         :param int height: drawn bitmap height, in pixels. Range 0-511"""
//|         ...
//|

STATIC mp_obj_t _bitmapsize(size_t n_args, const mp_obj_t *args) {
    uint32_t filter = mp_obj_get_int_truncated(args[1]);
    uint32_t wrapx = mp_obj_get_int_truncated(args[2]);
    uint32_t wrapy = mp_obj_get_int_truncated(args[3]);
    uint32_t width = mp_obj_get_int_truncated(args[4]);
    uint32_t height = mp_obj_get_int_truncated(args[5]);
    common_hal__eve_BitmapSize(EVEHAL(args[0]), filter, wrapx, wrapy, width, height);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitmapsize_obj, 6, 6, _bitmapsize);

//|     def BitmapSource(self, addr: int) -> None:
//|         """Set the source address for bitmap graphics
//|
//|         :param int addr: Bitmap start address, pixel-aligned. May be in SRAM or flash. Range 0-16777215"""
//|         ...
//|

STATIC mp_obj_t _bitmapsource(mp_obj_t self, mp_obj_t a0) {
    uint32_t addr = mp_obj_get_int_truncated(a0);
    common_hal__eve_BitmapSource(EVEHAL(self), addr);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bitmapsource_obj, _bitmapsource);

//|     def BitmapSwizzle(self, r: int, g: int, b: int, a: int) -> None:
//|         """Set the source for the r,g,b and a channels of a bitmap
//|
//|         :param int r: red component source channel. Range 0-7
//|         :param int g: green component source channel. Range 0-7
//|         :param int b: blue component source channel. Range 0-7
//|         :param int a: alpha component source channel. Range 0-7"""
//|         ...
//|

STATIC mp_obj_t _bitmapswizzle(size_t n_args, const mp_obj_t *args) {
    uint32_t r = mp_obj_get_int_truncated(args[1]);
    uint32_t g = mp_obj_get_int_truncated(args[2]);
    uint32_t b = mp_obj_get_int_truncated(args[3]);
    uint32_t a = mp_obj_get_int_truncated(args[4]);
    common_hal__eve_BitmapSwizzle(EVEHAL(args[0]), r, g, b, a);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitmapswizzle_obj, 5, 5, _bitmapswizzle);

//|     def BitmapTransformA(self, p: int, v: int) -> None:
//|         """Set the :math:`a` component of the bitmap transform matrix
//|
//|         :param int p: precision control: 0 is 8.8, 1 is 1.15. Range 0-1. The initial value is 0
//|         :param int v: The :math:`a` component of the bitmap transform matrix, in signed 8.8 or 1.15 bit fixed-point form. Range 0-131071. The initial value is 256
//|
//|         The initial value is **p** = 0, **v** = 256. This represents the value 1.0.
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _bitmaptransforma(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    uint32_t p = mp_obj_get_int_truncated(a0);
    uint32_t v = mp_obj_get_int_truncated(a1);
    common_hal__eve_BitmapTransformA(EVEHAL(self), p, v);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaptransforma_obj, _bitmaptransforma);

//|     def BitmapTransformB(self, p: int, v: int) -> None:
//|         """Set the :math:`b` component of the bitmap transform matrix
//|
//|         :param int p: precision control: 0 is 8.8, 1 is 1.15. Range 0-1. The initial value is 0
//|         :param int v: The :math:`b` component of the bitmap transform matrix, in signed 8.8 or 1.15 bit fixed-point form. Range 0-131071. The initial value is 0
//|
//|         The initial value is **p** = 0, **v** = 0. This represents the value 0.0.
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _bitmaptransformb(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    uint32_t p = mp_obj_get_int_truncated(a0);
    uint32_t v = mp_obj_get_int_truncated(a1);
    common_hal__eve_BitmapTransformB(EVEHAL(self), p, v);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaptransformb_obj, _bitmaptransformb);

//|     def BitmapTransformC(self, v: int) -> None:
//|         """Set the :math:`c` component of the bitmap transform matrix
//|
//|         :param int v: The :math:`c` component of the bitmap transform matrix, in signed 15.8 bit fixed-point form. Range 0-16777215. The initial value is 0
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _bitmaptransformc(mp_obj_t self, mp_obj_t a0) {
    uint32_t v = mp_obj_get_int_truncated(a0);
    common_hal__eve_BitmapTransformC(EVEHAL(self), v);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bitmaptransformc_obj, _bitmaptransformc);

//|     def BitmapTransformD(self, p: int, v: int) -> None:
//|         """Set the :math:`d` component of the bitmap transform matrix
//|
//|         :param int p: precision control: 0 is 8.8, 1 is 1.15. Range 0-1. The initial value is 0
//|         :param int v: The :math:`d` component of the bitmap transform matrix, in signed 8.8 or 1.15 bit fixed-point form. Range 0-131071. The initial value is 0
//|
//|         The initial value is **p** = 0, **v** = 0. This represents the value 0.0.
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _bitmaptransformd(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    uint32_t p = mp_obj_get_int_truncated(a0);
    uint32_t v = mp_obj_get_int_truncated(a1);
    common_hal__eve_BitmapTransformD(EVEHAL(self), p, v);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaptransformd_obj, _bitmaptransformd);

//|     def BitmapTransformE(self, p: int, v: int) -> None:
//|         """Set the :math:`e` component of the bitmap transform matrix
//|
//|         :param int p: precision control: 0 is 8.8, 1 is 1.15. Range 0-1. The initial value is 0
//|         :param int v: The :math:`e` component of the bitmap transform matrix, in signed 8.8 or 1.15 bit fixed-point form. Range 0-131071. The initial value is 256
//|
//|         The initial value is **p** = 0, **v** = 256. This represents the value 1.0.
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _bitmaptransforme(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    uint32_t p = mp_obj_get_int_truncated(a0);
    uint32_t v = mp_obj_get_int_truncated(a1);
    common_hal__eve_BitmapTransformE(EVEHAL(self), p, v);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bitmaptransforme_obj, _bitmaptransforme);

//|     def BitmapTransformF(self, v: int) -> None:
//|         """Set the :math:`f` component of the bitmap transform matrix
//|
//|         :param int v: The :math:`f` component of the bitmap transform matrix, in signed 15.8 bit fixed-point form. Range 0-16777215. The initial value is 0
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _bitmaptransformf(mp_obj_t self, mp_obj_t a0) {
    uint32_t v = mp_obj_get_int_truncated(a0);
    common_hal__eve_BitmapTransformF(EVEHAL(self), v);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bitmaptransformf_obj, _bitmaptransformf);

//|     def BlendFunc(self, src: int, dst: int) -> None:
//|         """Set pixel arithmetic
//|
//|         :param int src: specifies how the source blending factor is computed.  One of ``ZERO``, ``ONE``, ``SRC_ALPHA``, ``DST_ALPHA``, ``ONE_MINUS_SRC_ALPHA`` or ``ONE_MINUS_DST_ALPHA``. Range 0-7. The initial value is SRC_ALPHA(2)
//|         :param int dst: specifies how the destination blending factor is computed, one of the same constants as **src**. Range 0-7. The initial value is ONE_MINUS_SRC_ALPHA(4)
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _blendfunc(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    uint32_t src = mp_obj_get_int_truncated(a0);
    uint32_t dst = mp_obj_get_int_truncated(a1);
    common_hal__eve_BlendFunc(EVEHAL(self), src, dst);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(blendfunc_obj, _blendfunc);

//|     def Call(self, dest: int) -> None:
//|         """Execute a sequence of commands at another location in the display list
//|
//|         :param int dest: display list address. Range 0-65535"""
//|         ...
//|

STATIC mp_obj_t _call(mp_obj_t self, mp_obj_t a0) {
    uint32_t dest = mp_obj_get_int_truncated(a0);
    common_hal__eve_Call(EVEHAL(self), dest);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(call_obj, _call);

//|     def Cell(self, cell: int) -> None:
//|         """Set the bitmap cell number for the vertex2f command
//|
//|         :param int cell: bitmap cell number. Range 0-127. The initial value is 0
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _cell(mp_obj_t self, mp_obj_t a0) {
    uint32_t cell = mp_obj_get_int_truncated(a0);
    common_hal__eve_Cell(EVEHAL(self), cell);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cell_obj, _cell);

//|     def ClearColorA(self, alpha: int) -> None:
//|         """Set clear value for the alpha channel
//|
//|         :param int alpha: alpha value used when the color buffer is cleared. Range 0-255. The initial value is 0
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _clearcolora(mp_obj_t self, mp_obj_t a0) {
    uint32_t alpha = mp_obj_get_int_truncated(a0);
    common_hal__eve_ClearColorA(EVEHAL(self), alpha);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(clearcolora_obj, _clearcolora);

//|     def ClearColorRGB(self, red: int, green: int, blue: int) -> None:
//|         """Set clear values for red, green and blue channels
//|
//|         :param int red: red value used when the color buffer is cleared. Range 0-255. The initial value is 0
//|         :param int green: green value used when the color buffer is cleared. Range 0-255. The initial value is 0
//|         :param int blue: blue value used when the color buffer is cleared. Range 0-255. The initial value is 0
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _clearcolorrgb(size_t n_args, const mp_obj_t *args) {
    uint32_t red = mp_obj_get_int_truncated(args[1]);
    uint32_t green = mp_obj_get_int_truncated(args[2]);
    uint32_t blue = mp_obj_get_int_truncated(args[3]);
    common_hal__eve_ClearColorRGB(EVEHAL(args[0]), red, green, blue);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(clearcolorrgb_obj, 4, 4, _clearcolorrgb);

//|     def Clear(self, c: int, s: int, t: int) -> None:
//|         """Clear buffers to preset values
//|
//|         :param int c: clear color buffer. Range 0-1
//|         :param int s: clear stencil buffer. Range 0-1
//|         :param int t: clear tag buffer. Range 0-1"""
//|         ...
//|

STATIC mp_obj_t _clear(size_t n_args, const mp_obj_t *args) {
    uint32_t c = (n_args > 1) ? mp_obj_get_int_truncated(args[1]) : 1;
    uint32_t s = (n_args > 2) ? mp_obj_get_int_truncated(args[2]) : 1;
    uint32_t t = (n_args > 3) ? mp_obj_get_int_truncated(args[3]) : 1;
    common_hal__eve_Clear(EVEHAL(args[0]), c, s, t);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(clear_obj, 1, 4, _clear);

//|     def ClearStencil(self, s: int) -> None:
//|         """Set clear value for the stencil buffer
//|
//|         :param int s: value used when the stencil buffer is cleared. Range 0-255. The initial value is 0
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _clearstencil(mp_obj_t self, mp_obj_t a0) {
    uint32_t s = mp_obj_get_int_truncated(a0);
    common_hal__eve_ClearStencil(EVEHAL(self), s);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(clearstencil_obj, _clearstencil);

//|     def ClearTag(self, s: int) -> None:
//|         """Set clear value for the tag buffer
//|
//|         :param int s: value used when the tag buffer is cleared. Range 0-255. The initial value is 0
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|

STATIC mp_obj_t _cleartag(mp_obj_t self, mp_obj_t a0) {
    uint32_t s = mp_obj_get_int_truncated(a0);
    common_hal__eve_ClearTag(EVEHAL(self), s);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cleartag_obj, _cleartag);

//|     def ColorA(self, alpha: int) -> None:
//|         """Set the current color alpha
//|
//|         :param int alpha: alpha for the current color. Range 0-255. The initial value is 255
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _colora(mp_obj_t self, mp_obj_t a0) {
    uint32_t alpha = mp_obj_get_int_truncated(a0);
    common_hal__eve_ColorA(EVEHAL(self), alpha);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(colora_obj, _colora);

//|     def ColorMask(self, r: int, g: int, b: int, a: int) -> None:
//|         """Enable and disable writing of frame buffer color components
//|
//|         :param int r: allow updates to the frame buffer red component. Range 0-1. The initial value is 1
//|         :param int g: allow updates to the frame buffer green component. Range 0-1. The initial value is 1
//|         :param int b: allow updates to the frame buffer blue component. Range 0-1. The initial value is 1
//|         :param int a: allow updates to the frame buffer alpha component. Range 0-1. The initial value is 1
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _colormask(size_t n_args, const mp_obj_t *args) {
    uint32_t r = mp_obj_get_int_truncated(args[1]);
    uint32_t g = mp_obj_get_int_truncated(args[2]);
    uint32_t b = mp_obj_get_int_truncated(args[3]);
    uint32_t a = mp_obj_get_int_truncated(args[4]);
    common_hal__eve_ColorMask(EVEHAL(args[0]), r, g, b, a);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(colormask_obj, 5, 5, _colormask);

//|     def ColorRGB(self, red: int, green: int, blue: int) -> None:
//|         """Set the drawing color
//|
//|         :param int red: red value for the current color. Range 0-255. The initial value is 255
//|         :param int green: green for the current color. Range 0-255. The initial value is 255
//|         :param int blue: blue for the current color. Range 0-255. The initial value is 255
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _colorrgb(size_t n_args, const mp_obj_t *args) {
    uint32_t red = mp_obj_get_int_truncated(args[1]);
    uint32_t green = mp_obj_get_int_truncated(args[2]);
    uint32_t blue = mp_obj_get_int_truncated(args[3]);
    common_hal__eve_ColorRGB(EVEHAL(args[0]), red, green, blue);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(colorrgb_obj, 4, 4, _colorrgb);

//|     def Display(self) -> None:
//|         """End the display list"""
//|         ...

STATIC mp_obj_t _display(mp_obj_t self) {

    common_hal__eve_Display(EVEHAL(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_obj, _display);

//|     def End(self) -> None:
//|         """End drawing a graphics primitive
//|
//|         :meth:`Vertex2ii` and :meth:`Vertex2f` calls are ignored until the next :meth:`Begin`."""
//|         ...
//|

STATIC mp_obj_t _end(mp_obj_t self) {

    common_hal__eve_End(EVEHAL(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(end_obj, _end);

//|     def Jump(self, dest: int) -> None:
//|         """Execute commands at another location in the display list
//|
//|         :param int dest: display list address. Range 0-65535"""
//|         ...
//|

STATIC mp_obj_t _jump(mp_obj_t self, mp_obj_t a0) {
    uint32_t dest = mp_obj_get_int_truncated(a0);
    common_hal__eve_Jump(EVEHAL(self), dest);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(jump_obj, _jump);

//|     def Macro(self, m: int) -> None:
//|         """Execute a single command from a macro register
//|
//|         :param int m: macro register to read. Range 0-1"""
//|         ...
//|

STATIC mp_obj_t _macro(mp_obj_t self, mp_obj_t a0) {
    uint32_t m = mp_obj_get_int_truncated(a0);
    common_hal__eve_Macro(EVEHAL(self), m);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(macro_obj, _macro);

//|     def Nop(self) -> None:
//|         """No operation"""
//|         ...
//|

STATIC mp_obj_t _nop(mp_obj_t self) {

    common_hal__eve_Nop(EVEHAL(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(nop_obj, _nop);

//|     def PaletteSource(self, addr: int) -> None:
//|         """Set the base address of the palette
//|
//|         :param int addr: Address in graphics SRAM, 2-byte aligned. Range 0-4194303. The initial value is 0
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _palettesource(mp_obj_t self, mp_obj_t a0) {
    uint32_t addr = mp_obj_get_int_truncated(a0);
    common_hal__eve_PaletteSource(EVEHAL(self), addr);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(palettesource_obj, _palettesource);

//|     def RestoreContext(self) -> None:
//|         """Restore the current graphics context from the context stack"""
//|         ...
//|

STATIC mp_obj_t _restorecontext(mp_obj_t self) {

    common_hal__eve_RestoreContext(EVEHAL(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(restorecontext_obj, _restorecontext);

//|     def Return(self) -> None:
//|         """Return from a previous call command"""
//|         ...
//|

STATIC mp_obj_t _return(mp_obj_t self) {

    common_hal__eve_Return(EVEHAL(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(return_obj, _return);

//|     def SaveContext(self) -> None:
//|         """Push the current graphics context on the context stack"""
//|         ...
//|

STATIC mp_obj_t _savecontext(mp_obj_t self) {

    common_hal__eve_SaveContext(EVEHAL(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(savecontext_obj, _savecontext);

//|     def ScissorSize(self, width: int, height: int) -> None:
//|         """Set the size of the scissor clip rectangle
//|
//|         :param int width: The width of the scissor clip rectangle, in pixels. Range 0-4095. The initial value is hsize
//|         :param int height: The height of the scissor clip rectangle, in pixels. Range 0-4095. The initial value is 2048
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _scissorsize(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    uint32_t width = mp_obj_get_int_truncated(a0);
    uint32_t height = mp_obj_get_int_truncated(a1);
    common_hal__eve_ScissorSize(EVEHAL(self), width, height);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(scissorsize_obj, _scissorsize);

//|     def ScissorXY(self, x: int, y: int) -> None:
//|         """Set the top left corner of the scissor clip rectangle
//|
//|         :param int x: The :math:`x` coordinate of the scissor clip rectangle, in pixels. Range 0-2047. The initial value is 0
//|         :param int y: The :math:`y` coordinate of the scissor clip rectangle, in pixels. Range 0-2047. The initial value is 0
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _scissorxy(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    uint32_t x = mp_obj_get_int_truncated(a0);
    uint32_t y = mp_obj_get_int_truncated(a1);
    common_hal__eve_ScissorXY(EVEHAL(self), x, y);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(scissorxy_obj, _scissorxy);

//|     def StencilFunc(self, func: int, ref: int, mask: int) -> None:
//|         """Set function and reference value for stencil testing
//|
//|         :param int func: specifies the test function, one of ``NEVER``, ``LESS``, ``LEQUAL``, ``GREATER``, ``GEQUAL``, ``EQUAL``, ``NOTEQUAL``, or ``ALWAYS``. Range 0-7. The initial value is ALWAYS(7)
//|         :param int ref: specifies the reference value for the stencil test. Range 0-255. The initial value is 0
//|         :param int mask: specifies a mask that is ANDed with the reference value and the stored stencil value. Range 0-255. The initial value is 255
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _stencilfunc(size_t n_args, const mp_obj_t *args) {
    uint32_t func = mp_obj_get_int_truncated(args[1]);
    uint32_t ref = mp_obj_get_int_truncated(args[2]);
    uint32_t mask = mp_obj_get_int_truncated(args[3]);
    common_hal__eve_StencilFunc(EVEHAL(args[0]), func, ref, mask);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(stencilfunc_obj, 4, 4, _stencilfunc);

//|     def StencilMask(self, mask: int) -> None:
//|         """Control the writing of individual bits in the stencil planes
//|
//|         :param int mask: the mask used to enable writing stencil bits. Range 0-255. The initial value is 255
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _stencilmask(mp_obj_t self, mp_obj_t a0) {
    uint32_t mask = mp_obj_get_int_truncated(a0);
    common_hal__eve_StencilMask(EVEHAL(self), mask);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(stencilmask_obj, _stencilmask);

//|     def StencilOp(self, sfail: int, spass: int) -> None:
//|         """Set stencil test actions
//|
//|         :param int sfail: specifies the action to take when the stencil test fails, one of ``KEEP``, ``ZERO``, ``REPLACE``, ``INCR``, ``INCR_WRAP``, ``DECR``, ``DECR_WRAP``, and ``INVERT``. Range 0-7. The initial value is KEEP(1)
//|         :param int spass: specifies the action to take when the stencil test passes, one of the same constants as **sfail**. Range 0-7. The initial value is KEEP(1)
//|
//|         These values are part of the graphics context and are saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _stencilop(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    uint32_t sfail = mp_obj_get_int_truncated(a0);
    uint32_t spass = mp_obj_get_int_truncated(a1);
    common_hal__eve_StencilOp(EVEHAL(self), sfail, spass);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(stencilop_obj, _stencilop);

//|     def TagMask(self, mask: int) -> None:
//|         """Control the writing of the tag buffer
//|
//|         :param int mask: allow updates to the tag buffer. Range 0-1. The initial value is 1
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _tagmask(mp_obj_t self, mp_obj_t a0) {
    uint32_t mask = mp_obj_get_int_truncated(a0);
    common_hal__eve_TagMask(EVEHAL(self), mask);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tagmask_obj, _tagmask);

//|     def Tag(self, s: int) -> None:
//|         """Set the current tag value
//|
//|         :param int s: tag value. Range 0-255. The initial value is 255
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _tag(mp_obj_t self, mp_obj_t a0) {
    uint32_t s = mp_obj_get_int_truncated(a0);
    common_hal__eve_Tag(EVEHAL(self), s);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tag_obj, _tag);

STATIC mp_obj_t _vertexformat(mp_obj_t self, mp_obj_t a0) {
    uint32_t frac = mp_obj_get_int_truncated(a0);
    common_hal__eve_VertexFormat(EVEHAL(self), frac);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vertexformat_obj, _vertexformat);

//|     def Vertex2ii(self, x: int, y: int, handle: int, cell: int) -> None:
//|         """:param int x: x-coordinate in pixels. Range 0-511
//|         :param int y: y-coordinate in pixels. Range 0-511
//|         :param int handle: bitmap handle. Range 0-31
//|         :param int cell: cell number. Range 0-127
//|
//|         This method is an alternative to :meth:`Vertex2f`."""
//|         ...
//|

STATIC mp_obj_t _vertex2ii(size_t n_args, const mp_obj_t *args) {
    uint32_t x = mp_obj_get_int_truncated(args[1]);
    uint32_t y = mp_obj_get_int_truncated(args[2]);
    uint32_t handle = (n_args > 3) ? mp_obj_get_int_truncated(args[3]) : 0;
    uint32_t cell = (n_args > 4) ? mp_obj_get_int_truncated(args[4]) : 0;
    common_hal__eve_Vertex2ii(EVEHAL(args[0]), x, y, handle, cell);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(vertex2ii_obj, 3, 5, _vertex2ii);

#define ROM_DECLS \
    { MP_ROM_QSTR(MP_QSTR_AlphaFunc), MP_ROM_PTR(&alphafunc_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Begin), MP_ROM_PTR(&begin_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapExtFormat), MP_ROM_PTR(&bitmapextformat_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapHandle), MP_ROM_PTR(&bitmaphandle_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapLayoutH), MP_ROM_PTR(&bitmaplayouth_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapLayout), MP_ROM_PTR(&bitmaplayout_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapSizeH), MP_ROM_PTR(&bitmapsizeh_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapSize), MP_ROM_PTR(&bitmapsize_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapSource), MP_ROM_PTR(&bitmapsource_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapSwizzle), MP_ROM_PTR(&bitmapswizzle_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapTransformA), MP_ROM_PTR(&bitmaptransforma_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapTransformB), MP_ROM_PTR(&bitmaptransformb_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapTransformC), MP_ROM_PTR(&bitmaptransformc_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapTransformD), MP_ROM_PTR(&bitmaptransformd_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapTransformE), MP_ROM_PTR(&bitmaptransforme_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BitmapTransformF), MP_ROM_PTR(&bitmaptransformf_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_BlendFunc), MP_ROM_PTR(&blendfunc_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Call), MP_ROM_PTR(&call_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Cell), MP_ROM_PTR(&cell_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_ClearColorA), MP_ROM_PTR(&clearcolora_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_ClearColorRGB), MP_ROM_PTR(&clearcolorrgb_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Clear), MP_ROM_PTR(&clear_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_ClearStencil), MP_ROM_PTR(&clearstencil_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_ClearTag), MP_ROM_PTR(&cleartag_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_ColorA), MP_ROM_PTR(&colora_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_ColorMask), MP_ROM_PTR(&colormask_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_ColorRGB), MP_ROM_PTR(&colorrgb_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Display), MP_ROM_PTR(&display_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_End), MP_ROM_PTR(&end_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Jump), MP_ROM_PTR(&jump_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_LineWidth), MP_ROM_PTR(&linewidth_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Macro), MP_ROM_PTR(&macro_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Nop), MP_ROM_PTR(&nop_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_PaletteSource), MP_ROM_PTR(&palettesource_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_PointSize), MP_ROM_PTR(&pointsize_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_RestoreContext), MP_ROM_PTR(&restorecontext_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Return), MP_ROM_PTR(&return_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_SaveContext), MP_ROM_PTR(&savecontext_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_ScissorSize), MP_ROM_PTR(&scissorsize_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_ScissorXY), MP_ROM_PTR(&scissorxy_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_StencilFunc), MP_ROM_PTR(&stencilfunc_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_StencilMask), MP_ROM_PTR(&stencilmask_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_StencilOp), MP_ROM_PTR(&stencilop_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_TagMask), MP_ROM_PTR(&tagmask_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Tag), MP_ROM_PTR(&tag_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_VertexTranslateX), MP_ROM_PTR(&vertextranslatex_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_VertexTranslateY), MP_ROM_PTR(&vertextranslatey_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_VertexFormat), MP_ROM_PTR(&vertexformat_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_Vertex2ii), MP_ROM_PTR(&vertex2ii_obj) }

// }

// Hand-written functions {

//|     def Vertex2f(self, b: float) -> None:
//|         """Draw a point.
//|
//|         :param float x: pixel x-coordinate
//|         :param float y: pixel y-coordinate"""
//|         ...
//|
STATIC mp_obj_t _vertex2f(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    mp_float_t x = mp_obj_get_float(a0);
    mp_float_t y = mp_obj_get_float(a1);
    common_hal__eve_Vertex2f(EVEHAL(self), x, y);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vertex2f_obj, _vertex2f);

//|     def LineWidth(self, width: float) -> None:
//|         """Set the width of rasterized lines
//|
//|         :param float width: line width in pixels. Range 0-511. The initial value is 1
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _linewidth(mp_obj_t self, mp_obj_t a0) {
    mp_float_t width = mp_obj_get_float(a0);
    common_hal__eve_LineWidth(EVEHAL(self), width);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(linewidth_obj, _linewidth);

//|     def PointSize(self, size: float) -> None:
//|         """Set the diameter of rasterized points
//|
//|         :param float size: point diameter in pixels. Range 0-1023. The initial value is 1
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _pointsize(mp_obj_t self, mp_obj_t a0) {
    mp_float_t size = mp_obj_get_float(a0);
    common_hal__eve_PointSize(EVEHAL(self), size);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pointsize_obj, _pointsize);

//|     def VertexTranslateX(self, x: float) -> None:
//|         """Set the vertex transformation's x translation component
//|
//|         :param float x: signed x-coordinate in pixels. Range ±4095. The initial value is 0
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

STATIC mp_obj_t _vertextranslatex(mp_obj_t self, mp_obj_t a0) {
    mp_float_t x = mp_obj_get_float(a0);
    common_hal__eve_VertexTranslateX(EVEHAL(self), x);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vertextranslatex_obj, _vertextranslatex);

//|     def VertexTranslateY(self, y: float) -> None:
//|         """Set the vertex transformation's y translation component
//|
//|         :param float y: signed y-coordinate in pixels. Range ±4095. The initial value is 0
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|


STATIC mp_obj_t _vertextranslatey(mp_obj_t self, mp_obj_t a0) {
    mp_float_t y = mp_obj_get_float(a0);
    common_hal__eve_VertexTranslateY(EVEHAL(self), y);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vertextranslatey_obj, _vertextranslatey);

//|     def VertexFormat(self, frac: int) -> None:
//|         """Set the precision of vertex2f coordinates
//|
//|         :param int frac: Number of fractional bits in X,Y coordinates, 0-4. Range 0-7. The initial value is 4
//|
//|         This value is part of the graphics context and is saved and restored by :meth:`SaveContext` and :meth:`RestoreContext`."""
//|         ...
//|

// }

// Append an object x to the FIFO
#define ADD_X(self, x) \
    common_hal__eve_add(EVEHAL(self), sizeof(x), &(x));

//|     def cmd0(self, n: int) -> None:
//|         """Append the command word n to the FIFO
//|
//|         :param int n: The command code
//|
//|         This method is used by the ``eve`` module to efficiently add
//|         commands to the FIFO."""
//|         ...
//|

STATIC mp_obj_t _cmd0(mp_obj_t self, mp_obj_t n) {
    uint32_t code = 0xffffff00 | mp_obj_get_int_truncated(n);
    ADD_X(self, code);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cmd0_obj, _cmd0);

//|     def cmd(self, n: int, fmt: str, args: Tuple[str, ...]) -> None:
//|         """Append a command packet to the FIFO.
//|
//|         :param int n: The command code
//|         :param str fmt: The command format `struct` layout
//|         :param args: The command's arguments
//|         :type args: tuple(str, ...)
//|
//|         Supported format codes: h, H, i, I.
//|
//|         This method is used by the ``eve`` module to efficiently add
//|         commands to the FIFO."""
//|         ...
//|
STATIC mp_obj_t _cmd(size_t n_args, const mp_obj_t *args) {
    mp_obj_t self = args[0];
    mp_obj_t num = args[1];
    mp_buffer_info_t fmt;
    mp_get_buffer_raise(args[2], &fmt, MP_BUFFER_READ);
    size_t len;
    mp_obj_t *items;
    mp_obj_tuple_get(args[3], &len, &items);

    // Count how many 32-bit words required
    size_t n = 0;
    for (size_t i = 0; i < fmt.len; n++) {
        switch (((char *)fmt.buf)[i]) {
            case 'I':
            case 'i':
                i += 1;
                break;
            case 'H':
            case 'h':
                i += 2;
                break;
            default:
                break;
        }
    }

    uint32_t buf[16];
    uint32_t *p = buf;
    *p++ = 0xffffff00 | mp_obj_get_int_truncated(num);
    mp_obj_t *a = items;
    uint32_t lo;

    for (size_t i = 0; i < fmt.len;) {
        switch (((char *)fmt.buf)[i]) {
            case 'I':
            case 'i':
                *p++ = mp_obj_get_int_truncated(*a++);
                i += 1;
                break;
            case 'H':
            case 'h':
                lo = mp_obj_get_int_truncated(*a++) & 0xffff;
                *p++ = lo | (mp_obj_get_int_truncated(*a++) << 16);
                i += 2;
                break;
            default:
                break;
        }
    }

    common_hal__eve_add(EVEHAL(self), sizeof(uint32_t) * (1 + n), buf);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(cmd_obj, 4, 4, _cmd);

STATIC const mp_rom_map_elem_t _EVE_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&register_obj) },
    { MP_ROM_QSTR(MP_QSTR_cc), MP_ROM_PTR(&cc_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_Vertex2f), MP_ROM_PTR(&vertex2f_obj) },
    { MP_ROM_QSTR(MP_QSTR_cmd), MP_ROM_PTR(&cmd_obj) },
    { MP_ROM_QSTR(MP_QSTR_cmd0), MP_ROM_PTR(&cmd0_obj) },
    ROM_DECLS
};
STATIC MP_DEFINE_CONST_DICT(_EVE_locals_dict, _EVE_locals_dict_table);

STATIC mp_obj_t _EVE_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // mp_arg_check_num(n_args, kw_args, 1, 1, false);
    mp_obj__EVE_t *o = m_new_obj(mp_obj__EVE_t);
    o->base.type = &_EVE_type;
    o->_eve.n = 0;
    o->_eve.vscale = 16;
    return MP_OBJ_FROM_PTR(o);
}

STATIC const mp_obj_type_t _EVE_type = {
    { &mp_type_type },
    .name = MP_QSTR__EVE,
    .make_new = _EVE_make_new,
    .locals_dict = (void *)&_EVE_locals_dict,
};

STATIC const mp_rom_map_elem_t mp_module__eve_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__eve) },
    { MP_ROM_QSTR(MP_QSTR__EVE), MP_OBJ_FROM_PTR(&_EVE_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module__eve_globals, mp_module__eve_globals_table);

const mp_obj_module_t _eve_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module__eve_globals,
};
