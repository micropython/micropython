#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2016-2019 Damien P. George
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Python 2/3 compatibility code
from __future__ import print_function
import platform

if platform.python_version_tuple()[0] == "2":
    from binascii import hexlify as hexlify_py2

    str_cons = lambda val, enc=None: str(val)
    bytes_cons = lambda val, enc=None: bytearray(val)
    is_str_type = lambda o: type(o) is str
    is_bytes_type = lambda o: type(o) is bytearray
    is_int_type = lambda o: type(o) is int or type(o) is long

    def hexlify_to_str(b):
        x = hexlify_py2(b)
        return ":".join(x[i : i + 2] for i in range(0, len(x), 2))

else:
    from binascii import hexlify

    str_cons = str
    bytes_cons = bytes
    is_str_type = lambda o: type(o) is str
    is_bytes_type = lambda o: type(o) is bytes
    is_int_type = lambda o: type(o) is int

    def hexlify_to_str(b):
        return str(hexlify(b, ":"), "ascii")


# end compatibility code

import sys
import struct

sys.path.append(sys.path[0] + "/../py")
import makeqstrdata as qstrutil

# Threshold of str length below which it will be turned into a qstr when freezing.
# This helps to reduce frozen code size because qstrs are more efficient to encode
# as objects than full mp_obj_str_t instances.
PERSISTENT_STR_INTERN_THRESHOLD = 25


class MPYReadError(Exception):
    def __init__(self, filename, msg):
        self.filename = filename
        self.msg = msg

    def __str__(self):
        return "%s: %s" % (self.filename, self.msg)


class FreezeError(Exception):
    def __init__(self, rawcode, msg):
        self.rawcode = rawcode
        self.msg = msg

    def __str__(self):
        return "error while freezing %s: %s" % (self.rawcode.source_file, self.msg)


class Config:
    MPY_VERSION = 6
    MICROPY_LONGINT_IMPL_NONE = 0
    MICROPY_LONGINT_IMPL_LONGLONG = 1
    MICROPY_LONGINT_IMPL_MPZ = 2


config = Config()


MP_CODE_BYTECODE = 2
MP_CODE_NATIVE_PY = 3
MP_CODE_NATIVE_VIPER = 4
MP_CODE_NATIVE_ASM = 5

MP_NATIVE_ARCH_NONE = 0
MP_NATIVE_ARCH_X86 = 1
MP_NATIVE_ARCH_X64 = 2
MP_NATIVE_ARCH_ARMV6 = 3
MP_NATIVE_ARCH_ARMV6M = 4
MP_NATIVE_ARCH_ARMV7M = 5
MP_NATIVE_ARCH_ARMV7EM = 6
MP_NATIVE_ARCH_ARMV7EMSP = 7
MP_NATIVE_ARCH_ARMV7EMDP = 8
MP_NATIVE_ARCH_XTENSA = 9
MP_NATIVE_ARCH_XTENSAWIN = 10

MP_PERSISTENT_OBJ_FUN_TABLE = 0
MP_PERSISTENT_OBJ_NONE = 1
MP_PERSISTENT_OBJ_FALSE = 2
MP_PERSISTENT_OBJ_TRUE = 3
MP_PERSISTENT_OBJ_ELLIPSIS = 4
MP_PERSISTENT_OBJ_STR = 5
MP_PERSISTENT_OBJ_BYTES = 6
MP_PERSISTENT_OBJ_INT = 7
MP_PERSISTENT_OBJ_FLOAT = 8
MP_PERSISTENT_OBJ_COMPLEX = 9
MP_PERSISTENT_OBJ_TUPLE = 10

MP_SCOPE_FLAG_VIPERRELOC = 0x10
MP_SCOPE_FLAG_VIPERRODATA = 0x20
MP_SCOPE_FLAG_VIPERBSS = 0x40

MP_BC_MASK_EXTRA_BYTE = 0x9E

MP_BC_FORMAT_BYTE = 0
MP_BC_FORMAT_QSTR = 1
MP_BC_FORMAT_VAR_UINT = 2
MP_BC_FORMAT_OFFSET = 3

mp_unary_op_method_name = (
    "__pos__",
    "__neg__",
    "__invert__",
    "<not>",
)

mp_binary_op_method_name = (
    "__lt__",
    "__gt__",
    "__eq__",
    "__le__",
    "__ge__",
    "__ne__",
    "<in>",
    "<is>",
    "<exception match>",
    "__ior__",
    "__ixor__",
    "__iand__",
    "__ilshift__",
    "__irshift__",
    "__iadd__",
    "__isub__",
    "__imul__",
    "__imatmul__",
    "__ifloordiv__",
    "__itruediv__",
    "__imod__",
    "__ipow__",
    "__or__",
    "__xor__",
    "__and__",
    "__lshift__",
    "__rshift__",
    "__add__",
    "__sub__",
    "__mul__",
    "__matmul__",
    "__floordiv__",
    "__truediv__",
    "__mod__",
    "__pow__",
)


class Opcodes:
    # fmt: off
    # Load, Store, Delete, Import, Make, Build, Unpack, Call, Jump, Exception, For, sTack, Return, Yield, Op
    MP_BC_BASE_RESERVED               = (0x00) # ----------------
    MP_BC_BASE_QSTR_O                 = (0x10) # LLLLLLSSSDDII---
    MP_BC_BASE_VINT_E                 = (0x20) # MMLLLLSSDDBBBBBB
    MP_BC_BASE_VINT_O                 = (0x30) # UUMMCCCC--------
    MP_BC_BASE_JUMP_E                 = (0x40) # J-JJJJJEEEEF----
    MP_BC_BASE_BYTE_O                 = (0x50) # LLLLSSDTTTTTEEFF
    MP_BC_BASE_BYTE_E                 = (0x60) # --BREEEYYI------
    MP_BC_LOAD_CONST_SMALL_INT_MULTI  = (0x70) # LLLLLLLLLLLLLLLL
    #                                 = (0x80) # LLLLLLLLLLLLLLLL
    #                                 = (0x90) # LLLLLLLLLLLLLLLL
    #                                 = (0xa0) # LLLLLLLLLLLLLLLL
    MP_BC_LOAD_FAST_MULTI             = (0xb0) # LLLLLLLLLLLLLLLL
    MP_BC_STORE_FAST_MULTI            = (0xc0) # SSSSSSSSSSSSSSSS
    MP_BC_UNARY_OP_MULTI              = (0xd0) # OOOOOOO
    MP_BC_BINARY_OP_MULTI             = (0xd7) #        OOOOOOOOO
    #                                 = (0xe0) # OOOOOOOOOOOOOOOO
    #                                 = (0xf0) # OOOOOOOOOO------

    MP_BC_LOAD_CONST_SMALL_INT_MULTI_NUM = 64
    MP_BC_LOAD_CONST_SMALL_INT_MULTI_EXCESS = 16
    MP_BC_LOAD_FAST_MULTI_NUM         = 16
    MP_BC_STORE_FAST_MULTI_NUM        = 16
    MP_BC_UNARY_OP_MULTI_NUM          = 4  # MP_UNARY_OP_NUM_BYTECODE
    MP_BC_BINARY_OP_MULTI_NUM         = 35  # MP_BINARY_OP_NUM_BYTECODE

    MP_BC_LOAD_CONST_FALSE            = (MP_BC_BASE_BYTE_O + 0x00)
    MP_BC_LOAD_CONST_NONE             = (MP_BC_BASE_BYTE_O + 0x01)
    MP_BC_LOAD_CONST_TRUE             = (MP_BC_BASE_BYTE_O + 0x02)
    MP_BC_LOAD_CONST_SMALL_INT        = (MP_BC_BASE_VINT_E + 0x02) # signed var-int
    MP_BC_LOAD_CONST_STRING           = (MP_BC_BASE_QSTR_O + 0x00) # qstr
    MP_BC_LOAD_CONST_OBJ              = (MP_BC_BASE_VINT_E + 0x03) # ptr
    MP_BC_LOAD_NULL                   = (MP_BC_BASE_BYTE_O + 0x03)

    MP_BC_LOAD_FAST_N                 = (MP_BC_BASE_VINT_E + 0x04) # uint
    MP_BC_LOAD_DEREF                  = (MP_BC_BASE_VINT_E + 0x05) # uint
    MP_BC_LOAD_NAME                   = (MP_BC_BASE_QSTR_O + 0x01) # qstr
    MP_BC_LOAD_GLOBAL                 = (MP_BC_BASE_QSTR_O + 0x02) # qstr
    MP_BC_LOAD_ATTR                   = (MP_BC_BASE_QSTR_O + 0x03) # qstr
    MP_BC_LOAD_METHOD                 = (MP_BC_BASE_QSTR_O + 0x04) # qstr
    MP_BC_LOAD_SUPER_METHOD           = (MP_BC_BASE_QSTR_O + 0x05) # qstr
    MP_BC_LOAD_BUILD_CLASS            = (MP_BC_BASE_BYTE_O + 0x04)
    MP_BC_LOAD_SUBSCR                 = (MP_BC_BASE_BYTE_O + 0x05)

    MP_BC_STORE_FAST_N                = (MP_BC_BASE_VINT_E + 0x06) # uint
    MP_BC_STORE_DEREF                 = (MP_BC_BASE_VINT_E + 0x07) # uint
    MP_BC_STORE_NAME                  = (MP_BC_BASE_QSTR_O + 0x06) # qstr
    MP_BC_STORE_GLOBAL                = (MP_BC_BASE_QSTR_O + 0x07) # qstr
    MP_BC_STORE_ATTR                  = (MP_BC_BASE_QSTR_O + 0x08) # qstr
    MP_BC_STORE_SUBSCR                = (MP_BC_BASE_BYTE_O + 0x06)

    MP_BC_DELETE_FAST                 = (MP_BC_BASE_VINT_E + 0x08) # uint
    MP_BC_DELETE_DEREF                = (MP_BC_BASE_VINT_E + 0x09) # uint
    MP_BC_DELETE_NAME                 = (MP_BC_BASE_QSTR_O + 0x09) # qstr
    MP_BC_DELETE_GLOBAL               = (MP_BC_BASE_QSTR_O + 0x0a) # qstr

    MP_BC_DUP_TOP                     = (MP_BC_BASE_BYTE_O + 0x07)
    MP_BC_DUP_TOP_TWO                 = (MP_BC_BASE_BYTE_O + 0x08)
    MP_BC_POP_TOP                     = (MP_BC_BASE_BYTE_O + 0x09)
    MP_BC_ROT_TWO                     = (MP_BC_BASE_BYTE_O + 0x0a)
    MP_BC_ROT_THREE                   = (MP_BC_BASE_BYTE_O + 0x0b)

    MP_BC_UNWIND_JUMP                 = (MP_BC_BASE_JUMP_E + 0x00) # signed relative bytecode offset; then a byte
    MP_BC_JUMP                        = (MP_BC_BASE_JUMP_E + 0x02) # signed relative bytecode offset
    MP_BC_POP_JUMP_IF_TRUE            = (MP_BC_BASE_JUMP_E + 0x03) # signed relative bytecode offset
    MP_BC_POP_JUMP_IF_FALSE           = (MP_BC_BASE_JUMP_E + 0x04) # signed relative bytecode offset
    MP_BC_JUMP_IF_TRUE_OR_POP         = (MP_BC_BASE_JUMP_E + 0x05) # unsigned relative bytecode offset
    MP_BC_JUMP_IF_FALSE_OR_POP        = (MP_BC_BASE_JUMP_E + 0x06) # unsigned relative bytecode offset
    MP_BC_SETUP_WITH                  = (MP_BC_BASE_JUMP_E + 0x07) # unsigned relative bytecode offset
    MP_BC_SETUP_EXCEPT                = (MP_BC_BASE_JUMP_E + 0x08) # unsigned relative bytecode offset
    MP_BC_SETUP_FINALLY               = (MP_BC_BASE_JUMP_E + 0x09) # unsigned relative bytecode offset
    MP_BC_POP_EXCEPT_JUMP             = (MP_BC_BASE_JUMP_E + 0x0a) # unsigned relative bytecode offset
    MP_BC_FOR_ITER                    = (MP_BC_BASE_JUMP_E + 0x0b) # unsigned relative bytecode offset
    MP_BC_WITH_CLEANUP                = (MP_BC_BASE_BYTE_O + 0x0c)
    MP_BC_END_FINALLY                 = (MP_BC_BASE_BYTE_O + 0x0d)
    MP_BC_GET_ITER                    = (MP_BC_BASE_BYTE_O + 0x0e)
    MP_BC_GET_ITER_STACK              = (MP_BC_BASE_BYTE_O + 0x0f)

    MP_BC_BUILD_TUPLE                 = (MP_BC_BASE_VINT_E + 0x0a) # uint
    MP_BC_BUILD_LIST                  = (MP_BC_BASE_VINT_E + 0x0b) # uint
    MP_BC_BUILD_MAP                   = (MP_BC_BASE_VINT_E + 0x0c) # uint
    MP_BC_STORE_MAP                   = (MP_BC_BASE_BYTE_E + 0x02)
    MP_BC_BUILD_SET                   = (MP_BC_BASE_VINT_E + 0x0d) # uint
    MP_BC_BUILD_SLICE                 = (MP_BC_BASE_VINT_E + 0x0e) # uint
    MP_BC_STORE_COMP                  = (MP_BC_BASE_VINT_E + 0x0f) # uint
    MP_BC_UNPACK_SEQUENCE             = (MP_BC_BASE_VINT_O + 0x00) # uint
    MP_BC_UNPACK_EX                   = (MP_BC_BASE_VINT_O + 0x01) # uint

    MP_BC_RETURN_VALUE                = (MP_BC_BASE_BYTE_E + 0x03)
    MP_BC_RAISE_LAST                  = (MP_BC_BASE_BYTE_E + 0x04)
    MP_BC_RAISE_OBJ                   = (MP_BC_BASE_BYTE_E + 0x05)
    MP_BC_RAISE_FROM                  = (MP_BC_BASE_BYTE_E + 0x06)
    MP_BC_YIELD_VALUE                 = (MP_BC_BASE_BYTE_E + 0x07)
    MP_BC_YIELD_FROM                  = (MP_BC_BASE_BYTE_E + 0x08)

    MP_BC_MAKE_FUNCTION               = (MP_BC_BASE_VINT_O + 0x02) # uint
    MP_BC_MAKE_FUNCTION_DEFARGS       = (MP_BC_BASE_VINT_O + 0x03) # uint
    MP_BC_MAKE_CLOSURE                = (MP_BC_BASE_VINT_E + 0x00) # uint; extra byte
    MP_BC_MAKE_CLOSURE_DEFARGS        = (MP_BC_BASE_VINT_E + 0x01) # uint; extra byte
    MP_BC_CALL_FUNCTION               = (MP_BC_BASE_VINT_O + 0x04) # uint
    MP_BC_CALL_FUNCTION_VAR_KW        = (MP_BC_BASE_VINT_O + 0x05) # uint
    MP_BC_CALL_METHOD                 = (MP_BC_BASE_VINT_O + 0x06) # uint
    MP_BC_CALL_METHOD_VAR_KW          = (MP_BC_BASE_VINT_O + 0x07) # uint

    MP_BC_IMPORT_NAME                 = (MP_BC_BASE_QSTR_O + 0x0b) # qstr
    MP_BC_IMPORT_FROM                 = (MP_BC_BASE_QSTR_O + 0x0c) # qstr
    MP_BC_IMPORT_STAR                 = (MP_BC_BASE_BYTE_E + 0x09)
    # fmt: on

    # Create sets of related opcodes.
    ALL_OFFSET_SIGNED = (
        MP_BC_UNWIND_JUMP,
        MP_BC_JUMP,
        MP_BC_POP_JUMP_IF_TRUE,
        MP_BC_POP_JUMP_IF_FALSE,
    )

    # Create a dict mapping opcode value to opcode name.
    mapping = ["unknown" for _ in range(256)]
    for op_name in list(locals()):
        if op_name.startswith("MP_BC_"):
            mapping[locals()[op_name]] = op_name[len("MP_BC_") :]
    for i in range(MP_BC_LOAD_CONST_SMALL_INT_MULTI_NUM):
        name = "LOAD_CONST_SMALL_INT %d" % (i - MP_BC_LOAD_CONST_SMALL_INT_MULTI_EXCESS)
        mapping[MP_BC_LOAD_CONST_SMALL_INT_MULTI + i] = name
    for i in range(MP_BC_LOAD_FAST_MULTI_NUM):
        mapping[MP_BC_LOAD_FAST_MULTI + i] = "LOAD_FAST %d" % i
    for i in range(MP_BC_STORE_FAST_MULTI_NUM):
        mapping[MP_BC_STORE_FAST_MULTI + i] = "STORE_FAST %d" % i
    for i in range(MP_BC_UNARY_OP_MULTI_NUM):
        mapping[MP_BC_UNARY_OP_MULTI + i] = "UNARY_OP %d %s" % (i, mp_unary_op_method_name[i])
    for i in range(MP_BC_BINARY_OP_MULTI_NUM):
        mapping[MP_BC_BINARY_OP_MULTI + i] = "BINARY_OP %d %s" % (i, mp_binary_op_method_name[i])


# This definition of a small int covers all possible targets, in the sense that every
# target can encode as a small int, an integer that passes this test.  The minimum is set
# by MICROPY_OBJ_REPR_B on a 16-bit machine, where there are 14 bits for the small int.
def mp_small_int_fits(i):
    return -0x2000 <= i <= 0x1FFF


# this function mirrors that in py/bc.c
def mp_opcode_format(bytecode, ip, count_var_uint):
    opcode = bytecode[ip]
    ip_start = ip
    f = (0x000003A4 >> (2 * ((opcode) >> 4))) & 3
    if f == MP_BC_FORMAT_QSTR:
        ip += 3
    else:
        extra_byte = (opcode & MP_BC_MASK_EXTRA_BYTE) == 0
        ip += 1
        if f == MP_BC_FORMAT_VAR_UINT:
            if count_var_uint:
                while bytecode[ip] & 0x80 != 0:
                    ip += 1
                ip += 1
        elif f == MP_BC_FORMAT_OFFSET:
            if bytecode[ip] & 0x80 == 0:
                ip += 1
            else:
                ip += 2
        ip += extra_byte
    return f, ip - ip_start


def mp_opcode_decode(bytecode, ip):
    opcode = bytecode[ip]
    ip_start = ip
    f = (0x000003A4 >> (2 * ((opcode) >> 4))) & 3
    extra_byte = (opcode & MP_BC_MASK_EXTRA_BYTE) == 0
    ip += 1
    arg = 0
    if f in (MP_BC_FORMAT_QSTR, MP_BC_FORMAT_VAR_UINT):
        arg = bytecode[ip] & 0x7F
        while bytecode[ip] & 0x80 != 0:
            ip += 1
            arg = arg << 7 | bytecode[ip] & 0x7F
        ip += 1
    elif f == MP_BC_FORMAT_OFFSET:
        if bytecode[ip] & 0x80 == 0:
            arg = bytecode[ip]
            ip += 1
            if opcode in Opcodes.ALL_OFFSET_SIGNED:
                arg -= 0x40
        else:
            arg = bytecode[ip] & 0x7F | bytecode[ip + 1] << 7
            ip += 2
            if opcode in Opcodes.ALL_OFFSET_SIGNED:
                arg -= 0x4000
    ip += extra_byte
    return f, ip - ip_start, arg


def read_prelude_sig(read_byte):
    z = read_byte()
    # xSSSSEAA
    S = (z >> 3) & 0xF
    E = (z >> 2) & 0x1
    F = 0
    A = z & 0x3
    K = 0
    D = 0
    n = 0
    while z & 0x80:
        z = read_byte()
        # xFSSKAED
        S |= (z & 0x30) << (2 * n)
        E |= (z & 0x02) << n
        F |= ((z & 0x40) >> 6) << n
        A |= (z & 0x4) << n
        K |= ((z & 0x08) >> 3) << n
        D |= (z & 0x1) << n
        n += 1
    S += 1
    return S, E, F, A, K, D


def read_prelude_size(read_byte):
    I = 0
    C = 0
    n = 0
    while True:
        z = read_byte()
        # xIIIIIIC
        I |= ((z & 0x7E) >> 1) << (6 * n)
        C |= (z & 1) << n
        if not (z & 0x80):
            break
        n += 1
    return I, C


def extract_prelude(bytecode, ip):
    def local_read_byte():
        b = bytecode[ip_ref[0]]
        ip_ref[0] += 1
        return b

    ip_ref = [ip]  # to close over ip in Python 2 and 3
    (
        n_state,
        n_exc_stack,
        scope_flags,
        n_pos_args,
        n_kwonly_args,
        n_def_pos_args,
    ) = read_prelude_sig(local_read_byte)

    n_info, n_cell = read_prelude_size(local_read_byte)
    ip = ip_ref[0]

    ip2 = ip
    ip = ip2 + n_info + n_cell
    # ip now points to first opcode
    # ip2 points to simple_name qstr

    # Extract simple_name and argument qstrs (var uints).
    args = []
    for arg_num in range(1 + n_pos_args + n_kwonly_args):
        value = 0
        while True:
            b = local_read_byte()
            value = (value << 7) | (b & 0x7F)
            if b & 0x80 == 0:
                break
        args.append(value)

    return (
        ip2,
        ip,
        ip_ref[0],
        (n_state, n_exc_stack, scope_flags, n_pos_args, n_kwonly_args, n_def_pos_args),
        args,
    )


class QStrType:
    def __init__(self, str):
        self.str = str
        self.qstr_esc = qstrutil.qstr_escape(self.str)
        self.qstr_id = "MP_QSTR_" + self.qstr_esc


class GlobalQStrList:
    def __init__(self):
        # Initialise global list of qstrs with static qstrs
        self.qstrs = [None]  # MP_QSTRnull should never be referenced
        for n in qstrutil.static_qstr_list:
            self.qstrs.append(QStrType(n))

    def add(self, s):
        q = QStrType(s)
        self.qstrs.append(q)
        return q

    def get_by_index(self, i):
        return self.qstrs[i]

    def find_by_str(self, s):
        for q in self.qstrs:
            if q is not None and q.str == s:
                return q
        return None


class MPFunTable:
    def __repr__(self):
        return "mp_fun_table"


class CompiledModule:
    def __init__(
        self,
        mpy_source_file,
        mpy_segments,
        header,
        qstr_table,
        obj_table,
        raw_code,
        raw_code_file_offset,
        escaped_name,
    ):
        self.mpy_source_file = mpy_source_file
        self.mpy_segments = mpy_segments
        self.source_file = qstr_table[0]
        self.header = header
        self.qstr_table = qstr_table
        self.obj_table = obj_table
        self.raw_code_file_offset = raw_code_file_offset
        self.raw_code = raw_code
        self.escaped_name = escaped_name

    def hexdump(self):
        with open(self.mpy_source_file, "rb") as f:
            WIDTH = 16
            COL_OFF = "\033[0m"
            COL_TABLE = (
                ("", ""),  # META
                ("\033[0;31m", "\033[0;91m"),  # QSTR
                ("\033[0;32m", "\033[0;92m"),  # OBJ
                ("\033[0;34m", "\033[0;94m"),  # CODE
            )
            cur_col = ""
            cur_col_index = 0
            offset = 0
            segment_index = 0
            while True:
                data = bytes_cons(f.read(WIDTH))
                if not data:
                    break

                # Print out the hex dump of this line of data.
                line_hex = cur_col
                line_chr = cur_col
                line_comment = ""
                for i in range(len(data)):
                    # Determine the colour of the data, if any, and the line comment.
                    while segment_index < len(self.mpy_segments):
                        if offset + i == self.mpy_segments[segment_index].start:
                            cur_col = COL_TABLE[self.mpy_segments[segment_index].kind][
                                cur_col_index
                            ]
                            cur_col_index = 1 - cur_col_index
                            line_hex += cur_col
                            line_chr += cur_col
                            line_comment += " %s%s%s" % (
                                cur_col,
                                self.mpy_segments[segment_index].name,
                                COL_OFF,
                            )
                        if offset + i == self.mpy_segments[segment_index].end:
                            cur_col = ""
                            line_hex += COL_OFF
                            line_chr += COL_OFF
                            segment_index += 1
                        else:
                            break

                    # Add to the hex part of the line.
                    if i % 2 == 0:
                        line_hex += " "
                    line_hex += "%02x" % data[i]

                    # Add to the characters part of the line.
                    if 0x20 <= data[i] <= 0x7E:
                        line_chr += "%s" % chr(data[i])
                    else:
                        line_chr += "."

                # Print out this line.
                if cur_col:
                    line_hex += COL_OFF
                    line_chr += COL_OFF
                pad = " " * ((WIDTH - len(data)) * 5 // 2)
                print("%08x:%s%s  %s %s" % (offset, line_hex, pad, line_chr, line_comment))
                offset += WIDTH

    def disassemble(self):
        print("mpy_source_file:", self.mpy_source_file)
        print("source_file:", self.source_file.str)
        print("header:", hexlify_to_str(self.header))
        print("qstr_table[%u]:" % len(self.qstr_table))
        for q in self.qstr_table:
            print("    %s" % q.str)
        print("obj_table:", self.obj_table)
        self.raw_code.disassemble()

    def freeze(self, compiled_module_index):
        print()
        print("/" * 80)
        print("// frozen module %s" % self.escaped_name)
        print("// - original source file: %s" % self.mpy_source_file)
        print("// - frozen file name: %s" % self.source_file.str)
        print("// - .mpy header: %s" % ":".join("%02x" % b for b in self.header))
        print()

        self.raw_code.freeze()
        print()

        self.freeze_constants()

        print()
        print("static const mp_frozen_module_t frozen_module_%s = {" % self.escaped_name)
        print("    .constants = {")
        if len(self.qstr_table):
            print(
                "        .qstr_table = (qstr_short_t *)&const_qstr_table_data_%s,"
                % self.escaped_name
            )
        else:
            print("        .qstr_table = NULL,")
        if len(self.obj_table):
            print("        .obj_table = (mp_obj_t *)&const_obj_table_data_%s," % self.escaped_name)
        else:
            print("        .obj_table = NULL,")
        print("    },")
        print("    .rc = &raw_code_%s," % self.raw_code.escaped_name)
        print("};")

    def freeze_constant_obj(self, obj_name, obj):
        global const_str_content, const_int_content, const_obj_content

        if isinstance(obj, MPFunTable):
            return "&mp_fun_table"
        elif obj is None:
            return "MP_ROM_NONE"
        elif obj is False:
            return "MP_ROM_FALSE"
        elif obj is True:
            return "MP_ROM_TRUE"
        elif obj is Ellipsis:
            return "MP_ROM_PTR(&mp_const_ellipsis_obj)"
        elif is_str_type(obj) or is_bytes_type(obj):
            if len(obj) == 0:
                if is_str_type(obj):
                    return "MP_ROM_QSTR(MP_QSTR_)"
                else:
                    return "MP_ROM_PTR(&mp_const_empty_bytes_obj)"
            if is_str_type(obj):
                q = global_qstrs.find_by_str(obj)
                if q:
                    return "MP_ROM_QSTR(%s)" % q.qstr_id
                obj = bytes_cons(obj, "utf8")
                obj_type = "mp_type_str"
            else:
                obj_type = "mp_type_bytes"
            print(
                'static const mp_obj_str_t %s = {{&%s}, %u, %u, (const byte*)"%s"};'
                % (
                    obj_name,
                    obj_type,
                    qstrutil.compute_hash(obj, config.MICROPY_QSTR_BYTES_IN_HASH),
                    len(obj),
                    "".join(("\\x%02x" % b) for b in obj),
                )
            )
            const_str_content += len(obj)
            const_obj_content += 4 * 4
            return "MP_ROM_PTR(&%s)" % obj_name
        elif is_int_type(obj):
            if mp_small_int_fits(obj):
                # Encode directly as a small integer object.
                return "MP_ROM_INT(%d)" % obj
            elif config.MICROPY_LONGINT_IMPL == config.MICROPY_LONGINT_IMPL_NONE:
                raise FreezeError(self, "target does not support long int")
            elif config.MICROPY_LONGINT_IMPL == config.MICROPY_LONGINT_IMPL_LONGLONG:
                # TODO
                raise FreezeError(self, "freezing int to long-long is not implemented")
            elif config.MICROPY_LONGINT_IMPL == config.MICROPY_LONGINT_IMPL_MPZ:
                neg = 0
                if obj < 0:
                    obj = -obj
                    neg = 1
                bits_per_dig = config.MPZ_DIG_SIZE
                digs = []
                z = obj
                while z:
                    digs.append(z & ((1 << bits_per_dig) - 1))
                    z >>= bits_per_dig
                ndigs = len(digs)
                digs = ",".join(("%#x" % d) for d in digs)
                print(
                    "static const mp_obj_int_t %s = {{&mp_type_int}, "
                    "{.neg=%u, .fixed_dig=1, .alloc=%u, .len=%u, .dig=(uint%u_t*)(const uint%u_t[]){%s}}};"
                    % (obj_name, neg, ndigs, ndigs, bits_per_dig, bits_per_dig, digs)
                )
                const_int_content += (digs.count(",") + 1) * bits_per_dig // 8
                const_obj_content += 4 * 4
                return "MP_ROM_PTR(&%s)" % obj_name
        elif type(obj) is float:
            macro_name = "%s_macro" % obj_name
            print(
                "#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B"
            )
            print(
                "static const mp_obj_float_t %s = {{&mp_type_float}, (mp_float_t)%.16g};"
                % (obj_name, obj)
            )
            print("#define %s MP_ROM_PTR(&%s)" % (macro_name, obj_name))
            print("#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C")
            n = struct.unpack("<I", struct.pack("<f", obj))[0]
            n = ((n & ~0x3) | 2) + 0x80800000
            print("#define %s ((mp_rom_obj_t)(0x%08x))" % (macro_name, n))
            print("#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D")
            n = struct.unpack("<Q", struct.pack("<d", obj))[0]
            n += 0x8004000000000000
            print("#define %s ((mp_rom_obj_t)(0x%016x))" % (macro_name, n))
            print("#endif")
            const_obj_content += 3 * 4
            return macro_name
        elif type(obj) is complex:
            print(
                "static const mp_obj_complex_t %s = {{&mp_type_complex}, (mp_float_t)%.16g, (mp_float_t)%.16g};"
                % (obj_name, obj.real, obj.imag)
            )
            return "MP_ROM_PTR(&%s)" % obj_name
        elif type(obj) is tuple:
            if len(obj) == 0:
                return "MP_ROM_PTR(&mp_const_empty_tuple_obj)"
            else:
                obj_refs = []
                for i, sub_obj in enumerate(obj):
                    sub_obj_name = "%s_%u" % (obj_name, i)
                    obj_refs.append(self.freeze_constant_obj(sub_obj_name, sub_obj))
                print(
                    "static const mp_rom_obj_tuple_t %s = {{&mp_type_tuple}, %d, {"
                    % (obj_name, len(obj))
                )
                for ref in obj_refs:
                    print("    %s," % ref)
                print("}};")
                return "MP_ROM_PTR(&%s)" % obj_name
        else:
            raise FreezeError(self, "freezing of object %r is not implemented" % (obj,))

    def freeze_constants(self):
        if len(self.qstr_table):
            print(
                "static const qstr_short_t const_qstr_table_data_%s[%u] = {"
                % (self.escaped_name, len(self.qstr_table))
            )
            for q in self.qstr_table:
                print("    %s," % q.qstr_id)
            print("};")

        if not len(self.obj_table):
            return

        # generate constant objects
        print()
        print("// constants")
        obj_refs = []
        for i, obj in enumerate(self.obj_table):
            obj_name = "const_obj_%s_%u" % (self.escaped_name, i)
            obj_refs.append(self.freeze_constant_obj(obj_name, obj))

        # generate constant table
        print()
        print("// constant table")
        print(
            "static const mp_rom_obj_t const_obj_table_data_%s[%u] = {"
            % (self.escaped_name, len(self.obj_table))
        )
        for ref in obj_refs:
            print("    %s," % ref)
        print("};")

        global const_table_ptr_content
        const_table_ptr_content += len(self.obj_table)


class RawCode(object):
    # a set of all escaped names, to make sure they are unique
    escaped_names = set()

    # convert code kind number to string
    code_kind_str = {
        MP_CODE_BYTECODE: "MP_CODE_BYTECODE",
        MP_CODE_NATIVE_PY: "MP_CODE_NATIVE_PY",
        MP_CODE_NATIVE_VIPER: "MP_CODE_NATIVE_VIPER",
        MP_CODE_NATIVE_ASM: "MP_CODE_NATIVE_ASM",
    }

    def __init__(self, cm_escaped_name, qstr_table, fun_data, prelude_offset, code_kind):
        self.qstr_table = qstr_table
        self.fun_data = fun_data
        self.prelude_offset = prelude_offset
        self.code_kind = code_kind

        if code_kind in (MP_CODE_BYTECODE, MP_CODE_NATIVE_PY):
            (
                self.offset_names,
                self.offset_opcodes,
                self.offset_line_info,
                self.prelude,
                self.names,
            ) = extract_prelude(self.fun_data, prelude_offset)
            self.scope_flags = self.prelude[2]
            self.n_pos_args = self.prelude[3]
            self.simple_name = self.qstr_table[self.names[0]]
        else:
            self.simple_name = self.qstr_table[0]

        escaped_name = cm_escaped_name + "_" + self.simple_name.qstr_esc

        # make sure the escaped name is unique
        i = 2
        unique_escaped_name = escaped_name
        while unique_escaped_name in self.escaped_names:
            unique_escaped_name = escaped_name + str(i)
            i += 1
        self.escaped_names.add(unique_escaped_name)
        self.escaped_name = unique_escaped_name

    def disassemble_children(self):
        print("  children:", [rc.simple_name.str for rc in self.children])
        for rc in self.children:
            rc.disassemble()

    def freeze_children(self):
        # Freeze children and generate table of children.
        if len(self.children):
            for rc in self.children:
                print("// child of %s" % self.escaped_name)
                rc.freeze()
                print()
            print("static const mp_raw_code_t *const children_%s[] = {" % self.escaped_name)
            for rc in self.children:
                print("    &raw_code_%s," % rc.escaped_name)
            print("};")
            print()

    def freeze_raw_code(self, qstr_links=(), type_sig=0):
        # Generate mp_raw_code_t.
        print("static const mp_raw_code_t raw_code_%s = {" % self.escaped_name)
        print("    .kind = %s," % RawCode.code_kind_str[self.code_kind])
        print("    .scope_flags = 0x%02x," % self.scope_flags)
        print("    .n_pos_args = %u," % self.n_pos_args)
        print("    .fun_data = fun_data_%s," % self.escaped_name)
        print("    #if MICROPY_PERSISTENT_CODE_SAVE || MICROPY_DEBUG_PRINTERS")
        print("    .fun_data_len = %u," % len(self.fun_data))
        print("    #endif")
        if len(self.children):
            print("    .children = (void *)&children_%s," % self.escaped_name)
        else:
            print("    .children = NULL,")
        print("    #if MICROPY_PERSISTENT_CODE_SAVE")
        print("    .n_children = %u," % len(self.children))
        if self.code_kind == MP_CODE_BYTECODE:
            print("    #if MICROPY_PY_SYS_SETTRACE")
            print("    .prelude = {")
            print("        .n_state = %u," % self.prelude[0])
            print("        .n_exc_stack = %u," % self.prelude[1])
            print("        .scope_flags = %u," % self.prelude[2])
            print("        .n_pos_args = %u," % self.prelude[3])
            print("        .n_kwonly_args = %u," % self.prelude[4])
            print("        .n_def_pos_args = %u," % self.prelude[5])
            print("        .qstr_block_name_idx = %u," % self.names[0])
            print(
                "        .line_info = fun_data_%s + %u,"
                % (self.escaped_name, self.offset_line_info)
            )
            print(
                "        .opcodes = fun_data_%s + %u," % (self.escaped_name, self.offset_opcodes)
            )
            print("    },")
            print("    .line_of_definition = %u," % 0)  # TODO
            print("    #endif")
        print("    #if MICROPY_EMIT_MACHINE_CODE")
        print("    .prelude_offset = %u," % self.prelude_offset)
        print("    .n_qstr = %u," % len(qstr_links))
        print("    .qstr_link = NULL,")  # TODO
        print("    #endif")
        print("    #endif")
        print("    #if MICROPY_EMIT_MACHINE_CODE")
        print("    .type_sig = %u," % type_sig)
        print("    #endif")
        print("};")

        global raw_code_count, raw_code_content
        raw_code_count += 1
        raw_code_content += 4 * 4


class RawCodeBytecode(RawCode):
    def __init__(self, cm_escaped_name, qstr_table, obj_table, fun_data):
        self.obj_table = obj_table
        super(RawCodeBytecode, self).__init__(
            cm_escaped_name, qstr_table, fun_data, 0, MP_CODE_BYTECODE
        )

    def disassemble(self):
        bc = self.fun_data
        print("simple_name:", self.simple_name.str)
        print("  raw bytecode:", len(bc), hexlify_to_str(bc))
        print("  prelude:", self.prelude)
        print("  args:", [self.qstr_table[i].str for i in self.names[1:]])
        print("  line info:", hexlify_to_str(bc[self.offset_line_info : self.offset_opcodes]))
        ip = self.offset_opcodes
        while ip < len(bc):
            fmt, sz, arg = mp_opcode_decode(bc, ip)
            if bc[ip] == Opcodes.MP_BC_LOAD_CONST_OBJ:
                arg = repr(self.obj_table[arg])
            if fmt == MP_BC_FORMAT_QSTR:
                arg = self.qstr_table[arg].str
            elif fmt in (MP_BC_FORMAT_VAR_UINT, MP_BC_FORMAT_OFFSET):
                pass
            else:
                arg = ""
            print(
                "  %-11s %s %s" % (hexlify_to_str(bc[ip : ip + sz]), Opcodes.mapping[bc[ip]], arg)
            )
            ip += sz
        self.disassemble_children()

    def freeze(self):
        # generate bytecode data
        bc = self.fun_data
        print(
            "// frozen bytecode for file %s, scope %s"
            % (self.qstr_table[0].str, self.escaped_name)
        )
        print("static const byte fun_data_%s[%u] = {" % (self.escaped_name, len(bc)))

        print("    ", end="")
        for b in bc[: self.offset_names]:
            print("0x%02x," % b, end="")
        print(" // prelude")

        print("    ", end="")
        for b in bc[self.offset_names : self.offset_line_info]:
            print("0x%02x," % b, end="")
        print(" // names: %s" % ", ".join(self.qstr_table[i].str for i in self.names))

        print("    ", end="")
        for b in bc[self.offset_line_info : self.offset_opcodes]:
            print("0x%02x," % b, end="")
        print(" // code info")

        ip = self.offset_opcodes
        while ip < len(bc):
            fmt, sz, arg = mp_opcode_decode(bc, ip)
            opcode_name = Opcodes.mapping[bc[ip]]
            if fmt == MP_BC_FORMAT_QSTR:
                opcode_name += " " + repr(self.qstr_table[arg].str)
            elif fmt in (MP_BC_FORMAT_VAR_UINT, MP_BC_FORMAT_OFFSET):
                opcode_name += " %u" % arg
            print(
                "    %s, // %s" % (",".join("0x%02x" % b for b in bc[ip : ip + sz]), opcode_name)
            )
            ip += sz

        print("};")

        self.freeze_children()
        self.freeze_raw_code()

        global bc_content
        bc_content += len(bc)


class RawCodeNative(RawCode):
    def __init__(
        self,
        cm_escaped_name,
        qstr_table,
        kind,
        fun_data,
        prelude_offset,
        qstr_links,
        scope_flags,
        n_pos_args,
        type_sig,
    ):
        super(RawCodeNative, self).__init__(
            cm_escaped_name, qstr_table, fun_data, prelude_offset, kind
        )

        if kind in (MP_CODE_NATIVE_VIPER, MP_CODE_NATIVE_ASM):
            self.scope_flags = scope_flags
            self.n_pos_args = n_pos_args

        self.qstr_links = qstr_links
        self.type_sig = type_sig
        if config.native_arch in (
            MP_NATIVE_ARCH_X86,
            MP_NATIVE_ARCH_X64,
            MP_NATIVE_ARCH_XTENSA,
            MP_NATIVE_ARCH_XTENSAWIN,
        ):
            self.fun_data_attributes = '__attribute__((section(".text,\\"ax\\",@progbits # ")))'
        else:
            self.fun_data_attributes = '__attribute__((section(".text,\\"ax\\",%progbits @ ")))'

        # Allow single-byte alignment by default for x86/x64.
        # ARM needs word alignment, ARM Thumb needs halfword, due to instruction size.
        # Xtensa needs word alignment due to the 32-bit constant table embedded in the code.
        if config.native_arch in (
            MP_NATIVE_ARCH_ARMV6,
            MP_NATIVE_ARCH_XTENSA,
            MP_NATIVE_ARCH_XTENSAWIN,
        ):
            # ARMV6 or Xtensa -- four byte align.
            self.fun_data_attributes += " __attribute__ ((aligned (4)))"
        elif MP_NATIVE_ARCH_ARMV6M <= config.native_arch <= MP_NATIVE_ARCH_ARMV7EMDP:
            # ARMVxxM -- two byte align.
            self.fun_data_attributes += " __attribute__ ((aligned (2)))"

    def disassemble(self):
        fun_data = self.fun_data
        print("simple_name:", self.simple_name.str)
        print(
            "  raw data:",
            len(fun_data),
            hexlify_to_str(fun_data[:32]),
            "..." if len(fun_data) > 32 else "",
        )
        if self.code_kind != MP_CODE_NATIVE_PY:
            return
        print("  prelude:", self.prelude)
        print("  args:", [self.qstr_table[i].str for i in self.names[1:]])
        print("  line info:", fun_data[self.offset_line_info : self.offset_opcodes])
        ip = 0
        while ip < self.prelude_offset:
            sz = 16
            print(" ", hexlify_to_str(fun_data[ip : min(ip + sz, self.prelude_offset)]))
            ip += sz
        self.disassemble_children()

    def _asm_thumb_rewrite_mov(self, pc, val):
        print("    (%u & 0xf0) | (%s >> 12)," % (self.fun_data[pc], val), end="")
        print(" (%u & 0xfb) | (%s >> 9 & 0x04)," % (self.fun_data[pc + 1], val), end="")
        print(" (%s & 0xff)," % (val,), end="")
        print(" (%u & 0x07) | (%s >> 4 & 0x70)," % (self.fun_data[pc + 3], val))

    def _link_qstr(self, pc, kind, qst):
        if kind == 0:
            # Generic 16-bit link
            print("    %s & 0xff, %s >> 8," % (qst, qst))
            return 2
        else:
            # Architecture-specific link
            is_obj = kind == 2
            if is_obj:
                qst = "((uintptr_t)MP_OBJ_NEW_QSTR(%s))" % qst
            if config.native_arch in (
                MP_NATIVE_ARCH_X86,
                MP_NATIVE_ARCH_X64,
                MP_NATIVE_ARCH_ARMV6,
                MP_NATIVE_ARCH_XTENSA,
                MP_NATIVE_ARCH_XTENSAWIN,
            ):
                print(
                    "    %s & 0xff, (%s >> 8) & 0xff, (%s >> 16) & 0xff, %s >> 24,"
                    % (qst, qst, qst, qst)
                )
                return 4
            elif MP_NATIVE_ARCH_ARMV6M <= config.native_arch <= MP_NATIVE_ARCH_ARMV7EMDP:
                if is_obj:
                    # qstr object, movw and movt
                    self._asm_thumb_rewrite_mov(pc, qst)
                    self._asm_thumb_rewrite_mov(pc + 4, "(%s >> 16)" % qst)
                    return 8
                else:
                    # qstr number, movw instruction
                    self._asm_thumb_rewrite_mov(pc, qst)
                    return 4
            else:
                assert 0

    def freeze(self):
        if self.scope_flags & ~0x0F:
            raise FreezeError("unable to freeze code with relocations")

        # generate native code data
        print()
        print(
            "// frozen native code for file %s, scope %s"
            % (self.qstr_table[0].str, self.escaped_name)
        )
        print(
            "static const byte fun_data_%s[%u] %s = {"
            % (self.escaped_name, len(self.fun_data), self.fun_data_attributes)
        )

        i_top = len(self.fun_data)
        i = 0
        qi = 0
        while i < i_top:
            if qi < len(self.qstr_links) and i == self.qstr_links[qi][0]:
                # link qstr
                qi_off, qi_kind, qi_val = self.qstr_links[qi]
                i += self._link_qstr(i, qi_kind, qi_val.qstr_id)
                qi += 1
            else:
                # copy machine code (max 16 bytes)
                i16 = min(i + 16, i_top)
                if qi < len(self.qstr_links):
                    i16 = min(i16, self.qstr_links[qi][0])
                print("   ", end="")
                for ii in range(i, i16):
                    print(" 0x%02x," % self.fun_data[ii], end="")
                print()
                i = i16

        print("};")

        self.freeze_children()
        self.freeze_raw_code(self.qstr_links, self.type_sig)


class MPYSegment:
    META = 0
    QSTR = 1
    OBJ = 2
    CODE = 3

    def __init__(self, kind, name, start, end):
        self.kind = kind
        self.name = name
        self.start = start
        self.end = end


class MPYReader:
    def __init__(self, filename, fileobj):
        self.filename = filename
        self.fileobj = fileobj

    def tell(self):
        return self.fileobj.tell()

    def read_byte(self):
        return bytes_cons(self.fileobj.read(1))[0]

    def read_bytes(self, n):
        return bytes_cons(self.fileobj.read(n))

    def read_uint(self):
        i = 0
        while True:
            b = self.read_byte()
            i = (i << 7) | (b & 0x7F)
            if b & 0x80 == 0:
                break
        return i


def read_qstr(reader, segments):
    start_pos = reader.tell()
    ln = reader.read_uint()
    if ln & 1:
        # static qstr
        q = global_qstrs.get_by_index(ln >> 1)
        segments.append(MPYSegment(MPYSegment.META, q.str, start_pos, start_pos))
        return q
    ln >>= 1
    start_pos = reader.tell()
    data = str_cons(reader.read_bytes(ln), "utf8")
    reader.read_byte()  # read and discard null terminator
    segments.append(MPYSegment(MPYSegment.QSTR, data, start_pos, reader.tell()))
    return global_qstrs.add(data)


def read_obj(reader, segments):
    obj_type = reader.read_byte()
    if obj_type == MP_PERSISTENT_OBJ_FUN_TABLE:
        return MPFunTable()
    elif obj_type == MP_PERSISTENT_OBJ_NONE:
        return None
    elif obj_type == MP_PERSISTENT_OBJ_FALSE:
        return False
    elif obj_type == MP_PERSISTENT_OBJ_TRUE:
        return True
    elif obj_type == MP_PERSISTENT_OBJ_ELLIPSIS:
        return Ellipsis
    elif obj_type == MP_PERSISTENT_OBJ_TUPLE:
        ln = reader.read_uint()
        return tuple(read_obj(reader, segments) for _ in range(ln))
    else:
        ln = reader.read_uint()
        start_pos = reader.tell()
        buf = reader.read_bytes(ln)
        if obj_type in (MP_PERSISTENT_OBJ_STR, MP_PERSISTENT_OBJ_BYTES):
            reader.read_byte()  # read and discard null terminator
        if obj_type == MP_PERSISTENT_OBJ_STR:
            obj = str_cons(buf, "utf8")
            if len(obj) < PERSISTENT_STR_INTERN_THRESHOLD:
                if not global_qstrs.find_by_str(obj):
                    global_qstrs.add(obj)
        elif obj_type == MP_PERSISTENT_OBJ_BYTES:
            obj = buf
        elif obj_type == MP_PERSISTENT_OBJ_INT:
            obj = int(str_cons(buf, "ascii"), 10)
        elif obj_type == MP_PERSISTENT_OBJ_FLOAT:
            obj = float(str_cons(buf, "ascii"))
        elif obj_type == MP_PERSISTENT_OBJ_COMPLEX:
            obj = complex(str_cons(buf, "ascii"))
        else:
            raise MPYReadError(reader.filename, "corrupt .mpy file")
        segments.append(MPYSegment(MPYSegment.OBJ, obj, start_pos, reader.tell()))
        return obj


def read_raw_code(reader, cm_escaped_name, qstr_table, obj_table, segments):
    # Read raw code header.
    kind_len = reader.read_uint()
    kind = (kind_len & 3) + MP_CODE_BYTECODE
    has_children = (kind_len >> 2) & 1
    fun_data_len = kind_len >> 3

    # Read the body of the raw code.
    file_offset = reader.tell()
    fun_data = reader.read_bytes(fun_data_len)
    segments_len = len(segments)

    if kind == MP_CODE_BYTECODE:
        # Create bytecode raw code.
        rc = RawCodeBytecode(cm_escaped_name, qstr_table, obj_table, fun_data)
    else:
        # Create native raw code.
        qstr_links = []
        if kind in (MP_CODE_NATIVE_PY, MP_CODE_NATIVE_VIPER):
            # Read qstr link table.
            n_qstr_link = reader.read_uint()
            for _ in range(n_qstr_link):
                off = reader.read_uint()
                qst = read_qstr(reader, segments)
                qstr_links.append((off >> 2, off & 3, qst))

        native_scope_flags = 0
        native_n_pos_args = 0
        native_type_sig = 0
        if kind == MP_CODE_NATIVE_PY:
            prelude_offset = reader.read_uint()
        else:
            prelude_offset = 0
            native_scope_flags = reader.read_uint()
            if kind == MP_CODE_NATIVE_VIPER:
                # Read any additional sections for native viper.
                if native_scope_flags & MP_SCOPE_FLAG_VIPERRODATA:
                    rodata_size = reader.read_uint()
                if native_scope_flags & MP_SCOPE_FLAG_VIPERBSS:
                    bss_size = reader.read_uint()
                if native_scope_flags & MP_SCOPE_FLAG_VIPERRODATA:
                    reader.read_bytes(rodata_size)
                if native_scope_flags & MP_SCOPE_FLAG_VIPERRELOC:
                    while True:
                        op = reader.read_byte()
                        if op == 0xFF:
                            break
                        if op & 1:
                            addr = reader.read_uint()
                        op >>= 1
                        if op <= 5 and op & 1:
                            n = reader.read_uint()
            else:
                assert kind == MP_CODE_NATIVE_ASM
                native_n_pos_args = reader.read_uint()
                native_type_sig = reader.read_uint()

        rc = RawCodeNative(
            cm_escaped_name,
            qstr_table,
            kind,
            fun_data,
            prelude_offset,
            qstr_links,
            native_scope_flags,
            native_n_pos_args,
            native_type_sig,
        )

    # Add a segment for the raw code data.
    segments.insert(
        segments_len,
        MPYSegment(MPYSegment.CODE, rc.simple_name.str, file_offset, file_offset + fun_data_len),
    )

    # Read children, if there are any.
    rc.children = []
    if has_children:
        n_children = reader.read_uint()
        for _ in range(n_children):
            rc.children.append(
                read_raw_code(reader, cm_escaped_name, qstr_table, obj_table, segments)
            )

    return rc


def read_mpy(filename):
    with open(filename, "rb") as fileobj:
        reader = MPYReader(filename, fileobj)
        segments = []

        # Read and verify the header.
        header = reader.read_bytes(4)
        if header[0] != ord("M"):
            raise MPYReadError(filename, "not a valid .mpy file")
        if header[1] != config.MPY_VERSION:
            raise MPYReadError(filename, "incompatible .mpy version")
        feature_byte = header[2]
        config.MICROPY_PY_BUILTINS_STR_UNICODE = (feature_byte & 2) != 0
        mpy_native_arch = feature_byte >> 2
        if mpy_native_arch != MP_NATIVE_ARCH_NONE:
            if config.native_arch == MP_NATIVE_ARCH_NONE:
                config.native_arch = mpy_native_arch
            elif config.native_arch != mpy_native_arch:
                raise MPYReadError(filename, "native architecture mismatch")
        config.mp_small_int_bits = header[3]

        # Read number of qstrs, and number of objects.
        n_qstr = reader.read_uint()
        n_obj = reader.read_uint()

        # Read qstrs and construct qstr table.
        qstr_table = []
        for i in range(n_qstr):
            qstr_table.append(read_qstr(reader, segments))

        # Read objects and construct object table.
        obj_table = []
        for i in range(n_obj):
            obj_table.append(read_obj(reader, segments))

        # Compute the compiled-module escaped name.
        cm_escaped_name = qstr_table[0].str.replace("/", "_")[:-3]

        # Read the outer raw code, which will in turn read all its children.
        raw_code_file_offset = reader.tell()
        raw_code = read_raw_code(reader, cm_escaped_name, qstr_table, obj_table, segments)

    # Create the outer-level compiled module representing the whole .mpy file.
    return CompiledModule(
        filename,
        segments,
        header,
        qstr_table,
        obj_table,
        raw_code,
        raw_code_file_offset,
        cm_escaped_name,
    )


def hexdump_mpy(compiled_modules):
    for cm in compiled_modules:
        cm.hexdump()


def disassemble_mpy(compiled_modules):
    for cm in compiled_modules:
        cm.disassemble()


def freeze_mpy(base_qstrs, compiled_modules):
    # add to qstrs
    new = {}
    for q in global_qstrs.qstrs:
        # don't add duplicates
        if q is None or q.qstr_esc in base_qstrs or q.qstr_esc in new:
            continue
        new[q.qstr_esc] = (len(new), q.qstr_esc, q.str, bytes_cons(q.str, "utf8"))
    new = sorted(new.values(), key=lambda x: x[0])

    print('#include "py/mpconfig.h"')
    print('#include "py/objint.h"')
    print('#include "py/objstr.h"')
    print('#include "py/emitglue.h"')
    print('#include "py/nativeglue.h"')
    print()

    print("#if MICROPY_LONGINT_IMPL != %u" % config.MICROPY_LONGINT_IMPL)
    print('#error "incompatible MICROPY_LONGINT_IMPL"')
    print("#endif")
    print()

    if config.MICROPY_LONGINT_IMPL == config.MICROPY_LONGINT_IMPL_MPZ:
        print("#if MPZ_DIG_SIZE != %u" % config.MPZ_DIG_SIZE)
        print('#error "incompatible MPZ_DIG_SIZE"')
        print("#endif")
        print()

    print("#if MICROPY_PY_BUILTINS_FLOAT")
    print("typedef struct _mp_obj_float_t {")
    print("    mp_obj_base_t base;")
    print("    mp_float_t value;")
    print("} mp_obj_float_t;")
    print("#endif")
    print()

    print("#if MICROPY_PY_BUILTINS_COMPLEX")
    print("typedef struct _mp_obj_complex_t {")
    print("    mp_obj_base_t base;")
    print("    mp_float_t real;")
    print("    mp_float_t imag;")
    print("} mp_obj_complex_t;")
    print("#endif")
    print()

    if len(new) > 0:
        print("enum {")
        for i in range(len(new)):
            if i == 0:
                print("    MP_QSTR_%s = MP_QSTRnumber_of," % new[i][1])
            else:
                print("    MP_QSTR_%s," % new[i][1])
        print("};")

    # As in qstr.c, set so that the first dynamically allocated pool is twice this size; must be <= the len
    qstr_pool_alloc = min(len(new), 10)

    global bc_content, const_str_content, const_int_content, const_obj_content, const_table_qstr_content, const_table_ptr_content, raw_code_count, raw_code_content
    qstr_content = 0
    bc_content = 0
    const_str_content = 0
    const_int_content = 0
    const_obj_content = 0
    const_table_qstr_content = 0
    const_table_ptr_content = 0
    raw_code_count = 0
    raw_code_content = 0

    print()
    print("const qstr_hash_t mp_qstr_frozen_const_hashes[] = {")
    qstr_size = {"metadata": 0, "data": 0}
    for _, _, _, qbytes in new:
        qhash = qstrutil.compute_hash(qbytes, config.MICROPY_QSTR_BYTES_IN_HASH)
        print("    %d," % qhash)
    print("};")
    print()
    print("const qstr_len_t mp_qstr_frozen_const_lengths[] = {")
    for _, _, _, qbytes in new:
        print("    %d," % len(qbytes))
        qstr_size["metadata"] += (
            config.MICROPY_QSTR_BYTES_IN_LEN + config.MICROPY_QSTR_BYTES_IN_HASH
        )
        qstr_size["data"] += len(qbytes)
    print("};")
    print()
    print("extern const qstr_pool_t mp_qstr_const_pool;")
    print("const qstr_pool_t mp_qstr_frozen_const_pool = {")
    print("    &mp_qstr_const_pool, // previous pool")
    print("    MP_QSTRnumber_of, // previous pool size")
    print("    %u, // allocated entries" % qstr_pool_alloc)
    print("    %u, // used entries" % len(new))
    print("    (qstr_hash_t *)mp_qstr_frozen_const_hashes,")
    print("    (qstr_len_t *)mp_qstr_frozen_const_lengths,")
    print("    {")
    for _, _, qstr, qbytes in new:
        print('        "%s",' % qstrutil.escape_bytes(qstr, qbytes))
        qstr_content += (
            config.MICROPY_QSTR_BYTES_IN_LEN + config.MICROPY_QSTR_BYTES_IN_HASH + len(qbytes) + 1
        )
    print("    },")
    print("};")

    # Freeze all modules.
    for idx, cm in enumerate(compiled_modules):
        cm.freeze(idx)

    # Print separator, separating individual modules from global data structures.
    print()
    print("/" * 80)
    print("// collection of all frozen modules")

    # Define the string of frozen module names.
    print()
    print("const char mp_frozen_names[] = {")
    print("    #ifdef MP_FROZEN_STR_NAMES")
    # makemanifest.py might also include some frozen string content.
    print("    MP_FROZEN_STR_NAMES")
    print("    #endif")
    mp_frozen_mpy_names_content = 1
    for cm in compiled_modules:
        module_name = cm.source_file.str
        print('    "%s\\0"' % module_name)
        mp_frozen_mpy_names_content += len(cm.source_file.str) + 1
    print('    "\\0"')
    print("};")

    # Define the array of pointers to frozen module content.
    print()
    print("const mp_frozen_module_t *const mp_frozen_mpy_content[] = {")
    for cm in compiled_modules:
        print("    &frozen_module_%s," % cm.escaped_name)
    print("};")
    mp_frozen_mpy_content_size = len(compiled_modules * 4)

    # If a port defines MICROPY_FROZEN_LIST_ITEM then list all modules wrapped in that macro.
    print()
    print("#ifdef MICROPY_FROZEN_LIST_ITEM")
    for cm in compiled_modules:
        module_name = cm.source_file.str
        if module_name.endswith("/__init__.py"):
            short_name = module_name[: -len("/__init__.py")]
        else:
            short_name = module_name[: -len(".py")]
        print('MICROPY_FROZEN_LIST_ITEM("%s", "%s")' % (short_name, module_name))
    print("#endif")

    print()
    print("/*")
    print("byte sizes:")
    print("qstr content: %d unique, %d bytes" % (len(new), qstr_content))
    print("bc content: %d" % bc_content)
    print("const str content: %d" % const_str_content)
    print("const int content: %d" % const_int_content)
    print("const obj content: %d" % const_obj_content)
    print(
        "const table qstr content: %d entries, %d bytes"
        % (const_table_qstr_content, const_table_qstr_content * 4)
    )
    print(
        "const table ptr content: %d entries, %d bytes"
        % (const_table_ptr_content, const_table_ptr_content * 4)
    )
    print("raw code content: %d * 4 = %d" % (raw_code_count, raw_code_content))
    print("mp_frozen_mpy_names_content: %d" % mp_frozen_mpy_names_content)
    print("mp_frozen_mpy_content_size: %d" % mp_frozen_mpy_content_size)
    print(
        "total: %d"
        % (
            qstr_content
            + bc_content
            + const_str_content
            + const_int_content
            + const_obj_content
            + const_table_qstr_content * 4
            + const_table_ptr_content * 4
            + raw_code_content
            + mp_frozen_mpy_names_content
            + mp_frozen_mpy_content_size
        )
    )
    print("*/")


def merge_mpy(raw_codes, output_file):
    assert len(raw_codes) <= 2  # so var-uints all fit in 1 byte
    merged_mpy = bytearray()

    if len(raw_codes) == 1:
        with open(raw_codes[0].mpy_source_file, "rb") as f:
            merged_mpy.extend(f.read())
    else:
        main_rc = None
        for rc in raw_codes:
            if len(rc.qstr_table) > 1 or len(rc.obj_table) > 0:
                # Must use qstr_table and obj_table from this raw_code
                if main_rc is not None:
                    raise Exception(
                        "can't merge files when more than one has a populated qstr or obj table"
                    )
                main_rc = rc
        if main_rc is None:
            main_rc = raw_codes[0]

        header = bytearray(4)
        header[0] = ord("M")
        header[1] = config.MPY_VERSION
        header[2] = config.native_arch << 2 | config.MICROPY_PY_BUILTINS_STR_UNICODE << 1
        header[3] = config.mp_small_int_bits
        merged_mpy.extend(header)

        # Copy n_qstr, n_obj, qstr_table, obj_table from main_rc.
        with open(main_rc.mpy_source_file, "rb") as f:
            data = f.read(main_rc.raw_code_file_offset)
            merged_mpy.extend(data[4:])

        bytecode = bytearray()
        bytecode_len = 3 + len(raw_codes) * 5 + 2
        bytecode.append(bytecode_len << 3 | 1 << 2)  # kind, has_children and length
        bytecode.append(0b00000000)  # signature prelude
        bytecode.append(0b00000010)  # size prelude; n_info=1
        bytecode.extend(b"\x00")  # simple_name: qstr index 0 (will use source filename)
        for idx in range(len(raw_codes)):
            bytecode.append(0x32)  # MP_BC_MAKE_FUNCTION
            bytecode.append(idx)  # index raw code
            bytecode.extend(b"\x34\x00\x59")  # MP_BC_CALL_FUNCTION, 0 args, MP_BC_POP_TOP
        bytecode.extend(b"\x51\x63")  # MP_BC_LOAD_NONE, MP_BC_RETURN_VALUE

        merged_mpy.extend(bytecode)

        merged_mpy.append(len(raw_codes))  # n_children

        for rc in raw_codes:
            with open(rc.mpy_source_file, "rb") as f:
                f.seek(rc.raw_code_file_offset)
                data = f.read()  # read rest of mpy file
                merged_mpy.extend(data)

    if output_file is None:
        sys.stdout.buffer.write(merged_mpy)
    else:
        with open(output_file, "wb") as f:
            f.write(merged_mpy)


def main():
    global global_qstrs

    import argparse

    cmd_parser = argparse.ArgumentParser(description="A tool to work with MicroPython .mpy files.")
    cmd_parser.add_argument(
        "-x", "--hexdump", action="store_true", help="output an annotated hex dump of files"
    )
    cmd_parser.add_argument(
        "-d", "--disassemble", action="store_true", help="output disassembled contents of files"
    )
    cmd_parser.add_argument("-f", "--freeze", action="store_true", help="freeze files")
    cmd_parser.add_argument(
        "--merge", action="store_true", help="merge multiple .mpy files into one"
    )
    cmd_parser.add_argument("-q", "--qstr-header", help="qstr header file to freeze against")
    cmd_parser.add_argument(
        "-mlongint-impl",
        choices=["none", "longlong", "mpz"],
        default="mpz",
        help="long-int implementation used by target (default mpz)",
    )
    cmd_parser.add_argument(
        "-mmpz-dig-size",
        metavar="N",
        type=int,
        default=16,
        help="mpz digit size used by target (default 16)",
    )
    cmd_parser.add_argument("-o", "--output", default=None, help="output file")
    cmd_parser.add_argument("files", nargs="+", help="input .mpy files")
    args = cmd_parser.parse_args()

    # set config values relevant to target machine
    config.MICROPY_LONGINT_IMPL = {
        "none": config.MICROPY_LONGINT_IMPL_NONE,
        "longlong": config.MICROPY_LONGINT_IMPL_LONGLONG,
        "mpz": config.MICROPY_LONGINT_IMPL_MPZ,
    }[args.mlongint_impl]
    config.MPZ_DIG_SIZE = args.mmpz_dig_size
    config.native_arch = MP_NATIVE_ARCH_NONE

    # set config values for qstrs, and get the existing base set of qstrs
    if args.qstr_header:
        qcfgs, base_qstrs = qstrutil.parse_input_headers([args.qstr_header])
        config.MICROPY_QSTR_BYTES_IN_LEN = int(qcfgs["BYTES_IN_LEN"])
        config.MICROPY_QSTR_BYTES_IN_HASH = int(qcfgs["BYTES_IN_HASH"])
    else:
        config.MICROPY_QSTR_BYTES_IN_LEN = 1
        config.MICROPY_QSTR_BYTES_IN_HASH = 1
        base_qstrs = list(qstrutil.static_qstr_list)

    # Create initial list of global qstrs.
    global_qstrs = GlobalQStrList()

    # Load all .mpy files.
    try:
        compiled_modules = [read_mpy(file) for file in args.files]
    except MPYReadError as er:
        print(er, file=sys.stderr)
        sys.exit(1)

    if args.hexdump:
        hexdump_mpy(compiled_modules)

    if args.disassemble:
        if args.hexdump:
            print()
        disassemble_mpy(compiled_modules)

    if args.freeze:
        try:
            freeze_mpy(base_qstrs, compiled_modules)
        except FreezeError as er:
            print(er, file=sys.stderr)
            sys.exit(1)

    if args.merge:
        merge_mpy(compiled_modules, args.output)


if __name__ == "__main__":
    main()
