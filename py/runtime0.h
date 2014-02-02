typedef enum {
    RT_UNARY_OP_BOOL, // __bool__
    RT_UNARY_OP_LEN, // __len__
    RT_UNARY_OP_POSITIVE,
    RT_UNARY_OP_NEGATIVE,
    RT_UNARY_OP_INVERT,
    // these are not supported by the runtime and must be synthesised by the emitter
    RT_UNARY_OP_NOT,
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
    // these should return a bool
    RT_BINARY_OP_LESS,
    RT_BINARY_OP_MORE,
    RT_BINARY_OP_EQUAL,
    RT_BINARY_OP_LESS_EQUAL,
    RT_BINARY_OP_MORE_EQUAL,
    RT_BINARY_OP_NOT_EQUAL,
    RT_BINARY_OP_IN,
    RT_BINARY_OP_IS,
    RT_BINARY_OP_EXCEPTION_MATCH,
    // these are not supported by the runtime and must be synthesised by the emitter
    RT_BINARY_OP_NOT_IN,
    RT_BINARY_OP_IS_NOT,
} rt_binary_op_t;

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
    RT_F_BINARY_OP,
    RT_F_BUILD_TUPLE,
    RT_F_BUILD_LIST,
    RT_F_LIST_APPEND,
    RT_F_BUILD_MAP,
    RT_F_STORE_MAP,
    RT_F_BUILD_SET,
    RT_F_STORE_SET,
    RT_F_MAKE_FUNCTION_FROM_ID,
    RT_F_CALL_FUNCTION_N_KW_FOR_NATIVE,
    RT_F_CALL_METHOD_N_KW,
    RT_F_GETITER,
    RT_F_ITERNEXT,
    RT_F_NUMBER_OF,
} rt_fun_kind_t;

extern void *const rt_fun_table[RT_F_NUMBER_OF];

void rt_init(void);
void rt_deinit(void);
uint rt_get_unique_code_id(void);
void rt_assign_byte_code(uint unique_code_id, byte *code, uint len, int n_args, int n_locals, int n_stack, bool is_generator);
void rt_assign_native_code(uint unique_code_id, void *f, uint len, int n_args);
void rt_assign_inline_asm_code(uint unique_code_id, void *f, uint len, int n_args);
