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

#include <stddef.h>
#include <stdint.h>
#include "py/runtime.h"
#include "shared-module/_eve/__init__.h"

STATIC void write(common_hal__eve_t *eve, size_t len, void *buf) {
    eve->dest[2] = mp_obj_new_bytearray_by_ref(len, buf);
    mp_call_method_n_kw(1, 0, eve->dest);
}

void common_hal__eve_flush(common_hal__eve_t *eve) {
    if (eve->n != 0) {
        write(eve, eve->n, eve->buf);
        eve->n = 0;
    }
}

static void *append(common_hal__eve_t *eve, size_t m) {
    if ((eve->n + m) > sizeof(eve->buf)) {
        common_hal__eve_flush(eve);
    }
    uint8_t *r = eve->buf + eve->n;
    eve->n += m;
    return (void *)r;
}

void common_hal__eve_add(common_hal__eve_t *eve, size_t len, void *buf) {
    if (len <= sizeof(eve->buf)) {
        uint8_t *p = (uint8_t *)append(eve, len);
        // memcpy(p, buffer_info.buf, buffer_info.len);
        uint8_t *s = buf;
        for (size_t i = 0; i < len; i++) { *p++ = *s++;
        }
    } else {
        common_hal__eve_flush(eve);
        write(eve, len, buf);
    }
}

#define C4(eve, u) (*(uint32_t *)append((eve), sizeof(uint32_t)) = (u))

void common_hal__eve_Vertex2f(common_hal__eve_t *eve, mp_float_t x, mp_float_t y) {
    int16_t ix = (int)(eve->vscale * x);
    int16_t iy = (int)(eve->vscale * y);
    C4(eve, (1 << 30) | ((ix & 32767) << 15) | (iy & 32767));
}

void common_hal__eve_VertexFormat(common_hal__eve_t *eve, uint32_t frac) {
    C4(eve, ((39 << 24) | ((frac & 7))));
    eve->vscale = 1 << frac;
}



void common_hal__eve_AlphaFunc(common_hal__eve_t *eve, uint32_t func, uint32_t ref) {
    C4(eve, ((9 << 24) | ((func & 7) << 8) | ((ref & 255))));
}


void common_hal__eve_Begin(common_hal__eve_t *eve, uint32_t prim) {
    C4(eve, ((31 << 24) | ((prim & 15))));
}


void common_hal__eve_BitmapExtFormat(common_hal__eve_t *eve, uint32_t fmt) {
    C4(eve, ((46 << 24) | (fmt & 65535)));
}


void common_hal__eve_BitmapHandle(common_hal__eve_t *eve, uint32_t handle) {
    C4(eve, ((5 << 24) | ((handle & 31))));
}


void common_hal__eve_BitmapLayoutH(common_hal__eve_t *eve, uint32_t linestride, uint32_t height) {
    C4(eve, ((40 << 24) | (((linestride) & 3) << 2) | (((height) & 3))));
}


void common_hal__eve_BitmapLayout(common_hal__eve_t *eve, uint32_t format, uint32_t linestride, uint32_t height) {
    C4(eve, ((7 << 24) | ((format & 31) << 19) | ((linestride & 1023) << 9) | ((height & 511))));
}


void common_hal__eve_BitmapSizeH(common_hal__eve_t *eve, uint32_t width, uint32_t height) {
    C4(eve, ((41 << 24) | (((width) & 3) << 2) | (((height) & 3))));
}


void common_hal__eve_BitmapSize(common_hal__eve_t *eve, uint32_t filter, uint32_t wrapx, uint32_t wrapy, uint32_t width, uint32_t height) {
    C4(eve, ((8 << 24) | ((filter & 1) << 20) | ((wrapx & 1) << 19) | ((wrapy & 1) << 18) | ((width & 511) << 9) | ((height & 511))));
}


void common_hal__eve_BitmapSource(common_hal__eve_t *eve, uint32_t addr) {
    C4(eve, ((1 << 24) | ((addr & 0xffffff))));
}


void common_hal__eve_BitmapSwizzle(common_hal__eve_t *eve, uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    C4(eve, ((47 << 24) | ((r & 7) << 9) | ((g & 7) << 6) | ((b & 7) << 3) | ((a & 7))));
}


void common_hal__eve_BitmapTransformA(common_hal__eve_t *eve, uint32_t p, uint32_t v) {
    C4(eve, ((21 << 24) | ((p & 1) << 17) | ((v & 131071))));
}


void common_hal__eve_BitmapTransformB(common_hal__eve_t *eve, uint32_t p, uint32_t v) {
    C4(eve, ((22 << 24) | ((p & 1) << 17) | ((v & 131071))));
}


void common_hal__eve_BitmapTransformC(common_hal__eve_t *eve, uint32_t v) {
    C4(eve, ((23 << 24) | ((v & 16777215))));
}


void common_hal__eve_BitmapTransformD(common_hal__eve_t *eve, uint32_t p, uint32_t v) {
    C4(eve, ((24 << 24) | ((p & 1) << 17) | ((v & 131071))));
}


void common_hal__eve_BitmapTransformE(common_hal__eve_t *eve, uint32_t p, uint32_t v) {
    C4(eve, ((25 << 24) | ((p & 1) << 17) | ((v & 131071))));
}


void common_hal__eve_BitmapTransformF(common_hal__eve_t *eve, uint32_t v) {
    C4(eve, ((26 << 24) | ((v & 16777215))));
}


void common_hal__eve_BlendFunc(common_hal__eve_t *eve, uint32_t src, uint32_t dst) {
    C4(eve, ((11 << 24) | ((src & 7) << 3) | ((dst & 7))));
}


void common_hal__eve_Call(common_hal__eve_t *eve, uint32_t dest) {
    C4(eve, ((29 << 24) | ((dest & 65535))));
}


void common_hal__eve_Cell(common_hal__eve_t *eve, uint32_t cell) {
    C4(eve, ((6 << 24) | ((cell & 127))));
}


void common_hal__eve_ClearColorA(common_hal__eve_t *eve, uint32_t alpha) {
    C4(eve, ((15 << 24) | ((alpha & 255))));
}


void common_hal__eve_ClearColorRGB(common_hal__eve_t *eve, uint32_t red, uint32_t green, uint32_t blue) {
    C4(eve, ((2 << 24) | ((red & 255) << 16) | ((green & 255) << 8) | ((blue & 255))));
}


void common_hal__eve_Clear(common_hal__eve_t *eve, uint32_t c, uint32_t s, uint32_t t) {
    C4(eve, ((38 << 24) | ((c & 1) << 2) | ((s & 1) << 1) | ((t & 1))));
}


void common_hal__eve_ClearStencil(common_hal__eve_t *eve, uint32_t s) {
    C4(eve, ((17 << 24) | ((s & 255))));
}


void common_hal__eve_ClearTag(common_hal__eve_t *eve, uint32_t s) {
    C4(eve, ((18 << 24) | ((s & 255))));
}


void common_hal__eve_ColorA(common_hal__eve_t *eve, uint32_t alpha) {
    C4(eve, ((16 << 24) | ((alpha & 255))));
}


void common_hal__eve_ColorMask(common_hal__eve_t *eve, uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    C4(eve, ((32 << 24) | ((r & 1) << 3) | ((g & 1) << 2) | ((b & 1) << 1) | ((a & 1))));
}


void common_hal__eve_ColorRGB(common_hal__eve_t *eve, uint32_t red, uint32_t green, uint32_t blue) {
    C4(eve, ((4 << 24) | ((red & 255) << 16) | ((green & 255) << 8) | ((blue & 255))));
}


void common_hal__eve_Display(common_hal__eve_t *eve) {
    C4(eve, ((0 << 24)));
}


void common_hal__eve_End(common_hal__eve_t *eve) {
    C4(eve, ((33 << 24)));
}


void common_hal__eve_Jump(common_hal__eve_t *eve, uint32_t dest) {
    C4(eve, ((30 << 24) | ((dest & 65535))));
}


void common_hal__eve_LineWidth(common_hal__eve_t *eve, mp_float_t width) {
    int16_t iw = (int)(8 * width);
    C4(eve, ((14 << 24) | ((iw & 4095))));
}


void common_hal__eve_Macro(common_hal__eve_t *eve, uint32_t m) {
    C4(eve, ((37 << 24) | ((m & 1))));
}


void common_hal__eve_Nop(common_hal__eve_t *eve) {
    C4(eve, ((45 << 24)));
}


void common_hal__eve_PaletteSource(common_hal__eve_t *eve, uint32_t addr) {
    C4(eve, ((42 << 24) | (((addr) & 4194303))));
}


void common_hal__eve_PointSize(common_hal__eve_t *eve, mp_float_t size) {
    int16_t is = (int)(8 * size);
    C4(eve, ((13 << 24) | ((is & 8191))));
}


void common_hal__eve_RestoreContext(common_hal__eve_t *eve) {
    C4(eve, ((35 << 24)));
}


void common_hal__eve_Return(common_hal__eve_t *eve) {
    C4(eve, ((36 << 24)));
}


void common_hal__eve_SaveContext(common_hal__eve_t *eve) {
    C4(eve, ((34 << 24)));
}


void common_hal__eve_ScissorSize(common_hal__eve_t *eve, uint32_t width, uint32_t height) {
    C4(eve, ((28 << 24) | ((width & 4095) << 12) | ((height & 4095))));
}


void common_hal__eve_ScissorXY(common_hal__eve_t *eve, uint32_t x, uint32_t y) {
    C4(eve, ((27 << 24) | ((x & 2047) << 11) | ((y & 2047))));
}


void common_hal__eve_StencilFunc(common_hal__eve_t *eve, uint32_t func, uint32_t ref, uint32_t mask) {
    C4(eve, ((10 << 24) | ((func & 7) << 16) | ((ref & 255) << 8) | ((mask & 255))));
}


void common_hal__eve_StencilMask(common_hal__eve_t *eve, uint32_t mask) {
    C4(eve, ((19 << 24) | ((mask & 255))));
}


void common_hal__eve_StencilOp(common_hal__eve_t *eve, uint32_t sfail, uint32_t spass) {
    C4(eve, ((12 << 24) | ((sfail & 7) << 3) | ((spass & 7))));
}


void common_hal__eve_TagMask(common_hal__eve_t *eve, uint32_t mask) {
    C4(eve, ((20 << 24) | ((mask & 1))));
}


void common_hal__eve_Tag(common_hal__eve_t *eve, uint32_t s) {
    C4(eve, ((3 << 24) | ((s & 255))));
}


void common_hal__eve_VertexTranslateX(common_hal__eve_t *eve, mp_float_t x) {
    int16_t ix = (int)(16 * x);
    C4(eve, ((43 << 24) | (ix & 131071)));
}


void common_hal__eve_VertexTranslateY(common_hal__eve_t *eve, mp_float_t y) {
    int16_t iy = (int)(16 * y);
    C4(eve, ((44 << 24) | (iy & 131071)));
}


void common_hal__eve_Vertex2ii(common_hal__eve_t *eve, uint32_t x, uint32_t y, uint32_t handle, uint32_t cell) {
    C4(eve, ((2 << 30) | (((x) & 511) << 21) | (((y) & 511) << 12) | (((handle) & 31) << 7) | (((cell) & 127) << 0)));
}
