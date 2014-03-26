// Micro Python byte-codes.
// The comment at the end of the line (if it exists) tells the arguments to the byte-code.

#define MP_BC_LOAD_CONST_FALSE   (0x10)
#define MP_BC_LOAD_CONST_NONE    (0x11)
#define MP_BC_LOAD_CONST_TRUE    (0x12)
#define MP_BC_LOAD_CONST_ELLIPSIS    (0x13)
#define MP_BC_LOAD_CONST_SMALL_INT   (0x14) // 24-bit, in excess
#define MP_BC_LOAD_CONST_INT     (0x15) // qstr
#define MP_BC_LOAD_CONST_DEC     (0x16) // qstr
#define MP_BC_LOAD_CONST_ID      (0x17) // qstr
#define MP_BC_LOAD_CONST_BYTES   (0x18) // qstr
#define MP_BC_LOAD_CONST_STRING  (0x19) // qstr

#define MP_BC_LOAD_FAST_0        (0x20)
#define MP_BC_LOAD_FAST_1        (0x21)
#define MP_BC_LOAD_FAST_2        (0x22)
#define MP_BC_LOAD_FAST_N        (0x23) // uint
#define MP_BC_LOAD_DEREF         (0x24) // uint
#define MP_BC_LOAD_NAME          (0x25) // qstr
#define MP_BC_LOAD_GLOBAL        (0x26) // qstr
#define MP_BC_LOAD_ATTR          (0x27) // qstr
#define MP_BC_LOAD_METHOD        (0x28) // qstr
#define MP_BC_LOAD_BUILD_CLASS   (0x29)

#define MP_BC_STORE_FAST_0       (0x30)
#define MP_BC_STORE_FAST_1       (0x31)
#define MP_BC_STORE_FAST_2       (0x32)
#define MP_BC_STORE_FAST_N       (0x33) // uint
#define MP_BC_STORE_DEREF        (0x34) // uint
#define MP_BC_STORE_NAME         (0x35) // qstr
#define MP_BC_STORE_GLOBAL       (0x36) // qstr
#define MP_BC_STORE_ATTR         (0x37) // qstr
#define MP_BC_STORE_SUBSCR       (0x38)

#define MP_BC_DELETE_FAST_N      (0x39) // uint
#define MP_BC_DELETE_DEREF       (0x3a) // uint
#define MP_BC_DELETE_NAME        (0x3b) // qstr
#define MP_BC_DELETE_GLOBAL      (0x3c) // qstr
#define MP_BC_DELETE_ATTR        (0x3d) // qstr
#define MP_BC_DELETE_SUBSCR      (0x3e)

#define MP_BC_DUP_TOP            (0x40)
#define MP_BC_DUP_TOP_TWO        (0x41)
#define MP_BC_POP_TOP            (0x42)
#define MP_BC_ROT_TWO            (0x43)
#define MP_BC_ROT_THREE          (0x44)

#define MP_BC_JUMP               (0x45) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_POP_JUMP_IF_TRUE   (0x46) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_POP_JUMP_IF_FALSE  (0x47) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_JUMP_IF_TRUE_OR_POP    (0x48) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_JUMP_IF_FALSE_OR_POP   (0x49) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_SETUP_LOOP         (0x4a) // rel byte code offset, 16-bit unsigned
#define MP_BC_SETUP_WITH         (0x4d) // rel byte code offset, 16-bit unsigned
#define MP_BC_WITH_CLEANUP       (0x4e)
#define MP_BC_SETUP_EXCEPT       (0x4f) // rel byte code offset, 16-bit unsigned
#define MP_BC_SETUP_FINALLY      (0x50) // rel byte code offset, 16-bit unsigned
#define MP_BC_END_FINALLY        (0x51)
#define MP_BC_GET_ITER           (0x52)
#define MP_BC_FOR_ITER           (0x53) // rel byte code offset, 16-bit unsigned
#define MP_BC_POP_BLOCK          (0x54)
#define MP_BC_POP_EXCEPT         (0x55)
#define MP_BC_UNWIND_JUMP        (0x56) // rel byte code offset, 16-bit signed, in excess; then a byte

#define MP_BC_NOT                (0x60)
#define MP_BC_UNARY_OP           (0x61) // byte
#define MP_BC_BINARY_OP          (0x62) // byte

#define MP_BC_BUILD_TUPLE        (0x70) // uint
#define MP_BC_BUILD_LIST         (0x71) // uint
#define MP_BC_LIST_APPEND        (0x72) // uint
#define MP_BC_BUILD_MAP          (0x73) // uint
#define MP_BC_STORE_MAP          (0x74)
#define MP_BC_MAP_ADD            (0x75) // uint
#define MP_BC_BUILD_SET          (0x76) // uint
#define MP_BC_SET_ADD            (0x77) // uint
#define MP_BC_BUILD_SLICE        (0x78) // uint
#define MP_BC_UNPACK_SEQUENCE    (0x79) // uint
#define MP_BC_UNPACK_EX          (0x7a) // uint

#define MP_BC_RETURN_VALUE       (0x80)
#define MP_BC_RAISE_VARARGS      (0x81) // byte
#define MP_BC_YIELD_VALUE        (0x82)
#define MP_BC_YIELD_FROM         (0x83)

#define MP_BC_MAKE_FUNCTION      (0x90) // uint
#define MP_BC_MAKE_CLOSURE       (0x91) // uint
#define MP_BC_CALL_FUNCTION      (0x92) // uint
#define MP_BC_CALL_FUNCTION_VAR  (0x93) // uint
#define MP_BC_CALL_FUNCTION_KW   (0x94) // uint
#define MP_BC_CALL_FUNCTION_VAR_KW   (0x95) // uint
#define MP_BC_CALL_METHOD        (0x96) // uint
#define MP_BC_CALL_METHOD_VAR    (0x97) // uint
#define MP_BC_CALL_METHOD_KW     (0x98) // uint
#define MP_BC_CALL_METHOD_VAR_KW (0x99) // uint
#define MP_BC_MAKE_FUNCTION_DEFARGS  (0x9a) // uint
#define MP_BC_MAKE_CLOSURE_DEFARGS   (0x9b) // uint

#define MP_BC_IMPORT_NAME        (0xe0) // qstr
#define MP_BC_IMPORT_FROM        (0xe1) // qstr
#define MP_BC_IMPORT_STAR        (0xe2)

