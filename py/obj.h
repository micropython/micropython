// All Micro Python objects are at least this type
// It must be of pointer size

typedef machine_ptr_t mp_obj_t;
typedef machine_const_ptr_t mp_const_obj_t;

// Integers that fit in a pointer have this type
// (do we need to expose this in the public API?)

typedef machine_int_t mp_small_int_t;

// Anything that wants to be a Micro Python object must have
// mp_obj_base_t as its first member (except NULL and small ints)

struct _mp_obj_type_t;
struct _mp_obj_base_t {
    const struct _mp_obj_type_t *type;
};
typedef struct _mp_obj_base_t mp_obj_base_t;

// The NULL object is used to indicate the absence of an object
// It *cannot* be used when an mp_obj_t is expected, except where explicitly allowed

#define MP_OBJ_NULL ((mp_obj_t)NULL)

// These macros check for small int, qstr or object, and access small int and qstr values
//  - xxxx...xxx1: a small int, bits 1 and above are the value
//  - xxxx...xx10: a qstr, bits 2 and above are the value
//  - xxxx...xx00: a pointer to an mp_obj_base_t

// In SMALL_INT, next-to-highest bits is used as sign, so both must match for value in range
#define MP_SMALL_INT_MIN ((mp_small_int_t)(((machine_int_t)WORD_MSBIT_HIGH) >> 1))
#define MP_SMALL_INT_MAX ((mp_small_int_t)(~(MP_SMALL_INT_MIN)))
#define MP_OBJ_FITS_SMALL_INT(n) ((((n) ^ ((n) << 1)) & WORD_MSBIT_HIGH) == 0)
#define MP_OBJ_IS_SMALL_INT(o) ((((mp_small_int_t)(o)) & 1) != 0)
#define MP_OBJ_IS_QSTR(o) ((((mp_small_int_t)(o)) & 3) == 2)
#define MP_OBJ_IS_OBJ(o) ((((mp_small_int_t)(o)) & 3) == 0)
#define MP_OBJ_IS_TYPE(o, t) (MP_OBJ_IS_OBJ(o) && (((mp_obj_base_t*)(o))->type == (t))) // this does not work for checking a string, use below macro for that
#define MP_OBJ_IS_INT(o) (MP_OBJ_IS_SMALL_INT(o) || MP_OBJ_IS_TYPE(o, &mp_type_int))
#define MP_OBJ_IS_STR(o) (MP_OBJ_IS_QSTR(o) || MP_OBJ_IS_TYPE(o, &mp_type_str))

#define MP_OBJ_SMALL_INT_VALUE(o) (((mp_small_int_t)(o)) >> 1)
#define MP_OBJ_NEW_SMALL_INT(small_int) ((mp_obj_t)(((small_int) << 1) | 1))

#define MP_OBJ_QSTR_VALUE(o) (((mp_small_int_t)(o)) >> 2)
#define MP_OBJ_NEW_QSTR(qstr) ((mp_obj_t)((((machine_uint_t)qstr) << 2) | 2))

// These macros are used to declare and define constant function objects
// You can put "static" in front of the definitions to make them local

#define MP_DECLARE_CONST_FUN_OBJ(obj_name) extern const mp_obj_fun_native_t obj_name

#define MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, is_kw, n_args_min, n_args_max, fun_name) const mp_obj_fun_native_t obj_name = {{&mp_type_fun_native}, is_kw, n_args_min, n_args_max, (void *)fun_name}
#define MP_DEFINE_CONST_FUN_OBJ_0(obj_name, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, 0, 0, (mp_fun_0_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_1(obj_name, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, 1, 1, (mp_fun_1_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_2(obj_name, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, 2, 2, (mp_fun_2_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_3(obj_name, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, 3, 3, (mp_fun_3_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_VAR(obj_name, n_args_min, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, n_args_min, MP_OBJ_FUN_ARGS_MAX, (mp_fun_var_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(obj_name, n_args_min, n_args_max, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, false, n_args_min, n_args_max, (mp_fun_var_t)fun_name)
#define MP_DEFINE_CONST_FUN_OBJ_KW(obj_name, n_args_min, fun_name) MP_DEFINE_CONST_FUN_OBJ_VOID_PTR(obj_name, true, n_args_min, MP_OBJ_FUN_ARGS_MAX, (mp_fun_kw_t)fun_name)

// This macro is used to define constant dict objects
// You can put "static" in front of the definition to make it local

#define MP_DEFINE_CONST_DICT(dict_name, table_name) \
    const mp_obj_dict_t dict_name = { \
        .base = {&mp_type_dict}, \
        .map = { \
            .all_keys_are_qstrs = 1, \
            .table_is_fixed_array = 1, \
            .used = sizeof(table_name) / sizeof(mp_map_elem_t), \
            .alloc = sizeof(table_name) / sizeof(mp_map_elem_t), \
            .table = (mp_map_elem_t*)table_name, \
        }, \
    }

// These macros are used to declare and define constant staticmethond and classmethod objects
// You can put "static" in front of the definitions to make them local

#define MP_DECLARE_CONST_STATICMETHOD_OBJ(obj_name) extern const mp_obj_static_class_method_t obj_name
#define MP_DECLARE_CONST_CLASSMETHOD_OBJ(obj_name) extern const mp_obj_static_class_method_t obj_name

#define MP_DEFINE_CONST_STATICMETHOD_OBJ(obj_name, fun_name) const mp_obj_static_class_method_t obj_name = {{&mp_type_staticmethod}, fun_name}
#define MP_DEFINE_CONST_CLASSMETHOD_OBJ(obj_name, fun_name) const mp_obj_static_class_method_t obj_name = {{&mp_type_classmethod}, fun_name}

// Underlying map/hash table implementation (not dict object or map function)

typedef struct _mp_map_elem_t {
    mp_obj_t key;
    mp_obj_t value;
} mp_map_elem_t;

// TODO maybe have a truncated mp_map_t for fixed tables, since alloc=used
// put alloc last in the structure, so the truncated version does not need it
// this would save 1 ROM word for all ROM objects that have a locals_dict
// would also need a trucated dict structure

typedef struct _mp_map_t {
    machine_uint_t all_keys_are_qstrs : 1;
    machine_uint_t table_is_fixed_array : 1;
    machine_uint_t used : (8 * sizeof(machine_uint_t) - 2);
    machine_uint_t alloc;
    mp_map_elem_t *table;
} mp_map_t;

typedef enum _mp_map_lookup_kind_t {
    MP_MAP_LOOKUP,                    // 0
    MP_MAP_LOOKUP_ADD_IF_NOT_FOUND,   // 1
    MP_MAP_LOOKUP_REMOVE_IF_FOUND,    // 2
    MP_MAP_LOOKUP_FIRST = 4,
} mp_map_lookup_kind_t;

void mp_map_init(mp_map_t *map, int n);
void mp_map_init_fixed_table(mp_map_t *map, int n, const mp_obj_t *table);
mp_map_t *mp_map_new(int n);
void mp_map_deinit(mp_map_t *map);
void mp_map_free(mp_map_t *map);
mp_map_elem_t* mp_map_lookup(mp_map_t *map, mp_obj_t index, mp_map_lookup_kind_t lookup_kind);
void mp_map_clear(mp_map_t *map);

// Underlying set implementation (not set object)

typedef struct _mp_set_t {
    machine_uint_t alloc;
    machine_uint_t used;
    mp_obj_t *table;
} mp_set_t;

void mp_set_init(mp_set_t *set, int n);
mp_obj_t mp_set_lookup(mp_set_t *set, mp_obj_t index, mp_map_lookup_kind_t lookup_kind);
void mp_set_clear(mp_set_t *set);

// Type definitions for methods

typedef mp_obj_t (*mp_fun_0_t)(void);
typedef mp_obj_t (*mp_fun_1_t)(mp_obj_t);
typedef mp_obj_t (*mp_fun_2_t)(mp_obj_t, mp_obj_t);
typedef mp_obj_t (*mp_fun_3_t)(mp_obj_t, mp_obj_t, mp_obj_t);
typedef mp_obj_t (*mp_fun_t)(void);
typedef mp_obj_t (*mp_fun_var_t)(uint n, const mp_obj_t *);
typedef mp_obj_t (*mp_fun_kw_t)(uint n, const mp_obj_t *, mp_map_t *);

typedef enum {
    PRINT_STR,
    PRINT_REPR,
    PRINT_EXC, // Special format for printing exception in unhandled exception message
} mp_print_kind_t;

typedef void (*mp_print_fun_t)(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o, mp_print_kind_t kind);
typedef mp_obj_t (*mp_make_new_fun_t)(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args);
typedef mp_obj_t (*mp_call_fun_t)(mp_obj_t fun, uint n_args, uint n_kw, const mp_obj_t *args);
typedef mp_obj_t (*mp_unary_op_fun_t)(int op, mp_obj_t);
typedef mp_obj_t (*mp_binary_op_fun_t)(int op, mp_obj_t, mp_obj_t);
typedef void (*mp_load_attr_fun_t)(mp_obj_t self_in, qstr attr, mp_obj_t *dest); // for fail, do nothing; for attr, dest[0] = value; for method, dest[0] = method, dest[1] = self
typedef bool (*mp_store_attr_fun_t)(mp_obj_t self_in, qstr attr, mp_obj_t value); // return true if store succeeded
typedef bool (*mp_store_item_fun_t)(mp_obj_t self_in, mp_obj_t index, mp_obj_t value); // return true if store succeeded

typedef struct _mp_method_t {
    qstr name;
    mp_const_obj_t fun;
} mp_method_t;

// Buffer protocol
typedef struct _buffer_info_t {
    // if we'd bother to support various versions of structure
    // (with different number of fields), we can distinguish
    // them with ver = sizeof(struct). Cons: overkill for *micro*?
    //int ver; // ?

    void *buf;
    machine_int_t len;

    // Rationale: have array.array and have SIMD operations on them
    // Cons: users can pass item size to processing functions themselves,
    // though that's not "plug&play"
    // int itemsize;

    // Rationale: to load arbitrary-sized sprites directly to LCD
    // Cons: a bit adhoc usecase
    // int stride;
} buffer_info_t;
#define BUFFER_READ  (1)
#define BUFFER_WRITE (2)
#define BUFFER_RW (BUFFER_READ | BUFFER_WRITE)
typedef struct _mp_buffer_p_t {
    machine_int_t (*get_buffer)(mp_obj_t obj, buffer_info_t *bufinfo, int flags);
} mp_buffer_p_t;

// Stream protocol
typedef struct _mp_stream_p_t {
    // On error, functions should return -1 and fill in *errcode (values are
    // implementation-dependent, but will be exposed to user, e.g. via exception).
    machine_int_t (*read)(mp_obj_t obj, void *buf, machine_uint_t size, int *errcode);
    machine_int_t (*write)(mp_obj_t obj, const void *buf, machine_uint_t size, int *errcode);
    // add seek() ?
} mp_stream_p_t;

struct _mp_obj_type_t {
    mp_obj_base_t base;
    qstr name;
    mp_print_fun_t print;
    mp_make_new_fun_t make_new;     // to make an instance of the type

    mp_call_fun_t call;
    mp_unary_op_fun_t unary_op;     // can return NULL if op not supported
    mp_binary_op_fun_t binary_op;   // can return NULL if op not supported

    mp_load_attr_fun_t load_attr;
    mp_store_attr_fun_t store_attr;
    // Implements container[index] = val; note that load_item is implemented
    // by binary_op(RT_BINARY_OP_SUBSCR)
    mp_store_item_fun_t store_item;

    mp_fun_1_t getiter;
    mp_fun_1_t iternext; // may return MP_OBJ_NULL as an optimisation instead of raising StopIteration() (with no args)

    // Alternatively, pointer(s) to interfaces to save space
    // in mp_obj_type_t at the expense of extra pointer and extra dereference
    // when actually used.
    mp_buffer_p_t buffer_p;
    mp_stream_p_t stream_p;

    // these are for dynamically created types (classes)
    mp_obj_t bases_tuple;
    mp_obj_t locals_dict;

    /*
    What we might need to add here:

    store_subscr    list dict

    len             str tuple list map
    abs             float complex
    hash            bool int none str
    equal           int str

    unpack seq      list tuple
    */
};

typedef struct _mp_obj_type_t mp_obj_type_t;

// Constant types, globally accessible
extern const mp_obj_type_t mp_type_type;
extern const mp_obj_type_t mp_type_object;
extern const mp_obj_type_t mp_type_NoneType;
extern const mp_obj_type_t mp_type_bool;
extern const mp_obj_type_t mp_type_int;
extern const mp_obj_type_t mp_type_str;
extern const mp_obj_type_t mp_type_bytes;
extern const mp_obj_type_t mp_type_float;
extern const mp_obj_type_t mp_type_complex;
extern const mp_obj_type_t mp_type_tuple;
extern const mp_obj_type_t mp_type_list;
extern const mp_obj_type_t mp_type_map; // map (the python builtin, not the dict implementation detail)
extern const mp_obj_type_t mp_type_enumerate;
extern const mp_obj_type_t mp_type_filter;
extern const mp_obj_type_t mp_type_dict;
extern const mp_obj_type_t mp_type_set;
extern const mp_obj_type_t mp_type_slice;
extern const mp_obj_type_t mp_type_zip;
extern const mp_obj_type_t mp_type_array;
extern const mp_obj_type_t mp_type_super;
extern const mp_obj_type_t mp_type_gen_instance;
extern const mp_obj_type_t mp_type_fun_native;
extern const mp_obj_type_t mp_type_fun_bc;
extern const mp_obj_type_t mp_type_module;
extern const mp_obj_type_t mp_type_staticmethod;
extern const mp_obj_type_t mp_type_classmethod;

// Exceptions
extern const mp_obj_type_t mp_type_BaseException;
extern const mp_obj_type_t mp_type_ArithmeticError;
extern const mp_obj_type_t mp_type_AssertionError;
extern const mp_obj_type_t mp_type_AttributeError;
extern const mp_obj_type_t mp_type_EOFError;
extern const mp_obj_type_t mp_type_Exception;
extern const mp_obj_type_t mp_type_GeneratorExit;
extern const mp_obj_type_t mp_type_IOError;
extern const mp_obj_type_t mp_type_ImportError;
extern const mp_obj_type_t mp_type_IndentationError;
extern const mp_obj_type_t mp_type_IndexError;
extern const mp_obj_type_t mp_type_KeyError;
extern const mp_obj_type_t mp_type_LookupError;
extern const mp_obj_type_t mp_type_MemoryError;
extern const mp_obj_type_t mp_type_NameError;
extern const mp_obj_type_t mp_type_NotImplementedError;
extern const mp_obj_type_t mp_type_OSError;
extern const mp_obj_type_t mp_type_OverflowError;
extern const mp_obj_type_t mp_type_RuntimeError;
extern const mp_obj_type_t mp_type_StopIteration;
extern const mp_obj_type_t mp_type_SyntaxError;
extern const mp_obj_type_t mp_type_SystemError;
extern const mp_obj_type_t mp_type_TypeError;
extern const mp_obj_type_t mp_type_ValueError;
extern const mp_obj_type_t mp_type_ZeroDivisionError;

// Constant objects, globally accessible
// The macros are for convenience only
#define mp_const_none ((mp_obj_t)&mp_const_none_obj)
#define mp_const_false ((mp_obj_t)&mp_const_false_obj)
#define mp_const_true ((mp_obj_t)&mp_const_true_obj)
#define mp_const_empty_tuple ((mp_obj_t)&mp_const_empty_tuple_obj)
extern const struct _mp_obj_none_t mp_const_none_obj;
extern const struct _mp_obj_bool_t mp_const_false_obj;
extern const struct _mp_obj_bool_t mp_const_true_obj;
extern const struct _mp_obj_tuple_t mp_const_empty_tuple_obj;
extern const struct _mp_obj_ellipsis_t mp_const_ellipsis_obj;
extern const struct _mp_obj_exception_t mp_const_GeneratorExit_obj;

// General API for objects

mp_obj_t mp_obj_new_type(qstr name, mp_obj_t bases_tuple, mp_obj_t locals_dict);
mp_obj_t mp_obj_new_none(void);
mp_obj_t mp_obj_new_bool(bool value);
mp_obj_t mp_obj_new_cell(mp_obj_t obj);
mp_obj_t mp_obj_new_int(machine_int_t value);
mp_obj_t mp_obj_new_int_from_uint(machine_uint_t value);
mp_obj_t mp_obj_new_int_from_long_str(const char *s);
mp_obj_t mp_obj_new_int_from_ll(long long val); // this must return a multi-precision integer object (or raise an overflow exception)
mp_obj_t mp_obj_new_str(const byte* data, uint len, bool make_qstr_if_not_already);
mp_obj_t mp_obj_new_bytes(const byte* data, uint len);
#if MICROPY_ENABLE_FLOAT
mp_obj_t mp_obj_new_float(mp_float_t val);
mp_obj_t mp_obj_new_complex(mp_float_t real, mp_float_t imag);
#endif
mp_obj_t mp_obj_new_exception(const mp_obj_type_t *exc_type);
mp_obj_t mp_obj_new_exception_args(const mp_obj_type_t *exc_type, uint n_args, const mp_obj_t *args);
mp_obj_t mp_obj_new_exception_msg(const mp_obj_type_t *exc_type, const char *msg);
mp_obj_t mp_obj_new_exception_msg_varg(const mp_obj_type_t *exc_type, const char *fmt, ...); // counts args by number of % symbols in fmt, excluding %%; can only handle void* sizes (ie no float/double!)
mp_obj_t mp_obj_new_range(int start, int stop, int step);
mp_obj_t mp_obj_new_range_iterator(int cur, int stop, int step);
mp_obj_t mp_obj_new_fun_bc(uint scope_flags, qstr *args, uint n_args, mp_obj_t def_args, const byte *code);
mp_obj_t mp_obj_new_fun_asm(uint n_args, void *fun);
mp_obj_t mp_obj_new_gen_wrap(mp_obj_t fun);
mp_obj_t mp_obj_new_closure(mp_obj_t fun, mp_obj_t closure_tuple);
mp_obj_t mp_obj_new_tuple(uint n, const mp_obj_t *items);
mp_obj_t mp_obj_new_list(uint n, mp_obj_t *items);
mp_obj_t mp_obj_new_dict(int n_args);
mp_obj_t mp_obj_new_set(int n_args, mp_obj_t *items);
mp_obj_t mp_obj_new_slice(mp_obj_t start, mp_obj_t stop, mp_obj_t step);
mp_obj_t mp_obj_new_super(mp_obj_t type, mp_obj_t obj);
mp_obj_t mp_obj_new_bound_meth(mp_obj_t meth, mp_obj_t self);
mp_obj_t mp_obj_new_getitem_iter(mp_obj_t *args);
mp_obj_t mp_obj_new_module(qstr module_name);

mp_obj_type_t *mp_obj_get_type(mp_obj_t o_in);
const char *mp_obj_get_type_str(mp_obj_t o_in);
bool mp_obj_is_subclass_fast(mp_const_obj_t object, mp_const_obj_t classinfo); // arguments should be type objects

void mp_obj_print_helper(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind);
void mp_obj_print(mp_obj_t o, mp_print_kind_t kind);
void mp_obj_print_exception(mp_obj_t exc);

int mp_obj_is_true(mp_obj_t arg);
bool mp_obj_is_callable(mp_obj_t o_in);
machine_int_t mp_obj_hash(mp_obj_t o_in);
bool mp_obj_equal(mp_obj_t o1, mp_obj_t o2);
bool mp_obj_less(mp_obj_t o1, mp_obj_t o2);

machine_int_t mp_obj_get_int(mp_obj_t arg);
bool mp_obj_get_int_maybe(mp_obj_t arg, machine_int_t *value);
#if MICROPY_ENABLE_FLOAT
mp_float_t mp_obj_get_float(mp_obj_t self_in);
void mp_obj_get_complex(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag);
#endif
//qstr mp_obj_get_qstr(mp_obj_t arg);
void mp_obj_get_array(mp_obj_t o, uint *len, mp_obj_t **items);
void mp_obj_get_array_fixed_n(mp_obj_t o, uint len, mp_obj_t **items);
uint mp_get_index(const mp_obj_type_t *type, machine_uint_t len, mp_obj_t index, bool is_slice);
mp_obj_t mp_obj_len_maybe(mp_obj_t o_in); /* may return NULL */

// bool
#define MP_BOOL(x) (x ? mp_const_true : mp_const_false)

// cell
mp_obj_t mp_obj_cell_get(mp_obj_t self_in);
void mp_obj_cell_set(mp_obj_t self_in, mp_obj_t obj);

// int
// For long int, returns value truncated to machine_int_t
machine_int_t mp_obj_int_get(mp_obj_t self_in);
#if MICROPY_ENABLE_FLOAT
mp_float_t mp_obj_int_as_float(mp_obj_t self_in);
#endif
// Will rains exception if value doesn't fit into machine_int_t
machine_int_t mp_obj_int_get_checked(mp_obj_t self_in);

// exception
bool mp_obj_is_exception_type(mp_obj_t self_in);
bool mp_obj_is_exception_instance(mp_obj_t self_in);
bool mp_obj_exception_match(mp_obj_t exc, const mp_obj_type_t *exc_type);
void mp_obj_exception_clear_traceback(mp_obj_t self_in);
void mp_obj_exception_add_traceback(mp_obj_t self_in, qstr file, machine_uint_t line, qstr block);
void mp_obj_exception_get_traceback(mp_obj_t self_in, machine_uint_t *n, machine_uint_t **values);
mp_obj_t mp_obj_exception_get_value(mp_obj_t self_in);

// str
mp_obj_t mp_obj_str_builder_start(const mp_obj_type_t *type, uint len, byte **data);
mp_obj_t mp_obj_str_builder_end(mp_obj_t o_in);
bool mp_obj_str_equal(mp_obj_t s1, mp_obj_t s2);
uint mp_obj_str_get_hash(mp_obj_t self_in);
uint mp_obj_str_get_len(mp_obj_t self_in);
qstr mp_obj_str_get_qstr(mp_obj_t self_in); // use this if you will anyway convert the string to a qstr
const char *mp_obj_str_get_str(mp_obj_t self_in); // use this only if you need the string to be null terminated
const char *mp_obj_str_get_data(mp_obj_t self_in, uint *len);
void mp_str_print_quoted(void (*print)(void *env, const char *fmt, ...), void *env, const byte *str_data, uint str_len);

#if MICROPY_ENABLE_FLOAT
// float
typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;
mp_float_t mp_obj_float_get(mp_obj_t self_in);
mp_obj_t mp_obj_float_binary_op(int op, mp_float_t lhs_val, mp_obj_t rhs);

// complex
void mp_obj_complex_get(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag);
mp_obj_t mp_obj_complex_binary_op(int op, mp_float_t lhs_real, mp_float_t lhs_imag, mp_obj_t rhs_in);
#endif

// tuple
void mp_obj_tuple_get(mp_obj_t self_in, uint *len, mp_obj_t **items);
void mp_obj_tuple_del(mp_obj_t self_in);
machine_int_t mp_obj_tuple_hash(mp_obj_t self_in);
mp_obj_t mp_obj_tuple_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args);

// list
mp_obj_t mp_obj_list_append(mp_obj_t self_in, mp_obj_t arg);
void mp_obj_list_get(mp_obj_t self_in, uint *len, mp_obj_t **items);
void mp_obj_list_store(mp_obj_t self_in, mp_obj_t index, mp_obj_t value);
mp_obj_t mp_obj_list_sort(uint n_args, const mp_obj_t *args, mp_map_t *kwargs);

// dict
typedef struct _mp_obj_dict_t {
    mp_obj_base_t base;
    mp_map_t map;
} mp_obj_dict_t;
uint mp_obj_dict_len(mp_obj_t self_in);
mp_obj_t mp_obj_dict_store(mp_obj_t self_in, mp_obj_t key, mp_obj_t value);
mp_map_t *mp_obj_dict_get_map(mp_obj_t self_in);

// set
void mp_obj_set_store(mp_obj_t self_in, mp_obj_t item);

// slice
void mp_obj_slice_get(mp_obj_t self_in, machine_int_t *start, machine_int_t *stop, machine_int_t *step);

// array
uint mp_obj_array_len(mp_obj_t self_in);
mp_obj_t mp_obj_new_bytearray_by_ref(uint n, void *items);

// functions
#define MP_OBJ_FUN_ARGS_MAX (0xffff) // to set maximum value in n_args_max below
typedef struct _mp_obj_fun_native_t { // need this so we can define const objects (to go in ROM)
    mp_obj_base_t base;
    bool is_kw : 1;
    uint n_args_min : 15; // inclusive
    uint n_args_max : 16; // inclusive
    void *fun;
    // TODO add mp_map_t *globals
    // for const function objects, make an empty, const map
    // such functions won't be able to access the global scope, but that's probably okay
} mp_obj_fun_native_t;

void mp_obj_fun_bc_get(mp_obj_t self_in, int *n_args, const byte **code);
bool mp_obj_fun_prepare_simple_args(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args,
                            uint *out_args1_len, const mp_obj_t **out_args1, uint *out_args2_len, const mp_obj_t **out_args2);

mp_obj_t mp_identity(mp_obj_t self);
MP_DECLARE_CONST_FUN_OBJ(mp_identity_obj);

// module
typedef struct _mp_obj_module_t {
    mp_obj_base_t base;
    qstr name;
    mp_map_t *globals;
} mp_obj_module_t;
mp_map_t *mp_obj_module_get_globals(mp_obj_t self_in);

// staticmethod and classmethod types; defined here so we can make const versions
// this structure is used for instances of both staticmethod and classmethod
typedef struct _mp_obj_static_class_method_t {
    mp_obj_base_t base;
    mp_obj_t fun;
} mp_obj_static_class_method_t;

// sequence helpers
void mp_seq_multiply(const void *items, uint item_sz, uint len, uint times, void *dest);
bool m_seq_get_fast_slice_indexes(machine_uint_t len, mp_obj_t slice, machine_uint_t *begin, machine_uint_t *end);
#define m_seq_copy(dest, src, len, item_t) memcpy(dest, src, len * sizeof(item_t))
#define m_seq_cat(dest, src1, len1, src2, len2, item_t) { memcpy(dest, src1, (len1) * sizeof(item_t)); memcpy(dest + (len1), src2, (len2) * sizeof(item_t)); }
bool mp_seq_cmp_bytes(int op, const byte *data1, uint len1, const byte *data2, uint len2);
bool mp_seq_cmp_objs(int op, const mp_obj_t *items1, uint len1, const mp_obj_t *items2, uint len2);
mp_obj_t mp_seq_index_obj(const mp_obj_t *items, uint len, uint n_args, const mp_obj_t *args);
mp_obj_t mp_seq_count_obj(const mp_obj_t *items, uint len, mp_obj_t value);
