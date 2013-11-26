typedef enum {
    RT_UNARY_OP_NOT,
    RT_UNARY_OP_POSITIVE,
    RT_UNARY_OP_NEGATIVE,
    RT_UNARY_OP_INVERT,
} rt_unary_op_t;

typedef enum {
    RT_BINARY_OP_SUBSCR,
    RT_BINARY_OP_OR,
    RT_BINARY_OP_XOR,
    RT_BINARY_OP_AND,
    RT_BINARY_OP_LSHIFT,
    RT_BINARY_OP_RSHIFT,
    RT_BINARY_OP_ADD,
    RT_BINARY_OP_SUBTRACT,
    RT_BINARY_OP_MULTIPLY,
    RT_BINARY_OP_FLOOR_DIVIDE,
    RT_BINARY_OP_TRUE_DIVIDE,
    RT_BINARY_OP_MODULO,
    RT_BINARY_OP_POWER,
    RT_BINARY_OP_INPLACE_OR,
    RT_BINARY_OP_INPLACE_XOR,
    RT_BINARY_OP_INPLACE_AND,
    RT_BINARY_OP_INPLACE_LSHIFT,
    RT_BINARY_OP_INPLACE_RSHIFT,
    RT_BINARY_OP_INPLACE_ADD,
    RT_BINARY_OP_INPLACE_SUBTRACT,
    RT_BINARY_OP_INPLACE_MULTIPLY,
    RT_BINARY_OP_INPLACE_FLOOR_DIVIDE,
    RT_BINARY_OP_INPLACE_TRUE_DIVIDE,
    RT_BINARY_OP_INPLACE_MODULO,
    RT_BINARY_OP_INPLACE_POWER,
} rt_binary_op_t;

typedef enum {
    RT_COMPARE_OP_LESS,
    RT_COMPARE_OP_MORE,
    RT_COMPARE_OP_EQUAL,
    RT_COMPARE_OP_LESS_EQUAL,
    RT_COMPARE_OP_MORE_EQUAL,
    RT_COMPARE_OP_NOT_EQUAL,
    RT_COMPARE_OP_IN,
    RT_COMPARE_OP_NOT_IN,
    RT_COMPARE_OP_IS,
    RT_COMPARE_OP_IS_NOT,
    RT_COMPARE_OP_EXCEPTION_MATCH,
} rt_compare_op_t;

typedef enum {
    RT_F_LOAD_CONST_DEC = 0,
    RT_F_LOAD_CONST_STR,
    RT_F_LOAD_NAME,
    RT_F_LOAD_GLOBAL,
    RT_F_LOAD_BUILD_CLASS,
    RT_F_LOAD_ATTR,
    RT_F_LOAD_METHOD,
    RT_F_STORE_NAME,
    RT_F_STORE_ATTR,
    RT_F_STORE_SUBSCR,
    RT_F_IS_TRUE,
    RT_F_UNARY_OP,
    RT_F_BUILD_TUPLE,
    RT_F_BUILD_LIST,
    RT_F_LIST_APPEND,
    RT_F_BUILD_MAP,
    RT_F_STORE_MAP,
    RT_F_BUILD_SET,
    RT_F_STORE_SET,
    RT_F_MAKE_FUNCTION_FROM_ID,
    RT_F_CALL_FUNCTION_N,
    RT_F_CALL_METHOD_N,
    RT_F_BINARY_OP,
    RT_F_COMPARE_OP,
    RT_F_GETITER,
    RT_F_ITERNEXT,
    RT_F_NUMBER_OF,
} rt_fun_kind_t;

extern void *const rt_fun_table[RT_F_NUMBER_OF];

typedef machine_ptr_t py_obj_t; // must be of pointer size
typedef py_obj_t (*py_fun_0_t)(void);
typedef py_obj_t (*py_fun_1_t)(py_obj_t);
typedef py_obj_t (*py_fun_2_t)(py_obj_t, py_obj_t);
typedef py_obj_t (*py_fun_t)(void);
typedef py_obj_t (*py_fun_var_t)(int n, const py_obj_t *);

extern py_obj_t py_const_none;
extern py_obj_t py_const_false;
extern py_obj_t py_const_true;
extern py_obj_t py_const_stop_iteration; // special object indicating end of iteration (not StopIteration exception!)

void rt_init(void);
void rt_deinit(void);
int rt_get_unique_code_id(bool is_main_module);
void rt_assign_byte_code(int unique_code_id, byte *code, uint len, int n_args, int n_locals, int n_stack, bool is_generator);
void rt_assign_native_code(int unique_code_id, py_fun_t f, uint len, int n_args);
void rt_assign_inline_asm_code(int unique_code_id, py_fun_t f, uint len, int n_args);
void py_obj_print(py_obj_t o);

py_obj_t py_obj_new_int(machine_int_t value);
py_obj_t py_obj_new_str(qstr qstr);

int rt_is_true(py_obj_t arg);
machine_int_t py_obj_get_int(py_obj_t arg);
machine_float_t py_obj_get_float(py_obj_t arg);
qstr py_obj_get_qstr(py_obj_t arg);
py_obj_t *py_obj_get_array_fixed_n(py_obj_t o, machine_int_t n);

py_obj_t rt_load_const_dec(qstr qstr);
py_obj_t rt_load_const_str(qstr qstr);
py_obj_t rt_load_name(qstr qstr);
py_obj_t rt_load_global(qstr qstr);
py_obj_t rt_load_build_class(void);
void rt_store_name(qstr qstr, py_obj_t obj);
void rt_store_global(qstr qstr, py_obj_t obj);
py_obj_t rt_unary_op(int op, py_obj_t arg);
py_obj_t rt_binary_op(int op, py_obj_t lhs, py_obj_t rhs);
py_obj_t rt_compare_op(int op, py_obj_t lhs, py_obj_t rhs);
py_obj_t rt_make_function_from_id(int unique_code_id);
py_obj_t rt_make_function_0(py_fun_0_t f);
py_obj_t rt_make_function_1(py_fun_1_t f);
py_obj_t rt_make_function_2(py_fun_2_t f);
py_obj_t rt_make_function(int n_args, py_fun_t code);
py_obj_t rt_make_function_var(int n_fixed_args, py_fun_var_t f);
py_obj_t rt_call_function_0(py_obj_t fun);
py_obj_t rt_call_function_1(py_obj_t fun, py_obj_t arg);
py_obj_t rt_call_function_2(py_obj_t fun, py_obj_t arg1, py_obj_t arg2);
py_obj_t rt_call_function_n(py_obj_t fun, int n_args, const py_obj_t *args);
py_obj_t rt_call_function_n_kw(py_obj_t fun, uint n_args, uint n_kw, const py_obj_t *args);
py_obj_t rt_call_method_n(uint n_args, const py_obj_t *args);
py_obj_t rt_call_method_n_kw(uint n_args, uint n_kw, const py_obj_t *args);
py_obj_t rt_build_tuple(int n_args, py_obj_t *items);
py_obj_t rt_build_list(int n_args, py_obj_t *items);
py_obj_t rt_list_append(py_obj_t list, py_obj_t arg);
py_obj_t rt_build_set(int n_args, py_obj_t *items);
py_obj_t rt_store_set(py_obj_t set, py_obj_t item);
void rt_unpack_sequence(py_obj_t seq, uint num, py_obj_t *items);
py_obj_t rt_build_map(int n_args);
py_obj_t rt_store_map(py_obj_t map, py_obj_t key, py_obj_t value);
py_obj_t rt_load_attr(py_obj_t base, qstr attr);
void rt_load_method(py_obj_t base, qstr attr, py_obj_t *dest);
void rt_store_attr(py_obj_t base, qstr attr, py_obj_t val);
void rt_store_subscr(py_obj_t base, py_obj_t index, py_obj_t val);
py_obj_t rt_getiter(py_obj_t o);
py_obj_t rt_iternext(py_obj_t o);

// temporary way of making C modules
py_obj_t py_module_new(void);

// user defined objects

typedef struct _py_user_method_t {
    const char *name;
    machine_uint_t kind;
    void *fun;
} py_user_method_t;

typedef struct _py_user_info_t {
    const char *type_name;
    void (*print)(py_obj_t);
    const py_user_method_t methods[];
} py_user_info_t;

py_obj_t py_obj_new_user(const py_user_info_t *info, machine_uint_t data1, machine_uint_t data2);
void py_user_get_data(py_obj_t o, machine_uint_t *data1, machine_uint_t *data2);
void py_user_set_data(py_obj_t o, machine_uint_t data1, machine_uint_t data2);
