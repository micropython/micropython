/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019  Bernhard Boser
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
#include <inttypes.h>

#include "py/binary.h"
#include "py/objarray.h"
#include "py/objlist.h"
#include "py/objstringio.h"
#include "py/parsenum.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "supervisor/shared/translate.h"

////////////////////////////////////////////////////////////////
// stream management

typedef struct _msgpack_stream_t {
    mp_obj_t stream_obj;
    mp_uint_t (*read)(mp_obj_t obj, void *buf, mp_uint_t size, int *errcode);
    mp_uint_t (*write)(mp_obj_t obj, const void *buf, mp_uint_t size, int *errcode);
    int errcode;
} msgpack_stream_t;

STATIC msgpack_stream_t get_stream(mp_obj_t stream_obj, int flags) {
    const mp_stream_p_t *stream_p = mp_get_stream_raise(stream_obj, flags);
    msgpack_stream_t s = {stream_obj, stream_p->read, stream_p->write, 0};
    return s;
}

////////////////////////////////////////////////////////////////
// readers

STATIC void read(msgpack_stream_t *s, void *buf, mp_uint_t size) {
    if (size == 0) return;
    mp_uint_t ret = s->read(s->stream_obj, buf, size, &s->errcode);
    if (s->errcode != 0) {
        mp_raise_OSError(s->errcode);
    }
    if (ret == 0) {
        mp_raise_msg(&mp_type_EOFError, NULL);
    }
}

/*
STATIC uint32_t read_bytes(msgpack_stream_t *s, mp_uint_t n_bytes) {
    uint32_t res = 0;
    read(s, &res, n_bytes);
    return res;
}
*/
STATIC uint8_t read1(msgpack_stream_t *s) {
    uint8_t res = 0;
    read(s, &res, 1);
    return res;
}

STATIC uint16_t read2(msgpack_stream_t *s) {
    uint16_t res = 0;
    read(s, &res, 2);
    int n = 1;
    if (*(char *)&n == 1) res = __builtin_bswap16(res);
    return res;
}

STATIC uint32_t read4(msgpack_stream_t *s) {
    uint32_t res = 0;
    read(s, &res, 4);
    int n = 1;
    if (*(char *)&n == 1) res = __builtin_bswap32(res);
    return res;
}

/*
STATIC size_t read_size(msgpack_stream_t *s, uint8_t len_index) {
    size_t n_bytes = 4;
    switch (len_index) {
        case 0:  n_bytes = 1;  break;
        case 1:  n_bytes = 2;  break;
        case 2:  n_bytes = 4;  break;
    }
    size_t res = 0;
    read(s, &res, n_bytes);
    return res;
}
*/

STATIC size_t read_size(msgpack_stream_t *s, uint8_t len_index) {
    size_t res;
    switch (len_index) {
        case 0:  res = (size_t)read1(s);  break;
        case 1:  res = (size_t)read2(s);  break;
        case 2:  res = (size_t)read4(s);  break;
    }
    return res;
}

////////////////////////////////////////////////////////////////
// writers

STATIC void write(msgpack_stream_t *s, const void *buf, mp_uint_t size) {
    mp_uint_t ret = s->write(s->stream_obj, buf, size, &s->errcode);
    if (s->errcode != 0) {
        mp_raise_OSError(s->errcode);
    }
    if (ret == 0) {
        mp_raise_msg(&mp_type_EOFError, NULL);
    }
}

STATIC void write1(msgpack_stream_t *s, uint8_t obj) {
    write(s, &obj, 1);
}

STATIC void write2(msgpack_stream_t *s, uint16_t obj) {
    int n = 1;
    if (*(char *)&n == 1) obj = __builtin_bswap16(obj);
    write(s, &obj, 2);
}

STATIC void write4(msgpack_stream_t *s, uint32_t obj) {
    int n = 1;
    if (*(char *)&n == 1) obj = __builtin_bswap32(obj);
    write(s, &obj, 4);
}

// compute and write msgpack size code (array structures)
STATIC void write_size(msgpack_stream_t *s, uint8_t code, size_t size) {
    if ((uint8_t)size == size) {
        write1(s, code);
        write1(s, size);
    } else if ((uint16_t)size == size) {
        write1(s, code+1);
        write2(s, size);
    } else {
        write1(s, code+2);
        write4(s, size);
    }
}

////////////////////////////////////////////////////////////////
// packers

// This is a helper function to iterate through a dictionary.  The state of
// the iteration is held in *cur and should be initialised with zero for the
// first call.  Will return NULL when no more elements are available.
STATIC mp_map_elem_t *dict_iter_next(mp_obj_dict_t *dict, size_t *cur) {
    size_t max = dict->map.alloc;
    mp_map_t *map = &dict->map;

    for (size_t i = *cur; i < max; i++) {
        if (MP_MAP_SLOT_IS_FILLED(map, i)) {
            *cur = i + 1;
            return &(map->table[i]);
        }
    }

    return NULL;
}

STATIC void pack_int(msgpack_stream_t *s, int32_t x) {
    if (x > -32 && x < 128) {
        write1(s, x);
    } else if ((int8_t)x == x) {
        write1(s, 0xd0);
        write1(s, x);
    } else if ((int16_t)x == x) {
        write1(s, 0xd1);
        write2(s, x);
    } else {
        write1(s, 0xd2);
        write4(s, x);
    }
}

void pack_bin(msgpack_stream_t *s, const uint8_t* data, size_t len) {
    write_size(s, 0xc4, len);
    for (size_t i=0;  i<len;  i++) {
        write1(s, data[i]);
    }
}

void pack_str(msgpack_stream_t *s, const char* str, size_t len) {
    // size_t len = strlen(str);
    if (len < 32) {
        write1(s, 0b10100000 | (uint8_t)len);
    } else {
        write_size(s, 0xd9, len);
    }
    for (size_t l=0;  l<len;  l++) {
        write1(s, str[l]);
    }
}

void pack_array(msgpack_stream_t *s, size_t len) {
    // only writes the header, manually write the objects after calling pack_array!
    if (len < 16) {
        write1(s, 0b10010000 | (uint8_t)len);
    } else if (len < 0x10000) {
        write1(s, 0xdc);
        write2(s, len);
    } else {
        write1(s, 0xdd);
        write4(s, len);
    }
}

void pack_dict(msgpack_stream_t *s, size_t len) {
    // only writes the header, manually write the objects after calling pack_array!
    if (len < 16) {
        write1(s, 0b10000000 | (uint8_t)len);
    } else if (len < 0x10000) {
        write1(s, 0xde);
        write2(s, len);
    } else {
        write1(s, 0xdf);
        write4(s, len);
    }
}

void pack(mp_obj_t obj, msgpack_stream_t *s) {
    if (MP_OBJ_IS_SMALL_INT(obj)) {
        // int
        int32_t x = MP_OBJ_SMALL_INT_VALUE(obj);
        pack_int(s, x);
    } else if (MP_OBJ_IS_STR(obj)) {
        // string
        size_t len;
        const char *data = mp_obj_str_get_data(obj, &len);
        pack_str(s, data, len);
    } else if (MP_OBJ_IS_TYPE(obj, &mp_type_bytes)) {
        // bytes
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(obj, &bufinfo, MP_BUFFER_READ);
        pack_bin(s, bufinfo.buf, bufinfo.len);
    } else if (MP_OBJ_IS_TYPE(obj, &mp_type_tuple)) {
        // tuple
        mp_obj_tuple_t *self = MP_OBJ_TO_PTR(obj);
        pack_array(s, self->len);
        for (size_t i=0;  i<self->len;  i++) {
            pack(self->items[i], s);
        }
    } else if (MP_OBJ_IS_TYPE(obj, &mp_type_list)) {
        // list (layout differs from tuple)
        mp_obj_list_t *self = MP_OBJ_TO_PTR(obj);
        pack_array(s, self->len);
        for (size_t i=0;  i<self->len;  i++) {
            pack(self->items[i], s);
        }
    } else if (MP_OBJ_IS_TYPE(obj, &mp_type_dict)) {
        // dict
        mp_obj_dict_t *self = MP_OBJ_TO_PTR(obj);
        pack_dict(s, self->map.used);
        size_t cur = 0;
        mp_map_elem_t *next = NULL;
        while ((next = dict_iter_next(self, &cur)) != NULL) {
            pack(next->key, s);
            pack(next->value, s);
        }
    } else if (mp_obj_is_float(obj)) {
        union Float { mp_float_t f;  uint32_t u; };
        union Float data;
        data.f = mp_obj_float_get(obj);
        write1(s, 0xca);
        write4(s, data.u);
    } else if (obj == mp_const_none) {
        write1(s, 0xc0);
    } else if (obj == mp_const_false) {
        write1(s, 0xc2);
    } else if (obj == mp_const_true) {
        write1(s, 0xc3);
    } else {
        mp_raise_ValueError(translate("no packer"));
    }
}

////////////////////////////////////////////////////////////////
// unpacker

mp_obj_t unpack(msgpack_stream_t *s) {
    uint8_t code = read1(s);
    if (((code & 0b10000000) == 0) || ((code & 0b11100000) == 0b11100000)) {
        // int
        return MP_OBJ_NEW_SMALL_INT((int8_t)code);
    }
    if ((code & 0b11100000) == 0b10100000) {
        // str
        size_t len = code & 0b11111;
        // allocate on stack; len < 32
        char str[len];
        read(s, &str, len);
        return mp_obj_new_str(str, len);
    }
    if ((code & 0b11110000) == 0b10010000) {
        // array (tuple)
        size_t len = code & 0b1111;
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(len, NULL));
        for (size_t i=0; i<len; i++) {
            t->items[i] = unpack(s);
        }
        return MP_OBJ_FROM_PTR(t);
    }
    if ((code & 0b11110000) == 0b10000000) {
        // map (dict)
        size_t len = code & 0b1111;
        mp_obj_dict_t *d = MP_OBJ_TO_PTR(mp_obj_new_dict(len));
        for (size_t i=0; i<len; i++) {
            mp_obj_dict_store(d, unpack(s), unpack(s));
        }
        return MP_OBJ_FROM_PTR(d);
    }
    switch (code) {
        case 0xc0: return mp_const_none;
        case 0xc2: return mp_const_false;
        case 0xc3: return mp_const_true;
        case 0xc4:
        case 0xc5:
        case 0xc6: {
            // bin 8, 16, 32
            size_t size = read_size(s, code-0xc4);
            vstr_t vstr;
            vstr_init_len(&vstr, size);
            byte *p = (byte*)vstr.buf;
            read(s, p, size);
            return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
        }
        case 0xcc:
            return MP_OBJ_NEW_SMALL_INT(read1(s));
        case 0xcd:
            return MP_OBJ_NEW_SMALL_INT(read2(s));
        case 0xce:
            return MP_OBJ_NEW_SMALL_INT(read4(s));
        case 0xca: {
            union Float { mp_float_t f;  uint32_t u; };
            union Float data;
            data.u = read4(s);
            return mp_obj_new_float(data.f);
        }
        case 0xd0:
            return MP_OBJ_NEW_SMALL_INT((int8_t)read1(s));
        case 0xd1:
            return MP_OBJ_NEW_SMALL_INT((int16_t)read2(s));
        case 0xd2:
            return MP_OBJ_NEW_SMALL_INT((int32_t)read4(s));
        case 0xd9:
        case 0xda:
        case 0xdb: {
            // str 8, 16, 32
            size_t size = read_size(s, code-0xd9);
            vstr_t vstr;
            vstr_init_len(&vstr, size);
            byte *p = (byte*)vstr.buf;
            read(s, p, size);
            return mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
        }
        case 0xde:
        case 0xdf: {
            // map 16 & 32
            size_t len = read_size(s, code - 0xde + 1);
            mp_obj_dict_t *d = MP_OBJ_TO_PTR(mp_obj_new_dict(len));
            for (size_t i=0; i<len; i++) {
                mp_obj_dict_store(d, unpack(s), unpack(s));
            }
            return MP_OBJ_FROM_PTR(d);
        }
        case 0xdc:
        case 0xdd: {
            // array 16 & 32
            size_t len = read_size(s, code - 0xdc + 1);
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(len, NULL));
            for (size_t i=0; i<len; i++) {
                t->items[i] = unpack(s);
            }
            return MP_OBJ_FROM_PTR(t);
        }
        case 0xc1:      // never used
        case 0xc7:      // ext 8
        case 0xc8:      // ext 16
        case 0xc9:      // ext 32
        case 0xcb:      // float 64
        case 0xcf:      // uint 64
        case 0xd3:      // int 64
        case 0xd4:      // fixenxt 1
        case 0xd5:      // fixenxt 2
        case 0xd6:      // fixenxt 4
        case 0xd7:      // fixenxt 8
        case 0xd8:      // fixenxt 16
        default:
            mp_raise_ValueError(translate("no unpacker found"));
    }
}

void common_hal_msgpack_pack(mp_obj_t obj, mp_obj_t stream_obj) {
    msgpack_stream_t stream = get_stream(stream_obj, MP_STREAM_OP_WRITE);
    pack(obj, &stream);
}

mp_obj_t common_hal_msgpack_unpack(mp_obj_t stream_obj) {
    msgpack_stream_t stream = get_stream(stream_obj, MP_STREAM_OP_WRITE);
    return unpack(&stream);
}
