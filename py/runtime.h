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
    RT_F_LOAD_CONST_STR = 0,
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
    RT_F_BUILD_LIST,
    RT_F_BUILD_MAP,
    RT_F_STORE_MAP,
    RT_F_BUILD_SET,
    RT_F_MAKE_FUNCTION_FROM_ID,
    RT_F_CALL_FUNCTION_0,
    RT_F_CALL_FUNCTION_1,
    RT_F_CALL_FUNCTION_2,
    RT_F_CALL_METHOD_1,
    RT_F_CALL_METHOD_2,
    RT_F_CALL_METHOD_N,
    RT_F_BINARY_OP,
    RT_F_COMPARE_OP,
    RT_F_NUMBER_OF,
} rt_fun_kind_t;

extern void *rt_fun_table[RT_F_NUMBER_OF];

typedef machine_ptr_t py_obj_t; // must be of pointer size
typedef py_obj_t (*py_fun_0_t)();
typedef py_obj_t (*py_fun_1_t)(py_obj_t);
typedef py_obj_t (*py_fun_2_t)(py_obj_t, py_obj_t);
typedef py_obj_t (*py_fun_t)();

extern py_obj_t py_const_none;
extern py_obj_t py_const_false;
extern py_obj_t py_const_true;

void rt_init();
void rt_deinit();
int rt_get_new_unique_code_id();
void rt_assign_byte_code(int unique_code_id, byte *code, uint len, int n_args);
void rt_assign_native_code(int unique_code_id, py_fun_t f, uint len, int n_args);
void rt_assign_inline_asm_code(int unique_code_id, py_fun_t f, uint len, int n_args);
py_fun_t rt_get_code(qstr id);
void py_obj_print(py_obj_t o);
int rt_is_true(py_obj_t arg);
int rt_get_int(py_obj_t arg);
py_obj_t rt_load_const_str(qstr qstr);
//py_obj_t rt_load_const_code(qstr qstr);
py_obj_t rt_load_name(qstr qstr);
py_obj_t rt_load_global(qstr qstr);
py_obj_t rt_load_build_class();
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
py_obj_t rt_call_function_0(py_obj_t fun);
py_obj_t rt_call_function_1(py_obj_t fun, py_obj_t arg);
py_obj_t rt_call_function_2(py_obj_t fun, py_obj_t arg1, py_obj_t arg2);
py_obj_t rt_call_method_1(py_obj_t fun, py_obj_t self);
py_obj_t rt_call_method_2(py_obj_t fun, py_obj_t self, py_obj_t arg);
py_obj_t rt_call_method_n(int n_args, const py_obj_t *args);
py_obj_t rt_build_list(int n_args, py_obj_t *items);
py_obj_t rt_build_map(int n_args);
py_obj_t rt_store_map(py_obj_t map, py_obj_t key, py_obj_t value);
py_obj_t rt_build_set(int n_args, py_obj_t *items);
py_obj_t rt_load_attr(py_obj_t base, qstr attr);
void rt_load_method(py_obj_t base, qstr attr, py_obj_t *dest);
void rt_store_attr(py_obj_t base, qstr attr, py_obj_t val);
void rt_store_subscr(py_obj_t base, py_obj_t index, py_obj_t val);
