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
#include <linux/kprobes.h>
#include <linux/vmalloc.h>

#include <py/runtime.h>
#include <py/qstr.h>
#include <py/obj.h>
#include <py/objstr.h>
#include <py/objfun.h>
#include <py/bc.h>


STATIC void symbol_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
STATIC mp_obj_t symbol_call(mp_obj_t fun, size_t n_args, size_t n_kw, const mp_obj_t *args);
STATIC mp_obj_t symbol_unary_op(mp_unary_op_t op, mp_obj_t o_in);
STATIC mp_obj_t symbol_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in);
STATIC mp_obj_t symbol_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

const mp_obj_type_t mp_type_symbol = {
    { &mp_type_type },
    .name = MP_QSTR_Symbol,
    .make_new = symbol_make_new,
    .print = symbol_print,
    .call = symbol_call,
    .unary_op = symbol_unary_op,
    .binary_op = symbol_binary_op,
};

#define mp_obj_is_symbol(obj) mp_obj_is_type(obj, &mp_type_symbol)

typedef struct {
    mp_obj_base_t base;
    qstr name; // TODO can save the string ptr instead? is it okay w/ GC?
    unsigned long value;
    bool exported; // was it found via find_symbol (=true) or kallsyms_lookup_name (=false)
} sym_obj_t;

STATIC mp_obj_t symbol_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, false);

    sym_obj_t *o = m_new_obj(sym_obj_t);
    o->base.type = type;
    o->name = mp_obj_str_get_qstr(args[0]);
    o->value = mp_obj_int_get_uint_checked(args[1]);
    o->exported = false;
    return MP_OBJ_FROM_PTR(o);
}

STATIC void symbol_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    sym_obj_t *sym = (sym_obj_t*)MP_OBJ_TO_PTR(self_in);
    const char *s = qstr_str(sym->name);
    mp_printf(print, "Symbol(\"%s\", 0x%p, exported=%s)", s, sym->value,
        sym->exported ? "True": "False");
}

STATIC unsigned long convert_arg(mp_obj_t obj, size_t i) {
    mp_int_t n;

    if (mp_obj_is_int(obj)) { // covers small ints & ints
        return mp_obj_int_get_uint_checked(obj);
    } else if (mp_obj_get_int_maybe(obj, &n)) { // covers bool
        return n;
    } else if (mp_const_none == obj) {
        return 0;
    } else if (mp_obj_is_str_or_bytes(obj)) {
        return (unsigned long)mp_obj_str_get_str(obj);
    } else if (mp_obj_is_symbol(obj)) {
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
    return mp_obj_new_bytes((const byte*)get_ptr(obj), mp_obj_get_int(n));
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
        mp_int_t value = mp_obj_int_get_uint_checked(args[1]);
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


#define MAX_CB_NARGS 6 // max arguments passed in registers. can also handle stack arguments, but meh now.

STATIC size_t check_func_for_cb(mp_obj_t func) {
    if (!mp_obj_is_type(func, &mp_type_fun_bc)) {
        mp_raise_TypeError("expected Python function");
    }
    mp_obj_fun_bc_t *f = (mp_obj_fun_bc_t*)MP_OBJ_FROM_PTR(func);
    const byte *ip = f->bytecode;
    MP_BC_PRELUDE_SIG_DECODE(ip);

    if (n_kwonly_args != 0 || n_def_pos_args != 0) {
        mp_raise_TypeError("kwargs/defaults not supported");
    }

    if (n_pos_args > MAX_CB_NARGS) {
        mp_raise_TypeError("max args is " MP_STRINGIFY(MAX_CB_NARGS));
    }

    return n_pos_args;
}

typedef struct {
    mp_obj_base_t base;
    struct kprobe kp;
    mp_obj_t func;
    size_t nargs;
} kprobe_obj_t;

STATIC mp_obj_t kprobe_rm(mp_obj_t self_in) {
    kprobe_obj_t *self = MP_OBJ_TO_PTR(self_in);
    unregister_kprobe(&self->kp);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(kprobe_rm_obj, kprobe_rm);

STATIC mp_obj_t kprobe_del(mp_obj_t self_in) {
    // kprobe.rm might've been called already, but unregister_kprobe handles that.
    return kprobe_rm(self_in);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(kprobe_del_obj, kprobe_del);

STATIC const mp_rom_map_elem_t kprobe_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&kprobe_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_rm), MP_ROM_PTR(&kprobe_rm_obj) },
};

STATIC MP_DEFINE_CONST_DICT(kprobe_locals_dict, kprobe_locals_dict_table);

static const mp_obj_type_t kprobe_type = {
    { &mp_type_type },
    .name = MP_QSTR_kprobe,
    .locals_dict = (void*)&kprobe_locals_dict,
};

STATIC void mp_print_printk(void *data, const char *str, size_t len) {
    (void)data;
    static bool newline = true;

    printk(newline ? (KERN_WARNING "%*pE") : (KERN_CONT "%*pE"), (int)len, str);
    newline = str[len - 1] == '\n';
}

STATIC unsigned long call_py_func(mp_obj_t func, size_t nargs, bool *call_ok, bool *ret_none,
    unsigned long arg1, unsigned long arg2, unsigned long arg3,
    unsigned long arg4, unsigned long arg5, unsigned long arg6) {

    nlr_buf_t nlr;
    unsigned long ret = 0;
    mp_obj_t args[MAX_CB_NARGS];

    if (nlr_push(&nlr) == 0) {
        assert(nargs <= MP_ARRAY_SIZE(args));
        switch (nargs) {
        case 6: args[5] = mp_obj_new_int_from_uint(arg6);
        case 5: args[4] = mp_obj_new_int_from_uint(arg5);
        case 4: args[3] = mp_obj_new_int_from_uint(arg4);
        case 3: args[2] = mp_obj_new_int_from_uint(arg3);
        case 2: args[1] = mp_obj_new_int_from_uint(arg2);
        case 1: args[0] = mp_obj_new_int_from_uint(arg1);
        case 0: break;
        }

        mp_obj_t obj = mp_call_function_n_kw(func, nargs, 0, args);
        if (obj == mp_const_none) {
            *ret_none = true;
            // ret remains 0
        } else {
            *ret_none = false;
            ret = mp_obj_int_get_uint_checked(obj);
        }
        *call_ok = true;

        nlr_pop();
    } else {
        printk("mpy: exception in python callback\n");
        static const mp_print_t print = {NULL, mp_print_printk};
        mp_obj_print_exception(&print, MP_OBJ_FROM_PTR(nlr.ret_val));
        *call_ok = false;
    }

    return ret;
}

STATIC int kprobe_pre_handler(struct kprobe *p, struct pt_regs *regs) {
    kprobe_obj_t *kp_obj = container_of(p, kprobe_obj_t, kp);

    bool call_ok;
    bool ret_none;
    unsigned long ret = call_py_func(kp_obj->func, kp_obj->nargs, &call_ok, &ret_none,
        regs->di, regs->si, regs->dx, regs->cx, regs->r8, regs->r9);
    // if mp function call was a success, don't continue to the probed function.
    if (call_ok && !ret_none) {
        // set up registers for a return
        regs->ax = ret;
        regs->ip = ((unsigned long*)regs->sp)[0];
        regs->sp += 8;
        return 1;
    } else {
        // callback either raised an exception, or returned None. proceed to probed
        // function.
        return 0;
    }
}

STATIC void set_kprobe_target(struct kprobe *kp, mp_obj_t target) {
    if (mp_obj_is_int(target)) {
        kp->addr = (kprobe_opcode_t*)mp_obj_int_get_uint_checked(target);
    } else if (mp_obj_is_str(target)) {
        kp->symbol_name = mp_obj_str_get_str(target);
    } else if (mp_obj_is_symbol(target)) {
        kp->addr = (kprobe_opcode_t*)((sym_obj_t*)MP_OBJ_TO_PTR(target))->value;
    } else {
        mp_raise_TypeError("int/str/Symbol accepted for kprobe target");
    }
}

STATIC mp_obj_t kernel_ffi_kprobe(mp_obj_t target, mp_obj_t func) {
    size_t nargs = check_func_for_cb(func);

    kprobe_obj_t *kp_obj = m_new_obj_with_finaliser(kprobe_obj_t);
    kp_obj->base.type = &kprobe_type;
    kp_obj->func = func;
    kp_obj->nargs = nargs;

    memset(&kp_obj->kp, 0, sizeof(kp_obj->kp));
    kp_obj->kp.pre_handler = kprobe_pre_handler;
    set_kprobe_target(&kp_obj->kp, target);

    int ret = register_kprobe(&kp_obj->kp);
    if (ret) {
        mp_raise_OSError(-ret);
    }

    return MP_OBJ_TO_PTR(kp_obj);
}
MP_DEFINE_CONST_FUN_OBJ_2(kernel_ffi_kprobe_obj, kernel_ffi_kprobe);

typedef struct _mp_obj_fficallback_t {
    mp_obj_base_t base;
    mp_obj_t func;
    void *trampoline;
    unsigned int nargs;
} callback_obj_t;

STATIC mp_obj_t callback_ptr(mp_obj_t self_in) {
    callback_obj_t *self = self_in;
    return mp_obj_new_int_from_uint((mp_uint_t)self->trampoline);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(callback_ptr_obj, callback_ptr);

STATIC mp_obj_t callback_del(mp_obj_t self_in) {
    callback_obj_t *self = MP_OBJ_TO_PTR(self_in);
    vfree(self->trampoline);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(callback_del_obj, callback_del);

STATIC const mp_rom_map_elem_t callback_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_ptr), MP_ROM_PTR(&callback_ptr_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&callback_del_obj) },
};

STATIC MP_DEFINE_CONST_DICT(callback_locals_dict, callback_locals_dict_table);

STATIC const mp_obj_type_t callback_type = {
    { &mp_type_type },
    .name = MP_QSTR_callback,
    .locals_dict = (void*)&callback_locals_dict,
};

STATIC unsigned long callback_handler(unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4,
    unsigned long arg5, unsigned long arg6) {

    register unsigned long rax asm("rax");
    const callback_obj_t *cb_obj = (callback_obj_t*)rax;

    bool call_ok; // unused anyway, we have no fallback.
    bool ret_none;
    return call_py_func(cb_obj->func, cb_obj->nargs, &call_ok, &ret_none,
        arg1, arg2, arg3, arg4, arg5, arg6);
}

STATIC mp_obj_t kernel_ffi_callback(mp_obj_t func) {
    size_t nargs = check_func_for_cb(func);

    callback_obj_t *cb_obj = m_new_obj_with_finaliser(callback_obj_t);
    cb_obj->base.type = &callback_type;
    cb_obj->func = func;
    cb_obj->nargs = nargs;

    // this trampoline does the python equivalent of "partial(callback_handler, cb_obj)".
    // instead of a full wrapper that prepends the cb_obj argument, a hacky trick
    // is employed: rax is a volatile register but not used to pass any arguments.
    // a small trampoline that sets rax to the cb_obj then jumps to callback_handler
    // is created, and used as the callback pointer.
    struct {
        u8 movabs_rax_handler_addr[10];
        u8 push_rax[1];
        u8 movabs_rax_parameter[10];
        u8 ret[1];
    } __attribute__((packed)) *trampoline = __vmalloc(PAGE_SIZE, GFP_KERNEL, PAGE_KERNEL_EXEC);
    if (NULL == trampoline) {
        mp_raise_OSError(0);
    }

    static const u8 trampoline_base[] = {
        // movabs $callback_handler, %rax
        0x48, 0xb8, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // push %rax
        0x50,
        // movabs $cb_obj, %rax
        0x48, 0xb8, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
        // retq
        0xc3,
    };

    memcpy(trampoline, trampoline_base, sizeof(trampoline_base));

    // write the target pointer
    void **p = (void**)&trampoline->movabs_rax_handler_addr[2];
    *p = (void*)callback_handler;
    // write the parameter (callback object)
    p = (void**)&trampoline->movabs_rax_parameter[2];
    *p = cb_obj;

    cb_obj->trampoline = trampoline;

    return MP_OBJ_FROM_PTR(cb_obj);
}
MP_DEFINE_CONST_FUN_OBJ_1(kernel_ffi_callback_obj, kernel_ffi_callback);

STATIC const mp_rom_map_elem_t kernel_ffi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_kernel_ffi) },

    { MP_ROM_QSTR(MP_QSTR_Symbol), MP_ROM_PTR(&mp_type_symbol) },

    { MP_ROM_QSTR(MP_QSTR_str),    MP_ROM_PTR(&kernel_ffi_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_bytes),  MP_ROM_PTR(&kernel_ffi_bytes_obj) },

    { MP_ROM_QSTR(MP_QSTR_p8),     MP_ROM_PTR(&kernel_ffi_p8_obj) },
    { MP_ROM_QSTR(MP_QSTR_p16),    MP_ROM_PTR(&kernel_ffi_p16_obj) },
    { MP_ROM_QSTR(MP_QSTR_p32),    MP_ROM_PTR(&kernel_ffi_p32_obj) },
    { MP_ROM_QSTR(MP_QSTR_p64),    MP_ROM_PTR(&kernel_ffi_p64_obj) },

    { MP_ROM_QSTR(MP_QSTR_kprobe), MP_ROM_PTR(&kernel_ffi_kprobe_obj) },
    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&kernel_ffi_callback_obj) },
};

STATIC MP_DEFINE_CONST_DICT(kernel_ffi_module_globals, kernel_ffi_module_globals_table);

const mp_obj_module_t mp_module_kernel_ffi = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&kernel_ffi_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_kernel_ffi, mp_module_kernel_ffi, 1);
