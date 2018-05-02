// x86 specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_X86

// This is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmx86.h"

// x86 needs a table to know how many args a given function has
STATIC byte mp_f_n_args[MP_F_NUMBER_OF] = {
    [MP_F_CONVERT_OBJ_TO_NATIVE] = 2,
    [MP_F_CONVERT_NATIVE_TO_OBJ] = 2,
    [MP_F_LOAD_NAME] = 1,
    [MP_F_LOAD_GLOBAL] = 1,
    [MP_F_LOAD_BUILD_CLASS] = 0,
    [MP_F_LOAD_ATTR] = 2,
    [MP_F_LOAD_METHOD] = 3,
    [MP_F_LOAD_SUPER_METHOD] = 2,
    [MP_F_STORE_NAME] = 2,
    [MP_F_STORE_GLOBAL] = 2,
    [MP_F_STORE_ATTR] = 3,
    [MP_F_OBJ_SUBSCR] = 3,
    [MP_F_OBJ_IS_TRUE] = 1,
    [MP_F_UNARY_OP] = 2,
    [MP_F_BINARY_OP] = 3,
    [MP_F_BUILD_TUPLE] = 2,
    [MP_F_BUILD_LIST] = 2,
    [MP_F_LIST_APPEND] = 2,
    [MP_F_BUILD_MAP] = 1,
    [MP_F_STORE_MAP] = 3,
    #if MICROPY_PY_BUILTINS_SET
    [MP_F_BUILD_SET] = 2,
    [MP_F_STORE_SET] = 2,
    #endif
    [MP_F_MAKE_FUNCTION_FROM_RAW_CODE] = 3,
    [MP_F_NATIVE_CALL_FUNCTION_N_KW] = 3,
    [MP_F_CALL_METHOD_N_KW] = 3,
    [MP_F_CALL_METHOD_N_KW_VAR] = 3,
    [MP_F_NATIVE_GETITER] = 2,
    [MP_F_NATIVE_ITERNEXT] = 1,
    [MP_F_NLR_PUSH] = 1,
    [MP_F_NLR_POP] = 0,
    [MP_F_NATIVE_RAISE] = 1,
    [MP_F_IMPORT_NAME] = 3,
    [MP_F_IMPORT_FROM] = 2,
    [MP_F_IMPORT_ALL] = 1,
    #if MICROPY_PY_BUILTINS_SLICE
    [MP_F_NEW_SLICE] = 3,
    #endif
    [MP_F_UNPACK_SEQUENCE] = 3,
    [MP_F_UNPACK_EX] = 3,
    [MP_F_DELETE_NAME] = 1,
    [MP_F_DELETE_GLOBAL] = 1,
    [MP_F_NEW_CELL] = 1,
    [MP_F_MAKE_CLOSURE_FROM_RAW_CODE] = 3,
    [MP_F_SETUP_CODE_STATE] = 5,
    [MP_F_SMALL_INT_FLOOR_DIVIDE] = 2,
    [MP_F_SMALL_INT_MODULO] = 2,
};

#define N_X86 (1)
#define EXPORT_FUN(name) emit_native_x86_##name
#include "py/emitnative.c"

#endif
