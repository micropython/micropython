/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2017 Michael McWethy
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
#include <assert.h>
#include <string.h>

#include "py/runtime.h"
#include "py/binary.h"
#include "py/parsenum.h"

void struct_validate_format(char fmt) {
#if MICROPY_NONSTANDARD_TYPECODES
    if( fmt == 'S' || fmt == 'O') {
        mp_raise_RuntimeError("'S' and 'O' are not supported format types");
    }
#endif
}

char get_fmt_type(const char **fmt) {
    char t = **fmt;
    switch (t) {
        case '!':
            t = '>';
            break;
        case '@':
        case '=':
        case '<':
        case '>':
            break;
        default:
            return '@';
    }
    // Skip type char
    (*fmt)++;
    return t;
}

mp_uint_t get_fmt_num(const char **p) {
    const char *num = *p;
    uint len = 1;
    while (unichar_isdigit(*++num)) {
        len++;
    }
    mp_uint_t val = (mp_uint_t)MP_OBJ_SMALL_INT_VALUE(mp_parse_num_integer(*p, len, 10, NULL));
    *p = num;
    return val;
}


void shared_modules_struct_pack_into(mp_obj_t fmt_in, byte *p, byte* end_p, size_t n_args, const mp_obj_t *args) {
    const char *fmt = mp_obj_str_get_str(fmt_in);
    char fmt_type = get_fmt_type(&fmt);

    size_t i;
    for (i = 0; i < n_args;) {
        mp_uint_t sz = 1;
        if (*fmt == '\0') {
            // more arguments given than used by format string; CPython raises struct.error here
            mp_raise_RuntimeError("too many arguments provided with the given format");
        }
        struct_validate_format(*fmt);

        if (unichar_isdigit(*fmt)) {
            sz = get_fmt_num(&fmt);
        }
        if (p + sz > end_p) {
            mp_raise_RuntimeError("buffer too small");
        }

        if (*fmt == 's') {
            mp_buffer_info_t bufinfo;
            mp_get_buffer_raise(args[i++], &bufinfo, MP_BUFFER_READ);
            mp_uint_t to_copy = sz;
            if (bufinfo.len < to_copy) {
                to_copy = bufinfo.len;
            }
            memcpy(p, bufinfo.buf, to_copy);
            memset(p + to_copy, 0, sz - to_copy);
            p += sz;
        } else {
            while (sz--) {
                mp_binary_set_val(fmt_type, *fmt, args[i++], &p);
            }
        }
        fmt++;
    }
}

mp_uint_t calcsize_items(const char *fmt) {
    mp_uint_t cnt = 0;
    while (*fmt) {
        int num = 1;
        if (unichar_isdigit(*fmt)) {
            num = get_fmt_num(&fmt);
            if (*fmt == 's') {
                num = 1;
            }
        }
        cnt += num;
        fmt++;
    }
    return cnt;
}

mp_uint_t shared_modules_struct_calcsize(mp_obj_t fmt_in) {
    const char *fmt = mp_obj_str_get_str(fmt_in);
    char fmt_type = get_fmt_type(&fmt);

    mp_uint_t size;
    for (size = 0; *fmt; fmt++) {

        struct_validate_format(*fmt);

        mp_uint_t cnt = 1;
        if (unichar_isdigit(*fmt)) {
            cnt = get_fmt_num(&fmt);
        }

        if (*fmt == 's') {
            size += cnt;
        } else {
            mp_uint_t align;
            size_t sz = mp_binary_get_size(fmt_type, *fmt, &align);
            while (cnt--) {
                // Apply alignment
                size = (size + align - 1) & ~(align - 1);
                size += sz;
            }
        }
    }
    return size;
}


mp_obj_tuple_t * shared_modules_struct_unpack_from(mp_obj_t fmt_in, byte *p, byte *end_p) {

  const char *fmt = mp_obj_str_get_str(fmt_in);
  char fmt_type = get_fmt_type(&fmt);
  mp_uint_t num_items = calcsize_items(fmt);
  mp_obj_tuple_t *res = MP_OBJ_TO_PTR(mp_obj_new_tuple(num_items, NULL));

  for (uint i = 0; i < num_items;) {
      mp_uint_t sz = 1;

      struct_validate_format(*fmt);

      if (unichar_isdigit(*fmt)) {
          sz = get_fmt_num(&fmt);
      }
      if (p + sz > end_p) {
          mp_raise_RuntimeError("buffer too small");
      }
      mp_obj_t item;
      if (*fmt == 's') {
          item = mp_obj_new_bytes(p, sz);
          p += sz;
          res->items[i++] = item;
      } else {
          while (sz--) {
              item = mp_binary_get_val(fmt_type, *fmt, &p);
              res->items[i++] = item;
          }
      }
      fmt++;
  }
  return res;

}
