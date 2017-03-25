/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
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
#include "py/obj.h"
#include "py/runtime.h"
#include "py/repl.h"

#if MICROPY_HELPER_REPL

STATIC bool str_startswith_word(const char *str, const char *head) {
    mp_uint_t i;
    for (i = 0; str[i] && head[i]; i++) {
        if (str[i] != head[i]) {
            return false;
        }
    }
    return head[i] == '\0' && (str[i] == '\0' || !unichar_isident(str[i]));
}

bool mp_repl_continue_with_input(const char *input) {
    // check for blank input
    if (input[0] == '\0') {
        return false;
    }

    // check if input starts with a certain keyword
    bool starts_with_compound_keyword =
           input[0] == '@'
        || str_startswith_word(input, "if")
        || str_startswith_word(input, "while")
        || str_startswith_word(input, "for")
        || str_startswith_word(input, "try")
        || str_startswith_word(input, "with")
        || str_startswith_word(input, "def")
        || str_startswith_word(input, "class")
        #if MICROPY_PY_ASYNC_AWAIT
        || str_startswith_word(input, "async")
        #endif
        ;

    // check for unmatched open bracket, quote or escape quote
    #define Q_NONE (0)
    #define Q_1_SINGLE (1)
    #define Q_1_DOUBLE (2)
    #define Q_3_SINGLE (3)
    #define Q_3_DOUBLE (4)
    int n_paren = 0;
    int n_brack = 0;
    int n_brace = 0;
    int in_quote = Q_NONE;
    const char *i;
    for (i = input; *i; i++) {
        if (*i == '\'') {
            if ((in_quote == Q_NONE || in_quote == Q_3_SINGLE) && i[1] == '\'' && i[2] == '\'') {
                i += 2;
                in_quote = Q_3_SINGLE - in_quote;
            } else if (in_quote == Q_NONE || in_quote == Q_1_SINGLE) {
                in_quote = Q_1_SINGLE - in_quote;
            }
        } else if (*i == '"') {
            if ((in_quote == Q_NONE || in_quote == Q_3_DOUBLE) && i[1] == '"' && i[2] == '"') {
                i += 2;
                in_quote = Q_3_DOUBLE - in_quote;
            } else if (in_quote == Q_NONE || in_quote == Q_1_DOUBLE) {
                in_quote = Q_1_DOUBLE - in_quote;
            }
        } else if (*i == '\\' && (i[1] == '\'' || i[1] == '"' || i[1] == '\\')) {
            if (in_quote != Q_NONE) {
                i++;
            }
        } else if (in_quote == Q_NONE) {
            switch (*i) {
                case '(': n_paren += 1; break;
                case ')': n_paren -= 1; break;
                case '[': n_brack += 1; break;
                case ']': n_brack -= 1; break;
                case '{': n_brace += 1; break;
                case '}': n_brace -= 1; break;
                default: break;
            }
        }
    }

    // continue if unmatched brackets or quotes
    if (n_paren > 0 || n_brack > 0 || n_brace > 0 || in_quote == Q_3_SINGLE || in_quote == Q_3_DOUBLE) {
        return true;
    }

    // continue if last character was backslash (for line continuation)
    if (i[-1] == '\\') {
        return true;
    }

    // continue if compound keyword and last line was not empty
    if (starts_with_compound_keyword && i[-1] != '\n') {
        return true;
    }

    // otherwise, don't continue
    return false;
}

mp_uint_t mp_repl_autocomplete(const char *str, mp_uint_t len, const mp_print_t *print, const char **compl_str) {
    // scan backwards to find start of "a.b.c" chain
    const char *org_str = str;
    const char *top = str + len;
    for (const char *s = top; --s >= str;) {
        if (!(unichar_isalpha(*s) || unichar_isdigit(*s) || *s == '_' || *s == '.')) {
            ++s;
            str = s;
            break;
        }
    }

    // begin search in locals dict
    mp_obj_dict_t *dict = mp_locals_get();

    for (;;) {
        // get next word in string to complete
        const char *s_start = str;
        while (str < top && *str != '.') {
            ++str;
        }
        mp_uint_t s_len = str - s_start;

        if (str < top) {
            // a complete word, lookup in current dict

            mp_obj_t obj = MP_OBJ_NULL;
            for (mp_uint_t i = 0; i < dict->map.alloc; i++) {
                if (MP_MAP_SLOT_IS_FILLED(&dict->map, i)) {
                    size_t d_len;
                    const char *d_str = mp_obj_str_get_data(dict->map.table[i].key, &d_len);
                    if (s_len == d_len && strncmp(s_start, d_str, d_len) == 0) {
                        obj = dict->map.table[i].value;
                        break;
                    }
                }
            }

            if (obj == MP_OBJ_NULL) {
                // lookup failed
                return 0;
            }

            // found an object of this name; try to get its dict
            if (MP_OBJ_IS_TYPE(obj, &mp_type_module)) {
                dict = mp_obj_module_get_globals(obj);
            } else {
                mp_obj_type_t *type;
                if (MP_OBJ_IS_TYPE(obj, &mp_type_type)) {
                    type = MP_OBJ_TO_PTR(obj);
                } else {
                    type = mp_obj_get_type(obj);
                }
                if (type->locals_dict != NULL && type->locals_dict->base.type == &mp_type_dict) {
                    dict = type->locals_dict;
                } else {
                    // obj has no dict
                    return 0;
                }
            }

            // skip '.' to move to next word
            ++str;

        } else {
            // end of string, do completion on this partial name

            // look for matches
            int n_found = 0;
            const char *match_str = NULL;
            mp_uint_t match_len = 0;
            for (mp_uint_t i = 0; i < dict->map.alloc; i++) {
                if (MP_MAP_SLOT_IS_FILLED(&dict->map, i)) {
                    size_t d_len;
                    const char *d_str = mp_obj_str_get_data(dict->map.table[i].key, &d_len);
                    if (s_len <= d_len && strncmp(s_start, d_str, s_len) == 0) {
                        if (match_str == NULL) {
                            match_str = d_str;
                            match_len = d_len;
                        } else {
                            // search for longest common prefix of match_str and d_str
                            // (assumes these strings are null-terminated)
                            for (mp_uint_t j = s_len; j <= match_len && j <= d_len; ++j) {
                                if (match_str[j] != d_str[j]) {
                                    match_len = j;
                                    break;
                                }
                            }
                        }
                        ++n_found;
                    }
                }
            }

            // nothing found
            if (n_found == 0) {
                // If there're no better alternatives, and if it's first word
                // in the line, try to complete "import".
                if (s_start == org_str) {
                    static const char import_str[] = "import ";
                    if (memcmp(s_start, import_str, s_len) == 0) {
                        *compl_str = import_str + s_len;
                        return sizeof(import_str) - 1 - s_len;
                    }
                }

                return 0;
            }

            // 1 match found, or multiple matches with a common prefix
            if (n_found == 1 || match_len > s_len) {
                *compl_str = match_str + s_len;
                return match_len - s_len;
            }

            // multiple matches found, print them out

            #define WORD_SLOT_LEN (16)
            #define MAX_LINE_LEN  (4 * WORD_SLOT_LEN)

            int line_len = MAX_LINE_LEN; // force a newline for first word
            for (mp_uint_t i = 0; i < dict->map.alloc; i++) {
                if (MP_MAP_SLOT_IS_FILLED(&dict->map, i)) {
                    size_t d_len;
                    const char *d_str = mp_obj_str_get_data(dict->map.table[i].key, &d_len);
                    if (s_len <= d_len && strncmp(s_start, d_str, s_len) == 0) {
                        int gap = (line_len + WORD_SLOT_LEN - 1) / WORD_SLOT_LEN * WORD_SLOT_LEN - line_len;
                        if (gap < 2) {
                            gap += WORD_SLOT_LEN;
                        }
                        if (line_len + gap + d_len <= MAX_LINE_LEN) {
                            // TODO optimise printing of gap?
                            for (int j = 0; j < gap; ++j) {
                                mp_print_str(print, " ");
                            }
                            mp_print_str(print, d_str);
                            line_len += gap + d_len;
                        } else {
                            mp_printf(print, "\n%s", d_str);
                            line_len = d_len;
                        }
                    }
                }
            }
            mp_print_str(print, "\n");

            return (mp_uint_t)(-1); // indicate many matches
        }
    }
}

#endif // MICROPY_HELPER_REPL
