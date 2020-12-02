/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, 2020 Yonatan Goldschmidt
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


#define pr_fmt(fmt) "MPY: ffi: " fmt

#include <linux/kallsyms.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/ftrace.h>

#include <py/runtime.h>
#include <py/qstr.h>
#include <py/obj.h>
#include <py/objstr.h>
#include <py/objfun.h>
#include <py/bc.h>
#include <py/stackctrl.h>

#include "internal.h"


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

STATIC mp_obj_t _symbol_make_new(const mp_obj_type_t *type, qstr name, unsigned long value, bool exported) {
    sym_obj_t *o = m_new_obj(sym_obj_t);
    o->base.type = type;
    o->name = name;
    o->value = value;
    o->exported = exported;
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t symbol_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, false);

    return _symbol_make_new(type, mp_obj_str_get_qstr(args[0]), mp_obj_int_get_uint_checked(args[1]), false);
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
#ifdef INCLUDE_STRUCT_LAYOUT
        // attempt to load the ____ptr field of StructPtr et al
        mp_obj_t dest[2];
        mp_load_method_maybe(obj, MP_QSTR_____ptr, dest);
        if (dest[0] != MP_OBJ_NULL) {
            return mp_obj_int_get_uint_checked(dest[0]);
        }
#endif

        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
            "argument %d is not int/bool/None/str/bytes/Symbol"
#ifdef INCLUDE_STRUCT_LAYOUT
            "/*Ptr"
#endif
            , (int)i));
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
    // special case of equality - if we reached here, then lhs_in != rhs_in (pointer wise).
    // we'll check their ->value.
    if (mp_obj_is_symbol(rhs_in) && op == MP_BINARY_OP_EQUAL) {
        return ((sym_obj_t*)MP_OBJ_TO_PTR(lhs_in))->value == ((sym_obj_t*)MP_OBJ_TO_PTR(rhs_in))->value ?
            mp_const_true : mp_const_false;
    }

    // otherwise, fall to generic op, and pass integral value of symbol.
    return mp_binary_op(op, mp_obj_new_int_from_uint(((sym_obj_t*)MP_OBJ_TO_PTR(lhs_in))->value), rhs_in);
}

STATIC sym_obj_t *make_new_sym(qstr name, unsigned long value, bool exported) {
    return _symbol_make_new(&mp_type_symbol, name, value, exported);
}

bool kernel_ffi_auto_globals_enabled = true;

STATIC mp_obj_t kernel_ffi_auto_globals(mp_obj_t enable) {
    kernel_ffi_auto_globals_enabled = mp_obj_is_true(enable);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(kernel_ffi_auto_globals_obj, kernel_ffi_auto_globals);

STATIC unsigned long __resolve_symbol(const char *name, bool *is_exported) {
    // TODO what about module symbols? is it legit? does it keep a reference to the owner?
    unsigned long value = (unsigned long)__symbol_get(name);

    if (value) {
        *is_exported = true;
        return value;
    }

    *is_exported = false;
    // symbol value, or 0 if not found.
    return kallsyms_lookup_name(name);
}

STATIC mp_obj_t resolve_symbol(mp_obj_t name) {
    bool is_exported;
    // mp_obj_str_get_str gives it null terminated.
    unsigned long value = __resolve_symbol(mp_obj_str_get_str(name), &is_exported);
    if (value) {
        return make_new_sym(mp_obj_str_get_qstr(name), value, is_exported);
    }

    return MP_OBJ_NULL;
}

STATIC mp_obj_t kernel_ffi_symbol(mp_obj_t name) {
    mp_obj_t sym = resolve_symbol(name);
    if (MP_OBJ_NULL == sym) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_NameError, "kernel symbol '%s' not found",
            mp_obj_str_get_str(name)));
    }

    return sym;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(kernel_ffi_symbol_obj, kernel_ffi_symbol);

mp_obj_t mp_lazy_load_global(qstr qst) {
    if (!kernel_ffi_auto_globals_enabled) {
        return MP_OBJ_NULL;
    }

    return resolve_symbol(MP_OBJ_NEW_QSTR(qst));
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

        const unsigned long cr0 = read_cr0();
        const unsigned long cr0_no_wp = cr0 & ~X86_CR0_WP;

        // just place these here instead of calling write_cr0, which tries to
        // re-enable WP (see 8dbec27a242cd3e2816eeb98d3237b9f57cf6232 in linux)
        // TODO: use paravirt ops when appropriate...

        asm volatile("mov %0,%%cr0": : "r" (cr0_no_wp));
        *ptr = (u64)value;
        asm volatile("mov %0,%%cr0": : "r" (cr0));

        local_irq_restore(flags);

        return mp_const_none;
    } else {
        return mp_obj_new_int_from_uint(*ptr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(kernel_ffi_p64_obj, 1, 2, kernel_ffi_p64);

// kmalloc is a common operation, so a nicer API with exceptions and no messing with GFP_*
// is required.
STATIC mp_obj_t kernel_ffi_kmalloc(mp_obj_t n) {
    mp_uint_t value = mp_obj_int_get_uint_checked(n);

    void *p = kmalloc(value, GFP_KERNEL);
    if (!p) {
        mp_raise_OSError(-ENOMEM);
    }

    return mp_obj_new_int_from_uint((mp_uint_t)p);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(kernel_ffi_kmalloc_obj, kernel_ffi_kmalloc);

STATIC mp_obj_t kernel_ffi_current(void) {
    return mp_obj_new_int_from_uint((mp_uint_t)current);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(kernel_ffi_current_obj, kernel_ffi_current);

#define MAX_CB_NARGS 10 // arbitrary, can increase this. works for stack arguments as well.

STATIC size_t check_func_for_cb(mp_obj_t func, bool has_extra_first) {
    if (!mp_obj_is_type(func, &mp_type_fun_bc)) {
        mp_raise_TypeError("expected Python function");
    }
    mp_obj_fun_bc_t *f = (mp_obj_fun_bc_t*)MP_OBJ_FROM_PTR(func);
    const byte *ip = f->bytecode;
    MP_BC_PRELUDE_SIG_DECODE(ip);

    if (n_kwonly_args != 0 || n_def_pos_args != 0) {
        mp_raise_TypeError("kwargs/defaults not supported");
    }

    if (n_pos_args > MAX_CB_NARGS + (has_extra_first ? 1 : 0)) {
        mp_raise_TypeError("max args is " MP_STRINGIFY(MAX_CB_NARGS));
    }

    return n_pos_args;
}

#ifdef CONFIG_KPROBES

enum kp_type {
    KP_TYPE_MIN = 0,
    KP_ARGS_MODIFY = 0,
    KP_ARGS_WATCH,
    KP_REGS_MODIFY,
    KP_REGS_WATCH,
    KP_TYPE_NUM,
};

typedef struct {
    mp_obj_base_t base;
    struct kprobe kp;
    mp_obj_t func;
    mp_obj_t call_sym;
    size_t nargs;
    enum kp_type type;
    bool disabled;
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

STATIC const mp_obj_type_t kprobe_type = {
    { &mp_type_type },
    .name = MP_QSTR_kprobe,
    .locals_dict = (void*)&kprobe_locals_dict,
};

STATIC unsigned long call_py_func_threaded(mp_obj_t func, size_t nargs, bool *call_ok, mp_obj_t first_arg,
    unsigned long arg1, unsigned long arg2, unsigned long arg3,
    unsigned long arg4, unsigned long arg5, unsigned long arg6,
    unsigned long arg7, unsigned long arg8, unsigned long arg9,
    unsigned long arg10) {

    nlr_buf_t nlr;
    unsigned long ret = 0;
    mp_obj_t args[1 + MAX_CB_NARGS];

    bool has_first = first_arg != MP_OBJ_NULL;
    if (has_first) {
        args[0] = first_arg;
    }

    if (nlr_push(&nlr) == 0) {
        assert(nargs <= MP_ARRAY_SIZE(args));
        switch (nargs) {
        case 10: args[10] = mp_obj_new_int_from_uint(arg10);
        case 9: args[9] = mp_obj_new_int_from_uint(arg9);
        case 8: args[8] = mp_obj_new_int_from_uint(arg8);
        case 7: args[7] = mp_obj_new_int_from_uint(arg7);
        case 6: args[6] = mp_obj_new_int_from_uint(arg6);
        case 5: args[5] = mp_obj_new_int_from_uint(arg5);
        case 4: args[4] = mp_obj_new_int_from_uint(arg4);
        case 3: args[3] = mp_obj_new_int_from_uint(arg3);
        case 2: args[2] = mp_obj_new_int_from_uint(arg2);
        case 1: args[1] = mp_obj_new_int_from_uint(arg1);
        case 0: break;
        }

        mp_obj_t obj = mp_call_function_n_kw(func, nargs + (has_first ? 1 : 0), 0, &args[has_first ? 0 : 1]);
        if (obj == mp_const_none) {
            // ret remains 0
        } else {
            ret = mp_obj_int_get_uint_checked(obj);
        }
        *call_ok = true;

        nlr_pop();
    } else {
        assert(mp_obj_is_exception_instance(MP_OBJ_FROM_PTR(nlr.ret_val)));
        mp_obj_exception_t *exc = (mp_obj_exception_t*)nlr.ret_val;
        if (exc->args && exc->args->len == 1 &&
            // see mp_raise_recursion_depth
            exc->args->items[0] == MP_OBJ_NEW_QSTR(MP_QSTR_maximum_space_recursion_space_depth_space_exceeded)) {

            pr_err("stack exceeded when calling python code\n");
        } else {
            pr_err("exception in python callback\n");

            static const mp_print_t print = {NULL, mp_print_printk};
            if (nlr_push(&nlr) == 0) {
                // this nice fella can itself raise exceptions, so wrap it.
                mp_obj_print_exception(&print, MP_OBJ_FROM_PTR(exc));
                nlr_pop();
            } else {
                pr_err("double exception while printing previous exception\n");
            }
        }

        *call_ok = false;
    }

    return ret;
}

// wraps call_py_threaded in thread code.
// this sets the "top of the stack" for the new python "thread" so it's easier to have it separate
// from rest of the code.
STATIC unsigned long call_py_func(mp_obj_t func, size_t nargs, bool *call_ok, mp_obj_t first_arg,
    unsigned long arg1, unsigned long arg2, unsigned long arg3,
    unsigned long arg4, unsigned long arg5, unsigned long arg6,
    unsigned long arg7, unsigned long arg8, unsigned long arg9,
    unsigned long arg10) {

#if MICROPY_PY_THREAD
    bool created = false;

    // this needs to remain on my stack, so can't move to a separate function.
    mp_state_thread_t ts;
    if (NULL == __get_thread_for_current()) {
        // temporarily, add current context as a thread.
        if (!register_new_context(&ts)) {
            pr_err("failed to register thread context, skipping call\n");
            return 0;
        }

        mp_stack_set_top(&ts); // need to include ts in root-pointer scan (for locals dict)
        set_stack_limit();

        // empty locals
        mp_locals_set(mp_obj_new_dict(0));
        // use globals from main context
        mp_globals_set(mp_state_ctx.thread.dict_globals);

        created = true;
    }
#endif

    unsigned long ret = call_py_func_threaded(func, nargs, call_ok, first_arg, arg1, arg2, arg3, arg4, arg5,
        arg6, arg7, arg8, arg9, arg10);

#if MICROPY_PY_THREAD
    if (created) {
        // remove "thread"
        mp_thread_finish();
    }
#endif

    return ret;
}

STATIC void kprobe_empty_post_handler(struct kprobe *p, struct pt_regs *regs, unsigned long flags) { }

STATIC unsigned long get_stack_arg(unsigned long sp, int i) {
    assert(i >= 7);
    return ((unsigned long*)sp)[i - 6];
}

STATIC int kprobe_pre_handler(struct kprobe *p, struct pt_regs *regs) {
    kprobe_obj_t *kp_obj = container_of(p, kprobe_obj_t, kp);

    if (kp_obj->disabled) {
        return 0;
    }

    bool call_ok;
    unsigned long ret;
    switch (kp_obj->type) {
    case KP_ARGS_MODIFY:
    case KP_ARGS_WATCH:
        ret = call_py_func(kp_obj->func, kp_obj->nargs, &call_ok, kp_obj->call_sym,
            regs->di, regs->si, regs->dx, regs->cx, regs->r8, regs->r9,
            get_stack_arg(regs->sp, 7), get_stack_arg(regs->sp, 8), get_stack_arg(regs->sp, 9),
            get_stack_arg(regs->sp, 10));
        break;

    case KP_REGS_MODIFY:
    case KP_REGS_WATCH:
        ret = call_py_func(kp_obj->func, kp_obj->nargs, &call_ok, kp_obj->call_sym, (unsigned long)regs,
            0, 0, 0, 0, 0, 0, 0, 0, 0);
        break;

    default:
        MP_UNREACHABLE;
    }

    if (!call_ok) {
        // disable this kprobe! w/o calling disable_kprobe because it uses mutexes, which
        // may be unavailable in current context.
        kp_obj->disabled = true;
        return 0; // proceed to probed.
    }

    switch (kp_obj->type) {
    case KP_ARGS_MODIFY:
        regs->ax = ret;
        regs->ip = ((unsigned long*)regs->sp)[0];
        regs->sp += 8;
        /* fall through */
    case KP_REGS_MODIFY:
        return 1; // don't continue

    case KP_ARGS_WATCH:
    case KP_REGS_WATCH:
        // always proceed to probed on WATCH kprobes function.
        return 0;

    default:
        MP_UNREACHABLE;
    }
}

STATIC unsigned long resolve_target(mp_obj_t target) {
    unsigned long addr;

    if (mp_obj_is_int(target)) {
        addr = mp_obj_int_get_uint_checked(target);
    } else if (mp_obj_is_str(target)) {
        bool is_exported;
        addr = __resolve_symbol(mp_obj_str_get_str(target), &is_exported);
        if (0 == addr) {
            mp_raise_ValueError("target symbol not found");
        }
        addr = addr;
    } else if (mp_obj_is_symbol(target)) {
        addr = ((sym_obj_t*)MP_OBJ_TO_PTR(target))->value;
    } else {
        mp_raise_TypeError("int/str/Symbol required for target");
    }

    return addr;
}

STATIC mp_obj_t kernel_ffi_kprobe(mp_obj_t target, mp_obj_t _type, mp_obj_t func) {
    enum kp_type type = mp_obj_get_int(_type);
    if (type < KP_TYPE_MIN || type >= KP_TYPE_NUM) {
        mp_raise_ValueError("bad kprobe type, accepted values are KP_*");
    }

    size_t nargs = check_func_for_cb(func, type == KP_ARGS_MODIFY);

    const unsigned long addr = resolve_target(target);

    kprobe_post_handler_t post_handler;
    mp_obj_t call_sym;

    switch (type) {
    case KP_ARGS_MODIFY:
        if (nargs < 1) {
            mp_raise_ValueError("func should expect at least 1 argument (call_sym)");
        }
        nargs -= 1;
        post_handler = kprobe_empty_post_handler;
        call_sym = make_new_sym(MP_QSTR_, addr, false);
        break;

    case KP_ARGS_WATCH:
        post_handler = NULL;
        call_sym = MP_OBJ_NULL;
        break;

    case KP_REGS_MODIFY:
    case KP_REGS_WATCH:
        if (nargs != 1) {
            mp_raise_ValueError("func should expect exactly 1 argument (pt_regs)");
        }
        post_handler = type == KP_REGS_MODIFY ? kprobe_empty_post_handler : NULL;
        call_sym = MP_OBJ_NULL;
        break;

    default:
        MP_UNREACHABLE;
    }

    kprobe_obj_t *kp_obj = m_new_obj_with_finaliser(kprobe_obj_t);
    kp_obj->base.type = &kprobe_type;
    kp_obj->func = func;
    kp_obj->nargs = nargs;
    kp_obj->type = type;
    kp_obj->call_sym = call_sym;

    memset(&kp_obj->kp, 0, sizeof(kp_obj->kp));
    kp_obj->kp.pre_handler = kprobe_pre_handler;
    kp_obj->kp.post_handler = post_handler;
    kp_obj->kp.addr = (kprobe_opcode_t*)addr;

    int ret = register_kprobe(&kp_obj->kp);
    if (ret) {
        mp_raise_OSError(-ret);
    }

    return MP_OBJ_TO_PTR(kp_obj);
}
MP_DEFINE_CONST_FUN_OBJ_3(kernel_ffi_kprobe_obj, kernel_ffi_kprobe);

#endif // CONFIG_KPROBES

STATIC unsigned long callback_handler(unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4,
    unsigned long arg5, unsigned long arg6, unsigned long arg7,
    unsigned long arg8, unsigned long arg9, unsigned long arg10);

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

STATIC mp_obj_t kernel_ffi_callback(mp_obj_t func) {
    size_t nargs = check_func_for_cb(func, false);

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


#ifdef CONFIG_FUNCTION_TRACER

typedef struct {
    mp_obj_base_t base;
    // unlike what the docs of register_ftrace_function says, this can be allocated dynamically
    // and freed later. ftrace code checks this struct's locations via core_kernel_data(), and if
    // not core kernel then it is marked with FTRACE_OPS_FL_DYNAMIC and special care is taken
    // to ensure it's okay to free this struct once unregister_ftrace_function has returned.
    struct ftrace_ops ops;
    mp_obj_t func;
    size_t nargs;
    sym_obj_t *orig;
    bool removed;
    bool disabled;
} ftrace_obj_t;

STATIC mp_obj_t ftrace_rm(mp_obj_t self_in) {
    ftrace_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->removed) {
        // simple synchronization, but it's okay since the GC operation is locked.
        // and if one calls us twice from 2 contexts... well...
        self->removed = true;

        int ret = unregister_ftrace_function(&self->ops);
        if (ret) {
            pr_warn("unregister_ftrace_function: %d\n", ret);
        }

        ret = ftrace_set_filter_ip(&self->ops, self->orig->value - MCOUNT_INSN_SIZE, 1, 0);
        if (ret) {
            pr_warn("ftrace_set_filter_ip remove: %d\n", ret);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ftrace_rm_obj, ftrace_rm);

STATIC mp_obj_t ftrace_del(mp_obj_t self_in) {
    return ftrace_rm(self_in);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ftrace_del_obj, ftrace_del);

STATIC const mp_rom_map_elem_t ftrace_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&ftrace_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_rm), MP_ROM_PTR(&ftrace_rm_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ftrace_locals_dict, ftrace_locals_dict_table);

STATIC const mp_obj_type_t ftrace_type = {
    { &mp_type_type },
    .name = MP_QSTR_ftrace,
    .locals_dict = (void*)&ftrace_locals_dict,
};

static void notrace ftrace_trampoline(unsigned long ip, unsigned long parent_ip,
    struct ftrace_ops *ops, struct pt_regs *regs)
{
    ftrace_obj_t *ft_obj = container_of(ops, ftrace_obj_t, ops);

    if (!ft_obj->disabled) {
        regs->ip = (unsigned long)callback_handler;
        regs->ax = (unsigned long)ft_obj;
    }
}

STATIC mp_obj_t kernel_ffi_ftrace(mp_obj_t target, mp_obj_t func) {
    size_t nargs = check_func_for_cb(func, true);
    if (nargs < 1) {
        mp_raise_ValueError("func should expect at least 1 argument (call_sym)");
    }
    nargs -= 1;

    const unsigned long addr = resolve_target(target);

    ftrace_obj_t *ft_obj = m_new_obj_with_finaliser(ftrace_obj_t);
    ft_obj->base.type = &ftrace_type;

    ft_obj->ops.func = ftrace_trampoline;
    ft_obj->ops.flags = FTRACE_OPS_FL_SAVE_REGS | FTRACE_OPS_FL_RECURSION_SAFE | FTRACE_OPS_FL_IPMODIFY;

    ft_obj->func = func;
    ft_obj->nargs = nargs;
    ft_obj->orig = make_new_sym(MP_QSTR_, addr + MCOUNT_INSN_SIZE, false);
    ft_obj->disabled = false;

    int err = ftrace_set_filter_ip(&ft_obj->ops, addr, 0, 0);
    if (err) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "ftrace_set_filter_ip add: %d", err));
    }

    ft_obj->removed = false;

    err = register_ftrace_function(&ft_obj->ops);
    if (err) {
        ftrace_set_filter_ip(&ft_obj->ops, addr, 1, 0);
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "register_ftrace_function: %d", err));
    }

    return MP_OBJ_FROM_PTR(ft_obj);
}
MP_DEFINE_CONST_FUN_OBJ_2(kernel_ffi_ftrace_obj, kernel_ffi_ftrace);

#endif // CONFIG_FUNCTION_TRACER


STATIC unsigned long callback_handler(unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4,
    unsigned long arg5, unsigned long arg6, unsigned long arg7,
    unsigned long arg8, unsigned long arg9, unsigned long arg10) {

    register mp_obj_base_t *base asm("rax");

    mp_obj_t func;
    size_t nargs;
    mp_obj_t first_arg;
    if (base->type == &callback_type) {
        const callback_obj_t *cb_obj = container_of(base, callback_obj_t, base);

        func = cb_obj->func;
        nargs = cb_obj->nargs;
        first_arg = MP_OBJ_NULL;
    }
#ifdef CONFIG_FUNCTION_TRACER
    else if (base->type == &ftrace_type) {
        const ftrace_obj_t *ft_obj = container_of(base, ftrace_obj_t, base);

        func = ft_obj->func;
        nargs = ft_obj->nargs;
        first_arg = ft_obj->orig;
    }
#endif
    else {
        MP_UNREACHABLE;
    }

    bool call_ok;
    unsigned long ret = call_py_func(func, nargs, &call_ok, first_arg,
        arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

    if (call_ok) {
        return ret;
    }

    if (0) {
#ifdef CONFIG_FUNCTION_TRACER
    } else if (base->type == &ftrace_type) {
        ftrace_obj_t *ft_obj = container_of(base, ftrace_obj_t, base);

        ft_obj->disabled = true;
        // call the original
#define ul unsigned long
        return ((ul(*)(ul, ul, ul, ul, ul, ul, ul, ul, ul, ul))ft_obj->orig->value)(
            arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
#undef ul

#endif // CONFIG_FUNCTION_TRACER
    } else {
        assert(base->type == &callback_type);
        // we have no fallback.
        return 0;
    }
}


STATIC const mp_rom_map_elem_t kernel_ffi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_kernel_ffi) },

    { MP_ROM_QSTR(MP_QSTR_auto_globals), MP_ROM_PTR(&kernel_ffi_auto_globals_obj) },
    { MP_ROM_QSTR(MP_QSTR_symbol), MP_ROM_PTR(&kernel_ffi_symbol_obj) },

    { MP_ROM_QSTR(MP_QSTR_Symbol), MP_ROM_PTR(&mp_type_symbol) },

    { MP_ROM_QSTR(MP_QSTR_str),    MP_ROM_PTR(&kernel_ffi_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_bytes),  MP_ROM_PTR(&kernel_ffi_bytes_obj) },

    { MP_ROM_QSTR(MP_QSTR_p8),     MP_ROM_PTR(&kernel_ffi_p8_obj) },
    { MP_ROM_QSTR(MP_QSTR_p16),    MP_ROM_PTR(&kernel_ffi_p16_obj) },
    { MP_ROM_QSTR(MP_QSTR_p32),    MP_ROM_PTR(&kernel_ffi_p32_obj) },
    { MP_ROM_QSTR(MP_QSTR_p64),    MP_ROM_PTR(&kernel_ffi_p64_obj) },

    { MP_ROM_QSTR(MP_QSTR_kmalloc), MP_ROM_PTR(&kernel_ffi_kmalloc_obj) },
    { MP_ROM_QSTR(MP_QSTR_current), MP_ROM_PTR(&kernel_ffi_current_obj) },

#ifdef CONFIG_KPROBES
    { MP_ROM_QSTR(MP_QSTR_kprobe), MP_ROM_PTR(&kernel_ffi_kprobe_obj) },
    { MP_ROM_QSTR(MP_QSTR_KP_ARGS_MODIFY), MP_OBJ_NEW_SMALL_INT(KP_ARGS_MODIFY) },
    { MP_ROM_QSTR(MP_QSTR_KP_ARGS_WATCH),  MP_OBJ_NEW_SMALL_INT(KP_ARGS_WATCH) },
    { MP_ROM_QSTR(MP_QSTR_KP_REGS_MODIFY), MP_OBJ_NEW_SMALL_INT(KP_REGS_MODIFY) },
    { MP_ROM_QSTR(MP_QSTR_KP_REGS_WATCH),  MP_OBJ_NEW_SMALL_INT(KP_REGS_WATCH) },
#endif

    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&kernel_ffi_callback_obj) },

#ifdef CONFIG_FUNCTION_TRACER
    { MP_ROM_QSTR(MP_QSTR_ftrace), MP_ROM_PTR(&kernel_ffi_ftrace_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(kernel_ffi_module_globals, kernel_ffi_module_globals_table);

const mp_obj_module_t mp_module_kernel_ffi = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&kernel_ffi_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_kernel_ffi, mp_module_kernel_ffi, 1);
