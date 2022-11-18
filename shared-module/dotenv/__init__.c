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

#include "shared-bindings/dotenv/__init__.h"

#include "py/misc.h"
#include "py/mpstate.h"
#include "py/objstr.h"
#include "supervisor/filesystem.h"

#if defined(UNIX)
typedef FILE *file_arg;
STATIC bool open_file(const char *name, file_arg *active_file) {
    FILE *result = fopen(name, "r");
    if (result) {
        *active_file = result;
    }
    return result != NULL;
}
STATIC void close_file(file_arg *active_file) {
    fclose(*active_file);
}
STATIC uint8_t get_next_character(file_arg *active_file) {
    int value = fgetc(*active_file);
    if (value == EOF) {
        return 0;
    }
    return value;
}
STATIC void seek_minus_one(file_arg *active_file) {
    fseek(*active_file, -1, SEEK_CUR);
}
#else
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
typedef FIL file_arg;
STATIC bool open_file(const char *name, file_arg *active_file) {
    FATFS *fs = filesystem_circuitpy();
    FRESULT result = f_open(fs, active_file, name, FA_READ);
    return result == FR_OK;
}
STATIC void close_file(file_arg *active_file) {
    // nothing
}

// Return 0 if there is no next character (EOF).
STATIC uint8_t get_next_character(FIL *active_file) {
    uint8_t character = 0;
    UINT quantity_read;
    // If there's an error or quantity_read is 0, character will remain 0.
    f_read(active_file, &character, 1, &quantity_read);
    return character;
}
STATIC void seek_minus_one(file_arg *active_file) {
    f_lseek(active_file, f_tell(active_file) - 1);
}
#endif

// Discard whitespace, except for newlines, returning the next character after the whitespace.
// Return 0 if there is no next character (EOF).
STATIC uint8_t consume_whitespace(file_arg *active_file) {
    uint8_t character;
    do {
        character = get_next_character(active_file);
    } while (character != '\n' && character != 0 && unichar_isspace(character));
    return character;
}

// Starting at the start of a new line, determines if the key matches the given
// key. File pointer is set to be just before the = after the key.
STATIC bool key_matches(file_arg *active_file, const char *key) {
    uint8_t character;
    character = consume_whitespace(active_file);
    if (character == 0) {
        return false;
    }
    bool quoted = false;
    if (character == '\'') {
        // Beginning of single-quoted string.
        quoted = true;
        character = get_next_character(active_file);
    }
    size_t key_pos = 0;
    bool escaped = false;
    bool matches = true;
    size_t key_len = strlen(key);
    while (character != 0) {
        if (character == '\\' && !escaped && quoted) {
            escaped = true;
        } else if (!escaped && quoted && character == '\'') {
            quoted = false;
            // End of quoted key. Skip over the ending quote.
            character = get_next_character(active_file);
            break;
        } else if (!quoted && (unichar_isspace(character) || character == '=' || character == '\n' || character == '#' || character == 0)) {
            // End of unquoted key.
            break;
        } else {
            // Still on tentative key; see if it matches the next supplied key character,
            // but don't run off the end of the supplied key.
            if (key_pos < key_len) {
                matches = matches && (unsigned char)key[key_pos] == character;
                escaped = false;
                key_pos++;
            } else {
                // Key on line is too long.
                matches = false;
            }
        }
        character = get_next_character(active_file);
    }
    if (character == '=' || character == '\n' || character == '#' || character == 0) {
        // Rewind one so the value, if any, can be found.
        seek_minus_one(active_file);
    } else if (!unichar_isspace(character)) {
        // We're followed by something else that is invalid syntax.
        matches = false;
    }

    return matches && key_pos == key_len;
}

STATIC bool next_line(file_arg *active_file) {
    uint8_t character;
    bool quoted = false;
    bool escaped = false;
    // Track comments because they last until the end of the line.
    bool comment = false;
    // Consume all characters while quoted or others up to \n.
    do {
        character = get_next_character(active_file);

        if ((!quoted || character == '#') || comment) {
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
    } while (character != 0 && (quoted || character != '\n'));

    return character != 0;
}

STATIC mp_int_t read_value(file_arg *active_file, char *value, size_t value_len) {
    uint8_t character;
    // Consume spaces before "=", and get first character of interest.
    character = consume_whitespace(active_file);
    if (character != '=') {
        if (character == '#' || character == '\n') {
            // Keys without an = after them are valid with the value None.
            return -1;
        }
        // All other characters are invalid.
        return -1;
    }
    // Consume space after =
    if (character != '#') {
        // a # immediately after = is part of the value!
        character = consume_whitespace(active_file);
    }
    bool quoted = false;
    if (character == '\'') {
        quoted = true;
        character = get_next_character(active_file);
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
    bool first_char = true;
    while (character != 0) {
        // Consume the first \ if the value is quoted.
        if (quoted && character == '\\' && !escaped) {
            escaped = true;
            // Drop this backslash by short circuiting the rest of the loop.
            character = get_next_character(active_file);
            continue;
        }
        if (quoted && !escaped && character == '\'') {
            // trailing ' means the value is done.
            break;
        }
        // Unquoted values are ended by a newline or comment.
        if (!quoted && (character == '\n' || (character == '#' && !first_char))) {
            if (character == '\n') {
                // Rewind one so the next_line can find the \n.
                seek_minus_one(active_file);
            }
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
        character = get_next_character(active_file);
        first_char = false;
    }

    return value_pos - trailing_spaces;
}

mp_int_t dotenv_get_key(const char *path, const char *key, char *value, mp_int_t value_len) {
    file_arg active_file;
    if (!open_file(path, &active_file)) {
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
    close_file(&active_file);
    return actual_value_len;
}

mp_obj_t common_hal_dotenv_get_key(const char *path, const char *key) {
    // Use the stack for short values. Longer values will require a heap allocation after we know
    // the length.
    char value[64];
    mp_int_t actual_len = dotenv_get_key(path, key, value, sizeof(value));
    if (actual_len < 0) {
        return mp_const_none;
    }
    if ((size_t)actual_len >= sizeof(value)) {
        byte *buf = m_new(byte, actual_len + 1);
        dotenv_get_key(path, key, (char *)buf, actual_len);
        buf[actual_len] = 0;

        mp_obj_str_t *o = m_new_obj(mp_obj_str_t);
        o->base.type = &mp_type_str;
        o->len = actual_len;
        o->data = buf;
        o->hash = qstr_compute_hash(buf, actual_len);

        return MP_OBJ_FROM_PTR(o);
    }
    return mp_obj_new_str(value, actual_len);
}

bool dotenv_get_key_terminated(const char *path, const char *key, char *value, mp_int_t value_len) {
    mp_int_t actual_len = dotenv_get_key(path, key, value, value_len - 1);
    if (actual_len >= value_len) {
        return false;
    }
    value[actual_len] = '\0'; // terminate string
    return true;
}

bool dotenv_get_key_int(const char *path, const char *key, mp_int_t *value) {
    char buf[16];
    if (!dotenv_get_key_terminated(path, key, buf, (mp_int_t)sizeof(buf))) {
        return false;
    }
    char *end;
    long result = strtol(buf, &end, 0);
    if (end == buf || *end) { // If the whole buffer was not consumed it's an error
        return false;
    }
    *value = (mp_int_t)result;
    return true;
}
