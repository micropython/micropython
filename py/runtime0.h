// taken from python source, Include/code.h
#define MP_SCOPE_FLAG_OPTIMISED    0x01
#define MP_SCOPE_FLAG_NEWLOCALS    0x02
#define MP_SCOPE_FLAG_VARARGS      0x04
#define MP_SCOPE_FLAG_VARKEYWORDS  0x08
#define MP_SCOPE_FLAG_NESTED       0x10
#define MP_SCOPE_FLAG_GENERATOR    0x20
/* The MP_SCOPE_FLAG_NOFREE flag is set if there are no free or cell variables.
   This information is redundant, but it allows a single flag test
   to determine whether there is any extra work to be done when the
   call frame is setup.
*/
#define MP_SCOPE_FLAG_NOFREE       0x40

typedef enum {
    MP_UNARY_OP_BOOL, // __bool__
    MP_UNARY_OP_LEN, // __len__
    MP_UNARY_OP_POSITIVE,
    MP_UNARY_OP_NEGATIVE,
    MP_UNARY_OP_INVERT,
    // these are not supported by the runtime and must be synthesised by the emitter
    MP_UNARY_OP_NOT,
} mp_unary_op_t;

typedef enum {
    MP_BINARY_OP_SUBSCR,
    MP_BINARY_OP_OR,
    MP_BINARY_OP_XOR,
    MP_BINARY_OP_AND,
    MP_BINARY_OP_LSHIFT,
    MP_BINARY_OP_RSHIFT,
    MP_BINARY_OP_ADD,
    MP_BINARY_OP_SUBTRACT,
    MP_BINARY_OP_MULTIPLY,
    MP_BINARY_OP_FLOOR_DIVIDE,
    MP_BINARY_OP_TRUE_DIVIDE,
    MP_BINARY_OP_MODULO,
    MP_BINARY_OP_POWER,
    MP_BINARY_OP_INPLACE_OR,
    MP_BINARY_OP_INPLACE_XOR,
    MP_BINARY_OP_INPLACE_AND,
    MP_BINARY_OP_INPLACE_LSHIFT,
    MP_BINARY_OP_INPLACE_RSHIFT,
    MP_BINARY_OP_INPLACE_ADD,
    MP_BINARY_OP_INPLACE_SUBTRACT,
    MP_BINARY_OP_INPLACE_MULTIPLY,
    MP_BINARY_OP_INPLACE_FLOOR_DIVIDE,
    MP_BINARY_OP_INPLACE_TRUE_DIVIDE,
    MP_BINARY_OP_INPLACE_MODULO,
    MP_BINARY_OP_INPLACE_POWER,
    // these should return a bool
    MP_BINARY_OP_LESS,
    MP_BINARY_OP_MORE,
    MP_BINARY_OP_EQUAL,
    MP_BINARY_OP_LESS_EQUAL,
    MP_BINARY_OP_MORE_EQUAL,
    MP_BINARY_OP_NOT_EQUAL,
    MP_BINARY_OP_IN,
    MP_BINARY_OP_IS,
    MP_BINARY_OP_EXCEPTION_MATCH,
    // these are not supported by the runtime and must be synthesised by the emitter
    MP_BINARY_OP_NOT_IN,
    MP_BINARY_OP_IS_NOT,
} mp_binary_op_t;

typedef enum {
    MP_F_LOAD_CONST_DEC = 0,
    MP_F_LOAD_CONST_STR,
    MP_F_LOAD_NAME,
    MP_F_LOAD_GLOBAL,
    MP_F_LOAD_BUILD_CLASS,
    MP_F_LOAD_ATTR,
    MP_F_LOAD_METHOD,
    MP_F_STORE_NAME,
    MP_F_STORE_ATTR,
    MP_F_STORE_SUBSCR,
    MP_F_OBJ_IS_TRUE,
    MP_F_UNARY_OP,
    MP_F_BINARY_OP,
    MP_F_BUILD_TUPLE,
    MP_F_BUILD_LIST,
    MP_F_LIST_APPEND,
    MP_F_BUILD_MAP,
    MP_F_STORE_MAP,
    MP_F_BUILD_SET,
    MP_F_STORE_SET,
    MP_F_MAKE_FUNCTION_FROM_ID,
    MP_F_CALL_FUNCTION_N_KW_FOR_NATIVE,
    MP_F_CALL_METHOD_N_KW,
    MP_F_GETITER,
    MP_F_ITERNEXT,
    MP_F_NUMBER_OF,
} mp_fun_kind_t;

extern void *const mp_fun_table[MP_F_NUMBER_OF];
