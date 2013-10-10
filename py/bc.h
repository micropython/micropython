#define PYBC_LOAD_CONST_FALSE   (0x10)
#define PYBC_LOAD_CONST_NONE    (0x11)
#define PYBC_LOAD_CONST_TRUE    (0x12)
#define PYBC_LOAD_CONST_SMALL_INT   (0x13) // int
#define PYBC_LOAD_CONST_INT     (0x14) // qstr
#define PYBC_LOAD_CONST_DEC     (0x15) // qstr
#define PYBC_LOAD_CONST_ID      (0x16) // qstr
#define PYBC_LOAD_CONST_BYTES   (0x17) // qstr
#define PYBC_LOAD_CONST_STRING  (0x18) // qstr

#define PYBC_LOAD_FAST_0        (0x20)
#define PYBC_LOAD_FAST_1        (0x21)
#define PYBC_LOAD_FAST_2        (0x22)
#define PYBC_LOAD_FAST_N        (0x23) // uint
#define PYBC_LOAD_NAME          (0x24) // qstr
#define PYBC_LOAD_GLOBAL        (0x25) // qstr
#define PYBC_LOAD_ATTR          (0x26) // qstr
#define PYBC_LOAD_METHOD        (0x27) // qstr
#define PYBC_LOAD_BUILD_CLASS   (0x28)

#define PYBC_STORE_FAST_0       (0x30)
#define PYBC_STORE_FAST_1       (0x31)
#define PYBC_STORE_FAST_2       (0x32)
#define PYBC_STORE_FAST_N       (0x33) // uint
#define PYBC_STORE_NAME         (0x34) // qstr
#define PYBC_STORE_GLOBAL       (0x35) // qstr
#define PYBC_STORE_ATTR         (0x36) // qstr
#define PYBC_STORE_SUBSCR       (0x37)

#define PYBC_DELETE_FAST_N      (0x39) // uint
#define PYBC_DELETE_NAME        (0x3a) // qstr
#define PYBC_DELETE_GLOBAL      (0x3b) // qstr
#define PYBC_DELETE_DEREF       (0x3c) // qstr
#define PYBC_DELETE_ATTR        (0x3d) // qstr
#define PYBC_DELETE_SUBSCR      (0x3e)

#define PYBC_DUP_TOP            (0x40)
#define PYBC_DUP_TOP_TWO        (0x41)
#define PYBC_POP_TOP            (0x42)
#define PYBC_ROT_TWO            (0x43)
#define PYBC_ROT_THREE          (0x44)
#define PYBC_JUMP               (0x45) // pos
#define PYBC_POP_JUMP_IF_TRUE   (0x46) // pos
#define PYBC_POP_JUMP_IF_FALSE  (0x47) // pos
#define PYBC_JUMP_IF_TRUE_OR_POP    (0x48) // pos
#define PYBC_JUMP_IF_FALSE_OR_POP   (0x49) // pos
#define PYBC_SETUP_LOOP         (0x4a) // pos
#define PYBC_BREAK_LOOP         (0x4b) // pos
#define PYBC_CONTINUE_LOOP      (0x4c) // pos
#define PYBC_SETUP_WITH         (0x4d) // pos
#define PYBC_WITH_CLEANUP       (0x4e)
#define PYBC_SETUP_EXCEPT       (0x4f) // pos
#define PYBC_SETUP_FINALLY      (0x50) // pos
#define PYBC_END_FINALLY        (0x51)
#define PYBC_GET_ITER           (0x52)
#define PYBC_FOR_ITER           (0x53) // pos
#define PYBC_POP_BLOCK          (0x54)
#define PYBC_POP_EXCEPT         (0x55)

#define PYBC_UNARY_OP           (0x60) // byte
#define PYBC_BINARY_OP          (0x61) // byte
#define PYBC_COMPARE_OP         (0x62) // byte

#define PYBC_BUILD_TUPLE        (0x70) // uint
#define PYBC_BUILD_LIST         (0x71) // uint
#define PYBC_LIST_APPEND        (0x72) // uint
#define PYBC_BUILD_MAP          (0x73) // uint
#define PYBC_STORE_MAP          (0x74)
#define PYBC_MAP_ADD            (0x75) // uint
#define PYBC_BUILD_SET          (0x76) // uint
#define PYBC_SET_ADD            (0x77) // uint
#define PYBC_BUILD_SLICE        (0x78) // uint
#define PYBC_UNPACK_SEQUENCE    (0x79) // uint
#define PYBC_UNPACK_EX          (0x7a) // uint

#define PYBC_RETURN_VALUE       (0x80)
#define PYBC_RAISE_VARARGS      (0x81) // uint
#define PYBC_YIELD_VALUE        (0x82)
#define PYBC_YIELD_FROM         (0x83)

#define PYBC_MAKE_FUNCTION      (0x90) // uint
#define PYBC_MAKE_CLOSURE       (0x91) // uint?
#define PYBC_CALL_FUNCTION      (0x92) // uint
#define PYBC_CALL_FUNCTION_VAR  (0x93) // uint
#define PYBC_CALL_FUNCTION_KW   (0x94) // uint
#define PYBC_CALL_FUNCTION_VAR_KW   (0x95) // uint
#define PYBC_CALL_METHOD        (0x96) // uint
#define PYBC_CALL_METHOD_VAR    (0x97) // uint
#define PYBC_CALL_METHOD_KW     (0x98) // uint
#define PYBC_CALL_METHOD_VAR_KW (0x99) // uint

#define PYBC_IMPORT_NAME (0xe0)
#define PYBC_IMPORT_FROM (0xe1)
#define PYBC_IMPORT_STAR (0xe2)

py_obj_t py_execute_byte_code(const byte *code, uint len, const py_obj_t *args, uint n_args);
