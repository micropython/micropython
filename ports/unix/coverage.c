#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/obj.h"
#include "py/objfun.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/repl.h"
#include "py/mpz.h"
#include "py/builtin.h"
#include "py/emit.h"
#include "py/formatfloat.h"
#include "py/ringbuf.h"
#include "py/pairheap.h"
#include "py/stream.h"
#include "py/binary.h"
#include "py/bc.h"

// expected output of this file is found in extra_coverage.py.exp

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

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_stest_fileio,
    MP_QSTR_stest_fileio,
    MP_TYPE_FLAG_NONE,
    protocol, &fileio_stream_p,
    locals_dict, &rawfile_locals_dict
    );

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
};

STATIC MP_DEFINE_CONST_DICT(rawfile_locals_dict2, rawfile_locals_dict_table2);

STATIC const mp_stream_p_t textio_stream_p2 = {
    .read = stest_read2,
    .write = NULL,
    .is_text = true,
};

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_stest_textio2,
    MP_QSTR_stest_textio2,
    MP_TYPE_FLAG_NONE,
    protocol, &textio_stream_p2,
    locals_dict, &rawfile_locals_dict2
    );

// str/bytes objects without a valid hash
STATIC const mp_obj_str_t str_no_hash_obj = {{&mp_type_str}, 0, 10, (const byte *)"0123456789"};
STATIC const mp_obj_str_t bytes_no_hash_obj = {{&mp_type_bytes}, 0, 10, (const byte *)"0123456789"};

STATIC int pairheap_lt(mp_pairheap_t *a, mp_pairheap_t *b) {
    return (uintptr_t)a < (uintptr_t)b;
}

// ops array contain operations: x>=0 means push(x), x<0 means delete(-x)
STATIC void pairheap_test(size_t nops, int *ops) {
    mp_pairheap_t node[8];
    for (size_t i = 0; i < MP_ARRAY_SIZE(node); ++i) {
        mp_pairheap_init_node(pairheap_lt, &node[i]);
    }
    mp_pairheap_t *heap = mp_pairheap_new(pairheap_lt);
    mp_printf(&mp_plat_print, "create:");
    for (size_t i = 0; i < nops; ++i) {
        if (ops[i] >= 0) {
            heap = mp_pairheap_push(pairheap_lt, heap, &node[ops[i]]);
        } else {
            heap = mp_pairheap_delete(pairheap_lt, heap, &node[-ops[i]]);
        }
        if (mp_pairheap_is_empty(pairheap_lt, heap)) {
            mp_printf(&mp_plat_print, " -");
        } else {
            mp_printf(&mp_plat_print, " %d", mp_pairheap_peek(pairheap_lt, heap) - &node[0]);
            ;
        }
    }
    mp_printf(&mp_plat_print, "\npop all:");
    while (!mp_pairheap_is_empty(pairheap_lt, heap)) {
        mp_printf(&mp_plat_print, " %d", mp_pairheap_peek(pairheap_lt, heap) - &node[0]);
        ;
        heap = mp_pairheap_pop(pairheap_lt, heap);
    }
    mp_printf(&mp_plat_print, "\n");
}

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
        mp_printf(&mp_plat_print, "%.2s %.3s '%4.4s' '%5.5q' '%.3q'\n", "abc", "abc", "abc", MP_QSTR_True, MP_QSTR_True); // fixed string precision
        mp_printf(&mp_plat_print, "%.*s\n", -1, "abc"); // negative string precision
        mp_printf(&mp_plat_print, "%b %b\n", 0, 1); // bools
        #ifndef NDEBUG
        mp_printf(&mp_plat_print, "%s\n", NULL); // null string
        #else
        mp_printf(&mp_plat_print, "(null)\n"); // without debugging mp_printf won't check for null
        #endif
        mp_printf(&mp_plat_print, "%d\n", 0x80000000); // should print signed
        mp_printf(&mp_plat_print, "%u\n", 0x80000000); // should print unsigned
        mp_printf(&mp_plat_print, "%x\n", 0x80000000); // should print unsigned
        mp_printf(&mp_plat_print, "%X\n", 0x80000000); // should print unsigned
        mp_printf(&mp_plat_print, "abc\n%"); // string ends in middle of format specifier
        mp_printf(&mp_plat_print, "%%\n"); // literal % character
    }

    // GC
    {
        mp_printf(&mp_plat_print, "# GC\n");

        // calling gc_free while GC is locked
        gc_lock();
        gc_free(NULL);
        gc_unlock();

        // using gc_realloc to resize to 0, which means free the memory
        void *p = gc_alloc(4, false);
        mp_printf(&mp_plat_print, "%p\n", gc_realloc(p, 0, false));

        // calling gc_nbytes with a non-heap pointer
        mp_printf(&mp_plat_print, "%p\n", gc_nbytes(NULL));
    }

    // GC initialisation and allocation stress test, to check the logic behind ALLOC_TABLE_GAP_BYTE
    // (the following test should fail when ALLOC_TABLE_GAP_BYTE=0)
    {
        mp_printf(&mp_plat_print, "# GC part 2\n");

        // check the GC is unlocked and save its state
        assert(MP_STATE_THREAD(gc_lock_depth) == 0);
        mp_state_mem_t mp_state_mem_orig = mp_state_ctx.mem;

        // perform the test
        unsigned heap_size = 64 * MICROPY_BYTES_PER_GC_BLOCK;
        for (unsigned j = 0; j < 256 * MP_BYTES_PER_OBJ_WORD; ++j) {
            char *heap = calloc(heap_size, 1);
            gc_init(heap, heap + heap_size);

            m_malloc(MICROPY_BYTES_PER_GC_BLOCK);
            void *o = gc_alloc(MICROPY_BYTES_PER_GC_BLOCK, GC_ALLOC_FLAG_HAS_FINALISER);
            ((mp_obj_base_t *)o)->type = NULL; // ensure type is cleared so GC doesn't look for finaliser
            for (unsigned i = 0; i < heap_size / MICROPY_BYTES_PER_GC_BLOCK; ++i) {
                void *p = m_malloc_maybe(MICROPY_BYTES_PER_GC_BLOCK);
                if (!p) {
                    break;
                }
                *(void **)p = o;
                o = p;
            }
            gc_collect();
            free(heap);
            heap_size += MICROPY_BYTES_PER_GC_BLOCK / 16;
        }
        mp_printf(&mp_plat_print, "pass\n");

        // restore the GC state (the original heap)
        mp_state_ctx.mem = mp_state_mem_orig;
    }

    // tracked allocation
    {
        #define NUM_PTRS (8)
        #define NUM_BYTES (128)
        #define FLIP_POINTER(p) ((uint8_t *)((uintptr_t)(p) ^ 0x0f))

        mp_printf(&mp_plat_print, "# tracked allocation\n");
        mp_printf(&mp_plat_print, "m_tracked_head = %p\n", MP_STATE_VM(m_tracked_head));

        uint8_t *ptrs[NUM_PTRS];

        // allocate memory blocks
        for (size_t i = 0; i < NUM_PTRS; ++i) {
            ptrs[i] = m_tracked_calloc(1, NUM_BYTES);
            bool all_zero = true;
            for (size_t j = 0; j < NUM_BYTES; ++j) {
                if (ptrs[i][j] != 0) {
                    all_zero = false;
                    break;
                }
                ptrs[i][j] = j;
            }
            mp_printf(&mp_plat_print, "%d %d\n", i, all_zero);

            // hide the pointer from the GC and collect
            ptrs[i] = FLIP_POINTER(ptrs[i]);
            gc_collect();
        }

        // check the memory blocks have the correct content
        for (size_t i = 0; i < NUM_PTRS; ++i) {
            bool correct_contents = true;
            for (size_t j = 0; j < NUM_BYTES; ++j) {
                if (FLIP_POINTER(ptrs[i])[j] != j) {
                    correct_contents = false;
                    break;
                }
            }
            mp_printf(&mp_plat_print, "%d %d\n", i, correct_contents);
        }

        // free the memory blocks
        for (size_t i = 0; i < NUM_PTRS; ++i) {
            m_tracked_free(FLIP_POINTER(ptrs[i]));
        }

        mp_printf(&mp_plat_print, "m_tracked_head = %p\n", MP_STATE_VM(m_tracked_head));
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

        len = mp_repl_autocomplete("i", 1,  &mp_plat_print, &str);
        mp_printf(&mp_plat_print, "%.*s\n", (int)len, str);
        mp_repl_autocomplete("import ", 7,  &mp_plat_print, &str);
        len = mp_repl_autocomplete("import ut", 9,  &mp_plat_print, &str);
        mp_printf(&mp_plat_print, "%.*s\n", (int)len, str);
        mp_repl_autocomplete("import utime", 12,  &mp_plat_print, &str);

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
        mp_printf(&mp_plat_print, "%d\n", mp_obj_is_qstr(mp_obj_str_intern(mp_obj_new_str("intern me", 9))));
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

        // mpz_set_from_float with inf as argument
        mpz_set_from_float(&mpz, 1.0 / 0.0);
        mpz_as_uint_checked(&mpz, &value);
        mp_printf(&mp_plat_print, "%d\n", (int)value);

        // mpz_set_from_float with 0 as argument
        mpz_set_from_float(&mpz, 0);
        mpz_as_uint_checked(&mpz, &value);
        mp_printf(&mp_plat_print, "%d\n", (int)value);

        // mpz_set_from_float with 0<x<1 as argument
        mpz_set_from_float(&mpz, 1e-10);
        mpz_as_uint_checked(&mpz, &value);
        mp_printf(&mp_plat_print, "%d\n", (int)value);

        // mpz_set_from_float with 1<=x<2 as argument
        mpz_set_from_float(&mpz, 1.5);
        mpz_as_uint_checked(&mpz, &value);
        mp_printf(&mp_plat_print, "%d\n", (int)value);

        // mpz_set_from_float with 2<x as argument
        mpz_set_from_float(&mpz, 12345);
        mpz_as_uint_checked(&mpz, &value);
        mp_printf(&mp_plat_print, "%d\n", (int)value);

        // mpz_mul_inpl with dest==rhs, lhs!=rhs
        mpz_t mpz2;
        mpz_set_from_int(&mpz, 2);
        mpz_init_from_int(&mpz2, 3);
        mpz_mul_inpl(&mpz, &mpz2, &mpz);
        mpz_as_uint_checked(&mpz, &value);
        mp_printf(&mp_plat_print, "%d\n", (int)value);
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

        // mp_obj_int_get_uint_checked with non-negative small-int
        mp_printf(&mp_plat_print, "%d\n", (int)mp_obj_int_get_uint_checked(MP_OBJ_NEW_SMALL_INT(1)));

        // mp_obj_int_get_uint_checked with non-negative big-int
        mp_printf(&mp_plat_print, "%d\n", (int)mp_obj_int_get_uint_checked(mp_obj_new_int_from_ll(2)));

        // mp_obj_int_get_uint_checked with negative small-int (should raise exception)
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_obj_int_get_uint_checked(MP_OBJ_NEW_SMALL_INT(-1));
            nlr_pop();
        } else {
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }

        // mp_obj_int_get_uint_checked with negative big-int (should raise exception)
        if (nlr_push(&nlr) == 0) {
            mp_obj_int_get_uint_checked(mp_obj_new_int_from_ll(-2));
            nlr_pop();
        } else {
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }

        // call mp_obj_new_exception_args (it's a part of the public C API and not used in the core)
        mp_obj_print_exception(&mp_plat_print, mp_obj_new_exception_args(&mp_type_ValueError, 0, NULL));
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

    // VM
    {
        mp_printf(&mp_plat_print, "# VM\n");

        // call mp_execute_bytecode with invalide bytecode (should raise NotImplementedError)
        mp_module_context_t context;
        mp_obj_fun_bc_t fun_bc;
        fun_bc.context = &context;
        fun_bc.child_table = NULL;
        fun_bc.bytecode = (const byte *)"\x01"; // just needed for n_state
        mp_code_state_t *code_state = m_new_obj_var(mp_code_state_t, mp_obj_t, 1);
        code_state->fun_bc = &fun_bc;
        code_state->ip = (const byte *)"\x00"; // just needed for an invalid opcode
        code_state->sp = &code_state->state[0];
        code_state->exc_sp_idx = 0;
        code_state->old_globals = NULL;
        mp_vm_return_kind_t ret = mp_execute_bytecode(code_state, MP_OBJ_NULL);
        mp_printf(&mp_plat_print, "%d %d\n", ret, mp_obj_get_type(code_state->state[0]) == &mp_type_NotImplementedError);
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
        mp_handle_pending(true);

        // unlock scheduler
        mp_sched_unlock();
        mp_printf(&mp_plat_print, "unlocked\n");

        // drain pending callbacks
        while (mp_sched_num_pending()) {
            mp_handle_pending(true);
        }

        // setting the keyboard interrupt and raising it during mp_handle_pending
        mp_sched_keyboard_interrupt();
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_handle_pending(true);
            nlr_pop();
        } else {
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }

        // setting the keyboard interrupt (twice) and cancelling it during mp_handle_pending
        mp_sched_keyboard_interrupt();
        mp_sched_keyboard_interrupt();
        mp_handle_pending(false);

        // setting keyboard interrupt and a pending event (intr should be handled first)
        mp_sched_schedule(MP_OBJ_FROM_PTR(&mp_builtin_print_obj), MP_OBJ_NEW_SMALL_INT(10));
        mp_sched_keyboard_interrupt();
        if (nlr_push(&nlr) == 0) {
            mp_handle_pending(true);
            nlr_pop();
        } else {
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }
        mp_handle_pending(true);
    }

    // ringbuf
    {
        byte buf[100];
        ringbuf_t ringbuf = {buf, sizeof(buf), 0, 0};

        mp_printf(&mp_plat_print, "# ringbuf\n");

        // Single-byte put/get with empty ringbuf.
        mp_printf(&mp_plat_print, "%d %d\n", ringbuf_free(&ringbuf), ringbuf_avail(&ringbuf));
        ringbuf_put(&ringbuf, 22);
        mp_printf(&mp_plat_print, "%d %d\n", ringbuf_free(&ringbuf), ringbuf_avail(&ringbuf));
        mp_printf(&mp_plat_print, "%d\n", ringbuf_get(&ringbuf));
        mp_printf(&mp_plat_print, "%d %d\n", ringbuf_free(&ringbuf), ringbuf_avail(&ringbuf));

        // Two-byte put/get with empty ringbuf.
        ringbuf_put16(&ringbuf, 0xaa55);
        mp_printf(&mp_plat_print, "%d %d\n", ringbuf_free(&ringbuf), ringbuf_avail(&ringbuf));
        mp_printf(&mp_plat_print, "%04x\n", ringbuf_get16(&ringbuf));
        mp_printf(&mp_plat_print, "%d %d\n", ringbuf_free(&ringbuf), ringbuf_avail(&ringbuf));

        // Two-byte put with full ringbuf.
        for (int i = 0; i < 99; ++i) {
            ringbuf_put(&ringbuf, i);
        }
        mp_printf(&mp_plat_print, "%d %d\n", ringbuf_free(&ringbuf), ringbuf_avail(&ringbuf));
        mp_printf(&mp_plat_print, "%d\n", ringbuf_put16(&ringbuf, 0x11bb));
        // Two-byte put with one byte free.
        ringbuf_get(&ringbuf);
        mp_printf(&mp_plat_print, "%d %d\n", ringbuf_free(&ringbuf), ringbuf_avail(&ringbuf));
        mp_printf(&mp_plat_print, "%d\n", ringbuf_put16(&ringbuf, 0x3377));
        ringbuf_get(&ringbuf);
        mp_printf(&mp_plat_print, "%d %d\n", ringbuf_free(&ringbuf), ringbuf_avail(&ringbuf));
        mp_printf(&mp_plat_print, "%d\n", ringbuf_put16(&ringbuf, 0xcc99));
        for (int i = 0; i < 97; ++i) {
            ringbuf_get(&ringbuf);
        }
        mp_printf(&mp_plat_print, "%04x\n", ringbuf_get16(&ringbuf));
        mp_printf(&mp_plat_print, "%d %d\n", ringbuf_free(&ringbuf), ringbuf_avail(&ringbuf));

        // Two-byte put with wrap around on first byte:
        ringbuf.iput = 0;
        ringbuf.iget = 0;
        for (int i = 0; i < 99; ++i) {
            ringbuf_put(&ringbuf, i);
            ringbuf_get(&ringbuf);
        }
        mp_printf(&mp_plat_print, "%d\n", ringbuf_put16(&ringbuf, 0x11bb));
        mp_printf(&mp_plat_print, "%04x\n", ringbuf_get16(&ringbuf));

        // Two-byte put with wrap around on second byte:
        ringbuf.iput = 0;
        ringbuf.iget = 0;
        for (int i = 0; i < 98; ++i) {
            ringbuf_put(&ringbuf, i);
            ringbuf_get(&ringbuf);
        }
        mp_printf(&mp_plat_print, "%d\n", ringbuf_put16(&ringbuf, 0x22ff));
        mp_printf(&mp_plat_print, "%04x\n", ringbuf_get16(&ringbuf));

        // Two-byte get from empty ringbuf.
        ringbuf.iput = 0;
        ringbuf.iget = 0;
        mp_printf(&mp_plat_print, "%d\n", ringbuf_get16(&ringbuf));

        // Two-byte get from ringbuf with one byte available.
        ringbuf.iput = 0;
        ringbuf.iget = 0;
        ringbuf_put(&ringbuf, 0xaa);
        mp_printf(&mp_plat_print, "%d\n", ringbuf_get16(&ringbuf));
    }

    // pairheap
    {
        mp_printf(&mp_plat_print, "# pairheap\n");

        // Basic case.
        int t0[] = {0, 2, 1, 3};
        pairheap_test(MP_ARRAY_SIZE(t0), t0);

        // All pushed in reverse order.
        int t1[] = {7, 6, 5, 4, 3, 2, 1, 0};
        pairheap_test(MP_ARRAY_SIZE(t1), t1);

        // Basic deletion.
        int t2[] = {1, -1, -1, 1, 2, -2, 2, 3, -3};
        pairheap_test(MP_ARRAY_SIZE(t2), t2);

        // Deletion of first child that has next node (the -3).
        int t3[] = {1, 2, 3, 4, -1, -3};
        pairheap_test(MP_ARRAY_SIZE(t3), t3);

        // Deletion of node that's not first child (the -2).
        int t4[] = {1, 2, 3, 4, -2};
        pairheap_test(MP_ARRAY_SIZE(t4), t4);

        // Deletion of node that's not first child and has children (the -3).
        int t5[] = {3, 4, 5, 1, 2, -3};
        pairheap_test(MP_ARRAY_SIZE(t5), t5);
    }

    // mp_obj_is_type and derivatives
    {
        mp_printf(&mp_plat_print, "# mp_obj_is_type\n");

        // mp_obj_is_bool accepts only booleans
        mp_printf(&mp_plat_print, "%d %d\n", mp_obj_is_bool(mp_const_true), mp_obj_is_bool(mp_const_false));
        mp_printf(&mp_plat_print, "%d %d\n", mp_obj_is_bool(MP_OBJ_NEW_SMALL_INT(1)), mp_obj_is_bool(mp_const_none));

        // mp_obj_is_integer accepts ints and booleans
        mp_printf(&mp_plat_print, "%d %d\n", mp_obj_is_integer(MP_OBJ_NEW_SMALL_INT(1)), mp_obj_is_integer(mp_obj_new_int_from_ll(1)));
        mp_printf(&mp_plat_print, "%d %d\n", mp_obj_is_integer(mp_const_true), mp_obj_is_integer(mp_const_false));
        mp_printf(&mp_plat_print, "%d %d\n", mp_obj_is_integer(mp_obj_new_str("1", 1)), mp_obj_is_integer(mp_const_none));

        // mp_obj_is_int accepts small int and object ints
        mp_printf(&mp_plat_print, "%d %d\n", mp_obj_is_int(MP_OBJ_NEW_SMALL_INT(1)), mp_obj_is_int(mp_obj_new_int_from_ll(1)));
    }

    mp_printf(&mp_plat_print, "# end coverage.c\n");

    mp_obj_streamtest_t *s = mp_obj_malloc(mp_obj_streamtest_t, &mp_type_stest_fileio);
    s->buf = NULL;
    s->len = 0;
    s->pos = 0;
    s->error_code = 0;
    mp_obj_streamtest_t *s2 = mp_obj_malloc(mp_obj_streamtest_t, &mp_type_stest_textio2);

    // return a tuple of data for testing on the Python side
    mp_obj_t items[] = {(mp_obj_t)&str_no_hash_obj, (mp_obj_t)&bytes_no_hash_obj, MP_OBJ_FROM_PTR(s), MP_OBJ_FROM_PTR(s2)};
    return mp_obj_new_tuple(MP_ARRAY_SIZE(items), items);
}
MP_DEFINE_CONST_FUN_OBJ_0(extra_coverage_obj, extra_coverage);

#endif
