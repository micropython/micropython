#include <stdio.h>
#include <string.h>

#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/mpz.h"
#include "py/builtin.h"
#include "py/emit.h"
#include "py/formatfloat.h"
#include "py/stream.h"
#include "py/binary.h"

#if defined(MICROPY_UNIX_COVERAGE)

// stream testing object
typedef struct _mp_obj_streamtest_t {
    mp_obj_base_t base;
    uint8_t *buf;
    size_t len;
    size_t pos;
    int error_code;
} mp_obj_streamtest_t;

STATIC mp_obj_t stest_set_buf(mp_obj_t o_in, mp_obj_t buf_in) {
    mp_obj_streamtest_t *o = MP_OBJ_TO_PTR(o_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    o->buf = m_new(uint8_t, bufinfo.len);
    memcpy(o->buf, bufinfo.buf, bufinfo.len);
    o->len = bufinfo.len;
    o->pos = 0;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(stest_set_buf_obj, stest_set_buf);

STATIC mp_obj_t stest_set_error(mp_obj_t o_in, mp_obj_t err_in) {
    mp_obj_streamtest_t *o = MP_OBJ_TO_PTR(o_in);
    o->error_code = mp_obj_get_int(err_in);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(stest_set_error_obj, stest_set_error);

STATIC mp_uint_t stest_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_streamtest_t *o = MP_OBJ_TO_PTR(o_in);
    if (o->pos < o->len) {
        if (size > o->len - o->pos) {
            size = o->len - o->pos;
        }
        memcpy(buf, o->buf + o->pos, size);
        o->pos += size;
        return size;
    } else if (o->error_code == 0) {
        return 0;
    } else {
        *errcode = o->error_code;
        return MP_STREAM_ERROR;
    }
}

STATIC mp_uint_t stest_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_streamtest_t *o = MP_OBJ_TO_PTR(o_in);
    (void)buf;
    (void)size;
    *errcode = o->error_code;
    return MP_STREAM_ERROR;
}

STATIC mp_uint_t stest_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_streamtest_t *o = MP_OBJ_TO_PTR(o_in);
    (void)arg;
    (void)request;
    (void)errcode;
    if (o->error_code != 0) {
        *errcode = o->error_code;
        return MP_STREAM_ERROR;
    }
    return 0;
}

STATIC const mp_rom_map_elem_t rawfile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set_buf), MP_ROM_PTR(&stest_set_buf_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_error), MP_ROM_PTR(&stest_set_error_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_read1), MP_ROM_PTR(&mp_stream_read1_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write1), MP_ROM_PTR(&mp_stream_write1_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&mp_stream_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(rawfile_locals_dict, rawfile_locals_dict_table);

STATIC const mp_stream_p_t fileio_stream_p = {
    .read = stest_read,
    .write = stest_write,
    .ioctl = stest_ioctl,
};

STATIC const mp_obj_type_t mp_type_stest_fileio = {
    { &mp_type_type },
    .protocol = &fileio_stream_p,
    .locals_dict = (mp_obj_dict_t*)&rawfile_locals_dict,
};

// stream read returns non-blocking error
STATIC mp_uint_t stest_read2(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    (void)o_in;
    (void)buf;
    (void)size;
    *errcode = MP_EAGAIN;
    return MP_STREAM_ERROR;
}

STATIC const mp_rom_map_elem_t rawfile_locals_dict_table2[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
};

STATIC MP_DEFINE_CONST_DICT(rawfile_locals_dict2, rawfile_locals_dict_table2);

STATIC const mp_stream_p_t textio_stream_p2 = {
    .read = stest_read2,
    .write = NULL,
    .is_text = true,
};

STATIC const mp_obj_type_t mp_type_stest_textio2 = {
    { &mp_type_type },
    .protocol = &textio_stream_p2,
    .locals_dict = (mp_obj_dict_t*)&rawfile_locals_dict2,
};

// str/bytes objects without a valid hash
STATIC const mp_obj_str_t str_no_hash_obj = {{&mp_type_str}, 0, 10, (const byte*)"0123456789"};
STATIC const mp_obj_str_t bytes_no_hash_obj = {{&mp_type_bytes}, 0, 10, (const byte*)"0123456789"};

// function to run extra tests for things that can't be checked by scripts
STATIC mp_obj_t extra_coverage(void) {
    // mp_printf (used by ports that don't have a native printf)
    {
        mp_printf(&mp_plat_print, "# mp_printf\n");
        mp_printf(&mp_plat_print, "%d %+d % d\n", -123, 123, 123); // sign
        mp_printf(&mp_plat_print, "%05d\n", -123); // negative number with zero padding
        mp_printf(&mp_plat_print, "%ld\n", 123); // long
        mp_printf(&mp_plat_print, "%lx\n", 0x123); // long hex
        mp_printf(&mp_plat_print, "%X\n", 0x1abcdef); // capital hex
        mp_printf(&mp_plat_print, "%.2s %.3s\n", "abc", "abc"); // fixed string precision
        mp_printf(&mp_plat_print, "%.*s\n", -1, "abc"); // negative string precision
        mp_printf(&mp_plat_print, "%b %b\n", 0, 1); // bools
        mp_printf(&mp_plat_print, "%s\n", NULL); // null string
        mp_printf(&mp_plat_print, "%d\n", 0x80000000); // should print signed
        mp_printf(&mp_plat_print, "%u\n", 0x80000000); // should print unsigned
        mp_printf(&mp_plat_print, "%x\n", 0x80000000); // should print unsigned
        mp_printf(&mp_plat_print, "%X\n", 0x80000000); // should print unsigned
        mp_printf(&mp_plat_print, "abc\n%"); // string ends in middle of format specifier
    }

    // vstr
    {
        mp_printf(&mp_plat_print, "# vstr\n");
        vstr_t *vstr = vstr_new(16);
        vstr_hint_size(vstr, 32);
        vstr_add_str(vstr, "ts");
        vstr_ins_byte(vstr, 1, 'e');
        vstr_ins_char(vstr, 3, 't');
        vstr_ins_char(vstr, 10, 's');
        mp_printf(&mp_plat_print, "%.*s\n", (int)vstr->len, vstr->buf);

        vstr_cut_head_bytes(vstr, 2);
        mp_printf(&mp_plat_print, "%.*s\n", (int)vstr->len, vstr->buf);

        vstr_cut_tail_bytes(vstr, 10);
        mp_printf(&mp_plat_print, "%.*s\n", (int)vstr->len, vstr->buf);

        vstr_printf(vstr, "t%cst", 'e');
        mp_printf(&mp_plat_print, "%.*s\n", (int)vstr->len, vstr->buf);

        vstr_cut_out_bytes(vstr, 3, 10);
        mp_printf(&mp_plat_print, "%.*s\n", (int)vstr->len, vstr->buf);

        VSTR_FIXED(fix, 4);
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            vstr_add_str(&fix, "large");
            nlr_pop();
        } else {
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }

        fix.len = fix.alloc;
        if (nlr_push(&nlr) == 0) {
            vstr_null_terminated_str(&fix);
            nlr_pop();
        } else {
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }
    }

    // repl autocomplete
    {
        mp_printf(&mp_plat_print, "# repl\n");

        const char *str;
        size_t len = mp_repl_autocomplete("__n", 3, &mp_plat_print, &str);
        mp_printf(&mp_plat_print, "%.*s\n", (int)len, str);

        mp_store_global(MP_QSTR_sys, mp_import_name(MP_QSTR_sys, mp_const_none, MP_OBJ_NEW_SMALL_INT(0)));
        mp_repl_autocomplete("sys.", 4, &mp_plat_print, &str);
        len = mp_repl_autocomplete("sys.impl", 8, &mp_plat_print, &str);
        mp_printf(&mp_plat_print, "%.*s\n", (int)len, str);
    }

    // attrtuple
    {
        mp_printf(&mp_plat_print, "# attrtuple\n");

        static const qstr fields[] = {MP_QSTR_start, MP_QSTR_stop, MP_QSTR_step};
        static const mp_obj_t items[] = {MP_OBJ_NEW_SMALL_INT(1), MP_OBJ_NEW_SMALL_INT(2), MP_OBJ_NEW_SMALL_INT(3)};
        mp_obj_print_helper(&mp_plat_print, mp_obj_new_attrtuple(fields, 3, items), PRINT_REPR);
        mp_printf(&mp_plat_print, "\n");
    }

    // str
    {
        mp_printf(&mp_plat_print, "# str\n");

        // intern string
        mp_printf(&mp_plat_print, "%d\n", MP_OBJ_IS_QSTR(mp_obj_str_intern(mp_obj_new_str("intern me", 9))));
    }

    // bytearray
    {
        mp_printf(&mp_plat_print, "# bytearray\n");

        // create a bytearray via mp_obj_new_bytearray
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(mp_obj_new_bytearray(4, "data"), &bufinfo, MP_BUFFER_RW);
        mp_printf(&mp_plat_print, "%.*s\n", bufinfo.len, bufinfo.buf);
    }

    // mpz
    {
        mp_printf(&mp_plat_print, "# mpz\n");

        mp_uint_t value;
        mpz_t mpz;
        mpz_init_zero(&mpz);

        // mpz_as_uint_checked, with success
        mpz_set_from_int(&mpz, 12345678);
        mp_printf(&mp_plat_print, "%d\n", mpz_as_uint_checked(&mpz, &value));
        mp_printf(&mp_plat_print, "%d\n", (int)value);

        // mpz_as_uint_checked, with negative arg
        mpz_set_from_int(&mpz, -1);
        mp_printf(&mp_plat_print, "%d\n", mpz_as_uint_checked(&mpz, &value));

        // mpz_as_uint_checked, with overflowing arg
        mpz_set_from_int(&mpz, 1);
        mpz_shl_inpl(&mpz, &mpz, 70);
        mp_printf(&mp_plat_print, "%d\n", mpz_as_uint_checked(&mpz, &value));
    }

    // runtime utils
    {
        mp_printf(&mp_plat_print, "# runtime utils\n");

        // call mp_call_function_1_protected
        mp_call_function_1_protected(MP_OBJ_FROM_PTR(&mp_builtin_abs_obj), MP_OBJ_NEW_SMALL_INT(1));
        // call mp_call_function_1_protected with invalid args
        mp_call_function_1_protected(MP_OBJ_FROM_PTR(&mp_builtin_abs_obj), mp_obj_new_str("abc", 3));

        // call mp_call_function_2_protected
        mp_call_function_2_protected(MP_OBJ_FROM_PTR(&mp_builtin_divmod_obj), MP_OBJ_NEW_SMALL_INT(1), MP_OBJ_NEW_SMALL_INT(1));
        // call mp_call_function_2_protected with invalid args
        mp_call_function_2_protected(MP_OBJ_FROM_PTR(&mp_builtin_divmod_obj), mp_obj_new_str("abc", 3), mp_obj_new_str("abc", 3));
    }

    // warning
    {
        mp_emitter_warning(MP_PASS_CODE_SIZE, "test");
    }

    // format float
    {
        mp_printf(&mp_plat_print, "# format float\n");

        // format with inadequate buffer size
        char buf[5];
        mp_format_float(1, buf, sizeof(buf), 'g', 0, '+');
        mp_printf(&mp_plat_print, "%s\n", buf);

        // format with just enough buffer so that precision must be
        // set from 0 to 1 twice
        char buf2[8];
        mp_format_float(1, buf2, sizeof(buf2), 'g', 0, '+');
        mp_printf(&mp_plat_print, "%s\n", buf2);

        // format where precision is trimmed to avoid buffer overflow
        mp_format_float(1, buf2, sizeof(buf2), 'e', 0, '+');
        mp_printf(&mp_plat_print, "%s\n", buf2);
    }

    // binary
    {
        mp_printf(&mp_plat_print, "# binary\n");

        // call function with float and double typecodes
        float far[1];
        double dar[1];
        mp_binary_set_val_array_from_int('f', far, 0, 123);
        mp_printf(&mp_plat_print, "%.0f\n", (double)far[0]);
        mp_binary_set_val_array_from_int('d', dar, 0, 456);
        mp_printf(&mp_plat_print, "%.0lf\n", dar[0]);
    }

    // scheduler
    {
        mp_printf(&mp_plat_print, "# scheduler\n");

        // lock scheduler
        mp_sched_lock();

        // schedule multiple callbacks; last one should fail
        for (int i = 0; i < 5; ++i) {
            mp_printf(&mp_plat_print, "sched(%d)=%d\n", i, mp_sched_schedule(MP_OBJ_FROM_PTR(&mp_builtin_print_obj), MP_OBJ_NEW_SMALL_INT(i)));
        }

        // test nested locking/unlocking
        mp_sched_lock();
        mp_sched_unlock();

        // shouldn't do anything while scheduler is locked
        mp_handle_pending();

        // unlock scheduler
        mp_sched_unlock();
        mp_printf(&mp_plat_print, "unlocked\n");

        // drain pending callbacks
        while (mp_sched_num_pending()) {
            mp_handle_pending();
        }
    }

    mp_obj_streamtest_t *s = m_new_obj(mp_obj_streamtest_t);
    s->base.type = &mp_type_stest_fileio;
    s->buf = NULL;
    s->len = 0;
    s->pos = 0;
    s->error_code = 0;
    mp_obj_streamtest_t *s2 = m_new_obj(mp_obj_streamtest_t);
    s2->base.type = &mp_type_stest_textio2;

    // return a tuple of data for testing on the Python side
    mp_obj_t items[] = {(mp_obj_t)&str_no_hash_obj, (mp_obj_t)&bytes_no_hash_obj, MP_OBJ_FROM_PTR(s), MP_OBJ_FROM_PTR(s2)};
    return mp_obj_new_tuple(MP_ARRAY_SIZE(items), items);
}
MP_DEFINE_CONST_FUN_OBJ_0(extra_coverage_obj, extra_coverage);

#endif
