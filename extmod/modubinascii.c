/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Paul Sokolovsky
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
#include "extmod/modubinascii.h"

mp_obj_t mod_binascii_hexlify(size_t n_args, const mp_obj_t *args) {
    // Second argument is for an extension to allow a separator to be used
    // between values.
    const char *sep = NULL;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);

    // Code below assumes non-zero buffer length when computing size with
    // separator, so handle the zero-length case here.
    if (bufinfo.len == 0) {
        return mp_const_empty_bytes;
    }

    vstr_t vstr;
    size_t out_len = bufinfo.len * 2;
    if (n_args > 1) {
        // 1-char separator between hex numbers
        out_len += bufinfo.len - 1;
        sep = mp_obj_str_get_str(args[1]);
    }
    vstr_init_len(&vstr, out_len);
    byte *in = bufinfo.buf, *out = (byte*)vstr.buf;
    for (mp_uint_t i = bufinfo.len; i--;) {
        byte d = (*in >> 4);
        if (d > 9) {
            d += 'a' - '9' - 1;
        }
        *out++ = d + '0';
        d = (*in++ & 0xf);
        if (d > 9) {
            d += 'a' - '9' - 1;
        }
        *out++ = d + '0';
        if (sep != NULL && i != 0) {
            *out++ = *sep;
        }
    }
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_binascii_hexlify_obj, 1, 2, mod_binascii_hexlify);

mp_obj_t mod_binascii_unhexlify(mp_obj_t data) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

    if ((bufinfo.len & 1) != 0) {
        mp_raise_ValueError("odd-length string");
    }
    vstr_t vstr;
    vstr_init_len(&vstr, bufinfo.len / 2);
    byte *in = bufinfo.buf, *out = (byte*)vstr.buf;
    byte hex_byte = 0;
    for (mp_uint_t i = bufinfo.len; i--;) {
        byte hex_ch = *in++;
        if (unichar_isxdigit(hex_ch)) {
            hex_byte += unichar_xdigit_value(hex_ch);
        } else {
            mp_raise_ValueError("non-hex digit found");
        }
        if (i & 1) {
            hex_byte <<= 4;
        } else {
            *out++ = hex_byte;
            hex_byte = 0;
        }
    }
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_binascii_unhexlify_obj, mod_binascii_unhexlify);

// If ch is a character in the base64 alphabet, and is not a pad character, then
// the corresponding integer between 0 and 63, inclusively, is returned.
// Otherwise, -1 is returned.
static int mod_binascii_sextet(byte ch) {
    if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A';
    } else if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 26;
    } else if (ch >= '0' && ch <= '9') {
        return ch - '0' + 52;
    } else if (ch == '+') {
        return 62;
    } else if (ch == '/') {
        return 63;
    } else {
        return -1;
    }
}

mp_obj_t mod_binascii_a2b_base64(mp_obj_t data) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    byte *in = bufinfo.buf;

    vstr_t vstr;
    vstr_init(&vstr, (bufinfo.len / 4) * 3 + 1); // Potentially over-allocate
    byte *out = (byte *)vstr.buf;

    uint shift = 0;
    int nbits = 0; // Number of meaningful bits in shift
    bool hadpad = false; // Had a pad character since last valid character
    for (size_t i = 0; i < bufinfo.len; i++) {
        if (in[i] == '=') {
            if ((nbits == 2) || ((nbits == 4) && hadpad)) {
                nbits = 0;
                break;
            }
            hadpad = true;
        }

        int sextet = mod_binascii_sextet(in[i]);
        if (sextet == -1) {
            continue;
        }
        hadpad = false;
        shift = (shift << 6) | sextet;
        nbits += 6;

        if (nbits >= 8) {
            nbits -= 8;
            out[vstr.len++] = (shift >> nbits) & 0xFF;
        }
    }

    if (nbits) {
        mp_raise_ValueError("incorrect padding");
    }

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_binascii_a2b_base64_obj, mod_binascii_a2b_base64);

mp_obj_t mod_binascii_b2a_base64(mp_obj_t data) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

    vstr_t vstr;
    vstr_init_len(&vstr, ((bufinfo.len != 0) ? (((bufinfo.len - 1) / 3) + 1) * 4 : 0) + 1);

    // First pass, we convert input buffer to numeric base 64 values
    byte *in = bufinfo.buf, *out = (byte*)vstr.buf;
    mp_uint_t i;
    for (i = bufinfo.len; i >= 3; i -= 3) {
        *out++ = (in[0] & 0xFC) >> 2;
        *out++ = (in[0] & 0x03) << 4 | (in[1] & 0xF0) >> 4;
        *out++ = (in[1] & 0x0F) << 2 | (in[2] & 0xC0) >> 6;
        *out++ = in[2] & 0x3F;
        in += 3;
    }
    if (i != 0) {
        *out++ = (in[0] & 0xFC) >> 2;
        if (i == 2) {
            *out++ = (in[0] & 0x03) << 4 | (in[1] & 0xF0) >> 4;
            *out++ = (in[1] & 0x0F) << 2;
        }
        else {
            *out++ = (in[0] & 0x03) << 4;
            *out++ = 64;
        }
        *out = 64;
    }

    // Second pass, we convert number base 64 values to actual base64 ascii encoding
    out = (byte*)vstr.buf;
    for (mp_uint_t j = vstr.len - 1; j--;) {
        if (*out < 26) {
            *out += 'A';
        } else if (*out < 52) {
            *out += 'a' - 26;
        } else if (*out < 62) {
            *out += '0' - 52;
        } else if (*out == 62) {
            *out ='+';
        } else if (*out == 63) {
            *out = '/';
        } else {
            *out = '=';
        }
        out++;
    }
    *out = '\n';
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_binascii_b2a_base64_obj, mod_binascii_b2a_base64);

#if MICROPY_PY_UBINASCII_CRC32
#include "uzlib/tinf.h"

mp_obj_t mod_binascii_crc32(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);
    uint32_t crc = (n_args > 1) ? mp_obj_get_int_truncated(args[1]) : 0;
    crc = uzlib_crc32(bufinfo.buf, bufinfo.len, crc ^ 0xffffffff);
    return mp_obj_new_int_from_uint(crc ^ 0xffffffff);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_binascii_crc32_obj, 1, 2, mod_binascii_crc32);
#endif

#if MICROPY_PY_UBINASCII

STATIC const mp_rom_map_elem_t mp_module_binascii_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ubinascii) },
    { MP_ROM_QSTR(MP_QSTR_hexlify), MP_ROM_PTR(&mod_binascii_hexlify_obj) },
    { MP_ROM_QSTR(MP_QSTR_unhexlify), MP_ROM_PTR(&mod_binascii_unhexlify_obj) },
    { MP_ROM_QSTR(MP_QSTR_a2b_base64), MP_ROM_PTR(&mod_binascii_a2b_base64_obj) },
    { MP_ROM_QSTR(MP_QSTR_b2a_base64), MP_ROM_PTR(&mod_binascii_b2a_base64_obj) },
    #if MICROPY_PY_UBINASCII_CRC32
    { MP_ROM_QSTR(MP_QSTR_crc32), MP_ROM_PTR(&mod_binascii_crc32_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_binascii_globals, mp_module_binascii_globals_table);

const mp_obj_module_t mp_module_ubinascii = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_binascii_globals,
};

#endif //MICROPY_PY_UBINASCII
