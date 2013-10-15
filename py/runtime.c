#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpyconfig.h"
#include "runtime.h"
#include "bc.h"

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define WRITE_NATIVE (1)
#define DEBUG_printf(args...) printf(args)
#define DEBUG_OP_printf(args...) printf(args)
#else // don't print debugging info
#define DEBUG_printf(args...) (void)0
#define DEBUG_OP_printf(args...) (void)0
#endif

typedef machine_int_t py_small_int_t;

#define IS_O(o, k) (((((py_small_int_t)(o)) & 1) == 0) && (((py_obj_base_t*)(o))->kind == (k)))
#define IS_SMALL_INT(o) (((py_small_int_t)(o)) & 1)
#define FROM_SMALL_INT(o) (((py_small_int_t)(o)) >> 1)
#define TO_SMALL_INT(o) ((py_obj_t)(((o) << 1) | 1))

#if MICROPY_ENABLE_FLOAT
typedef machine_float_t float_t;
#endif

typedef enum {
    O_CONST,
    O_STR,
#if MICROPY_ENABLE_FLOAT
    O_FLOAT,
#endif
    O_EXCEPTION_0,
    O_EXCEPTION_2,
    O_RANGE,
    O_RANGE_IT,
    O_FUN_0,
    O_FUN_1,
    O_FUN_2,
    O_FUN_N,
    O_FUN_BC,
    O_FUN_ASM,
    O_BOUND_METH,
    O_LIST,
    O_SET,
    O_MAP,
    O_CLASS,
    O_OBJ,
} py_obj_kind_t;

typedef enum {
    MAP_QSTR,
    MAP_PY_OBJ,
} py_map_kind_t;

typedef struct _py_map_elem_t {
    py_obj_t key;
    py_obj_t value;
} py_map_elem_t;

typedef struct _py_map_t {
    py_map_kind_t kind; // TODO merge this 1-bit field into alloc or used
    machine_uint_t alloc;
    machine_uint_t used;
    py_map_elem_t *table;
} py_map_t;

typedef struct _py_obj_base_t py_obj_base_t;

struct _py_obj_base_t {
    py_obj_kind_t kind;
    union {
        const char *id;
        qstr u_str;
#if MICROPY_ENABLE_FLOAT
        float_t u_flt;
#endif
        struct { // for O_EXCEPTION_0
            qstr id;
        } u_exc0;
        struct { // for O_EXCEPTION_2
            // TODO reduce size or make generic object or something
            qstr id;
            const char *fmt;
            const char *s1;
            const char *s2;
        } u_exc2;
        struct { // for O_RANGE
            // TODO make generic object or something
            machine_int_t start;
            machine_int_t stop;
            machine_int_t step;
        } u_range;
        struct { // for O_RANGE_IT
            // TODO make generic object or something
            machine_int_t cur;
            machine_int_t stop;
            machine_int_t step;
        } u_range_it;
        struct { // for O_FUN_[012N]
            int n_args;
            void *fun;
        } u_fun;
        struct { // for O_FUN_BC
            int n_args;
            byte *code;
            uint len;
        } u_fun_bc;
        struct { // for O_FUN_ASM
            int n_args;
            void *fun;
        } u_fun_asm;
        struct { // for O_BOUND_METH
            py_obj_t meth;
            py_obj_t self;
        } u_bound_meth;
        struct { // for O_LIST
            int alloc;
            int len;
            py_obj_t *items;
        } u_list;
        struct { // for O_SET
            int alloc;
            int used;
            py_obj_t *table;
        } u_set;
        py_map_t u_map; // for O_MAP
        struct { // for O_CLASS
            py_map_t *locals;
        } u_class;
        struct { // for O_OBJ
            py_obj_base_t *class; // points to a O_CLASS object
            py_map_t *members;
        } u_obj;
    };
};

py_obj_t py_const_none;
py_obj_t py_const_false;
py_obj_t py_const_true;
py_obj_t py_const_stop_iteration;

// locals and globals need to be pointers because they can be the same in outer module scope
py_map_t *map_locals;
py_map_t *map_globals;
py_map_t map_builtins;

// approximatelly doubling primes; made with Mathematica command: Table[Prime[Floor[(1.7)^n]], {n, 3, 24}]
static int doubling_primes[] = {7, 19, 43, 89, 179, 347, 647, 1229, 2297, 4243, 7829, 14347, 26017, 47149, 84947, 152443, 273253, 488399, 869927, 1547173, 2745121, 4861607};

int get_doubling_prime_greater_or_equal_to(int x) {
    for (int i = 0; i < sizeof(doubling_primes) / sizeof(int); i++) {
        if (doubling_primes[i] >= x) {
            return doubling_primes[i];
        }
    }
    // ran out of primes in the table!
    // return something sensible, at least make it odd
    return x | 1;
}

void py_map_init(py_map_t *map, py_map_kind_t kind, int n) {
    map->kind = kind;
    map->alloc = get_doubling_prime_greater_or_equal_to(n + 1);
    map->used = 0;
    map->table = m_new(py_map_elem_t, map->alloc);
    for (int i = 0; i < map->alloc; i++) {
        map->table[i].key = NULL;
        map->table[i].value = NULL;
    }
}

py_map_t *py_map_new(py_map_kind_t kind, int n) {
    py_map_t *map = m_new(py_map_t, 1);
    py_map_init(map, kind, n);
    return map;
}

int py_obj_hash(py_obj_t o_in) {
    if (IS_SMALL_INT(o_in)) {
        return FROM_SMALL_INT(o_in);
    } else if (IS_O(o_in, O_STR)) {
        return ((py_obj_base_t*)o_in)->u_str;
    } else {
        assert(0);
        return 0;
    }
}

bool py_obj_equal(py_obj_t o1, py_obj_t o2) {
    if (o1 == o2) {
        return true;
    } else if (IS_SMALL_INT(o1) && IS_SMALL_INT(o2)) {
        return false;
    } else if (IS_O(o1, O_STR) && IS_O(o2, O_STR)) {
        return ((py_obj_base_t*)o1)->u_str == ((py_obj_base_t*)o2)->u_str;
    } else {
        assert(0);
        return false;
    }
}

py_map_elem_t* py_map_lookup_helper(py_map_t *map, py_obj_t index, bool add_if_not_found) {
    bool is_map_py_obj = (map->kind == MAP_PY_OBJ);
    machine_uint_t hash;
    if (is_map_py_obj) {
        hash = py_obj_hash(index);
    } else {
        hash = (machine_uint_t)index;
    }
    uint pos = hash % map->alloc;
    for (;;) {
        py_map_elem_t *elem = &map->table[pos];
        if (elem->key == NULL) {
            // not in table
            if (add_if_not_found) {
                if (map->used + 1 >= map->alloc) {
                    // not enough room in table, rehash it
                    int old_alloc = map->alloc;
                    py_map_elem_t *old_table = map->table;
                    map->alloc = get_doubling_prime_greater_or_equal_to(map->alloc + 1);
                    map->used = 0;
                    map->table = m_new(py_map_elem_t, map->alloc);
                    for (int i = 0; i < old_alloc; i++) {
                        if (old_table[i].key != NULL) {
                            py_map_lookup_helper(map, old_table[i].key, true)->value = old_table[i].value;
                        }
                    }
                    m_free(old_table);
                    // restart the search for the new element
                    pos = hash % map->alloc;
                } else {
                    map->used += 1;
                    elem->key = index;
                    return elem;
                }
            } else {
                return NULL;
            }
        } else if (elem->key == index || (is_map_py_obj && py_obj_equal(elem->key, index))) {
            // found it
            if (add_if_not_found) {
                elem->key = index;
            }
            return elem;
        } else {
            // not yet found, keep searching in this table
            pos = (pos + 1) % map->alloc;
        }
    }
}

py_map_elem_t* py_qstr_map_lookup(py_map_t *map, qstr index, bool add_if_not_found) {
    py_obj_t o = (py_obj_t)(machine_uint_t)index;
    return py_map_lookup_helper(map, o, add_if_not_found);
}

py_map_elem_t* py_map_lookup(py_obj_t o, py_obj_t index, bool add_if_not_found) {
    assert(IS_O(o, O_MAP));
    return py_map_lookup_helper(&((py_obj_base_t *)o)->u_map, index, add_if_not_found);
}

static bool fit_small_int(py_small_int_t o) {
    return true;
}

py_obj_t py_obj_new_const(const char *id) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_CONST;
    o->id = id;
    return (py_obj_t)o;
}

py_obj_t py_obj_new_str(qstr qstr) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_STR;
    o->u_str = qstr;
    return (py_obj_t)o;
}

#if MICROPY_ENABLE_FLOAT
py_obj_t py_obj_new_float(float_t val) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_FLOAT;
    o->u_flt = val;
    return (py_obj_t)o;
}
#endif

py_obj_t py_obj_new_exception_0(qstr id) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_EXCEPTION_0;
    o->u_exc0.id = id;
    return (py_obj_t)o;
}

py_obj_t py_obj_new_exception_2(qstr id, const char *fmt, const char *s1, const char *s2) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_EXCEPTION_2;
    o->u_exc2.id = id;
    o->u_exc2.fmt = fmt;
    o->u_exc2.s1 = s1;
    o->u_exc2.s2 = s2;
    return (py_obj_t)o;
}

// range is a class and instances are immutable sequence objects
py_obj_t py_obj_new_range(int start, int stop, int step) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_RANGE;
    o->u_range.start = start;
    o->u_range.stop = stop;
    o->u_range.step = step;
    return o;
}

py_obj_t py_obj_new_range_iterator(int cur, int stop, int step) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_RANGE_IT;
    o->u_range_it.cur = cur;
    o->u_range_it.stop = stop;
    o->u_range_it.step = step;
    return o;
}

py_obj_t list_append(py_obj_t self_in, py_obj_t arg) {
    assert(IS_O(self_in, O_LIST));
    py_obj_base_t *self = self_in;
    if (self->u_list.len >= self->u_list.alloc) {
        self->u_list.alloc *= 2;
        self->u_list.items = m_renew(py_obj_t, self->u_list.items, self->u_list.alloc);
    }
    self->u_list.items[self->u_list.len++] = arg;
    return arg;
}

static qstr q_append;
static qstr q_print;
static qstr q_len;
static qstr q___build_class__;
static qstr q_AttributeError;
static qstr q_NameError;
static qstr q_TypeError;

typedef enum {
    PY_CODE_NONE,
    PY_CODE_BYTE,
    PY_CODE_NATIVE,
    PY_CODE_INLINE_ASM,
} py_code_kind_t;

typedef struct _py_code_t {
    py_code_kind_t kind;
    int n_args;
    union {
        struct {
            byte *code;
            uint len;
        } u_byte;
        struct {
            py_fun_t fun;
        } u_native;
        struct {
            void *fun;
        } u_inline_asm;
    };
} py_code_t;

static int next_unique_code_id;
static py_code_t *unique_codes;

py_obj_t fun_list_append;

py_obj_t py_builtin_print(py_obj_t o) {
    if (IS_O(o, O_STR)) {
        // special case, print string raw
        printf("%s\n", qstr_str(((py_obj_base_t*)o)->u_str));
    } else {
        // print the object Python style
        py_obj_print(o);
        printf("\n");
    }
    return py_const_none;
}

py_obj_t py_builtin_len(py_obj_t o_in) {
    py_small_int_t len = 0;
    if (IS_O(o_in, O_LIST)) {
        py_obj_base_t *o = o_in;
        len = o->u_list.len;
    } else if (IS_O(o_in, O_MAP)) {
        py_obj_base_t *o = o_in;
        len = o->u_map.used;
    } else {
        assert(0);
    }
    return TO_SMALL_INT(len);
}

py_obj_t py_builtin___build_class__(py_obj_t o_class_fun, py_obj_t o_class_name) {
    // we differ from CPython: we set the new __locals__ object here
    py_map_t *old_locals = map_locals;
    py_map_t *class_locals = py_map_new(MAP_QSTR, 0);
    map_locals = class_locals;

    // call the class code
    rt_call_function_1(o_class_fun, (py_obj_t)0xdeadbeef);

    // restore old __locals__ object
    map_locals = old_locals;

    // create and return the new class
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_CLASS;
    o->u_class.locals = class_locals;
    return o;
}

py_obj_t py_builtin_range(py_obj_t o_arg) {
    return py_obj_new_range(0, rt_get_int(o_arg), 1);
}

#ifdef WRITE_NATIVE
FILE *fp_native = NULL;
#endif

void rt_init() {
    q_append = qstr_from_str_static("append");
    q_print = qstr_from_str_static("print");
    q_len = qstr_from_str_static("len");
    q___build_class__ = qstr_from_str_static("__build_class__");
    q_AttributeError = qstr_from_str_static("AttributeError");
    q_NameError = qstr_from_str_static("NameError");
    q_TypeError = qstr_from_str_static("TypeError");

    py_const_none = py_obj_new_const("None");
    py_const_false = py_obj_new_const("False");
    py_const_true = py_obj_new_const("True");
    py_const_stop_iteration = py_obj_new_const("StopIteration");

    // locals = globals for outer module (see Objects/frameobject.c/PyFrame_New())
    map_locals = map_globals = py_map_new(MAP_QSTR, 1);
    py_qstr_map_lookup(map_globals, qstr_from_str_static("__name__"), true)->value = py_obj_new_str(qstr_from_str_static("__main__"));

    py_map_init(&map_builtins, MAP_QSTR, 3);
    py_qstr_map_lookup(&map_builtins, q_print, true)->value = rt_make_function_1(py_builtin_print);
    py_qstr_map_lookup(&map_builtins, q_len, true)->value = rt_make_function_1(py_builtin_len);
    py_qstr_map_lookup(&map_builtins, q___build_class__, true)->value = rt_make_function_2(py_builtin___build_class__);
    py_qstr_map_lookup(&map_builtins, qstr_from_str_static("range"), true)->value = rt_make_function_1(py_builtin_range);

    next_unique_code_id = 1;
    unique_codes = NULL;

    fun_list_append = rt_make_function_2(list_append);

#ifdef WRITE_NATIVE
    fp_native = fopen("out-native", "wb");
#endif
}

void rt_deinit() {
#ifdef WRITE_NATIVE
    if (fp_native != NULL) {
        fclose(fp_native);
    }
#endif
}

int rt_get_new_unique_code_id() {
    return next_unique_code_id++;
}

static void alloc_unique_codes() {
    if (unique_codes == NULL) {
        unique_codes = m_new(py_code_t, next_unique_code_id);
        for (int i = 0; i < next_unique_code_id; i++) {
            unique_codes[i].kind = PY_CODE_NONE;
        }
    }
}

void rt_assign_byte_code(int unique_code_id, byte *code, uint len, int n_args) {
    alloc_unique_codes();

    assert(unique_code_id < next_unique_code_id);
    unique_codes[unique_code_id].kind = PY_CODE_BYTE;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].u_byte.code = code;
    unique_codes[unique_code_id].u_byte.len = len;

    DEBUG_printf("assign byte code: id=%d code=%p len=%u n_args=%d\n", unique_code_id, code, len, n_args);
}

void rt_assign_native_code(int unique_code_id, py_fun_t fun, uint len, int n_args) {
    alloc_unique_codes();

    assert(1 <= unique_code_id && unique_code_id < next_unique_code_id);
    unique_codes[unique_code_id].kind = PY_CODE_NATIVE;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].u_native.fun = fun;

#ifdef DEBUG_PRINT
    DEBUG_printf("assign native code: id=%d fun=%p len=%u n_args=%d\n", unique_code_id, fun, len, n_args);
    byte *fun_data = (byte*)(((machine_uint_t)fun) & (~1)); // need to clear lower bit in case it's thumb code
    for (int i = 0; i < 128 && i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", fun_data[i]);
    }
    DEBUG_printf("\n");

#ifdef WRITE_NATIVE
    if (fp_native != NULL) {
        fwrite(fun_data, len, 1, fp_native);
        fflush(fp_native);
    }
#endif
#endif
}

void rt_assign_inline_asm_code(int unique_code_id, py_fun_t fun, uint len, int n_args) {
    alloc_unique_codes();

    assert(1 <= unique_code_id && unique_code_id < next_unique_code_id);
    unique_codes[unique_code_id].kind = PY_CODE_INLINE_ASM;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].u_inline_asm.fun = fun;

#ifdef DEBUG_PRINT
    DEBUG_printf("assign inline asm code: id=%d fun=%p len=%u n_args=%d\n", unique_code_id, fun, len, n_args);
    byte *fun_data = (byte*)(((machine_uint_t)fun) & (~1)); // need to clear lower bit in case it's thumb code
    for (int i = 0; i < 128 && i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", fun_data[i]);
    }
    DEBUG_printf("\n");

#ifdef WRITE_NATIVE
    if (fp_native != NULL) {
        fwrite(fun_data, len, 1, fp_native);
    }
#endif
#endif
}

bool py_obj_is_callable(py_obj_t o_in) {
    if (IS_SMALL_INT(o_in)) {
        return false;
    } else {
        py_obj_base_t *o = o_in;
        switch (o->kind) {
            case O_FUN_0:
            case O_FUN_1:
            case O_FUN_2:
            case O_FUN_N:
            case O_FUN_BC:
            case O_FUN_ASM:
            // what about O_CLASS, and an O_OBJ that has a __call__ method?
                return true;
            default:
                return false;
        }
    }
}

const char *py_obj_get_type_str(py_obj_t o_in) {
    if (IS_SMALL_INT(o_in)) {
        return "int";
    } else {
        py_obj_base_t *o = o_in;
        switch (o->kind) {
            case O_CONST:
                if (o == py_const_none) {
                    return "NoneType";
                } else {
                    return "bool";
                }
            case O_STR:
                return "str";
#if MICROPY_ENABLE_FLOAT
            case O_FLOAT:
                return "float";
#endif
            case O_FUN_0:
            case O_FUN_1:
            case O_FUN_2:
            case O_FUN_N:
            case O_FUN_BC:
                return "function";
            case O_LIST:
                return "list";
            case O_SET:
                return "set";
            case O_MAP:
                return "dict";
            case O_OBJ:
            {
                py_map_elem_t *qn = py_qstr_map_lookup(o->u_obj.class->u_class.locals, qstr_from_str_static("__qualname__"), false);
                assert(qn != NULL);
                assert(IS_O(qn->value, O_STR));
                return qstr_str(((py_obj_base_t*)qn->value)->u_str);
            }
            default:
                assert(0);
                return "UnknownType";
        }
    }
}

void py_obj_print(py_obj_t o_in) {
    if (IS_SMALL_INT(o_in)) {
        printf("%d", (int)FROM_SMALL_INT(o_in));
    } else {
        py_obj_base_t *o = o_in;
        switch (o->kind) {
            case O_CONST:
                printf("%s", o->id);
                break;
            case O_STR:
                // TODO need to escape chars etc
                printf("'%s'", qstr_str(o->u_str));
                break;
#if MICROPY_ENABLE_FLOAT
            case O_FLOAT:
                printf("%f", o->u_flt);
                break;
#endif
            case O_EXCEPTION_2:
                printf("%s: ", qstr_str(o->u_exc2.id));
                printf(o->u_exc2.fmt, o->u_exc2.s1, o->u_exc2.s2);
                break;
            case O_LIST:
                printf("[");
                for (int i = 0; i < o->u_list.len; i++) {
                    if (i > 0) {
                        printf(", ");
                    }
                    py_obj_print(o->u_list.items[i]);
                }
                printf("]");
                break;
            case O_SET:
            {
                bool first = true;
                printf("{");
                for (int i = 0; i < o->u_set.alloc; i++) {
                    if (o->u_set.table[i] != NULL) {
                        if (!first) {
                            printf(", ");
                        }
                        first = false;
                        py_obj_print(o->u_set.table[i]);
                    }
                }
                printf("}");
                break;
            }
            case O_MAP:
            {
                bool first = true;
                printf("{");
                for (int i = 0; i < o->u_map.alloc; i++) {
                    if (o->u_map.table[i].key != NULL) {
                        if (!first) {
                            printf(", ");
                        }
                        first = false;
                        py_obj_print(o->u_map.table[i].key);
                        printf(": ");
                        py_obj_print(o->u_map.table[i].value);
                    }
                }
                printf("}");
                break;
            }
            default:
                printf("<? %d>", o->kind);
                assert(0);
        }
    }
}

int rt_is_true(py_obj_t arg) {
    DEBUG_OP_printf("is true %p\n", arg);
    if (IS_SMALL_INT(arg)) {
        if (FROM_SMALL_INT(arg) == 0) {
            return 0;
        } else {
            return 1;
        }
    } else if (arg == py_const_none) {
        return 0;
    } else if (arg == py_const_false) {
        return 0;
    } else if (arg == py_const_true) {
        return 1;
    } else {
        assert(0);
        return 0;
    }
}

int rt_get_int(py_obj_t arg) {
    if (IS_SMALL_INT(arg)) {
        return FROM_SMALL_INT(arg);
    } else {
        assert(0);
        return 0;
    }
}

py_obj_t rt_load_const_str(qstr qstr) {
    DEBUG_OP_printf("load '%s'\n", qstr_str(qstr));
    return py_obj_new_str(qstr);
}

py_obj_t rt_load_name(qstr qstr) {
    // logic: search locals, globals, builtins
    DEBUG_OP_printf("load name %s\n", qstr_str(qstr));
    py_map_elem_t *elem = py_qstr_map_lookup(map_locals, qstr, false);
    if (elem == NULL) {
        elem = py_qstr_map_lookup(map_globals, qstr, false);
        if (elem == NULL) {
            elem = py_qstr_map_lookup(&map_builtins, qstr, false);
            if (elem == NULL) {
                nlr_jump(py_obj_new_exception_2(q_NameError, "name '%s' is not defined", qstr_str(qstr), NULL));
            }
        }
    }
    return elem->value;
}

py_obj_t rt_load_global(qstr qstr) {
    // logic: search globals, builtins
    DEBUG_OP_printf("load global %s\n", qstr_str(qstr));
    py_map_elem_t *elem = py_qstr_map_lookup(map_globals, qstr, false);
    if (elem == NULL) {
        elem = py_qstr_map_lookup(&map_builtins, qstr, false);
        if (elem == NULL) {
            nlr_jump(py_obj_new_exception_2(q_NameError, "name '%s' is not defined", qstr_str(qstr), NULL));
        }
    }
    return elem->value;
}

py_obj_t rt_load_build_class() {
    DEBUG_OP_printf("load_build_class\n");
    py_map_elem_t *elem = py_qstr_map_lookup(&map_builtins, q___build_class__, false);
    if (elem == NULL) {
        printf("name doesn't exist: __build_class__\n");
        assert(0);
    }
    return elem->value;
}

void rt_store_name(qstr qstr, py_obj_t obj) {
    DEBUG_OP_printf("store name %s <- %p\n", qstr_str(qstr), obj);
    py_qstr_map_lookup(map_locals, qstr, true)->value = obj;
}

void rt_store_global(qstr qstr, py_obj_t obj) {
    DEBUG_OP_printf("store global %s <- %p\n", qstr_str(qstr), obj);
    py_qstr_map_lookup(map_globals, qstr, true)->value = obj;
}

py_obj_t rt_unary_op(int op, py_obj_t arg) {
    assert(0);
    return py_const_none;
}

py_obj_t rt_binary_op(int op, py_obj_t lhs, py_obj_t rhs) {
    DEBUG_OP_printf("binary %d %p %p\n", op, lhs, rhs);
    if (op == RT_BINARY_OP_SUBSCR) {
        if (IS_O(lhs, O_LIST) && IS_SMALL_INT(rhs)) {
            return ((py_obj_base_t*)lhs)->u_list.items[FROM_SMALL_INT(rhs)];
        } else {
            assert(0);
        }
    } else if (IS_SMALL_INT(lhs) && IS_SMALL_INT(rhs)) {
        py_small_int_t val;
        switch (op) {
            case RT_BINARY_OP_ADD:
            case RT_BINARY_OP_INPLACE_ADD: val = FROM_SMALL_INT(lhs) + FROM_SMALL_INT(rhs); break;
            case RT_BINARY_OP_SUBTRACT: val = FROM_SMALL_INT(lhs) - FROM_SMALL_INT(rhs); break;
            case RT_BINARY_OP_MULTIPLY: val = FROM_SMALL_INT(lhs) * FROM_SMALL_INT(rhs); break;
            case RT_BINARY_OP_FLOOR_DIVIDE: val = FROM_SMALL_INT(lhs) / FROM_SMALL_INT(rhs); break;
#if MICROPY_ENABLE_FLOAT
            case RT_BINARY_OP_TRUE_DIVIDE: return py_obj_new_float((float_t)FROM_SMALL_INT(lhs) / (float_t)FROM_SMALL_INT(rhs));
#endif
            default: printf("%d\n", op); assert(0); val = 0;
        }
        if (fit_small_int(val)) {
            return TO_SMALL_INT(val);
        }
    } else if (IS_O(lhs, O_STR) && IS_O(rhs, O_STR)) {
        const char *lhs_str = qstr_str(((py_obj_base_t*)lhs)->u_str);
        const char *rhs_str = qstr_str(((py_obj_base_t*)rhs)->u_str);
        char *val;
        switch (op) {
            case RT_BINARY_OP_ADD:
            case RT_BINARY_OP_INPLACE_ADD: val = m_new(char, strlen(lhs_str) + strlen(rhs_str) + 1); strcpy(val, lhs_str); strcat(val, rhs_str); break;
            default: printf("%d\n", op); assert(0); val = NULL;
        }
        return py_obj_new_str(qstr_from_str_take(val));
    }
    assert(0);
    return py_const_none;
}

py_obj_t rt_compare_op(int op, py_obj_t lhs, py_obj_t rhs) {
    DEBUG_OP_printf("compare %d %p %p\n", op, lhs, rhs);
    if (IS_SMALL_INT(lhs) && IS_SMALL_INT(rhs)) {
        int cmp;
        switch (op) {
            case RT_COMPARE_OP_LESS: cmp = FROM_SMALL_INT(lhs) < FROM_SMALL_INT(rhs); break;
            case RT_COMPARE_OP_MORE: cmp = FROM_SMALL_INT(lhs) > FROM_SMALL_INT(rhs); break;
            default: assert(0); cmp = 0;
        }
        if (cmp) {
            return py_const_true;
        } else {
            return py_const_false;
        }
    }
    assert(0);
    return py_const_none;
}

py_obj_t rt_make_function_from_id(int unique_code_id) {
    DEBUG_OP_printf("make_function_from_id %d\n", unique_code_id);
    if (unique_code_id < 1 || unique_code_id >= next_unique_code_id) {
        // illegal code id
        return py_const_none;
    }
    py_code_t *c = &unique_codes[unique_code_id];
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    switch (c->kind) {
        case PY_CODE_BYTE:
            o->kind = O_FUN_BC;
            o->u_fun_bc.n_args = c->n_args;
            o->u_fun_bc.code = c->u_byte.code;
            o->u_fun_bc.len = c->u_byte.len;
            break;
        case PY_CODE_NATIVE:
            switch (c->n_args) {
                case 0: o->kind = O_FUN_0; break;
                case 1: o->kind = O_FUN_1; break;
                case 2: o->kind = O_FUN_2; break;
                default: assert(0);
            }
            o->u_fun.fun = c->u_native.fun;
            break;
        case PY_CODE_INLINE_ASM:
            o->kind = O_FUN_ASM;
            o->u_fun_asm.n_args = c->n_args;
            o->u_fun_asm.fun = c->u_inline_asm.fun;
            break;
        default:
            assert(0);
    }
    return o;
}

py_obj_t rt_make_function_0(py_fun_0_t fun) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_FUN_0;
    o->u_fun.fun = fun;
    return o;
}

py_obj_t rt_make_function_1(py_fun_1_t fun) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_FUN_1;
    o->u_fun.fun = fun;
    return o;
}

py_obj_t rt_make_function_2(py_fun_2_t fun) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_FUN_2;
    o->u_fun.fun = fun;
    return o;
}

py_obj_t rt_make_function(int n_args, py_fun_t code) {
    // assumes code is a pointer to a py_fun_t (i think this is safe...)
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_FUN_N;
    o->u_fun.n_args = n_args;
    o->u_fun.fun = code;
    return o;
}

py_obj_t rt_call_function_0(py_obj_t fun) {
    return rt_call_function_n(fun, 0, NULL);
}

py_obj_t rt_call_function_1(py_obj_t fun, py_obj_t arg) {
    return rt_call_function_n(fun, 1, &arg);
}

py_obj_t rt_call_function_2(py_obj_t fun, py_obj_t arg1, py_obj_t arg2) {
    py_obj_t args[2];
    args[1] = arg1;
    args[0] = arg2;
    return rt_call_function_n(fun, 2, args);
}

typedef machine_uint_t (*inline_asm_fun_0_t)();
typedef machine_uint_t (*inline_asm_fun_1_t)(machine_uint_t);
typedef machine_uint_t (*inline_asm_fun_2_t)(machine_uint_t, machine_uint_t);
typedef machine_uint_t (*inline_asm_fun_3_t)(machine_uint_t, machine_uint_t, machine_uint_t);

// convert a Python object to a sensible value for inline asm
machine_uint_t rt_convert_obj_for_inline_asm(py_obj_t obj) {
    // TODO for byte_array, pass pointer to the array
    if (IS_SMALL_INT(obj)) {
        return FROM_SMALL_INT(obj);
    } else if (obj == py_const_none) {
        return 0;
    } else if (obj == py_const_false) {
        return 0;
    } else if (obj == py_const_true) {
        return 1;
    } else {
        py_obj_base_t *o = obj;
        switch (o->kind) {
            case O_STR:
                // pointer to the string (it's probably constant though!)
                return (machine_uint_t)qstr_str(o->u_str);

#if MICROPY_ENABLE_FLOAT
            case O_FLOAT:
                // convert float to int (could also pass in float registers)
                return (machine_int_t)o->u_flt;
#endif

            case O_LIST:
                // pointer to start of list (could pass length, but then could use len(x) for that)
                return (machine_uint_t)o->u_list.items;

            default:
                // just pass along a pointer to the object
                return (machine_uint_t)obj;
        }
    }
}

// convert a return value from inline asm to a sensible Python object
py_obj_t rt_convert_val_from_inline_asm(machine_uint_t val) {
    return TO_SMALL_INT(val);
}

// args are in reverse order in the array
py_obj_t rt_call_function_n(py_obj_t fun, int n_args, const py_obj_t *args) {
    int n_args_fun = 0;
    if (IS_O(fun, O_FUN_0)) {
        py_obj_base_t *o = fun;
        if (n_args != 0) {
            n_args_fun = 0;
            goto bad_n_args;
        }
        DEBUG_OP_printf("calling native %p()\n", o->u_fun.fun);
        return ((py_fun_0_t)o->u_fun.fun)();

    } else if (IS_O(fun, O_FUN_1)) {
        py_obj_base_t *o = fun;
        if (n_args != 1) {
            n_args_fun = 1;
            goto bad_n_args;
        }
        DEBUG_OP_printf("calling native %p(%p)\n", o->u_fun.fun, args[0]);
        return ((py_fun_1_t)o->u_fun.fun)(args[0]);

    } else if (IS_O(fun, O_FUN_2)) {
        py_obj_base_t *o = fun;
        if (n_args != 2) {
            n_args_fun = 2;
            goto bad_n_args;
        }
        DEBUG_OP_printf("calling native %p(%p, %p)\n", o->u_fun.fun, args[1], args[0]);
        return ((py_fun_2_t)o->u_fun.fun)(args[1], args[0]);

    // TODO O_FUN_N

    } else if (IS_O(fun, O_FUN_BC)) {
        py_obj_base_t *o = fun;
        if (n_args != o->u_fun_bc.n_args) {
            n_args_fun = o->u_fun_bc.n_args;
            goto bad_n_args;
        }
        DEBUG_OP_printf("calling byte code %p(n_args=%d)\n", o->u_fun_bc.code, n_args);
        return py_execute_byte_code(o->u_fun_bc.code, o->u_fun_bc.len, args, n_args);

    } else if (IS_O(fun, O_FUN_ASM)) {
        py_obj_base_t *o = fun;
        if (n_args != o->u_fun_asm.n_args) {
            n_args_fun = o->u_fun_asm.n_args;
            goto bad_n_args;
        }
        DEBUG_OP_printf("calling inline asm %p(n_args=%d)\n", o->u_fun_asm.fun, n_args);
        machine_uint_t ret;
        if (n_args == 0) {
            ret = ((inline_asm_fun_0_t)o->u_fun_asm.fun)();
        } else if (n_args == 1) {
            ret = ((inline_asm_fun_1_t)o->u_fun_asm.fun)(rt_convert_obj_for_inline_asm(args[0]));
        } else if (n_args == 2) {
            ret = ((inline_asm_fun_2_t)o->u_fun_asm.fun)(rt_convert_obj_for_inline_asm(args[1]), rt_convert_obj_for_inline_asm(args[0]));
        } else if (n_args == 3) {
            ret = ((inline_asm_fun_3_t)o->u_fun_asm.fun)(rt_convert_obj_for_inline_asm(args[2]), rt_convert_obj_for_inline_asm(args[1]), rt_convert_obj_for_inline_asm(args[0]));
        } else {
            assert(0);
            ret = 0;
        }
        return rt_convert_val_from_inline_asm(ret);

    } else if (IS_O(fun, O_BOUND_METH)) {
        py_obj_base_t *o = fun;
        DEBUG_OP_printf("calling bound method %p(self=%p, n_args=%d)\n", o->u_bound_meth.meth, o->u_bound_meth.self, n_args);
        if (n_args == 0) {
            return rt_call_function_n(o->u_bound_meth.meth, 1, &o->u_bound_meth.self);
        } else if (n_args == 1) {
            py_obj_t args2[2];
            args2[1] = o->u_bound_meth.self;
            args2[0] = args[0];
            return rt_call_function_n(o->u_bound_meth.meth, 2, args2);
        } else {
            // TODO not implemented
            assert(0);
            return py_const_none;
            //return rt_call_function_2(o->u_bound_meth.meth, n_args + 1, o->u_bound_meth.self + args);
        }

    } else if (IS_O(fun, O_CLASS)) {
        // instantiate an instance of a class
        if (n_args != 0) {
            n_args_fun = 0;
            goto bad_n_args;
        }
        DEBUG_OP_printf("instantiate object of class %p with no args\n", fun);
        py_obj_base_t *o = m_new(py_obj_base_t, 1);
        o->kind = O_OBJ;
        o->u_obj.class = fun;
        o->u_obj.members = py_map_new(MAP_QSTR, 0);
        return o;

    } else {
        printf("fun %p %d\n", fun, ((py_obj_base_t*)fun)->kind);
        assert(0);
        return py_const_none;
    }

bad_n_args:
    printf("TypeError: function takes %d positional arguments but %d were given\n", n_args_fun, n_args);
    assert(0);
    return py_const_none;
}

// args contains: arg(n_args-1)  arg(n_args-2)  ...  arg(0)  self/NULL  fun
// if n_args==0 then there are only self/NULL and fun
py_obj_t rt_call_method_n(int n_args, const py_obj_t *args) {
    DEBUG_OP_printf("call method %p(self=%p, n_args=%d)\n", args[n_args + 1], args[n_args], n_args);
    return rt_call_function_n(args[n_args + 1], n_args + ((args[n_args] == NULL) ? 0 : 1), args);
}

// items are in reverse order
py_obj_t rt_build_list(int n_args, py_obj_t *items) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_LIST;
    o->u_list.alloc = n_args;
    if (o->u_list.alloc < 4) {
        o->u_list.alloc = 4;
    }
    o->u_list.len = n_args;
    o->u_list.items = m_new(py_obj_t, o->u_list.alloc);
    for (int i = 0; i < n_args; i++) {
        o->u_list.items[i] = items[n_args - i - 1];
    }
    return o;
}

py_obj_t py_set_lookup(py_obj_t o_in, py_obj_t index, bool add_if_not_found) {
    assert(IS_O(o_in, O_SET));
    py_obj_base_t *o = o_in;
    int hash = py_obj_hash(index);
    int pos = hash % o->u_set.alloc;
    for (;;) {
        py_obj_t elem = o->u_set.table[pos];
        if (elem == NULL) {
            // not in table
            if (add_if_not_found) {
                if (o->u_set.used + 1 >= o->u_set.alloc) {
                    // not enough room in table, rehash it
                    int old_alloc = o->u_set.alloc;
                    py_obj_t *old_table = o->u_set.table;
                    o->u_set.alloc = get_doubling_prime_greater_or_equal_to(o->u_set.alloc + 1);
                    o->u_set.used = 0;
                    o->u_set.table = m_new(py_obj_t, o->u_set.alloc);
                    for (int i = 0; i < old_alloc; i++) {
                        if (old_table[i] != NULL) {
                            py_set_lookup(o, old_table[i], true);
                        }
                    }
                    m_free(old_table);
                    // restart the search for the new element
                    pos = hash % o->u_set.alloc;
                } else {
                    o->u_set.used += 1;
                    o->u_set.table[pos] = index;
                    return index;
                }
            } else {
                return NULL;
            }
        } else if (py_obj_equal(elem, index)) {
            // found it
            return elem;
        } else {
            // not yet found, keep searching in this table
            pos = (pos + 1) % o->u_set.alloc;
        }
    }
}

py_obj_t rt_build_set(int n_args, py_obj_t *items) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_SET;
    o->u_set.alloc = get_doubling_prime_greater_or_equal_to(n_args + 1);
    o->u_set.used = 0;
    o->u_set.table = m_new(py_obj_t, o->u_set.alloc);
    for (int i = 0; i < o->u_set.alloc; i++) {
        o->u_set.table[i] = NULL;
    }
    for (int i = 0; i < n_args; i++) {
        py_set_lookup(o, items[i], true);
    }
    return o;
}

py_obj_t rt_build_map(int n_args) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_MAP;
    py_map_init(&o->u_map, MAP_PY_OBJ, n_args);
    return o;
}

py_obj_t rt_store_map(py_obj_t map, py_obj_t key, py_obj_t value) {
    assert(IS_O(map, O_MAP)); // should always be
    py_map_lookup(map, key, true)->value = value;
    return map;
}

py_obj_t build_bound_method(py_obj_t self, py_obj_t meth) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_BOUND_METH;
    o->u_bound_meth.meth = meth;
    o->u_bound_meth.self = self;
    return o;
}

py_obj_t rt_load_attr(py_obj_t base, qstr attr) {
    DEBUG_OP_printf("load attr %s\n", qstr_str(attr));
    if (IS_O(base, O_LIST) && attr == q_append) {
        return build_bound_method(base, fun_list_append);
    } else if (IS_O(base, O_CLASS)) {
        py_obj_base_t *o = base;
        py_map_elem_t *elem = py_qstr_map_lookup(o->u_class.locals, attr, false);
        if (elem == NULL) {
            goto no_attr;
        }
        return elem->value;
    } else if (IS_O(base, O_OBJ)) {
        // logic: look in obj members then class locals (TODO check this against CPython)
        py_obj_base_t *o = base;
        py_map_elem_t *elem = py_qstr_map_lookup(o->u_obj.members, attr, false);
        if (elem != NULL) {
            // object member, always treated as a value
            return elem->value;
        }
        elem = py_qstr_map_lookup(o->u_obj.class->u_class.locals, attr, false);
        if (elem != NULL) {
            if (py_obj_is_callable(elem->value)) {
                // class member is callable so build a bound method
                return build_bound_method(base, elem->value);
            } else {
                // class member is a value, so just return that value
                return elem->value;
            }
        }
        goto no_attr;
    }

no_attr:
    nlr_jump(py_obj_new_exception_2(q_AttributeError, "'%s' object has no attribute '%s'", py_obj_get_type_str(base), qstr_str(attr)));
}

void rt_load_method(py_obj_t base, qstr attr, py_obj_t *dest) {
    DEBUG_OP_printf("load method %s\n", qstr_str(attr));
    if (IS_O(base, O_LIST) && attr == q_append) {
        dest[1] = fun_list_append;
        dest[0] = base;
        return;
    } else if (IS_O(base, O_OBJ)) {
        // logic: look in obj members then class locals (TODO check this against CPython)
        py_obj_base_t *o = base;
        py_map_elem_t *elem = py_qstr_map_lookup(o->u_obj.members, attr, false);
        if (elem != NULL) {
            // object member, always treated as a value
            dest[1] = elem->value;
            dest[0] = NULL;
            return;
        }
        elem = py_qstr_map_lookup(o->u_obj.class->u_class.locals, attr, false);
        if (elem != NULL) {
            if (py_obj_is_callable(elem->value)) {
                // class member is callable so build a bound method
                dest[1] = elem->value;
                dest[0] = base;
                return;
            } else {
                // class member is a value, so just return that value
                dest[1] = elem->value;
                dest[0] = NULL;
                return;
            }
        }
        goto no_attr;
    }

no_attr:
    dest[1] = rt_load_attr(base, attr);
    dest[0] = NULL;
}

void rt_store_attr(py_obj_t base, qstr attr, py_obj_t val) {
    DEBUG_OP_printf("store attr %p.%s <- %p\n", base, qstr_str(attr), val);
    if (IS_O(base, O_OBJ)) {
        // logic: look in class locals (no add) then obj members (add) (TODO check this against CPython)
        py_obj_base_t *o = base;
        py_map_elem_t *elem = py_qstr_map_lookup(o->u_obj.class->u_class.locals, attr, false);
        if (elem != NULL) {
            elem->value = val;
        } else {
            elem = py_qstr_map_lookup(o->u_obj.class->u_class.locals, attr, true)->value = val;
        }
    } else {
        printf("?AttributeError: '%s' object has no attribute '%s'\n", py_obj_get_type_str(base), qstr_str(attr));
        assert(0);
    }
}

void rt_store_subscr(py_obj_t base, py_obj_t index, py_obj_t value) {
    if (IS_O(base, O_LIST) && IS_SMALL_INT(index)) {
        // list store
        py_obj_base_t *o = base;
        int idx = FROM_SMALL_INT(index);
        if (idx < 0) {
            idx += o->u_list.len;
        }
        if (0 <= idx && idx < o->u_list.len) {
            o->u_list.items[idx] = value;
        } else {
            assert(0);
        }
    } else if (IS_O(base, O_MAP)) {
        // map store
        py_map_lookup(base, index, true)->value = value;
    } else {
        assert(0);
    }
}

py_obj_t rt_getiter(py_obj_t o_in) {
    if (IS_O(o_in, O_RANGE)) {
        py_obj_base_t *o = o_in;
        return py_obj_new_range_iterator(o->u_range.start, o->u_range.stop, o->u_range.step);
    } else {
        nlr_jump(py_obj_new_exception_2(q_TypeError, "'%s' object is not iterable", py_obj_get_type_str(o_in), NULL));
    }
}

py_obj_t rt_iternext(py_obj_t o_in) {
    if (IS_O(o_in, O_RANGE_IT)) {
        py_obj_base_t *o = o_in;
        if ((o->u_range_it.step > 0 && o->u_range_it.cur < o->u_range_it.stop) || (o->u_range_it.step < 0 && o->u_range_it.cur > o->u_range_it.stop)) {
            py_obj_t o_out = TO_SMALL_INT(o->u_range_it.cur);
            o->u_range_it.cur += o->u_range_it.step;
            return o_out;
        } else {
            return py_const_stop_iteration;
        }
    } else {
        nlr_jump(py_obj_new_exception_2(q_TypeError, "? '%s' object is not iterable", py_obj_get_type_str(o_in), NULL));
    }
}

void *rt_fun_table[RT_F_NUMBER_OF] = {
    rt_load_const_str,
    rt_load_name,
    rt_load_global,
    rt_load_build_class,
    rt_load_attr,
    rt_load_method,
    rt_store_name,
    rt_store_attr,
    rt_store_subscr,
    rt_is_true,
    rt_unary_op,
    rt_build_list,
    rt_build_map,
    rt_store_map,
    rt_build_set,
    rt_make_function_from_id,
    rt_call_function_n,
    rt_call_method_n,
    rt_binary_op,
    rt_compare_op,
};

/*
void rt_f_vector(rt_fun_kind_t fun_kind) {
    (rt_f_table[fun_kind])();
}
*/
