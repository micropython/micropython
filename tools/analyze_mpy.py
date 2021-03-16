#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import sys
import binascii
import io

bytecode_format_sizes = {
    "MP_OPCODE_BYTE": 1,
    "MP_OPCODE_QSTR": 3,
    "MP_OPCODE_VAR_UINT": None,  # Unknown because uint encoding uses the top bit to indicate the end.
    "MP_OPCODE_OFFSET": 3,
    "MP_OPCODE_BYTE_EXTRA": 2,
    "MP_OPCODE_VAR_UINT_EXTRA": None,
    "MP_OPCODE_OFFSET_EXTRA": 4,
}

bytecodes = {
    0x00: {"name": "MP_BC_LOAD_FAST_MULTI", "format": "MP_OPCODE_BYTE"},
    0x10: {"name": "MP_BC_LOAD_CONST_FALSE", "format": "MP_OPCODE_BYTE"},
    0x11: {"name": "MP_BC_LOAD_CONST_NONE", "format": "MP_OPCODE_BYTE"},
    0x12: {"name": "MP_BC_LOAD_CONST_TRUE", "format": "MP_OPCODE_BYTE"},
    0x14: {"name": "MP_BC_LOAD_CONST_SMALL_INT", "format": "MP_OPCODE_VAR_UINT"},
    0x16: {"name": "MP_BC_LOAD_CONST_STRING", "format": "MP_OPCODE_QSTR"},
    0x17: {"name": "MP_BC_LOAD_CONST_OBJ", "format": "MP_OPCODE_VAR_UINT"},
    # define MP_BC_LOAD_CONST_OBJ     (0x17) // ptr
    0x18: {"name": "MP_BC_LOAD_NULL", "format": "MP_OPCODE_BYTE"},
    # define MP_BC_LOAD_FAST_N        (0x19) // uint
    0x1A: {"name": "MP_BC_LOAD_DEREF", "format": "MP_OPCODE_VAR_UINT"},
    0x1B: {"name": "MP_BC_LOAD_NAME", "format": "MP_OPCODE_QSTR"},
    0x1C: {"name": "MP_BC_LOAD_GLOBAL", "format": "MP_OPCODE_QSTR"},
    0x1D: {"name": "MP_BC_LOAD_ATTR", "format": "MP_OPCODE_QSTR"},
    0x1E: {"name": "MP_BC_LOAD_METHOD", "format": "MP_OPCODE_QSTR"},
    0x1F: {"name": "MP_BC_LOAD_SUPER_METHOD", "format": "MP_OPCODE_QSTR"},
    0x20: {"name": "MP_BC_LOAD_BUILD_CLASS", "format": "MP_OPCODE_BYTE"},
    # define MP_BC_LOAD_BUILD_CLASS   (0x20)
    # define MP_BC_LOAD_SUBSCR        (0x21)
    0x21: {"name": "MP_BC_LOAD_SUBSCR", "format": "MP_OPCODE_BYTE"},
    # define MP_BC_STORE_FAST_N       (0x22) // uint
    # define MP_BC_STORE_DEREF        (0x23) // uint
    # define MP_BC_STORE_NAME         (0x24) // qstr
    0x24: {"name": "MP_BC_STORE_NAME", "format": "MP_OPCODE_QSTR"},
    0x25: {"name": "MP_BC_STORE_GLOBAL", "format": "MP_OPCODE_QSTR"},
    0x26: {"name": "MP_BC_STORE_ATTR", "format": "MP_OPCODE_QSTR"},
    0x27: {"name": "MP_BC_LOAD_SUBSCR", "format": "MP_OPCODE_BYTE"},
    0x28: {"name": "MP_BC_DELETE_FAST", "format": "MP_OPCODE_VAR_UINT"},
    # define MP_BC_DELETE_FAST        (0x28) // uint
    # define MP_BC_DELETE_DEREF       (0x29) // uint
    # define MP_BC_DELETE_NAME        (0x2a) // qstr
    # define MP_BC_DELETE_GLOBAL      (0x2b) // qstr
    0x30: {"name": "MP_BC_DUP_TOP", "format": "MP_OPCODE_BYTE"},
    # define MP_BC_DUP_TOP_TWO        (0x31)
    0x32: {"name": "MP_BC_POP_TOP", "format": "MP_OPCODE_BYTE"},
    0x33: {"name": "MP_BC_ROT_TWO", "format": "MP_OPCODE_BYTE"},
    0x34: {"name": "MP_BC_ROT_THREE", "format": "MP_OPCODE_BYTE"},
    0x35: {"name": "MP_BC_JUMP", "format": "MP_OPCODE_OFFSET"},
    0x36: {"name": "MP_BC_POP_JUMP_IF_TRUE", "format": "MP_OPCODE_OFFSET"},
    0x37: {"name": "MP_BC_POP_JUMP_IF_FALSE", "format": "MP_OPCODE_OFFSET"},
    # define MP_BC_JUMP_IF_TRUE_OR_POP    (0x38) // rel byte code offset, 16-bit signed, in excess
    # define MP_BC_JUMP_IF_FALSE_OR_POP   (0x39) // rel byte code offset, 16-bit signed, in excess
    # define MP_BC_SETUP_WITH         (0x3d) // rel byte code offset, 16-bit unsigned
    # define MP_BC_WITH_CLEANUP       (0x3e)
    # define MP_BC_SETUP_EXCEPT       (0x3f) // rel byte code offset, 16-bit unsigned
    # define MP_BC_SETUP_FINALLY      (0x40) // rel byte code offset, 16-bit unsigned
    # define MP_BC_END_FINALLY        (0x41)
    # define MP_BC_GET_ITER           (0x42)
    # define MP_BC_FOR_ITER           (0x43) // rel byte code offset, 16-bit unsigned
    0x43: {"name": "MP_BC_FOR_ITER", "format": "MP_OPCODE_OFFSET"},
    0x44: {"name": "MP_BC_POP_BLOCK", "format": "MP_OPCODE_BYTE"},
    # define MP_BC_POP_EXCEPT         (0x45)
    # define MP_BC_UNWIND_JUMP        (0x46) // rel byte code offset, 16-bit signed, in excess; then a byte
    0x47: {"name": "MP_BC_GET_ITER_STACK", "format": "MP_OPCODE_BYTE"},
    0x50: {"name": "MP_BC_BUILD_TUPLE", "format": "MP_OPCODE_VAR_UINT"},
    0x51: {"name": "MP_BC_BUILD_LIST", "format": "MP_OPCODE_VAR_UINT"},
    0x53: {"name": "MP_BC_BUILD_MAP", "format": "MP_OPCODE_VAR_UINT"},
    0x54: {"name": "MP_BC_STORE_MAP", "format": "MP_OPCODE_BYTE"},
    # define MP_BC_BUILD_SET          (0x56) // uint
    # define MP_BC_BUILD_SLICE        (0x58) // uint
    # define MP_BC_STORE_COMP         (0x57) // uint
    0x57: {"name": "MP_BC_STORE_COMP", "format": "MP_OPCODE_VAR_UINT"},
    # define MP_BC_UNPACK_SEQUENCE    (0x59) // uint
    # define MP_BC_UNPACK_EX          (0x5a) // uint
    0x5B: {"name": "MP_BC_RETURN_VALUE", "format": "MP_OPCODE_BYTE"},
    0x5C: {"name": "MP_BC_RAISE_VARARGS", "format": "MP_OPCODE_BYTE_EXTRA"},
    # define MP_BC_YIELD_VALUE        (0x5d)
    # define MP_BC_YIELD_FROM         (0x5e)
    # define MP_BC_MAKE_FUNCTION         (0x60) // uint
    0x60: {"name": "MP_BC_MAKE_FUNCTION", "format": "MP_OPCODE_VAR_UINT"},
    0x61: {"name": "MP_BC_MAKE_FUNCTION_DEFARGS", "format": "MP_OPCODE_VAR_UINT"},
    0x62: {"name": "MP_BC_MAKE_CLOSURE", "format": "MP_OPCODE_VAR_UINT_EXTRA"},
    0x63: {"name": "MP_BC_MAKE_CLOSURE", "format": "MP_OPCODE_VAR_UINT_EXTRA"},
    0x64: {"name": "MP_BC_CALL_FUNCTION", "format": "MP_OPCODE_VAR_UINT"},
    0x65: {"name": "MP_BC_CALL_FUNCTION_VAR_KW", "format": "MP_OPCODE_VAR_UINT"},
    0x66: {"name": "MP_BC_CALL_METHOD", "format": "MP_OPCODE_VAR_UINT"},
    0x67: {"name": "MP_BC_CALL_METHOD_VAR_KW", "format": "MP_OPCODE_VAR_UINT"},
    0x68: {"name": "MP_BC_IMPORT_NAME", "format": "MP_OPCODE_QSTR"},
    0x69: {"name": "MP_BC_IMPORT_FROM", "format": "MP_OPCODE_QSTR"},
    # define MP_BC_IMPORT_FROM        (0x69) // qstr
    # define MP_BC_IMPORT_STAR        (0x6a)
    # define MP_BC_LOAD_CONST_SMALL_INT_MULTI (0x70) // + N(64)
    0x7F: {"name": "MP_BC_LOAD_CONST_SMALL_INT_MULTI -1", "format": "MP_OPCODE_BYTE"},
    0x80: {"name": "MP_BC_LOAD_CONST_SMALL_INT_MULTI 0", "format": "MP_OPCODE_BYTE"},
    0x81: {"name": "MP_BC_LOAD_CONST_SMALL_INT_MULTI 1", "format": "MP_OPCODE_BYTE"},
    0x82: {"name": "MP_BC_LOAD_CONST_SMALL_INT_MULTI 2", "format": "MP_OPCODE_BYTE"},
    0x83: {"name": "MP_BC_LOAD_CONST_SMALL_INT_MULTI 3", "format": "MP_OPCODE_BYTE"},
    0x84: {"name": "MP_BC_LOAD_CONST_SMALL_INT_MULTI 4", "format": "MP_OPCODE_BYTE"},
    # define MP_BC_LOAD_FAST_MULTI            (0xb0) // + N(16)
    0xB0: {"name": "MP_BC_LOAD_FAST_MULTI 0", "format": "MP_OPCODE_BYTE"},
    0xB1: {"name": "MP_BC_LOAD_FAST_MULTI 1", "format": "MP_OPCODE_BYTE"},
    0xB2: {"name": "MP_BC_LOAD_FAST_MULTI 2", "format": "MP_OPCODE_BYTE"},
    0xB3: {"name": "MP_BC_LOAD_FAST_MULTI 3", "format": "MP_OPCODE_BYTE"},
    0xB4: {"name": "MP_BC_LOAD_FAST_MULTI 4", "format": "MP_OPCODE_BYTE"},
    0xB5: {"name": "MP_BC_LOAD_FAST_MULTI 5", "format": "MP_OPCODE_BYTE"},
    0xB6: {"name": "MP_BC_LOAD_FAST_MULTI 6", "format": "MP_OPCODE_BYTE"},
    0xB7: {"name": "MP_BC_LOAD_FAST_MULTI 7", "format": "MP_OPCODE_BYTE"},
    0xB8: {"name": "MP_BC_LOAD_FAST_MULTI 8", "format": "MP_OPCODE_BYTE"},
    # define MP_BC_STORE_FAST_MULTI           (0xc0) // + N(16)
    0xC0: {"name": "MP_BC_STORE_FAST_MULTI 0", "format": "MP_OPCODE_BYTE"},
    0xC1: {"name": "MP_BC_STORE_FAST_MULTI 1", "format": "MP_OPCODE_BYTE"},
    0xC2: {"name": "MP_BC_STORE_FAST_MULTI 2", "format": "MP_OPCODE_BYTE"},
    0xC3: {"name": "MP_BC_STORE_FAST_MULTI 3", "format": "MP_OPCODE_BYTE"},
    0xC4: {"name": "MP_BC_STORE_FAST_MULTI 4", "format": "MP_OPCODE_BYTE"},
    0xC5: {"name": "MP_BC_STORE_FAST_MULTI 5", "format": "MP_OPCODE_BYTE"},
    0xC6: {"name": "MP_BC_STORE_FAST_MULTI 6", "format": "MP_OPCODE_BYTE"},
    0xC7: {"name": "MP_BC_STORE_FAST_MULTI 7", "format": "MP_OPCODE_BYTE"},
    # define MP_BC_UNARY_OP_MULTI             (0xd0) // + op(<MP_UNARY_OP_NUM_BYTECODE)
    # // 9 relational operations, should return a bool
    0xD7: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_LESS", "format": "MP_OPCODE_BYTE"},
    0xD8: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_MORE", "format": "MP_OPCODE_BYTE"},
    0xD9: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_EQUAL", "format": "MP_OPCODE_BYTE"},
    0xDA: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_LESS_EQUAL", "format": "MP_OPCODE_BYTE"},
    0xDB: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_MORE_EQUAL", "format": "MP_OPCODE_BYTE"},
    0xDC: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_NOT_EQUAL", "format": "MP_OPCODE_BYTE"},
    # dc: MP_BINARY_OP_NOT_EQUAL,
    # dd: MP_BINARY_OP_IN,
    # de: MP_BINARY_OP_IS,
    # df: MP_BINARY_OP_EXCEPTION_MATCH,
    #
    # // 12 inplace arithmetic operations
    # e0: MP_BINARY_OP_INPLACE_OR,
    # e1: MP_BINARY_OP_INPLACE_XOR,
    # e2: MP_BINARY_OP_INPLACE_AND,
    # e3: MP_BINARY_OP_INPLACE_LSHIFT,
    # e4: MP_BINARY_OP_INPLACE_RSHIFT,
    # e5: MP_BINARY_OP_INPLACE_ADD,
    0xE5: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_INPLACE_ADD", "format": "MP_OPCODE_BYTE"},
    0xE6: {
        "name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_INPLACE_SUBTRACT",
        "format": "MP_OPCODE_BYTE",
    },
    # e7: MP_BINARY_OP_INPLACE_MULTIPLY,
    # e8: MP_BINARY_OP_INPLACE_FLOOR_DIVIDE,
    # e9: MP_BINARY_OP_INPLACE_TRUE_DIVIDE,
    # ea: MP_BINARY_OP_INPLACE_MODULO,
    # eb: MP_BINARY_OP_INPLACE_POWER,
    #
    # // 12 normal arithmetic operations
    # ec: MP_BINARY_OP_OR,
    # ed: MP_BINARY_OP_XOR,
    # ee: MP_BINARY_OP_AND,
    # ef: MP_BINARY_OP_LSHIFT,
    # f0: MP_BINARY_OP_RSHIFT,
    # define MP_BC_BINARY_OP_MULTI            (0xd7) // + op(<MP_BINARY_OP_NUM_BYTECODE)
    0xF1: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_ADD", "format": "MP_OPCODE_BYTE"},
    0xF2: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_SUBTRACT", "format": "MP_OPCODE_BYTE"},
    0xF3: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_MULTIPLY", "format": "MP_OPCODE_BYTE"},
    0xF4: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_FLOOR_DIVIDE", "format": "MP_OPCODE_BYTE"},
    0xF5: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_TRUE_DIVIDE", "format": "MP_OPCODE_BYTE"},
    0xF6: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_MODULO", "format": "MP_OPCODE_BYTE"},
    0xF7: {"name": "MP_BC_BINARY_OP_MULTI MP_BINARY_OP_POWER", "format": "MP_OPCODE_BYTE"},
}


def read_uint(encoded_uint, peek=False):
    unum = 0
    i = 0
    while True:
        if peek:
            b = encoded_uint.peek()[i]
        else:
            b = encoded_uint.read(1)[0]
        unum = (unum << 7) | (b & 0x7F)
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
        bc = io.BufferedReader(io.BytesIO(bc))
        prelude = Prelude(bc)
        encoded_code_info = bc.read(prelude.code_info_size)
        bc.read(1)
        while bc.peek(1)[0] == 0xFF:
            bc.read(1)
        bc = bytearray(bc.read())
        # print(encoded_code_info, bc)

        self.qstrs = []

        self.simple_name = self._load_qstr(encoded_raw_code)
        self.source_file = self._load_qstr(encoded_raw_code)
        # the simple name and source file qstr indexes get written back into the byte code somehow
        # print(bc)
        self._load_bytecode_qstrs(encoded_raw_code, bc)

        # print(encoded_raw_code.peek(20)[:20])

        n_obj = read_uint(encoded_raw_code)
        n_raw_code = read_uint(encoded_raw_code)

        self.const_table = []
        for i in range(prelude.n_pos_args + prelude.n_kwonly_args):
            self.const_table.append(self._load_qstr(encoded_raw_code))
            print("load args", self.const_table[-1])
        for i in range(n_obj):
            self.const_table.append(self._load_obj(encoded_raw_code))
            print("load obj", self.const_table[-1])
        for i in range(n_raw_code):
            print("load raw code")
            self.const_table.append(RawCode(encoded_raw_code))

        print(self.qstrs[self.simple_name], self.qstrs[self.source_file])
        # print(binascii.hexlify(encoded_raw_code.peek(20)[:20]))

    def _load_qstr(self, encoded_qstr):
        string_len = read_uint(encoded_qstr)
        string = encoded_qstr.read(string_len).decode("utf-8")
        print(string)
        if string in self.qstrs:
            return self.qstrs.index(string)
        new_index = len(self.qstrs)
        self.qstrs.append(string)
        return new_index

    def _load_obj(self, encoded_obj):
        obj_type = encoded_obj.read(1)
        if obj_type == b"e":
            return "..."
        else:
            str_len = read_uint(encoded_obj)
            s = encoded_obj.read(str_len)
            if obj_type == b"s":
                return s.decode("utf-8")
            elif obj_type == b"b":
                return s
            elif obj_type == b"i":
                return int(s)
            elif obj_type == b"f":
                return float(s)
            elif obj_type == b"c":
                return float(s)
        raise RuntimeError("Unknown object type {}".format(obj_type))

    def _load_bytecode_qstrs(self, encoded_raw_code, bytecode):
        i = 0
        while i < len(bytecode):
            bc = bytecode[i]
            if bc not in bytecodes:
                raise RuntimeError("missing code 0x{:x} at {}".format(bc, i))
                return
            bc = bytecodes[bc]
            opcode = bc["name"]
            print(opcode)
            opcode_size = bytecode_format_sizes[bc["format"]]
            if bc["format"] == "MP_OPCODE_QSTR":
                qstr_index = self._load_qstr(encoded_raw_code)
                bytecode[i + 1] = qstr_index
                bytecode[i + 2] = qstr_index >> 8
            if not opcode_size:
                i += 2
                while (bytecode[i] & 0x80) != 0:
                    i += 1
                if bc["format"] == "MP_OPCODE_VAR_UINT_EXTRA":
                    i += 1
            else:
                i += opcode_size


class mpyFile:
    def __init__(self, encoded_mpy):
        # this matches mp-raw_code_save in py/persistentcode.c
        first_byte = encoded_mpy.read(1)
        if first_byte != b"M":
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
