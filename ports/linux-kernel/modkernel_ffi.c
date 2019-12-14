/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Yonatan Goldschmidt
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

#include <linux/kallsyms.h>
#include <linux/module.h>

#include <py/runtime.h>
#include <py/qstr.h>
#include <py/obj.h>
#include <py/objstr.h>


STATIC void symbol_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
STATIC mp_obj_t symbol_call(mp_obj_t fun, size_t n_args, size_t n_kw, const mp_obj_t *args);
STATIC mp_obj_t symbol_unary_op(mp_unary_op_t op, mp_obj_t o_in);
STATIC mp_obj_t symbol_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in);

const mp_obj_type_t mp_type_symbol = {
    { &mp_type_type },
    .name = MP_QSTR_Symbol,
    .print = symbol_print,
    .call = symbol_call,
    .unary_op = symbol_unary_op,
    .binary_op = symbol_binary_op,
};

typedef struct {
    mp_obj_base_t base;
    qstr name; // TODO can save the string ptr instead? is it okay w/ GC?
    unsigned long value;
    bool exported; // was it found via find_symbol (=true) or kallsyms_lookup_name (=false)
} sym_obj_t;

STATIC void symbol_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    sym_obj_t *sym = (sym_obj_t*)MP_OBJ_TO_PTR(self_in);
    size_t l;
    const char *s = qstr_data(sym->name, &l);
    mp_printf(print, "Symbol(\"%s\", 0x%p, exported=%s)", s, sym->value,
        sym->exported ? "True": "False");
}

STATIC unsigned long convert_arg(mp_obj_t obj, size_t i) {
    mp_int_t n;
    if (mp_obj_get_int_maybe(obj, &n)) { // covers int/bool
        return n;
    } else if (mp_const_none == obj) {
        return 0;
    } else if (mp_obj_is_str_or_bytes(obj)) {
        return (unsigned long)mp_obj_str_get_str(obj);
    } else if (mp_obj_is_type(obj, &mp_type_symbol)) {
        return ((sym_obj_t*)MP_OBJ_TO_PTR(obj))->value;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
            "argument %d is not int/bool/None/str/bytes/Symbol", (int)i));
    }
}

STATIC mp_obj_t symbol_call(mp_obj_t fun, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    if (n_kw) {
        mp_raise_TypeError("kwargs not allowed");
    }

#define MAX_ARGS 10
    if (n_args > MAX_ARGS) {
        mp_raise_TypeError("maximum number of args is " MP_STRINGIFY(MAX_ARGS));
    }

    unsigned long converted_args[MAX_ARGS] = {};
    for (size_t i = 0; i < n_args; ++i) {
        converted_args[i] = convert_arg(args[i], i + 1);
    }

    // TODO don't allow calls if the Symbol is not a function?
    // the kernel has this info for kallsyms symbols, but it is not exported nicely.
    sym_obj_t *sym = (sym_obj_t*)MP_OBJ_TO_PTR(fun);

#define ul unsigned long
#define ca converted_args
    // TODO make sure it's okay with all calling conventions. when it's not, call with appropriate number of args.
    unsigned long ret = ((ul(*)(ul, ul, ul, ul, ul, ul, ul, ul, ul, ul))sym->value)(ca[0], ca[1], ca[2], ca[3], ca[4],
        ca[5], ca[6], ca[7], ca[8], ca[9]);
#undef ul
#undef ca

    return mp_obj_new_int_from_uint(ret);
}

STATIC mp_obj_t symbol_unary_op(mp_unary_op_t op, mp_obj_t o_in) {
    switch (op) {
    case MP_UNARY_OP_INT:
        return mp_obj_new_int_from_uint(((sym_obj_t*)MP_OBJ_TO_PTR(o_in))->value);

    default:
        return mp_generic_unary_op(op, o_in);
    }
}

STATIC mp_obj_t symbol_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    return mp_binary_op(op, mp_obj_new_int_from_uint(((sym_obj_t*)MP_OBJ_TO_PTR(lhs_in))->value), rhs_in);
}

STATIC sym_obj_t *make_new_sym(qstr name, unsigned long value, bool exported) {
    sym_obj_t *sym = m_new_obj(sym_obj_t);
    sym->base.type = &mp_type_symbol;
    sym->name = name;
    sym->value = value;
    sym->exported = exported;
    return sym;
}

mp_obj_t mp_lazy_load_global(qstr qst) {
    size_t l;
    const char *s = qstr_data(qst, &l); // it's null terminated.

    // TODO what about module symbols? is it legit? does it keep a reference to the owner?
    unsigned long value = (unsigned long)__symbol_get(s);
    if (value) {
        return make_new_sym(qst, value, true);
    }

    value = kallsyms_lookup_name(s);
    if (value) {
        return make_new_sym(qst, value, false);
    }

    return MP_OBJ_NULL;
}

STATIC const char *get_ptr(mp_obj_t obj) {
    if (mp_obj_is_int(obj)) {
        return (const char*)mp_obj_int_get_truncated(obj);
    } else if (mp_obj_is_type(obj, &mp_type_symbol)) {
        return (const char*)((sym_obj_t*)MP_OBJ_TO_PTR(obj))->value;
    } else {
        mp_raise_TypeError("int/Symbol accepted");
    }
}

STATIC mp_obj_t kernel_ffi_str(mp_obj_t obj) {
    const char *s = get_ptr(obj);
    return mp_obj_new_str(s, strlen(s));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(kernel_ffi_str_obj, kernel_ffi_str);

STATIC mp_obj_t kernel_ffi_bytes(mp_obj_t obj, mp_obj_t n) {
    return mp_obj_new_bytes((const byte*)get_ptr(obj), mp_obj_int_get_checked(n));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(kernel_ffi_bytes_obj, kernel_ffi_bytes);

STATIC mp_obj_t kernel_ffi_p8(size_t n_args, const mp_obj_t *args) {
    u8 *ptr = (u8*)get_ptr(args[0]);

    if (n_args == 2) {
        mp_int_t value = mp_obj_int_get_checked(args[1]);
        u8 value8 = (u8)value;
        if (value8 != value) {
            mp_raise_ValueError("value overflow");
        }

        *ptr = (u8)value;
        return mp_const_none;
    } else {
        return mp_obj_new_int_from_uint(*ptr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(kernel_ffi_p8_obj, 1, 2, kernel_ffi_p8);

STATIC mp_obj_t kernel_ffi_p16(size_t n_args, const mp_obj_t *args) {
    u16 *ptr = (u16*)get_ptr(args[0]);

    if (n_args == 2) {
        mp_int_t value = mp_obj_int_get_checked(args[1]);
        u16 value16 = (u16)value;
        if (value16 != value) {
            mp_raise_ValueError("value overflow");
        }

        *ptr = (u16)value;
        return mp_const_none;
    } else {
        return mp_obj_new_int_from_uint(*ptr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(kernel_ffi_p16_obj, 1, 2, kernel_ffi_p16);

STATIC mp_obj_t kernel_ffi_p32(size_t n_args, const mp_obj_t *args) {
    u32 *ptr = (u32*)get_ptr(args[0]);

    if (n_args == 2) {
        mp_int_t value = mp_obj_int_get_checked(args[1]);
        u32 value32 = (u32)value;
        if (value32 != value) {
            mp_raise_ValueError("value overflow");
        }

        *ptr = (u32)value;
        return mp_const_none;
    } else {
        return mp_obj_new_int_from_uint(*ptr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(kernel_ffi_p32_obj, 1, 2, kernel_ffi_p32);

STATIC mp_obj_t kernel_ffi_p64(size_t n_args, const mp_obj_t *args) {
    u64 *ptr = (u64*)get_ptr(args[0]);

    if (n_args == 2) {
        mp_int_t value = mp_obj_int_get_truncated(args[1]);
        u64 value64 = (u64)value;
        if (value64 != value) {
            mp_raise_ValueError("value overflow");
        }

        // temporarily disable WP bit, to allow writing to unwritable areas.
        // (most likely a "const" variable)
        // disable interrupts in this short while, to prevent the CPU from jumping
        // to any other interrupt/preemption/whatever while the protection is off.

        unsigned long flags;
        local_irq_save(flags);
        write_cr0(read_cr0() & (~0x10000));

        *ptr = (u64)value;

        write_cr0(read_cr0() | 0x10000);
        local_irq_restore(flags);

        return mp_const_none;
    } else {
        return mp_obj_new_int_from_uint(*ptr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(kernel_ffi_p64_obj, 1, 2, kernel_ffi_p64);

STATIC const mp_rom_map_elem_t kernel_ffi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_kernel_ffi) },

    { MP_ROM_QSTR(MP_QSTR_Symbol), MP_ROM_PTR(&mp_type_symbol) },
    { MP_ROM_QSTR(MP_QSTR_str),    MP_ROM_PTR(&kernel_ffi_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_bytes),  MP_ROM_PTR(&kernel_ffi_bytes_obj) },
    { MP_ROM_QSTR(MP_QSTR_p8),     MP_ROM_PTR(&kernel_ffi_p8_obj) },
    { MP_ROM_QSTR(MP_QSTR_p16),    MP_ROM_PTR(&kernel_ffi_p16_obj) },
    { MP_ROM_QSTR(MP_QSTR_p32),    MP_ROM_PTR(&kernel_ffi_p32_obj) },
    { MP_ROM_QSTR(MP_QSTR_p64),    MP_ROM_PTR(&kernel_ffi_p64_obj) },
};

STATIC MP_DEFINE_CONST_DICT(kernel_ffi_module_globals, kernel_ffi_module_globals_table);

const mp_obj_module_t mp_module_kernel_ffi = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&kernel_ffi_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_kernel_ffi, mp_module_kernel_ffi, 1);
