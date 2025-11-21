#define MICROPY_STACK_CHECK (1)
#define MICROPY_PY_RE (1)
#define MICROPY_PY_RE_MATCH_GROUPS (1)
#define MICROPY_PY_RE_MATCH_SPAN_START_END (1)
#define MICROPY_PY_RE_SUB (0) // requires vstr interface

#if defined __has_builtin
#if __has_builtin(__builtin_alloca)
#define alloca __builtin_alloca
#endif
#endif

#if !defined(alloca)
#if defined(_PICOLIBC__) && !defined(HAVE_BUILTIN_ALLOCA)
#define alloca(n) m_malloc(n)
#else
#include <alloca.h>
#endif
#endif

#include "py/dynruntime.h"

#define STACK_LIMIT (2048)

const char *stack_top;

void mp_cstack_check(void) {
    // Assumes descending stack on target
    volatile char dummy;
    if (stack_top - &dummy >= STACK_LIMIT) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("maximum recursion depth exceeded"));
    }
}

#if !defined(__linux__)
void *memcpy(void *dst, const void *src, size_t n) {
    return mp_fun_table.memmove_(dst, src, n);
}
void *memset(void *s, int c, size_t n) {
    return mp_fun_table.memset_(s, c, n);
}
#endif

void *memmove(void *dest, const void *src, size_t n) {
    return mp_fun_table.memmove_(dest, src, n);
}

mp_obj_full_type_t match_type;
mp_obj_full_type_t re_type;

#include "extmod/modre.c"

mp_map_elem_t match_locals_dict_table[5];
static MP_DEFINE_CONST_DICT(match_locals_dict, match_locals_dict_table);

mp_map_elem_t re_locals_dict_table[3];
static MP_DEFINE_CONST_DICT(re_locals_dict, re_locals_dict_table);

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    char dummy;
    stack_top = &dummy;

    // Because MP_QSTR_start/end/split are static, xtensa and xtensawin will make a small data section
    // to copy in this key/value pair if they are specified as a struct, so assign them separately.

    match_type.base.type = (void*)&mp_fun_table.type_type;
    match_type.name = MP_QSTR_match;
    MP_OBJ_TYPE_SET_SLOT(&match_type, print, match_print, 0);
    match_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_group), MP_OBJ_FROM_PTR(&match_group_obj) };
    match_locals_dict_table[1] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_groups), MP_OBJ_FROM_PTR(&match_groups_obj) };
    match_locals_dict_table[2] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_span), MP_OBJ_FROM_PTR(&match_span_obj) };
    match_locals_dict_table[3] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_start), MP_OBJ_FROM_PTR(&match_start_obj) };
    match_locals_dict_table[4] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_end), MP_OBJ_FROM_PTR(&match_end_obj) };
    MP_OBJ_TYPE_SET_SLOT(&match_type, locals_dict, (void*)&match_locals_dict, 1);

    re_type.base.type = (void*)&mp_fun_table.type_type;
    re_type.name = MP_QSTR_re;
    MP_OBJ_TYPE_SET_SLOT(&re_type, print, re_print, 0);
    re_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_match), MP_OBJ_FROM_PTR(&re_match_obj) };
    re_locals_dict_table[1] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_search), MP_OBJ_FROM_PTR(&re_search_obj) };
    re_locals_dict_table[2] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_split), MP_OBJ_FROM_PTR(&re_split_obj) };
    MP_OBJ_TYPE_SET_SLOT(&re_type, locals_dict, (void*)&re_locals_dict, 1);

    mp_store_global(MP_QSTR_compile, MP_OBJ_FROM_PTR(&mod_re_compile_obj));
    mp_store_global(MP_QSTR_match, MP_OBJ_FROM_PTR(&re_match_obj));
    mp_store_global(MP_QSTR_search, MP_OBJ_FROM_PTR(&re_search_obj));

    MP_DYNRUNTIME_INIT_EXIT
}
