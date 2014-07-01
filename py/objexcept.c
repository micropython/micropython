/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <stdarg.h>
#include <assert.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "objtuple.h"
#include "objtype.h"
#include "runtime.h"
#include "runtime0.h"
#include "gc.h"

typedef struct _mp_obj_exception_t {
    mp_obj_base_t base;
    mp_obj_t traceback; // a list object, holding (file,line,block) as numbers (not Python objects); a hack for now
    mp_obj_tuple_t *args;
} mp_obj_exception_t;

// Instance of MemoryError exception - needed by mp_malloc_fail
const mp_obj_exception_t mp_const_MemoryError_obj = {{&mp_type_MemoryError}, MP_OBJ_NULL, mp_const_empty_tuple};

// Local non-heap memory for allocating an exception when we run out of RAM
STATIC mp_obj_exception_t mp_emergency_exception_obj;

// Instance of GeneratorExit exception - needed by generator.close()
// This would belong to objgenerator.c, but to keep mp_obj_exception_t
// definition module-private so far, have it here.
const mp_obj_exception_t mp_const_GeneratorExit_obj = {{&mp_type_GeneratorExit}, MP_OBJ_NULL, mp_const_empty_tuple};

STATIC void mp_obj_exception_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_exception_t *o = o_in;
    mp_print_kind_t k = kind & ~PRINT_EXC_SUBCLASS;
    bool is_subclass = kind & PRINT_EXC_SUBCLASS;
    if (!is_subclass && (k == PRINT_REPR || k == PRINT_EXC)) {
        print(env, "%s", qstr_str(o->base.type->name));
    }

    if (k == PRINT_EXC) {
        print(env, ": ");
    }

    if (k == PRINT_STR || k == PRINT_EXC) {
        if (o->args == NULL || o->args->len == 0) {
            print(env, "");
            return;
        } else if (o->args->len == 1) {
            mp_obj_print_helper(print, env, o->args->items[0], PRINT_STR);
            return;
        }
    }
    mp_obj_tuple_print(print, env, o->args, kind);
}

mp_obj_t mp_obj_exception_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_obj_type_t *type = type_in;

    if (n_kw != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "%s does not take keyword arguments", mp_obj_get_type_str(type_in)));
    }

    mp_obj_exception_t *o = m_new_obj_var_maybe(mp_obj_exception_t, mp_obj_t, 0);
    if (o == NULL) {
        // Couldn't allocate heap memory; use local data instead.
        o = &mp_emergency_exception_obj;
        // We can't store any args.
        n_args = 0;
        o->args = mp_const_empty_tuple;
    } else {
        o->args = mp_obj_new_tuple(n_args, args);
    }
    o->base.type = type;
    o->traceback = MP_OBJ_NULL;
    return o;
}

// Get exception "value" - that is, first argument, or None
mp_obj_t mp_obj_exception_get_value(mp_obj_t self_in) {
    mp_obj_exception_t *self = self_in;
    if (self->args->len == 0) {
        return mp_const_none;
    } else {
        return self->args->items[0];
    }
}

STATIC void exception_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_exception_t *self = self_in;
    if (attr == MP_QSTR_args) {
        dest[0] = self->args;
    } else if (self->base.type == &mp_type_StopIteration && attr == MP_QSTR_value) {
        dest[0] = mp_obj_exception_get_value(self);
    }
}

STATIC mp_obj_t exc___init__(uint n_args, const mp_obj_t *args) {
    mp_obj_exception_t *self = args[0];
    mp_obj_t argst = mp_obj_new_tuple(n_args - 1, args + 1);
    self->args = argst;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(exc___init___obj, 1, MP_OBJ_FUN_ARGS_MAX, exc___init__);

STATIC const mp_map_elem_t exc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___init__), (mp_obj_t)&exc___init___obj },
};

STATIC MP_DEFINE_CONST_DICT(exc_locals_dict, exc_locals_dict_table);

const mp_obj_type_t mp_type_BaseException = {
    { &mp_type_type },
    .name = MP_QSTR_BaseException,
    .print = mp_obj_exception_print,
    .make_new = mp_obj_exception_make_new,
    .load_attr = exception_load_attr,
    .locals_dict = (mp_obj_t)&exc_locals_dict,
};

#define MP_DEFINE_EXCEPTION_BASE(base_name) \
STATIC const mp_obj_tuple_t mp_type_ ## base_name ## _base_tuple = {{&mp_type_tuple}, 1, {(mp_obj_t)&mp_type_ ## base_name}};\

#define MP_DEFINE_EXCEPTION(exc_name, base_name) \
const mp_obj_type_t mp_type_ ## exc_name = { \
    { &mp_type_type }, \
    .name = MP_QSTR_ ## exc_name, \
    .print = mp_obj_exception_print, \
    .make_new = mp_obj_exception_make_new, \
    .load_attr = exception_load_attr, \
    .bases_tuple = (mp_obj_t)&mp_type_ ## base_name ## _base_tuple, \
};

// List of all exceptions, arranged as in the table at:
// http://docs.python.org/3/library/exceptions.html
MP_DEFINE_EXCEPTION_BASE(BaseException)
MP_DEFINE_EXCEPTION(SystemExit, BaseException)
//MP_DEFINE_EXCEPTION(KeyboardInterrupt, BaseException)
MP_DEFINE_EXCEPTION(GeneratorExit, BaseException)
MP_DEFINE_EXCEPTION(Exception, BaseException)
  MP_DEFINE_EXCEPTION_BASE(Exception)
  MP_DEFINE_EXCEPTION(StopIteration, Exception)
  MP_DEFINE_EXCEPTION(ArithmeticError, Exception)
    MP_DEFINE_EXCEPTION_BASE(ArithmeticError)
    //MP_DEFINE_EXCEPTION(FloatingPointError, ArithmeticError)
    MP_DEFINE_EXCEPTION(OverflowError, ArithmeticError)
    MP_DEFINE_EXCEPTION(ZeroDivisionError, ArithmeticError)
  MP_DEFINE_EXCEPTION(AssertionError, Exception)
  MP_DEFINE_EXCEPTION(AttributeError, Exception)
  //MP_DEFINE_EXCEPTION(BufferError, Exception)
  //MP_DEFINE_EXCEPTION(EnvironmentError, Exception)
  MP_DEFINE_EXCEPTION(EOFError, Exception)
  MP_DEFINE_EXCEPTION(ImportError, Exception)
  MP_DEFINE_EXCEPTION(IOError, Exception)
  MP_DEFINE_EXCEPTION(LookupError, Exception)
    MP_DEFINE_EXCEPTION_BASE(LookupError)
    MP_DEFINE_EXCEPTION(IndexError, LookupError)
    MP_DEFINE_EXCEPTION(KeyError, LookupError)
  MP_DEFINE_EXCEPTION(MemoryError, Exception)
  MP_DEFINE_EXCEPTION(NameError, Exception)
    /*
    MP_DEFINE_EXCEPTION_BASE(NameError)
    MP_DEFINE_EXCEPTION(UnboundLocalError, NameError)
    */
  MP_DEFINE_EXCEPTION(OSError, Exception)
    /*
    MP_DEFINE_EXCEPTION_BASE(OSError)
    MP_DEFINE_EXCEPTION(BlockingIOError, OSError)
    MP_DEFINE_EXCEPTION(ChildProcessError, OSError)
    MP_DEFINE_EXCEPTION(ConnectionError, OSError)
      MP_DEFINE_EXCEPTION(BrokenPipeError, ConnectionError)
      MP_DEFINE_EXCEPTION(ConnectionAbortedError, ConnectionError)
      MP_DEFINE_EXCEPTION(ConnectionRefusedError, ConnectionError)
      MP_DEFINE_EXCEPTION(ConnectionResetError, ConnectionError)
    MP_DEFINE_EXCEPTION(InterruptedError, OSError)
    MP_DEFINE_EXCEPTION(IsADirectoryError, OSError)
    MP_DEFINE_EXCEPTION(NotADirectoryError, OSError)
    MP_DEFINE_EXCEPTION(PermissionError, OSError)
    MP_DEFINE_EXCEPTION(ProcessLookupError, OSError)
    MP_DEFINE_EXCEPTION(TimeoutError, OSError)
    MP_DEFINE_EXCEPTION(FileExistsError, OSError)
    MP_DEFINE_EXCEPTION(FileNotFoundError, OSError)
    MP_DEFINE_EXCEPTION(ReferenceError, Exception)
    */
  MP_DEFINE_EXCEPTION(RuntimeError, Exception)
    MP_DEFINE_EXCEPTION_BASE(RuntimeError)
    MP_DEFINE_EXCEPTION(NotImplementedError, RuntimeError)
  MP_DEFINE_EXCEPTION(SyntaxError, Exception)
    MP_DEFINE_EXCEPTION_BASE(SyntaxError)
    MP_DEFINE_EXCEPTION(IndentationError, SyntaxError)
    /*
      MP_DEFINE_EXCEPTION_BASE(IndentationError)
      MP_DEFINE_EXCEPTION(TabError, IndentationError)
      */
  MP_DEFINE_EXCEPTION(SystemError, Exception)
  MP_DEFINE_EXCEPTION(TypeError, Exception)
  MP_DEFINE_EXCEPTION(ValueError, Exception)
    //TODO: Implement UnicodeErrors which take arguments
  /*
  MP_DEFINE_EXCEPTION(Warning, Exception)
    MP_DEFINE_EXCEPTION_BASE(Warning)
    MP_DEFINE_EXCEPTION(DeprecationWarning, Warning)
    MP_DEFINE_EXCEPTION(PendingDeprecationWarning, Warning)
    MP_DEFINE_EXCEPTION(RuntimeWarning, Warning)
    MP_DEFINE_EXCEPTION(SyntaxWarning, Warning)
    MP_DEFINE_EXCEPTION(UserWarning, Warning)
    MP_DEFINE_EXCEPTION(FutureWarning, Warning)
    MP_DEFINE_EXCEPTION(ImportWarning, Warning)
    MP_DEFINE_EXCEPTION(UnicodeWarning, Warning)
    MP_DEFINE_EXCEPTION(BytesWarning, Warning)
    MP_DEFINE_EXCEPTION(ResourceWarning, Warning)
    */

mp_obj_t mp_obj_new_exception(const mp_obj_type_t *exc_type) {
    return mp_obj_new_exception_args(exc_type, 0, NULL);
}

// "Optimized" version for common(?) case of having 1 exception arg
mp_obj_t mp_obj_new_exception_arg1(const mp_obj_type_t *exc_type, mp_obj_t arg) {
    return mp_obj_new_exception_args(exc_type, 1, &arg);
}

mp_obj_t mp_obj_new_exception_args(const mp_obj_type_t *exc_type, uint n_args, const mp_obj_t *args) {
    assert(exc_type->make_new == mp_obj_exception_make_new);
    return exc_type->make_new((mp_obj_t)exc_type, n_args, 0, args);
}

mp_obj_t mp_obj_new_exception_msg(const mp_obj_type_t *exc_type, const char *msg) {
    return mp_obj_new_exception_msg_varg(exc_type, msg);
}

mp_obj_t mp_obj_new_exception_msg_varg(const mp_obj_type_t *exc_type, const char *fmt, ...) {
    // check that the given type is an exception type
    assert(exc_type->make_new == mp_obj_exception_make_new);

    // make exception object
    mp_obj_exception_t *o = m_new_obj_var_maybe(mp_obj_exception_t, mp_obj_t, 0);
    if (o == NULL) {
        // Couldn't allocate heap memory; use local data instead.
        // Unfortunately, we won't be able to format the string...
        o = &mp_emergency_exception_obj;
        o->base.type = exc_type;
        o->traceback = MP_OBJ_NULL;
        o->args = mp_const_empty_tuple;
    } else {
        o->base.type = exc_type;
        o->traceback = MP_OBJ_NULL;
        o->args = mp_obj_new_tuple(1, NULL);

        if (fmt == NULL) {
            // no message
            assert(0);
        } else {
            // render exception message and store as .args[0]
            // TODO: optimize bufferbloat
            vstr_t *vstr = vstr_new();
            va_list ap;
            va_start(ap, fmt);
            vstr_vprintf(vstr, fmt, ap);
            va_end(ap);
            o->args->items[0] = mp_obj_new_str(vstr->buf, vstr->len, false);
            vstr_free(vstr);
        }
    }

    return o;
}

// return true if the given object is an exception type
bool mp_obj_is_exception_type(mp_obj_t self_in) {
    if (MP_OBJ_IS_TYPE(self_in, &mp_type_type)) {
        // optimisation when self_in is a builtin exception
        mp_obj_type_t *self = self_in;
        if (self->make_new == mp_obj_exception_make_new) {
            return true;
        }
    }
    return mp_obj_is_subclass_fast(self_in, &mp_type_BaseException);
}

// return true if the given object is an instance of an exception type
bool mp_obj_is_exception_instance(mp_obj_t self_in) {
    return mp_obj_is_exception_type(mp_obj_get_type(self_in));
}

// return true if exception (type or instance) is a subclass of given
// exception type.
bool mp_obj_exception_match(mp_obj_t exc, const mp_obj_type_t *exc_type) {
    // TODO: move implementation from RT_BINARY_OP_EXCEPTION_MATCH here.
    return mp_binary_op(MP_BINARY_OP_EXCEPTION_MATCH, exc, (mp_obj_t)exc_type) == mp_const_true;
}

// traceback handling functions

#define GET_NATIVE_EXCEPTION(self, self_in) \
    /* make sure self_in is an exception instance */ \
    assert(mp_obj_is_exception_instance(self_in)); \
    mp_obj_exception_t *self; \
    if (mp_obj_is_native_exception_instance(self_in)) { \
        self = self_in; \
    } else { \
        self = ((mp_obj_instance_t*)self_in)->subobj[0]; \
    }

void mp_obj_exception_clear_traceback(mp_obj_t self_in) {
    GET_NATIVE_EXCEPTION(self, self_in);
    // just set the traceback to the null object
    // we don't want to call any memory management functions here
    self->traceback = MP_OBJ_NULL;
}

void mp_obj_exception_add_traceback(mp_obj_t self_in, qstr file, machine_uint_t line, qstr block) {
    #if MICROPY_ENABLE_GC
    if (gc_is_locked()) {
        // We can't allocate memory, so don't bother to try
        return;
    }
    #endif

    GET_NATIVE_EXCEPTION(self, self_in);

    // for traceback, we are just using the list object for convenience, it's not really a list of Python objects
    if (self->traceback == MP_OBJ_NULL) {
        self->traceback = mp_obj_new_list(0, NULL);
    }
    mp_obj_list_append(self->traceback, (mp_obj_t)(machine_uint_t)file);
    mp_obj_list_append(self->traceback, (mp_obj_t)(machine_uint_t)line);
    mp_obj_list_append(self->traceback, (mp_obj_t)(machine_uint_t)block);
}

void mp_obj_exception_get_traceback(mp_obj_t self_in, machine_uint_t *n, machine_uint_t **values) {
    GET_NATIVE_EXCEPTION(self, self_in);

    if (self->traceback == MP_OBJ_NULL) {
        *n = 0;
        *values = NULL;
    } else {
        uint n2;
        mp_obj_list_get(self->traceback, &n2, (mp_obj_t**)values);
        *n = n2;
    }
}
