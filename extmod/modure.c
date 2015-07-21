/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"

#if MICROPY_PY_URE

#include "re1.5/re1.5.h"

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


STATIC void match_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_match_t *self = self_in;
    mp_printf(print, "<match num=%d>", self->num_matches);
}

STATIC mp_obj_t match_group(mp_obj_t self_in, mp_obj_t no_in) {
    mp_obj_match_t *self = self_in;
    mp_int_t no = mp_obj_get_int(no_in);
    if (no < 0 || no >= self->num_matches) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_IndexError, no_in));
    }

    const char *start = self->caps[no * 2];
    if (start == NULL) {
        // no match for this group
        return mp_const_none;
    }
    return mp_obj_new_str(start, self->caps[no * 2 + 1] - start, false);
}
MP_DEFINE_CONST_FUN_OBJ_2(match_group_obj, match_group);

STATIC const mp_map_elem_t match_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_group), (mp_obj_t) &match_group_obj },
};

STATIC MP_DEFINE_CONST_DICT(match_locals_dict, match_locals_dict_table);

STATIC const mp_obj_type_t match_type = {
    { &mp_type_type },
    .name = MP_QSTR_match,
    .print = match_print,
    .locals_dict = (mp_obj_t)&match_locals_dict,
};

STATIC void re_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_re_t *self = self_in;
    mp_printf(print, "<re %p>", self);
}

STATIC mp_obj_t re_exec(bool is_anchored, uint n_args, const mp_obj_t *args) {
    (void)n_args;
    mp_obj_re_t *self = args[0];
    Subject subj;
    mp_uint_t len;
    subj.begin = mp_obj_str_get_data(args[1], &len);
    subj.end = subj.begin + len;
    int caps_num = (self->re.sub + 1) * 2;
    mp_obj_match_t *match = m_new_obj_var(mp_obj_match_t, char*, caps_num);
    // cast is a workaround for a bug in msvc: it treats const char** as a const pointer instead of a pointer to pointer to const char
    memset((char*)match->caps, 0, caps_num * sizeof(char*));
    int res = re1_5_recursiveloopprog(&self->re, &subj, match->caps, caps_num, is_anchored);
    if (res == 0) {
        m_del_var(mp_obj_match_t, char*, caps_num, match);
        return mp_const_none;
    }

    match->base.type = &match_type;
    match->num_matches = caps_num / 2; // caps_num counts start and end pointers
    match->str = args[1];
    return match;
}

STATIC mp_obj_t re_match(uint n_args, const mp_obj_t *args) {
    return re_exec(true, n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(re_match_obj, 2, 4, re_match);

STATIC mp_obj_t re_search(uint n_args, const mp_obj_t *args) {
    return re_exec(false, n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(re_search_obj, 2, 4, re_search);

STATIC mp_obj_t re_split(uint n_args, const mp_obj_t *args) {
    mp_obj_re_t *self = args[0];
    Subject subj;
    mp_uint_t len;
    subj.begin = mp_obj_str_get_data(args[1], &len);
    subj.end = subj.begin + len;
    int caps_num = (self->re.sub + 1) * 2;

    int maxsplit = 0;
    if (n_args > 2) {
        maxsplit = mp_obj_get_int(args[2]);
    }

    mp_obj_t retval = mp_obj_new_list(0, NULL);
    const char **caps = alloca(caps_num * sizeof(char*));
    while (true) {
        // cast is a workaround for a bug in msvc: it treats const char** as a const pointer instead of a pointer to pointer to const char
        memset((char**)caps, 0, caps_num * sizeof(char*));
        int res = re1_5_recursiveloopprog(&self->re, &subj, caps, caps_num, false);

        // if we didn't have a match, or had an empty match, it's time to stop
        if (!res || caps[0] == caps[1]) {
            break;
        }

        mp_obj_t s = mp_obj_new_str(subj.begin, caps[0] - subj.begin, false);
        mp_obj_list_append(retval, s);
        if (self->re.sub > 0) {
            mp_not_implemented("Splitting with sub-captures");
        }
        subj.begin = caps[1];
        if (maxsplit > 0 && --maxsplit == 0) {
            break;
        }
    }

    mp_obj_t s = mp_obj_new_str(subj.begin, subj.end - subj.begin, false);
    mp_obj_list_append(retval, s);
    return retval;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(re_split_obj, 2, 3, re_split);

STATIC int count_str(const char *s, const char *p) {
    int c = 0, l = strlen(p);
    while (*s != '\0') {
        if (strncmp(s++, p, l)) continue;
        c++;
    }
    return c;
}

STATIC char *replace_str(const char *str, const char *old, const char *new) {
    if (str == NULL || old == NULL || new == NULL || !strlen(old))
        return NULL;
    size_t cache_sz_inc = 16;
    const size_t cache_sz_inc_factor = 3;
    const size_t cache_sz_inc_max = 1048576;

    char *pret, *ret = NULL;
    const char *pstr2, *pstr = str;
    size_t i, count = 0;
    ptrdiff_t *pos_cache = NULL;
    size_t cache_sz = 0;
    size_t cpylen, orglen, retlen, newlen, oldlen = strlen(old);

    while ((pstr2 = strstr(pstr, old)) != NULL) {
        count++;
        if (cache_sz < count) {
            cache_sz += cache_sz_inc;
            pos_cache = realloc(pos_cache, sizeof(*pos_cache) * cache_sz);
            if (pos_cache == NULL) {
                goto end_repl_str;
            }
            cache_sz_inc *= cache_sz_inc_factor;
            if (cache_sz_inc > cache_sz_inc_max) {
                cache_sz_inc = cache_sz_inc_max;
            }
        }
        pos_cache[count - 1] = pstr2 - str;
        pstr = pstr2 + oldlen;
    }

    orglen = pstr - str + strlen(pstr);
    if (count > 0) {
        newlen = strlen(new);
        retlen = orglen + (newlen - oldlen) * count;
    } else	
        retlen = orglen;

    ret = malloc(retlen + 1);
    if (ret == NULL) {
        goto end_repl_str;
    }

    if (count == 0) {
        memcpy(ret, str, strlen(str));
    } else {
        pret = ret;
        memcpy(pret, str, pos_cache[0]);
        pret += pos_cache[0];
        for (i = 0; i < count; i++) {
            memcpy(pret, new, newlen);
            pret += newlen;
            pstr = str + pos_cache[i] + oldlen;
            cpylen = (i == count - 1 ? orglen : pos_cache[i + 1]) - pos_cache[i] - oldlen;
            memcpy(pret, pstr, cpylen);
            pret += cpylen;
        }
        ret[retlen] = '\0';
    }

end_repl_str:
    free(pos_cache);
    return ret;
}

#if !defined itoa
char* itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}
#endif

STATIC mp_obj_t re_exec_sub(bool is_anchored, uint n_args, const mp_obj_t *args) {
    mp_obj_re_t *self = args[0];
    mp_obj_t repl = args[1];
    mp_obj_t ret = args[2];
    const mp_obj_t args2[] = { self, ret };
    mp_obj_match_t *match = re_exec(is_anchored, 2, args2);
    if (!MP_OBJ_IS_TYPE(match, &mp_type_NoneType)) {
        
        const char *str_repl = mp_obj_str_get_str(repl);
        mp_uint_t str_repl_len = mp_obj_str_get_len(repl);
        char *str_repl_cpy = m_new(char, str_repl_len);
        memcpy(str_repl_cpy, str_repl, str_repl_len);
        str_repl_cpy[str_repl_len] = '\0';
        mp_uint_t str_repl_cpy_len = str_repl_len;
        
		int no = 0; 
        for (no = 0; no < match->num_matches; no++) {
            
			int sub_no = 0; 
            for (sub_no = 0; sub_no < 2; sub_no++) {
                
                // Ignore replace of group "\0".
                if(!no && !sub_no%2)
                    continue;
                
                // https://docs.python.org/3/library/re.html:
                // *syntax. \g<number> uses the corresponding group number; 
                // \g<2> is therefore equivalent to \2, but isn’t ambiguous in a replacement such as \g<2>0. 
                // \20 would be interpreted as a reference to group 20, not a reference to group 2 followed by the literal character '0'. 
                // The backreference \g<0> substitutes in the entire substring matched by the RE.                
                const char *sub_group = (sub_no%2?"\\g<#>\0":"\\#\0");
                
                char *sub = replace_str(sub_group, "#\0", itoa(no,(char[]){0,0,0,0}, 10));
                if (sub != NULL) {                   
                    if( count_str(str_repl_cpy, sub) > 0 ) {
                                   
                        const char *start_match = match->caps[no * 2];
                        if (start_match != NULL) {
                            mp_uint_t start_match_len = (match->caps[no * 2 + 1] - start_match);
                            char *str_match_element = m_new(char, start_match_len);
                            memcpy(str_match_element, start_match, start_match_len);
                            str_match_element[start_match_len] = '\0';
                            
                            char *str_repl_new = NULL; 
                            if ((str_repl_new = replace_str(str_repl_cpy, sub, str_match_element)) != NULL) {
                                mp_uint_t str_repl_new_len = strlen(str_repl_new);
                                str_repl_cpy = m_renew(char, str_repl_cpy, str_repl_cpy_len, str_repl_new_len);
                                memcpy(str_repl_cpy, str_repl_new, str_repl_new_len);
                                str_repl_cpy[str_repl_new_len] = '\0';
                                str_repl_cpy_len = str_repl_new_len;
                                
                                free(str_repl_new);
                            } 
                            m_del(char, str_match_element, start_match_len);
                        }
                    }
                    free(sub);
                }
			}
        }
        m_del_var(mp_obj_match_t, char*, match->num_matches, match);
        
        if(str_repl_cpy != NULL) {
            ret = mp_obj_new_str(str_repl_cpy, str_repl_cpy_len, false);
            m_del(char, str_repl_cpy, str_repl_cpy_len);
        }
        
    }
    return ret;
}

STATIC mp_obj_t re_sub(uint n_args, const mp_obj_t *args) {
    return re_exec_sub(false, n_args, args);
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(re_sub_obj, 3, 4, re_sub);

STATIC const mp_map_elem_t re_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_match), (mp_obj_t) &re_match_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_search), (mp_obj_t) &re_search_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_split), (mp_obj_t) &re_split_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sub), (mp_obj_t)&re_sub_obj },
};

STATIC MP_DEFINE_CONST_DICT(re_locals_dict, re_locals_dict_table);

STATIC const mp_obj_type_t re_type = {
    { &mp_type_type },
    .name = MP_QSTR_ure,
    .print = re_print,
    .locals_dict = (mp_obj_t)&re_locals_dict,
};

STATIC mp_obj_t mod_re_compile(uint n_args, const mp_obj_t *args) {
    const char *re_str = mp_obj_str_get_str(args[0]);
    int size = re1_5_sizecode(re_str);
    mp_obj_re_t *o = m_new_obj_var(mp_obj_re_t, char, size);
    o->base.type = &re_type;
    int flags = 0;
    if (n_args > 1) {
        flags = mp_obj_get_int(args[1]);
    }
    int error = re1_5_compilecode(&o->re, re_str);
    if (error != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Error in regex"));
    }
    if (flags & FLAG_DEBUG) {
        re1_5_dumpcode(&o->re);
    }
    return o;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_re_compile_obj, 1, 2, mod_re_compile);

STATIC mp_obj_t mod_re_exec(bool is_anchored, uint n_args, const mp_obj_t *args) {
    (void)n_args;
    mp_obj_re_t *self = mod_re_compile(1, args);

    const mp_obj_t args2[] = {self, args[1]};
    mp_obj_match_t *match = re_exec(is_anchored, 2, args2);
    return match;
}

STATIC mp_obj_t mod_re_match(uint n_args, const mp_obj_t *args) {
    return mod_re_exec(true, n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_re_match_obj, 2, 4, mod_re_match);

STATIC mp_obj_t mod_re_search(uint n_args, const mp_obj_t *args) {
    return mod_re_exec(false, n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_re_search_obj, 2, 4, mod_re_search);

STATIC mp_obj_t mod_re_exec_sub(bool is_anchored, uint n_args, const mp_obj_t *args) {
    (void)n_args;
    mp_obj_re_t *self = mod_re_compile(1, args);

    const mp_obj_t args3[] = { self, args[1], args[2], (n_args>3 ? args[3] : 0) };
    mp_obj_t retval = re_exec_sub(is_anchored, n_args, args3);
    return retval;
}

STATIC mp_obj_t mod_re_sub(uint n_args, const mp_obj_t *args) {
    return mod_re_exec_sub(false, n_args, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_re_sub_obj, 3, 5, mod_re_sub);

STATIC const mp_map_elem_t mp_module_re_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ure) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_compile), (mp_obj_t)&mod_re_compile_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_match), (mp_obj_t)&mod_re_match_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_search), (mp_obj_t)&mod_re_search_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sub), (mp_obj_t)&mod_re_sub_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_DEBUG), MP_OBJ_NEW_SMALL_INT(FLAG_DEBUG) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_re_globals, mp_module_re_globals_table);

const mp_obj_module_t mp_module_ure = {
    .base = { &mp_type_module },
    .name = MP_QSTR_ure,
    .globals = (mp_obj_dict_t*)&mp_module_re_globals,
};

// Source files #include'd here to make sure they're compiled in
// only if module is enabled by config setting.

#define re1_5_fatal(x) assert(!x)
#include "re1.5/compilecode.c"
#include "re1.5/dumpcode.c"
#include "re1.5/recursiveloop.c"
#include "re1.5/charclass.c"

#endif //MICROPY_PY_URE
