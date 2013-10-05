#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "machine.h"
#include "runtime.h"
#include "vm.h"

//#define DEBUG_printf(args...) (void)0
#define DEBUG_printf(args...) printf(args)

#define DEBUG_OP_printf(args...) (void)0
//#define DEBUG_OP_printf(args...) printf(args)

// enable/disable float support with this definition
#define PY_FLOAT (1)

typedef machine_int_t py_small_int_t;

#define IS_O(o, k) (((((py_small_int_t)(o)) & 1) == 0) && (((py_obj_base_t*)(o))->kind == (k)))
#define IS_SMALL_INT(o) (((py_small_int_t)(o)) & 1)
#define FROM_SMALL_INT(o) (((py_small_int_t)(o)) >> 1)
#define TO_SMALL_INT(o) ((py_obj_t)(((o) << 1) | 1))

#ifdef PY_FLOAT
typedef machine_float_t float_t;
#endif

typedef enum {
    O_CONST,
    O_STR,
#ifdef PY_FLOAT
    O_FLOAT,
#endif
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
    py_map_kind_t kind;
    machine_uint_t alloc;
    machine_uint_t used;
    py_map_elem_t *table;
} py_map_t;

typedef struct _py_obj_base_t {
    py_obj_kind_t kind;
    union {
        const char *id;
        qstr u_str;
#ifdef PY_FLOAT
        float_t flt;
#endif
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
        /*
        struct { // for O_MAP
            int alloc;
            int used;
            py_map_elem_t *table;
        } u_map;
        */
        struct { // for O_CLASS
            py_map_t *map;
        } u_class;
    };
} py_obj_base_t;

py_obj_t py_const_none;
py_obj_t py_const_false;
py_obj_t py_const_true;

py_map_t map_name;
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

#ifdef PY_FLOAT
py_obj_t py_obj_new_float(float_t val) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_FLOAT;
    o->flt = val;
    return (py_obj_t)o;
}
#endif

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
            py_fun_t fun;
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

py_obj_t py_builtin___build_class__(py_obj_t o1, py_obj_t o2) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_CLASS;
    o->u_class.map = py_map_new(MAP_QSTR, 0);
    return o;
}

FILE *fp_native = NULL;

void rt_init() {
    q_append = qstr_from_str_static("append");
    q_print = qstr_from_str_static("print");
    q_len = qstr_from_str_static("len");
    q___build_class__ = qstr_from_str_static("__build_class__");

    py_const_none = py_obj_new_const("None");
    py_const_false = py_obj_new_const("False");
    py_const_true = py_obj_new_const("True");

    py_map_init(&map_name, MAP_QSTR, 0);

    py_map_init(&map_builtins, MAP_QSTR, 3);
    py_qstr_map_lookup(&map_builtins, q_print, true)->value = rt_make_function_1(py_builtin_print);
    py_qstr_map_lookup(&map_builtins, q_len, true)->value = rt_make_function_1(py_builtin_len);
    py_qstr_map_lookup(&map_builtins, q___build_class__, true)->value = rt_make_function_2(py_builtin___build_class__);

    next_unique_code_id = 1;
    unique_codes = NULL;

    fun_list_append = rt_make_function_2(list_append);

    fp_native = fopen("out-native", "wb");
}

void rt_deinit() {
    if (fp_native != NULL) {
        fclose(fp_native);
    }
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

    DEBUG_printf("assign native code: id=%d fun=%p len=%u n_args=%d\n", unique_code_id, fun, len, n_args);
    byte *fun_data = (byte*)(((machine_uint_t)fun) & (~1)); // need to clear lower bit in case it's thumb code
    for (int i = 0; i < 128 && i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", fun_data[i]);
    }
    DEBUG_printf("\n");

    if (fp_native != NULL) {
        fwrite(fun_data, len, 1, fp_native);
    }
}

void rt_assign_inline_asm_code(int unique_code_id, py_fun_t fun, uint len, int n_args) {
    alloc_unique_codes();

    assert(1 <= unique_code_id && unique_code_id < next_unique_code_id);
    unique_codes[unique_code_id].kind = PY_CODE_INLINE_ASM;
    unique_codes[unique_code_id].n_args = n_args;
    unique_codes[unique_code_id].u_inline_asm.fun = fun;

    DEBUG_printf("assign inline asm code: id=%d fun=%p len=%u n_args=%d\n", unique_code_id, fun, len, n_args);
    byte *fun_data = (byte*)(((machine_uint_t)fun) & (~1)); // need to clear lower bit in case it's thumb code
    for (int i = 0; i < 128 && i < len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", fun_data[i]);
    }
    DEBUG_printf("\n");

    if (fp_native != NULL) {
        fwrite(fun_data, len, 1, fp_native);
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
#ifdef PY_FLOAT
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
#ifdef PY_FLOAT
            case O_FLOAT:
                printf("%f", o->flt);
                break;
#endif
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
    DEBUG_OP_printf("load %s\n", qstr_str(qstr));
    py_map_elem_t *elem = py_qstr_map_lookup(&map_name, qstr, false);
    if (elem == NULL) {
        elem = py_qstr_map_lookup(&map_builtins, qstr, false);
        if (elem == NULL) {
            printf("name doesn't exist: %s\n", qstr_str(qstr));
            assert(0);
        }
    }
    return elem->value;
}

py_obj_t rt_load_global(qstr qstr) {
    return rt_load_name(qstr); // TODO
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
    DEBUG_OP_printf("store %s <- %p\n", qstr_str(qstr), obj);
    py_qstr_map_lookup(&map_name, qstr, true)->value = obj;
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
#ifdef PY_FLOAT
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
    if (IS_O(fun, O_FUN_0)) {
        py_obj_base_t *o = fun;
        DEBUG_OP_printf("calling native %p...\n", o->u_fun.fun);
        return ((py_fun_0_t)o->u_fun.fun)();
    } else if (IS_O(fun, O_FUN_BC)) {
        py_obj_base_t *o = fun;
        assert(o->u_fun_bc.n_args == 0);
        DEBUG_OP_printf("calling byte code %p...\n", o->u_fun_bc.code);
        return py_execute_byte_code(o->u_fun_bc.code, o->u_fun_bc.len, NULL, 0);
    } else {
        printf("fun0:%p\n", fun);
        assert(0);
        return py_const_none;
    }
}

py_obj_t rt_call_function_1(py_obj_t fun, py_obj_t arg) {
    if (IS_O(fun, O_FUN_1)) {
        py_obj_base_t *o = fun;
        DEBUG_OP_printf("calling native %p with 1 arg\n", o->u_fun.fun);
        return ((py_fun_1_t)o->u_fun.fun)(arg);
    } else if (IS_O(fun, O_FUN_BC)) {
        py_obj_base_t *o = fun;
        assert(o->u_fun_bc.n_args == 1);
        DEBUG_OP_printf("calling byte code %p with 1 arg\n", o->u_fun_bc.code);
        return py_execute_byte_code(o->u_fun_bc.code, o->u_fun_bc.len, &arg, 1);
    } else if (IS_O(fun, O_BOUND_METH)) {
        py_obj_base_t *o = fun;
        return rt_call_function_2(o->u_bound_meth.meth, o->u_bound_meth.self, arg);
    } else {
        printf("fun1:%p\n", fun);
        assert(0);
        return py_const_none;
    }
}

py_obj_t rt_call_function_2(py_obj_t fun, py_obj_t arg1, py_obj_t arg2) {
    if (IS_O(fun, O_FUN_2)) {
        py_obj_base_t *o = fun;
        DEBUG_OP_printf("calling native %p...\n", o->u_fun.fun);
        return ((py_fun_2_t)o->u_fun.fun)(arg1, arg2);
    } else if (IS_O(fun, O_FUN_BC)) {
        py_obj_base_t *o = fun;
        assert(o->u_fun_bc.n_args == 2);
        DEBUG_OP_printf("calling byte code %p...\n", o->u_fun_bc.code);
        py_obj_t args[2];
        args[0] = arg1;
        args[1] = arg2;
        return py_execute_byte_code(o->u_fun_bc.code, o->u_fun_bc.len, &args[0], 2);
    } else {
        assert(0);
        return py_const_none;
    }
}

py_obj_t rt_call_method_1(py_obj_t fun, py_obj_t self) {
    DEBUG_OP_printf("call method %p %p\n", fun, self);
    if (self == NULL) {
        return rt_call_function_0(fun);
    } else {
        return rt_call_function_1(fun, self);
    }
}

py_obj_t rt_call_method_2(py_obj_t fun, py_obj_t self, py_obj_t arg) {
    DEBUG_OP_printf("call method %p %p %p\n", fun, self, arg);
    if (self == NULL) {
        return rt_call_function_1(fun, arg);
    } else {
        return rt_call_function_2(fun, self, arg);
    }
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

py_obj_t build_bound_method(py_obj_t self, py_obj_t meth) {
    py_obj_base_t *o = m_new(py_obj_base_t, 1);
    o->kind = O_BOUND_METH;
    o->u_bound_meth.meth = meth;
    o->u_bound_meth.self = self;
    return o;
}

py_obj_t rt_load_attr(py_obj_t base, qstr attr) {
    DEBUG_OP_printf("load %s\n", qstr_str(attr));
    if (IS_O(base, O_LIST) && attr == q_append) {
        return build_bound_method(base, fun_list_append);
    } else if (IS_O(base, O_CLASS)) {
        py_obj_base_t *o = base;
        py_map_elem_t *elem = py_qstr_map_lookup(o->u_class.map, attr, false);
        if (elem == NULL) {
            printf("Nope! %s\n", qstr_str(attr));
            assert(0);
        }
        return elem->value;
    } else {
        printf("AttributeError: '%s' object has no attribute '%s'\n", py_obj_get_type_str(base), qstr_str(attr));
        assert(0);
        return py_const_none;
    }
}

void rt_load_method(py_obj_t base, qstr attr, py_obj_t *dest) {
    DEBUG_OP_printf("load method %s\n", qstr_str(attr));
    if (IS_O(base, O_LIST) && attr == q_append) {
        dest[1] = fun_list_append;
        dest[0] = base;
    } else {
        dest[1] = rt_load_attr(base, attr);
        dest[0] = NULL;
    }
}

void *rt_fun_table[RT_F_NUMBER_OF] = {
    rt_load_const_str,
    rt_load_name,
    rt_load_global,
    rt_load_attr,
    rt_load_method,
    rt_store_name,
    rt_store_subscr,
    rt_is_true,
    rt_unary_op,
    rt_build_list,
    rt_build_map,
    rt_store_map,
    rt_build_set,
    rt_make_function_from_id,
    rt_call_function_0,
    rt_call_function_1,
    rt_call_function_2,
    rt_call_method_1,
    rt_call_method_2,
    rt_binary_op,
    rt_compare_op,
};

/*
void rt_f_vector(rt_fun_kind_t fun_kind) {
    (rt_f_table[fun_kind])();
}
*/
