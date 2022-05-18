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

#include <string.h>

#include "shared-bindings/dotenv/__init__.h"

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/mpstate.h"
#include "py/objstr.h"

STATIC uint8_t consume_spaces(FIL *active_file) {
    uint8_t character = ' ';
    size_t quantity_read = 1;
    while (unichar_isspace(character) && quantity_read > 0) {
        f_read(active_file, &character, 1, &quantity_read);
    }
    return character;
}

// Starting at the start of a new line, determines if the key matches the given
// key. File pointer is left after the = after the key.
STATIC bool key_matches(FIL *active_file, const char *key) {
    uint8_t character = ' ';
    size_t quantity_read = 1;
    character = consume_spaces(active_file);
    bool quoted = false;
    if (character == '\'') {
        quoted = true;
        f_read(active_file, &character, 1, &quantity_read);
    }
    size_t key_pos = 0;
    bool escaped = false;
    bool matches = true;
    size_t key_len = strlen(key);
    while (quantity_read > 0) {
        if (character == '\\' && !escaped && quoted) {
            escaped = true;
        } else if (!escaped && quoted && character == '\'') {
            quoted = false;
            // Move past the quoted before breaking so we can check the validity of data past it.
            f_read(active_file, &character, 1, &quantity_read);
            break;
        } else if (!quoted && (unichar_isspace(character) || character == '=' || character == '\n' || character == '#')) {
            break;
        } else {
            matches = matches && key[key_pos] == character;
            escaped = false;
            key_pos++;
        }

        f_read(active_file, &character, 1, &quantity_read);
    }
    if (unichar_isspace(character)) {
        character = consume_spaces(active_file);
    }
    if (character == '=' || character == '\n' || character == '#') {
        // Rewind one so the value can find it.
        f_lseek(active_file, f_tell(active_file) - 1);
    } else {
        // We're followed by something else that is invalid syntax.
        matches = false;
    }
    return matches && key_pos == key_len;
}

STATIC bool next_line(FIL *active_file) {
    uint8_t character = ' ';
    size_t quantity_read = 1;
    bool quoted = false;
    bool escaped = false;
    // Track comments because they last until the end of the line.
    bool comment = false;
    FRESULT result = FR_OK;
    // Consume all characters while quoted or others up to \n.
    while (result == FR_OK && quantity_read > 0 && (quoted || character != '\n')) {
        if (character == '#' || comment) {
            // Comments consume any escaping.
            comment = true;
        } else if (!escaped) {
            if (character == '\'') {
                quoted = !quoted;
            } else if (character == '\\') {
                escaped = true;
            }
        } else {
            escaped = false;
        }
        result = f_read(active_file, &character, 1, &quantity_read);
    }
    return result == FR_OK && quantity_read > 0;
}

STATIC mp_int_t read_value(FIL *active_file, char *value, size_t value_len) {
    uint8_t character = ' ';
    size_t quantity_read = 1;
    // Consume spaces before =
    character = consume_spaces(active_file);
    if (character != '=') {
        if (character == '#' || character == '\n') {
            // Keys without an = after them are valid with the value None.
            return 0;
        }
        // All other characters are invalid.
        return -1;
    }
    character = ' ';
    // Consume space after =
    while (unichar_isspace(character) && quantity_read > 0) {
        f_read(active_file, &character, 1, &quantity_read);
    }
    bool quoted = false;
    if (character == '\'') {
        quoted = true;
        f_read(active_file, &character, 1, &quantity_read);
    }
    if (character == '"') {
        // We don't support double quoted values.
        return -1;
    }
    // Copy the value over.
    size_t value_pos = 0;
    bool escaped = false;
    // Count trailing spaces so we can ignore them at the end of unquoted
    // values.
    size_t trailing_spaces = 0;
    while (quantity_read > 0) {
        // Consume the first \ if the value is quoted.
        if (quoted && character == '\\' && !escaped) {
            escaped = true;
            // Drop this slash by short circuiting the rest of the loop.
            f_read(active_file, &character, 1, &quantity_read);
            continue;
        }
        if (quoted && !escaped && character == '\'') {
            // trailing ' means the value is done.
            break;
        }
        // Unquoted values are ended by a newline or comment.
        if (!quoted && (character == '\n' || character == '#')) {
            break;
        }
        if (!quoted && unichar_isspace(character)) {
            trailing_spaces += 1;
        } else {
            trailing_spaces = 0;
        }
        escaped = false;
        // Only copy the value over if we have space. Otherwise, we'll just
        // count the overall length.
        if (value_pos < value_len) {
            value[value_pos] = character;
        }
        value_pos++;
        f_read(active_file, &character, 1, &quantity_read);
    }

    return value_pos - trailing_spaces;
}

mp_int_t dotenv_get_key(const char *path, const char *key, char *value, mp_int_t value_len) {
    FIL active_file;
    FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
    FRESULT result = f_open(fs, &active_file, path, FA_READ);
    if (result != FR_OK) {
        return -1;
    }

    mp_int_t actual_value_len = -1;
    bool read_ok = true;
    while (read_ok) {
        if (key_matches(&active_file, key)) {
            actual_value_len = read_value(&active_file, value, value_len);
        }

        read_ok = next_line(&active_file);
    }
    f_close(&active_file);
    return actual_value_len;
}

mp_obj_t common_hal_dotenv_get_key(const char *path, const char *key) {
    // Use the stack for short values. Longer values will require a heap allocation after we know
    // the length.
    char value[64];
    mp_int_t actual_len = dotenv_get_key(path, key, value, sizeof(value));
    if (actual_len <= 0) {
        return mp_const_none;
    }
    if ((size_t)actual_len >= sizeof(value)) {
        mp_obj_str_t *str = MP_OBJ_TO_PTR(mp_obj_new_str_copy(&mp_type_str, NULL, actual_len + 1));
        dotenv_get_key(path, key, (char *)str->data, actual_len + 1);
        str->hash = qstr_compute_hash(str->data, actual_len);
        return MP_OBJ_FROM_PTR(str);
    }
    return mp_obj_new_str(value, actual_len);
}
