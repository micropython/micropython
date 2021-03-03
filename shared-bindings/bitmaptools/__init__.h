/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Kevin Matocha
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BITMAPTOOLS__INIT__H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BITMAPTOOLS__INIT__H

#include "py/obj.h"

void common_hal_bitmaptools_rotozoom(displayio_bitmap_t *self, int16_t ox, int16_t oy,
                                            int16_t dest_clip0_x, int16_t dest_clip0_y,
                                            int16_t dest_clip1_x, int16_t dest_clip1_y,
                                            displayio_bitmap_t *source, int16_t px, int16_t py,
                                            int16_t source_clip0_x, int16_t source_clip0_y,
                                            int16_t source_clip1_x, int16_t source_clip1_y,
                                            float angle,
                                            float scale,
                                            uint32_t skip_index, bool skip_index_none);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BITMAPTOOLS__INIT__H
