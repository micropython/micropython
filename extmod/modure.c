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
#include "py/objstr.h"
#include "py/stackctrl.h"

#if MICROPY_PY_BUILTINS_STR_UNICODE
#include "py/unicode.h"
#endif

#if MICROPY_PY_URE

#define re1_5_stack_chk() MP_STACK_CHECK()

#include "lib/re1.5/re1.5.h"

#define FLAG_DEBUG 0x1000

typedef struct _mp_obj_re_t {
    mp_obj_base_t base;
    ByteProg re;
} mp_obj_re_t;

typedef struct _mp_obj_match_t {
    mp_obj_base_t base;
    int num_matches;
    mp_obj_t str;
    const char *caps[0];
} mp_obj_match_t;

STATIC mp_obj_t mod_re_compile(size_t n_args, const mp_obj_t *args);
#if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_obj_type_t re_type;
#endif

STATIC void match_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_match_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<match num=%d>", self->num_matches);
}

STATIC mp_obj_t match_group(mp_obj_t self_in, mp_obj_t no_in) {
    mp_obj_match_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t no = mp_obj_get_int(no_in);
    if (no < 0 || no >= self->num_matches) {
        mp_raise_type_arg(&mp_type_IndexError, no_in);
    }

    const char *start = self->caps[no * 2];
    if (start == NULL) {
        // no match for this group
        return mp_const_none;
    }
    return mp_obj_new_str_of_type(mp_obj_get_type(self->str),
        (const byte *)start, self->caps[no * 2 + 1] - start);
}
MP_DEFINE_CONST_FUN_OBJ_2(match_group_obj, match_group);

#if MICROPY_PY_URE_MATCH_GROUPS

STATIC mp_obj_t match_groups(mp_obj_t self_in) {
    mp_obj_match_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->num_matches <= 1) {
        return mp_const_empty_tuple;
    }
    mp_obj_tuple_t *groups = MP_OBJ_TO_PTR(mp_obj_new_tuple(self->num_matches - 1, NULL));
    for (int i = 1; i < self->num_matches; ++i) {
        groups->items[i - 1] = match_group(self_in, MP_OBJ_NEW_SMALL_INT(i));
    }
    return MP_OBJ_FROM_PTR(groups);
}
MP_DEFINE_CONST_FUN_OBJ_1(match_groups_obj, match_groups);

#endif

#if MICROPY_PY_URE_MATCH_SPAN_START_END

STATIC void match_span_helper(size_t n_args, const mp_obj_t *args, mp_obj_t span[2]) {
    mp_obj_match_t *self = MP_OBJ_TO_PTR(args[0]);

    mp_int_t no = 0;
    if (n_args == 2) {
        no = mp_obj_get_int(args[1]);
        if (no < 0 || no >= self->num_matches) {
            mp_raise_type_arg(&mp_type_IndexError, args[1]);
        }
    }

    mp_int_t s = -1;
    mp_int_t e = -1;
    const char *start = self->caps[no * 2];
    if (start != NULL) {
        // have a match for this group
        const char *begin = mp_obj_str_get_str(self->str);
        s = start - begin;
        e = self->caps[no * 2 + 1] - begin;
    }

    #if MICROPY_PY_BUILTINS_STR_UNICODE
    if (mp_obj_get_type(self->str) == &mp_type_str) {
        const byte *begin = (const byte *)mp_obj_str_get_str(self->str);
        if (s != -1) {
            s = utf8_ptr_to_index(begin, begin + s);
        }
        if (e != -1) {
            e = utf8_ptr_to_index(begin, begin + e);
        }
    }
    #endif

    span[0] = mp_obj_new_int(s);
    span[1] = mp_obj_new_int(e);
}

STATIC mp_obj_t match_span(size_t n_args, const mp_obj_t *args) {
    mp_obj_t span[2];
    match_span_helper(n_args, args, span);
    return mp_obj_new_tuple(2, span);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(match_span_obj, 1, 2, match_span);

STATIC mp_obj_t match_start(size_t n_args, const mp_obj_t *args) {
    mp_obj_t span[2];
    match_span_helper(n_args, args, span);
    return span[0];
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(match_start_obj, 1, 2, match_start);

STATIC mp_obj_t match_end(size_t n_args, const mp_obj_t *args) {
    mp_obj_t span[2];
    match_span_helper(n_args, args, span);
    return span[1];
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(match_end_obj, 1, 2, match_end);

#endif

#if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_rom_map_elem_t match_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_group), MP_ROM_PTR(&match_group_obj) },
    #if MICROPY_PY_URE_MATCH_GROUPS
    { MP_ROM_QSTR(MP_QSTR_groups), MP_ROM_PTR(&match_groups_obj) },
    #endif
    #if MICROPY_PY_URE_MATCH_SPAN_START_END
    { MP_ROM_QSTR(MP_QSTR_span), MP_ROM_PTR(&match_span_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&match_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_end), MP_ROM_PTR(&match_end_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(match_locals_dict, match_locals_dict_table);

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    match_type,
    MP_QSTR_match,
    MP_TYPE_FLAG_NONE,
    print, match_print,
    locals_dict, &match_locals_dict
    );
#endif

STATIC void re_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_re_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<re %p>", self);
}

STATIC mp_obj_t ure_exec(bool is_anchored, uint n_args, const mp_obj_t *args) {
    (void)n_args;
    mp_obj_re_t *self;
    if (mp_obj_is_type(args[0], (mp_obj_type_t *)&re_type)) {
        self = MP_OBJ_TO_PTR(args[0]);
    } else {
        self = MP_OBJ_TO_PTR(mod_re_compile(1, args));
    }
    Subject subj;
    size_t len;
    subj.begin_line = subj.begin = mp_obj_str_get_data(args[1], &len);
    subj.end = subj.begin + len;
    int caps_num = (self->re.sub + 1) * 2;
    mp_obj_match_t *match = m_new_obj_var(mp_obj_match_t, char *, caps_num);
    // cast is a workaround for a bug in msvc: it treats const char** as a const pointer instead of a pointer to pointer to const char
    memset((char *)match->caps, 0, caps_num * sizeof(char *));
    int res = re1_5_recursiveloopprog(&self->re, &subj, match->caps, caps_num, is_anchored);
    if (res == 0) {
        m_del_var(mp_obj_match_t, char *, caps_num, match);
        return mp_const_none;
    }

    match->base.type = (mp_obj_type_t *)&match_type;
    match->num_matches = caps_num / 2; // caps_num counts start and end pointers
    match->str = args[1];
    return MP_OBJ_FROM_PTR(match);
}

STATIC mp_obj_t re_match(size_t n_args, const mp_obj_t *args) {
    return ure_exec(true, n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(re_match_obj, 2, 4, re_match);

STATIC mp_obj_t re_search(size_t n_args, const mp_obj_t *args) {
    return ure_exec(false, n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(re_search_obj, 2, 4, re_search);

STATIC mp_obj_t re_split(size_t n_args, const mp_obj_t *args) {
    mp_obj_re_t *self = MP_OBJ_TO_PTR(args[0]);
    Subject subj;
    size_t len;
    const mp_obj_type_t *str_type = mp_obj_get_type(args[1]);
    subj.begin_line = subj.begin = mp_obj_str_get_data(args[1], &len);
    subj.end = subj.begin + len;
    int caps_num = (self->re.sub + 1) * 2;

    int maxsplit = 0;
    if (n_args > 2) {
        maxsplit = mp_obj_get_int(args[2]);
    }

    mp_obj_t retval = mp_obj_new_list(0, NULL);
    const char **caps = mp_local_alloc(caps_num * sizeof(char *));
    while (true) {
        // cast is a workaround for a bug in msvc: it treats const char** as a const pointer instead of a pointer to pointer to const char
        memset((char **)caps, 0, caps_num * sizeof(char *));
        int res = re1_5_recursiveloopprog(&self->re, &subj, caps, caps_num, false);

        // if we didn't have a match, or had an empty match, it's time to stop
        if (!res || caps[0] == caps[1]) {
            break;
        }

        mp_obj_t s = mp_obj_new_str_of_type(str_type, (const byte *)subj.begin, caps[0] - subj.begin);
        mp_obj_list_append(retval, s);
        if (self->re.sub > 0) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("splitting with sub-captures"));
        }
        subj.begin = caps[1];
        if (maxsplit > 0 && --maxsplit == 0) {
            break;
        }
    }
    // cast is a workaround for a bug in msvc (see above)
    mp_local_free((char **)caps);

    mp_obj_t s = mp_obj_new_str_of_type(str_type, (const byte *)subj.begin, subj.end - subj.begin);
    mp_obj_list_append(retval, s);
    return retval;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(re_split_obj, 2, 3, re_split);

#if MICROPY_PY_URE_SUB

STATIC mp_obj_t re_sub_helper(size_t n_args, const mp_obj_t *args) {
    mp_obj_re_t *self;
    if (mp_obj_is_type(args[0], (mp_obj_type_t *)&re_type)) {
        self = MP_OBJ_TO_PTR(args[0]);
    } else {
        self = MP_OBJ_TO_PTR(mod_re_compile(1, args));
    }
    mp_obj_t replace = args[1];
    mp_obj_t where = args[2];
    mp_int_t count = 0;
    if (n_args > 3) {
        count = mp_obj_get_int(args[3]);
        // Note: flags are currently ignored
    }

    size_t where_len;
    const char *where_str = mp_obj_str_get_data(where, &where_len);
    Subject subj;
    subj.begin_line = subj.begin = where_str;
    subj.end = subj.begin + where_len;
    int caps_num = (self->re.sub + 1) * 2;

    vstr_t vstr_return;
    vstr_return.buf = NULL; // We'll init the vstr after the first match
    mp_obj_match_t *match = mp_local_alloc(sizeof(mp_obj_match_t) + caps_num * sizeof(char *));
    match->base.type = (mp_obj_type_t *)&match_type;
    match->num_matches = caps_num / 2; // caps_num counts start and end pointers
    match->str = where;

    for (;;) {
        // cast is a workaround for a bug in msvc: it treats const char** as a const pointer instead of a pointer to pointer to const char
        memset((char *)match->caps, 0, caps_num * sizeof(char *));
        int res = re1_5_recursiveloopprog(&self->re, &subj, match->caps, caps_num, false);

        // If we didn't have a match, or had an empty match, it's time to stop
        if (!res || match->caps[0] == match->caps[1]) {
            break;
        }

        // Initialise the vstr if it's not already
        if (vstr_return.buf == NULL) {
            vstr_init(&vstr_return, match->caps[0] - subj.begin);
        }

        // Add pre-match string
        vstr_add_strn(&vstr_return, subj.begin, match->caps[0] - subj.begin);

        // Get replacement string
        const char *repl = mp_obj_str_get_str((mp_obj_is_callable(replace) ? mp_call_function_1(replace, MP_OBJ_FROM_PTR(match)) : replace));

        // Append replacement string to result, substituting any regex groups
        while (*repl != '\0') {
            if (*repl == '\\') {
                ++repl;
                bool is_g_format = false;
                if (*repl == 'g' && repl[1] == '<') {
                    // Group specified with syntax "\g<number>"
                    repl += 2;
                    is_g_format = true;
                }

                if ('0' <= *repl && *repl <= '9') {
                    // Group specified with syntax "\g<number>" or "\number"
                    unsigned int match_no = 0;
                    do {
                        match_no = match_no * 10 + (*repl++ - '0');
                    } while ('0' <= *repl && *repl <= '9');
                    if (is_g_format && *repl == '>') {
                        ++repl;
                    }

                    if (match_no >= (unsigned int)match->num_matches) {
                        mp_raise_type_arg(&mp_type_IndexError, MP_OBJ_NEW_SMALL_INT(match_no));
                    }

                    const char *start_match = match->caps[match_no * 2];
                    if (start_match != NULL) {
                        // Add the substring matched by group
                        const char *end_match = match->caps[match_no * 2 + 1];
                        vstr_add_strn(&vstr_return, start_match, end_match - start_match);
                    }
                } else if (*repl == '\\') {
                    // Add the \ character
                    vstr_add_byte(&vstr_return, *repl++);
                }
            } else {
                // Just add the current byte from the replacement string
                vstr_add_byte(&vstr_return, *repl++);
            }
        }

        // Move start pointer to end of last match
        subj.begin = match->caps[1];

        // Stop substitutions if count was given and gets to 0
        if (count > 0 && --count == 0) {
            break;
        }
    }

    mp_local_free(match);

    if (vstr_return.buf == NULL) {
        // Optimisation for case of no substitutions
        return where;
    }

    // Add post-match string
    vstr_add_strn(&vstr_return, subj.begin, subj.end - subj.begin);

    if (mp_obj_get_type(where) == &mp_type_str) {
        return mp_obj_new_str_from_utf8_vstr(&vstr_return);
    } else {
        return mp_obj_new_bytes_from_vstr(&vstr_return);
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(re_sub_obj, 3, 5, re_sub_helper);

#endif

#if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_rom_map_elem_t re_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_match), MP_ROM_PTR(&re_match_obj) },
    { MP_ROM_QSTR(MP_QSTR_search), MP_ROM_PTR(&re_search_obj) },
    { MP_ROM_QSTR(MP_QSTR_split), MP_ROM_PTR(&re_split_obj) },
    #if MICROPY_PY_URE_SUB
    { MP_ROM_QSTR(MP_QSTR_sub), MP_ROM_PTR(&re_sub_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(re_locals_dict, re_locals_dict_table);

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    re_type,
    MP_QSTR_ure,
    MP_TYPE_FLAG_NONE,
    print, re_print,
    locals_dict, &re_locals_dict
    );
#endif

STATIC mp_obj_t mod_re_compile(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    const char *re_str = mp_obj_str_get_str(args[0]);
    int size = re1_5_sizecode(re_str);
    if (size == -1) {
        goto error;
    }
    mp_obj_re_t *o = mp_obj_malloc_var(mp_obj_re_t, char, size, (mp_obj_type_t *)&re_type);
    #if MICROPY_PY_URE_DEBUG
    int flags = 0;
    if (n_args > 1) {
        flags = mp_obj_get_int(args[1]);
    }
    #endif
    int error = re1_5_compilecode(&o->re, re_str);
    if (error != 0) {
    error:
        mp_raise_ValueError(MP_ERROR_TEXT("error in regex"));
    }
    #if MICROPY_PY_URE_DEBUG
    if (flags & FLAG_DEBUG) {
        re1_5_dumpcode(&o->re);
    }
    #endif
    return MP_OBJ_FROM_PTR(o);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_re_compile_obj, 1, 2, mod_re_compile);

#if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_rom_map_elem_t mp_module_re_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ure) },
    { MP_ROM_QSTR(MP_QSTR_compile), MP_ROM_PTR(&mod_re_compile_obj) },
    { MP_ROM_QSTR(MP_QSTR_match), MP_ROM_PTR(&re_match_obj) },
    { MP_ROM_QSTR(MP_QSTR_search), MP_ROM_PTR(&re_search_obj) },
    #if MICROPY_PY_URE_SUB
    { MP_ROM_QSTR(MP_QSTR_sub), MP_ROM_PTR(&re_sub_obj) },
    #endif
    #if MICROPY_PY_URE_DEBUG
    { MP_ROM_QSTR(MP_QSTR_DEBUG), MP_ROM_INT(FLAG_DEBUG) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_re_globals, mp_module_re_globals_table);

const mp_obj_module_t mp_module_ure = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_re_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ure, mp_module_ure);
#endif

// Source files #include'd here to make sure they're compiled in
// only if module is enabled by config setting.

#define re1_5_fatal(x) assert(!x)

#include "lib/re1.5/compilecode.c"
#include "lib/re1.5/recursiveloop.c"
#include "lib/re1.5/charclass.c"

#if MICROPY_PY_URE_DEBUG
// Make sure the output print statements go to the same output as other Python output.
#define printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)
#include "lib/re1.5/dumpcode.c"
#undef printf
#endif

#endif // MICROPY_PY_URE
