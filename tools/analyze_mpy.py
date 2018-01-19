import sys
import binascii
import io

bytecode_format_sizes = {
    "MP_OPCODE_BYTE": 1,
    "MP_OPCODE_QSTR": 3,
    "MP_OPCODE_VAR_UINT": None, # Unknown because uint encoding uses the top bit to indicate the end.
    "MP_OPCODE_OFFSET": 3,
    "MP_OPCODE_BYTE_EXTRA": 2,
    "MP_OPCODE_VAR_UINT_EXTRA": None,
    "MP_OPCODE_OFFSET_EXTRA": 4
}

bytecodes = {
    0x10: {"name": "MP_BC_LOAD_CONST_FALSE",
           "format": "MP_OPCODE_BYTE"},
#define MP_BC_LOAD_CONST_NONE    (0x11)
#define MP_BC_LOAD_CONST_TRUE    (0x12)
#define MP_BC_LOAD_CONST_SMALL_INT   (0x14) // signed var-int
#define MP_BC_LOAD_CONST_STRING  (0x16) // qstr
#define MP_BC_LOAD_CONST_OBJ     (0x17) // ptr
#define MP_BC_LOAD_NULL          (0x18)

#define MP_BC_LOAD_FAST_N        (0x19) // uint
#define MP_BC_LOAD_DEREF         (0x1a) // uint
#define MP_BC_LOAD_NAME          (0x1b) // qstr
#define MP_BC_LOAD_GLOBAL        (0x1c) // qstr
#define MP_BC_LOAD_ATTR          (0x1d) // qstr
#define MP_BC_LOAD_METHOD        (0x1e) // qstr
#define MP_BC_LOAD_SUPER_METHOD  (0x1f) // qstr
#define MP_BC_LOAD_BUILD_CLASS   (0x20)
#define MP_BC_LOAD_SUBSCR        (0x21)

#define MP_BC_STORE_FAST_N       (0x22) // uint
#define MP_BC_STORE_DEREF        (0x23) // uint
#define MP_BC_STORE_NAME         (0x24) // qstr
#define MP_BC_STORE_GLOBAL       (0x25) // qstr
#define MP_BC_STORE_ATTR         (0x26) // qstr
#define MP_BC_STORE_SUBSCR       (0x27)

#define MP_BC_DELETE_FAST        (0x28) // uint
#define MP_BC_DELETE_DEREF       (0x29) // uint
#define MP_BC_DELETE_NAME        (0x2a) // qstr
#define MP_BC_DELETE_GLOBAL      (0x2b) // qstr

#define MP_BC_DUP_TOP            (0x30)
#define MP_BC_DUP_TOP_TWO        (0x31)
#define MP_BC_POP_TOP            (0x32)
#define MP_BC_ROT_TWO            (0x33)
#define MP_BC_ROT_THREE          (0x34)

#define MP_BC_JUMP               (0x35) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_POP_JUMP_IF_TRUE   (0x36) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_POP_JUMP_IF_FALSE  (0x37) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_JUMP_IF_TRUE_OR_POP    (0x38) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_JUMP_IF_FALSE_OR_POP   (0x39) // rel byte code offset, 16-bit signed, in excess
#define MP_BC_SETUP_WITH         (0x3d) // rel byte code offset, 16-bit unsigned
#define MP_BC_WITH_CLEANUP       (0x3e)
#define MP_BC_SETUP_EXCEPT       (0x3f) // rel byte code offset, 16-bit unsigned
#define MP_BC_SETUP_FINALLY      (0x40) // rel byte code offset, 16-bit unsigned
#define MP_BC_END_FINALLY        (0x41)
#define MP_BC_GET_ITER           (0x42)
#define MP_BC_FOR_ITER           (0x43) // rel byte code offset, 16-bit unsigned
#define MP_BC_POP_BLOCK          (0x44)
#define MP_BC_POP_EXCEPT         (0x45)
#define MP_BC_UNWIND_JUMP        (0x46) // rel byte code offset, 16-bit signed, in excess; then a byte
#define MP_BC_GET_ITER_STACK     (0x47)

#define MP_BC_BUILD_TUPLE        (0x50) // uint
#define MP_BC_BUILD_LIST         (0x51) // uint
#define MP_BC_BUILD_MAP          (0x53) // uint
#define MP_BC_STORE_MAP          (0x54)
#define MP_BC_BUILD_SET          (0x56) // uint
#define MP_BC_BUILD_SLICE        (0x58) // uint
#define MP_BC_STORE_COMP         (0x57) // uint
#define MP_BC_UNPACK_SEQUENCE    (0x59) // uint
#define MP_BC_UNPACK_EX          (0x5a) // uint

#define MP_BC_RETURN_VALUE       (0x5b)
#define MP_BC_RAISE_VARARGS      (0x5c) // byte
#define MP_BC_YIELD_VALUE        (0x5d)
#define MP_BC_YIELD_FROM         (0x5e)

#define MP_BC_MAKE_FUNCTION         (0x60) // uint
#define MP_BC_MAKE_FUNCTION_DEFARGS (0x61) // uint
#define MP_BC_MAKE_CLOSURE          (0x62) // uint
#define MP_BC_MAKE_CLOSURE_DEFARGS  (0x63) // uint
#define MP_BC_CALL_FUNCTION         (0x64) // uint
#define MP_BC_CALL_FUNCTION_VAR_KW  (0x65) // uint
#define MP_BC_CALL_METHOD           (0x66) // uint
#define MP_BC_CALL_METHOD_VAR_KW    (0x67) // uint

#define MP_BC_IMPORT_NAME        (0x68) // qstr
#define MP_BC_IMPORT_FROM        (0x69) // qstr
#define MP_BC_IMPORT_STAR        (0x6a)

#define MP_BC_LOAD_CONST_SMALL_INT_MULTI (0x70) // + N(64)
#define MP_BC_LOAD_FAST_MULTI            (0xb0) // + N(16)
#define MP_BC_STORE_FAST_MULTI           (0xc0) // + N(16)
#define MP_BC_UNARY_OP_MULTI             (0xd0) // + op(<MP_UNARY_OP_NUM_BYTECODE)
#define MP_BC_BINARY_OP_MULTI            (0xd7) // + op(<MP_BINARY_OP_NUM_BYTECODE)
}

def read_uint(encoded_uint, peek=False):
    unum = 0
    i = 0
    while True:
        if peek:
            b = encoded_uint.peek()[i]
        else:
            b = encoded_uint.read(1)[0]
        unum = (unum << 7) | (b & 0x7f)
        if (b & 0x80) == 0:
            break
        i += 1
    return unum

class Prelude:
    def __init__(self, encoded_prelude):
            self.n_state = read_uint(encoded_prelude)
            self.n_exc_stack = read_uint(encoded_prelude)
            self.scope_flags = encoded_prelude.read(1)[0]
            self.n_pos_args = encoded_prelude.read(1)[0]
            self.n_kwonly_args = encoded_prelude.read(1)[0]
            self.n_def_pos_args = encoded_prelude.read(1)[0]
            self.code_info_size = read_uint(encoded_prelude, peek=True)

class RawCode:
    # mp_raw_code_kind_t kind : 3;
    # mp_uint_t scope_flags : 7;
    # mp_uint_t n_pos_args : 11;
    # union {
    #     struct {
    #         const byte *bytecode;
    #         const mp_uint_t *const_table;
    #         #if MICROPY_PERSISTENT_CODE_SAVE
    #         mp_uint_t bc_len;
    #         uint16_t n_obj;
    #         uint16_t n_raw_code;
    #         #endif
    #     } u_byte;
    #     struct {
    #         void *fun_data;
    #         const mp_uint_t *const_table;
    #         mp_uint_t type_sig; // for viper, compressed as 2-bit types; ret is MSB, then arg0, arg1, etc
    #     } u_native;
    # } data;
    def __init__(self, encoded_raw_code):
        bc_len = read_uint(encoded_raw_code)
        bc = encoded_raw_code.read(bc_len)
        prelude = Prelude(io.BufferedReader(io.BytesIO(bc)))
        encoded_code_info = bc[:prelude.code_info_size]
        bc_start = prelude.code_info_size
        while bc[bc_start] == 0xff:
            bc_start += 1
        bc = bc[bc_start:]

        self.qstrs = []

        self.simple_name = self._load_qstr(encoded_raw_code)
        self.source_file = self._load_qstr(encoded_raw_code)
        # the simple name and source file qstr indexes get written back into the byte code somehow
        self._load_bytecode_qstrs(encoded_raw_code, bc)
        print(self.qstrs[self.simple_name], self.qstrs[self.source_file])
        print(binascii.hexlify(encoded_raw_code.peek(20)[:20]))

    def _load_qstr(self, encoded_qstr):
        string_len = read_uint(encoded_qstr)
        string = encoded_qstr.read(string_len).decode("utf-8")
        if string in self.qstrs:
            return self.qstr.index(string)
        new_index = len(self.qstrs)
        self.qstrs.append(string)
        return new_index

    def _load_bytecode_qstrs(encoded_raw_code, bytecode):
        i = 0
        while i < len(bytecode):
            opcode, opcode_size = mp_opcode_format(bytecode[i])
            if opcode == MP_OPCODE_QSTR:
                qstr_index = self._load_qstr(encoded_raw_code)
                bytecode[i+1] = qstr_index
                bytecode[i+2] = qstr_index >> 8
            opcode += opcode_size

class mpyFile:
    def __init__(self, encoded_mpy):
        # this matches mp-raw_code_save in py/persistentcode.c
        first_byte = encoded_mpy.read(1)
        if first_byte != b'M':
            raise ValueError("Not a valid first byte. Should be 'M' but is {}".format(first_byte))
        self.version = encoded_mpy.read(1)[0]
        self.feature_flags = encoded_mpy.read(1)[0]
        self.small_int_bits = encoded_mpy.read(1)[0]
        self.raw_code = RawCode(encoded_mpy)

if __name__ == "__main__":
    with open(sys.argv[1], "rb") as f:
        mpy = mpyFile(f)
        print(mpy.version)
        print(mpy.feature_flags)
        print(mpy.small_int_bits)
