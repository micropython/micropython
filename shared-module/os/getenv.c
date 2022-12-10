/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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

#include <stdlib.h>
#include <string.h>

#include "shared-bindings/os/__init__.h"
#include "shared-module/os/__init__.h"

#include "py/gc.h"
#include "py/misc.h"
#include "py/mpstate.h"
#include "py/objstr.h"
#include "py/parsenum.h"
#include "py/runtime.h"
#include "supervisor/filesystem.h"
#include "supervisor/memory.h"

#define GETENV_PATH "/settings.toml"

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
typedef FIL file_arg;
STATIC bool open_file(const char *name, file_arg *active_file) {
    #if defined(UNIX)
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t file_obj = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), mp_obj_new_str(name, strlen(name)), MP_ROM_QSTR(MP_QSTR_rb));
        mp_arg_validate_type(file_obj, &mp_type_vfs_fat_fileio, MP_QSTR_file);
        pyb_file_obj_t *file = MP_OBJ_TO_PTR(file_obj);
        *active_file = file->fp;
        nlr_pop();
        return true;
    } else {
        return false;
    }
    #else
    FATFS *fs = filesystem_circuitpy();
    FRESULT result = f_open(fs, active_file, name, FA_READ);
    return result == FR_OK;
    #endif
}
STATIC void close_file(file_arg *active_file) {
    // nothing
}
STATIC bool is_eof(file_arg *active_file) {
    return f_eof(active_file);
}

// Return 0 if there is no next character (EOF).
STATIC uint8_t get_next_byte(FIL *active_file) {
    uint8_t character = 0;
    UINT quantity_read;
    // If there's an error or quantity_read is 0, character will remain 0.
    f_read(active_file, &character, 1, &quantity_read);
    return character;
}
STATIC void seek_eof(file_arg *active_file) {
    f_lseek(active_file, f_size(active_file));
}

// For a fixed buffer, record the required size rather than throwing
STATIC void vstr_add_byte_nonstd(vstr_t *vstr, byte b) {
    if (!vstr->fixed_buf || vstr->alloc > vstr->len) {
        vstr_add_byte(vstr, b);
    }
}

// For a fixed buffer, record the required size rather than throwing
STATIC void vstr_add_char_nonstd(vstr_t *vstr, unichar c) {
    size_t ulen =
        (c < 0x80) ? 1 :
        (c < 0x800) ? 2 :
        (c < 0x10000) ? 3 : 4;
    if (!vstr->fixed_buf || vstr->alloc > vstr->len + ulen) {
        vstr_add_char(vstr, c);
    }
}

STATIC void next_line(file_arg *active_file) {
    uint8_t character;
    do {
        character = get_next_byte(active_file);
    } while (character != 0 && character != '\n');
}

// Discard whitespace, except for newlines, returning the next character after the whitespace.
// Return 0 if there is no next character (EOF).
STATIC uint8_t consume_whitespace(file_arg *active_file) {
    uint8_t character;
    do {
        character = get_next_byte(active_file);
    } while (character != '\n' && character != 0 && unichar_isspace(character));
    return character;
}

// Starting at the start of a new line, determines if the key matches the given
// key.
//
// If result is true, the key matches and file pointer is pointing just after the "=".
// If the result is false, the key does NOT match and the file pointer is
// pointing at the start of the next line, if any
STATIC bool key_matches(file_arg *active_file, const char *key) {
    uint8_t character;
    character = consume_whitespace(active_file);
    if (character == '[' || character == 0) {
        seek_eof(active_file);
        return false;
    }
    while (*key) {
        if (character != *key++) {
            // A character didn't match the key, so it's not a match
            // If the non-matching char was not the end of the line,
            // then consume the rest of the line
            if (character != '\n') {
                next_line(active_file);
            }
            return false;
        }
        character = get_next_byte(active_file);
    }
    // the next character could be whitespace; consume if necessary
    if (unichar_isspace(character)) {
        character = consume_whitespace(active_file);
    }
    // If we're not looking at the "=" then the key didn't match
    if (character != '=') {
        // A character didn't match the key, so it's not a match
        // If the non-matching char was not the end of the line,
        // then consume the rest of the line
        if (character != '\n') {
            next_line(active_file);
        }
        return false;
    }
    return true;
}

STATIC os_getenv_err_t read_unicode_escape(file_arg *active_file, int sz, vstr_t *buf) {
    char hex_buf[sz + 1];
    for (int i = 0; i < sz; i++) {
        hex_buf[i] = get_next_byte(active_file);
    }
    hex_buf[sz] = 0;
    char *end;
    unsigned long c = strtoul(hex_buf, &end, 16);
    if (end != &hex_buf[sz]) {
        return GETENV_ERR_UNEXPECTED | *end;
    }
    if (c >= 0x110000) {
        return GETENV_ERR_UNICODE;
    }
    vstr_add_char_nonstd(buf, c);
    return GETENV_OK;
}

// Read a quoted string
STATIC os_getenv_err_t read_string_value(file_arg *active_file, vstr_t *buf) {
    while (true) {
        int character = get_next_byte(active_file);
        switch (character) {
            case 0:
            case '\n':
                return GETENV_ERR_UNEXPECTED | character;

            case '"':
                character = consume_whitespace(active_file);
                switch (character) {
                    case '#':
                        next_line(active_file);
                        MP_FALLTHROUGH;
                    case '\n':
                        return GETENV_OK;
                    default:
                        return GETENV_ERR_UNEXPECTED | character;
                }

            case '\\':
                character = get_next_byte(active_file);
                switch (character) {
                    case 0:
                    case '\n':
                        return GETENV_ERR_UNEXPECTED | character;
                    case 'b':
                        character = '\b';
                        break;
                    case 'r':
                        character = '\r';
                        break;
                    case 'n':
                        character = '\n';
                        break;
                    case 't':
                        character = '\t';
                        break;
                    case 'v':
                        character = '\v';
                        break;
                    case 'f':
                        character = '\f';
                        break;
                    case 'U':
                    case 'u': {
                        int sz = (character == 'u') ? 4 : 8;
                        os_getenv_err_t res;
                        res = read_unicode_escape(active_file, sz, buf);
                        if (res != GETENV_OK) {
                            return res;
                        }
                        continue;
                    }
                        // default falls through, other escaped characters
                        // represent themselves
                }
                MP_FALLTHROUGH;
            default:
                vstr_add_byte_nonstd(buf, character);
        }
    }
}

// Read a numeric value (non-quoted value) as a string
STATIC os_getenv_err_t read_bare_value(file_arg *active_file, vstr_t *buf, int first_character) {
    int character = first_character;
    while (true) {
        switch (character) {
            case 0:
                return GETENV_ERR_UNEXPECTED | character;
            case '\n':
                return GETENV_OK;
            case '#':
                next_line(active_file);
                return GETENV_OK;
            default:
                vstr_add_byte_nonstd(buf, character);
        }
        character = get_next_byte(active_file);
    }
}

STATIC mp_int_t read_value(file_arg *active_file, vstr_t *buf, bool *quoted) {
    uint8_t character;
    character = consume_whitespace(active_file);
    *quoted = (character == '"');

    if (*quoted) {
        return read_string_value(active_file, buf);
    } else {
        return read_bare_value(active_file, buf, character);
    }
}

STATIC os_getenv_err_t os_getenv_vstr(const char *path, const char *key, vstr_t *buf, bool *quoted) {
    file_arg active_file;
    if (!open_file(path, &active_file)) {
        return GETENV_ERR_OPEN;
    }

    os_getenv_err_t result = GETENV_ERR_NOT_FOUND;
    while (!is_eof(&active_file)) {
        if (key_matches(&active_file, key)) {
            result = read_value(&active_file, buf, quoted);
        }
    }
    close_file(&active_file);
    return result;
}

STATIC os_getenv_err_t os_getenv_buf_terminated(const char *key, char *value, size_t value_len, bool *quoted) {
    vstr_t buf;
    vstr_init_fixed_buf(&buf, value_len, value);
    os_getenv_err_t result = os_getenv_vstr(GETENV_PATH, key, &buf, quoted);

    if (result == GETENV_OK) {
        vstr_add_byte_nonstd(&buf, 0);
        memcpy(value, buf.buf, MIN(buf.len, value_len));
        if (buf.len > value_len) {
            result = GETENV_ERR_LENGTH;
        }
    }
    return result;
}

os_getenv_err_t common_hal_os_getenv_str(const char *key, char *value, size_t value_len) {
    bool quoted;
    os_getenv_err_t result = os_getenv_buf_terminated(key, value, value_len, &quoted);
    if (result == GETENV_OK && !quoted) {
        result = GETENV_ERR_UNEXPECTED | value[0];
    }
    return result;
}

STATIC void throw_getenv_error(os_getenv_err_t error) {
    if (error == GETENV_OK) {
        return;
    }
    if (error & GETENV_ERR_UNEXPECTED) {
        byte character = (error & 0xff);
        mp_print_t print;
        vstr_t vstr;
        vstr_init_print(&vstr, 8 + 4 + 1, &print);
        if (character) {
            mp_str_print_quoted(&print, &character, 1, true);
        } else {
            mp_str_print_quoted(&print, (byte *)"EOF", 3, true);
        }
        mp_raise_ValueError_varg(translate("Invalid byte %.*s"),
            vstr.len, vstr.buf);
    }
    switch (error) {
        case GETENV_ERR_OPEN:
            mp_raise_ValueError(translate("File not found"));
        case GETENV_ERR_UNICODE:
            mp_raise_ValueError(translate("Invalid unicode escape"));
        case GETENV_ERR_NOT_FOUND:
            mp_raise_ValueError(translate("Key not found"));
        default:
            mp_raise_RuntimeError(translate("Internal error"));
    }
}

mp_obj_t common_hal_os_getenv_path(const char *path, const char *key, mp_obj_t default_) {
    vstr_t buf;
    bool quoted;

    vstr_init(&buf, 64);
    os_getenv_err_t result = os_getenv_vstr(path, key, &buf, &quoted);
    if (result == GETENV_ERR_NOT_FOUND || result == GETENV_ERR_OPEN) {
        return default_;
    }
    throw_getenv_error(result);

    if (quoted) {
        return mp_obj_new_str_from_vstr(&mp_type_str, &buf);
    } else {
        return mp_parse_num_integer(buf.buf, buf.len, 0, NULL);
    }
}

mp_obj_t common_hal_os_getenv(const char *key, mp_obj_t default_) {
    return common_hal_os_getenv_path(GETENV_PATH, key, default_);
}

os_getenv_err_t common_hal_os_getenv_int(const char *key, mp_int_t *value) {
    char buf[16];
    bool quoted;
    os_getenv_err_t result = os_getenv_buf_terminated(key, buf, sizeof(buf), &quoted);
    if (result != GETENV_OK) {
        return result;
    }
    if (quoted) {
        return GETENV_ERR_UNEXPECTED | '"';
    }
    char *end;
    long num = strtol(buf, &end, 0);
    if (end == buf || *end) { // If the whole buffer was not consumed it's an error
        return GETENV_ERR_UNEXPECTED | *end;
    }
    *value = (mp_int_t)num;
    return GETENV_OK;
}
