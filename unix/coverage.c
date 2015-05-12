#include <stdio.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/repl.h"

#if defined(MICROPY_UNIX_COVERAGE)

// function to run extra tests for things that can't be checked by scripts
STATIC mp_obj_t extra_coverage(void) {
    // vstr
    {
        printf("# vstr\n");
        vstr_t *vstr = vstr_new_size(16);
        vstr_hint_size(vstr, 32);
        vstr_add_str(vstr, "ts");
        vstr_ins_byte(vstr, 1, 'e');
        vstr_ins_char(vstr, 3, 't');
        vstr_ins_char(vstr, 10, 's');
        printf("%.*s\n", (int)vstr->len, vstr->buf);

        vstr_cut_head_bytes(vstr, 2);
        printf("%.*s\n", (int)vstr->len, vstr->buf);

        vstr_cut_tail_bytes(vstr, 10);
        printf("%.*s\n", (int)vstr->len, vstr->buf);

        vstr_printf(vstr, "t%cst", 'e');
        printf("%.*s\n", (int)vstr->len, vstr->buf);

        vstr_cut_out_bytes(vstr, 3, 10);
        printf("%.*s\n", (int)vstr->len, vstr->buf);

        VSTR_FIXED(fix, 4);
        vstr_add_str(&fix, "large");
        printf("%.*s\n", (int)fix.len, fix.buf);
    }

    // repl autocomplete
    {
        printf("# repl\n");

        const char *str;
        mp_uint_t len = mp_repl_autocomplete("__n", 3, &mp_plat_print, &str);
        printf("%.*s\n", (int)len, str);

        mp_store_global(MP_QSTR_sys, mp_import_name(MP_QSTR_sys, mp_const_none, MP_OBJ_NEW_SMALL_INT(0)));
        mp_repl_autocomplete("sys.", 4, &mp_plat_print, &str);
        len = mp_repl_autocomplete("sys.impl", 8, &mp_plat_print, &str);
        printf("%.*s\n", (int)len, str);
    }

    // attrtuple
    {
        printf("# attrtuple\n");

        static const qstr fields[] = {MP_QSTR_start, MP_QSTR_stop, MP_QSTR_step};
        static const mp_obj_t items[] = {MP_OBJ_NEW_SMALL_INT(1), MP_OBJ_NEW_SMALL_INT(2), MP_OBJ_NEW_SMALL_INT(3)};
        mp_obj_print_helper(&mp_plat_print, mp_obj_new_attrtuple(fields, 3, items), PRINT_REPR);
        printf("\n");
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(extra_coverage_obj, extra_coverage);

#endif
