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
if platform.python_version_tuple()[0] == '2':
    str_cons = lambda val, enc=None: val
    bytes_cons = lambda val, enc=None: bytearray(val)
    is_str_type = lambda o: type(o) is str
    is_bytes_type = lambda o: type(o) is bytearray
    is_int_type = lambda o: type(o) is int or type(o) is long
else:
    str_cons = str
    bytes_cons = bytes
    is_str_type = lambda o: type(o) is str
    is_bytes_type = lambda o: type(o) is bytes
    is_int_type = lambda o: type(o) is int
# end compatibility code

import sys
import struct
from collections import namedtuple

sys.path.append(sys.path[0] + '/../py')
import makeqstrdata as qstrutil

class FreezeError(Exception):
    def __init__(self, rawcode, msg):
        self.rawcode = rawcode
        self.msg = msg

    def __str__(self):
        return 'error while freezing %s: %s' % (self.rawcode.source_file, self.msg)

class Config:
    MPY_VERSION = 4
    MICROPY_LONGINT_IMPL_NONE = 0
    MICROPY_LONGINT_IMPL_LONGLONG = 1
    MICROPY_LONGINT_IMPL_MPZ = 2
config = Config()

class QStrType:
    def __init__(self, str):
        self.str = str
        self.qstr_esc = qstrutil.qstr_escape(self.str)
        self.qstr_id = 'MP_QSTR_' + self.qstr_esc

# Initialise global list of qstrs with static qstrs
global_qstrs = [None] # MP_QSTR_NULL should never be referenced
for n in qstrutil.static_qstr_list:
    global_qstrs.append(QStrType(n))

class QStrWindow:
    def __init__(self, size):
        self.window = []
        self.size = size

    def push(self, val):
        self.window = [val] + self.window[:self.size - 1]

    def access(self, idx):
        val = self.window[idx]
        self.window = [val] + self.window[:idx] + self.window[idx + 1:]
        return val

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

MP_OPCODE_BYTE = 0
MP_OPCODE_QSTR = 1
MP_OPCODE_VAR_UINT = 2
MP_OPCODE_OFFSET = 3

# extra bytes:
MP_BC_UNWIND_JUMP = 0x46
MP_BC_MAKE_CLOSURE = 0x62
MP_BC_MAKE_CLOSURE_DEFARGS = 0x63
MP_BC_RAISE_VARARGS = 0x5c
# extra byte if caching enabled:
MP_BC_LOAD_NAME = 0x1b
MP_BC_LOAD_GLOBAL = 0x1c
MP_BC_LOAD_ATTR = 0x1d
MP_BC_STORE_ATTR = 0x26

def make_opcode_format():
    def OC4(a, b, c, d):
        return a | (b << 2) | (c << 4) | (d << 6)
    U = 0
    B = 0
    Q = 1
    V = 2
    O = 3
    return bytes_cons((
    # this table is taken verbatim from py/bc.c
    OC4(U, U, U, U), # 0x00-0x03
    OC4(U, U, U, U), # 0x04-0x07
    OC4(U, U, U, U), # 0x08-0x0b
    OC4(U, U, U, U), # 0x0c-0x0f
    OC4(B, B, B, U), # 0x10-0x13
    OC4(V, U, Q, V), # 0x14-0x17
    OC4(B, V, V, Q), # 0x18-0x1b
    OC4(Q, Q, Q, Q), # 0x1c-0x1f
    OC4(B, B, V, V), # 0x20-0x23
    OC4(Q, Q, Q, B), # 0x24-0x27
    OC4(V, V, Q, Q), # 0x28-0x2b
    OC4(U, U, U, U), # 0x2c-0x2f
    OC4(B, B, B, B), # 0x30-0x33
    OC4(B, O, O, O), # 0x34-0x37
    OC4(O, O, U, U), # 0x38-0x3b
    OC4(U, O, B, O), # 0x3c-0x3f
    OC4(O, B, B, O), # 0x40-0x43
    OC4(O, U, O, B), # 0x44-0x47
    OC4(U, U, U, U), # 0x48-0x4b
    OC4(U, U, U, U), # 0x4c-0x4f
    OC4(V, V, U, V), # 0x50-0x53
    OC4(B, U, V, V), # 0x54-0x57
    OC4(V, V, V, B), # 0x58-0x5b
    OC4(B, B, B, U), # 0x5c-0x5f
    OC4(V, V, V, V), # 0x60-0x63
    OC4(V, V, V, V), # 0x64-0x67
    OC4(Q, Q, B, U), # 0x68-0x6b
    OC4(U, U, U, U), # 0x6c-0x6f

    OC4(B, B, B, B), # 0x70-0x73
    OC4(B, B, B, B), # 0x74-0x77
    OC4(B, B, B, B), # 0x78-0x7b
    OC4(B, B, B, B), # 0x7c-0x7f
    OC4(B, B, B, B), # 0x80-0x83
    OC4(B, B, B, B), # 0x84-0x87
    OC4(B, B, B, B), # 0x88-0x8b
    OC4(B, B, B, B), # 0x8c-0x8f
    OC4(B, B, B, B), # 0x90-0x93
    OC4(B, B, B, B), # 0x94-0x97
    OC4(B, B, B, B), # 0x98-0x9b
    OC4(B, B, B, B), # 0x9c-0x9f
    OC4(B, B, B, B), # 0xa0-0xa3
    OC4(B, B, B, B), # 0xa4-0xa7
    OC4(B, B, B, B), # 0xa8-0xab
    OC4(B, B, B, B), # 0xac-0xaf

    OC4(B, B, B, B), # 0xb0-0xb3
    OC4(B, B, B, B), # 0xb4-0xb7
    OC4(B, B, B, B), # 0xb8-0xbb
    OC4(B, B, B, B), # 0xbc-0xbf

    OC4(B, B, B, B), # 0xc0-0xc3
    OC4(B, B, B, B), # 0xc4-0xc7
    OC4(B, B, B, B), # 0xc8-0xcb
    OC4(B, B, B, B), # 0xcc-0xcf

    OC4(B, B, B, B), # 0xd0-0xd3
    OC4(U, U, U, B), # 0xd4-0xd7
    OC4(B, B, B, B), # 0xd8-0xdb
    OC4(B, B, B, B), # 0xdc-0xdf

    OC4(B, B, B, B), # 0xe0-0xe3
    OC4(B, B, B, B), # 0xe4-0xe7
    OC4(B, B, B, B), # 0xe8-0xeb
    OC4(B, B, B, B), # 0xec-0xef

    OC4(B, B, B, B), # 0xf0-0xf3
    OC4(B, B, B, B), # 0xf4-0xf7
    OC4(U, U, U, U), # 0xf8-0xfb
    OC4(U, U, U, U), # 0xfc-0xff
    ))

# this function mirrors that in py/bc.c
def mp_opcode_format(bytecode, ip, count_var_uint, opcode_format=make_opcode_format()):
    opcode = bytecode[ip]
    ip_start = ip
    f = (opcode_format[opcode >> 2] >> (2 * (opcode & 3))) & 3
    if f == MP_OPCODE_QSTR:
        if config.MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE:
            if (opcode == MP_BC_LOAD_NAME
                or opcode == MP_BC_LOAD_GLOBAL
                or opcode == MP_BC_LOAD_ATTR
                or opcode == MP_BC_STORE_ATTR):
                ip += 1
        ip += 3
    else:
        extra_byte = (
            opcode == MP_BC_UNWIND_JUMP
            or opcode == MP_BC_RAISE_VARARGS
            or opcode == MP_BC_MAKE_CLOSURE
            or opcode == MP_BC_MAKE_CLOSURE_DEFARGS
        )
        ip += 1
        if f == MP_OPCODE_VAR_UINT:
            if count_var_uint:
                while bytecode[ip] & 0x80 != 0:
                    ip += 1
                ip += 1
        elif f == MP_OPCODE_OFFSET:
            ip += 2
        ip += extra_byte
    return f, ip - ip_start

def decode_uint(bytecode, ip):
    unum = 0
    while True:
        val = bytecode[ip]
        ip += 1
        unum = (unum << 7) | (val & 0x7f)
        if not (val & 0x80):
            break
    return ip, unum

def extract_prelude(bytecode, ip):
    ip, n_state = decode_uint(bytecode, ip)
    ip, n_exc_stack = decode_uint(bytecode, ip)
    scope_flags = bytecode[ip]; ip += 1
    n_pos_args = bytecode[ip]; ip += 1
    n_kwonly_args = bytecode[ip]; ip += 1
    n_def_pos_args = bytecode[ip]; ip += 1
    ip2, code_info_size = decode_uint(bytecode, ip)
    ip += code_info_size
    while bytecode[ip] != 0xff:
        ip += 1
    ip += 1
    # ip now points to first opcode
    # ip2 points to simple_name qstr
    return ip, ip2, (n_state, n_exc_stack, scope_flags, n_pos_args, n_kwonly_args, n_def_pos_args, code_info_size)

class MPFunTable:
    pass

class RawCode(object):
    # a set of all escaped names, to make sure they are unique
    escaped_names = set()

    # convert code kind number to string
    code_kind_str = {
       MP_CODE_BYTECODE: 'MP_CODE_BYTECODE',
       MP_CODE_NATIVE_PY: 'MP_CODE_NATIVE_PY',
       MP_CODE_NATIVE_VIPER: 'MP_CODE_NATIVE_VIPER',
       MP_CODE_NATIVE_ASM: 'MP_CODE_NATIVE_ASM',
    }

    def __init__(self, code_kind, bytecode, prelude_offset, qstrs, objs, raw_codes):
        # set core variables
        self.code_kind = code_kind
        self.bytecode = bytecode
        self.prelude_offset = prelude_offset
        self.qstrs = qstrs
        self.objs = objs
        self.raw_codes = raw_codes

        if self.prelude_offset is None:
            # no prelude, assign a dummy simple_name
            self.prelude_offset = 0
            self.simple_name = global_qstrs[1]
        else:
            # extract prelude
            self.ip, self.ip2, self.prelude = extract_prelude(self.bytecode, self.prelude_offset)
            self.simple_name = self._unpack_qstr(self.ip2)
            self.source_file = self._unpack_qstr(self.ip2 + 2)

    def _unpack_qstr(self, ip):
        qst = self.bytecode[ip] | self.bytecode[ip + 1] << 8
        return global_qstrs[qst]

    def dump(self):
        # dump children first
        for rc in self.raw_codes:
            rc.freeze('')
        # TODO

    def freeze_children(self, parent_name):
        self.escaped_name = parent_name + self.simple_name.qstr_esc

        # make sure the escaped name is unique
        i = 2
        while self.escaped_name in RawCode.escaped_names:
            self.escaped_name = parent_name + self.simple_name.qstr_esc + str(i)
            i += 1
        RawCode.escaped_names.add(self.escaped_name)

        # emit children first
        for rc in self.raw_codes:
            rc.freeze(self.escaped_name + '_')

    def freeze_constants(self):
        # generate constant objects
        for i, obj in enumerate(self.objs):
            obj_name = 'const_obj_%s_%u' % (self.escaped_name, i)
            if obj is MPFunTable:
                pass
            elif obj is Ellipsis:
                print('#define %s mp_const_ellipsis_obj' % obj_name)
            elif is_str_type(obj) or is_bytes_type(obj):
                if is_str_type(obj):
                    obj = bytes_cons(obj, 'utf8')
                    obj_type = 'mp_type_str'
                else:
                    obj_type = 'mp_type_bytes'
                print('STATIC const mp_obj_str_t %s = {{&%s}, %u, %u, (const byte*)"%s"};'
                    % (obj_name, obj_type, qstrutil.compute_hash(obj, config.MICROPY_QSTR_BYTES_IN_HASH),
                        len(obj), ''.join(('\\x%02x' % b) for b in obj)))
            elif is_int_type(obj):
                if config.MICROPY_LONGINT_IMPL == config.MICROPY_LONGINT_IMPL_NONE:
                    # TODO check if we can actually fit this long-int into a small-int
                    raise FreezeError(self, 'target does not support long int')
                elif config.MICROPY_LONGINT_IMPL == config.MICROPY_LONGINT_IMPL_LONGLONG:
                    # TODO
                    raise FreezeError(self, 'freezing int to long-long is not implemented')
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
                    digs = ','.join(('%#x' % d) for d in digs)
                    print('STATIC const mp_obj_int_t %s = {{&mp_type_int}, '
                        '{.neg=%u, .fixed_dig=1, .alloc=%u, .len=%u, .dig=(uint%u_t*)(const uint%u_t[]){%s}}};'
                        % (obj_name, neg, ndigs, ndigs, bits_per_dig, bits_per_dig, digs))
            elif type(obj) is float:
                print('#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B')
                print('STATIC const mp_obj_float_t %s = {{&mp_type_float}, %.16g};'
                    % (obj_name, obj))
                print('#endif')
            elif type(obj) is complex:
                print('STATIC const mp_obj_complex_t %s = {{&mp_type_complex}, %.16g, %.16g};'
                    % (obj_name, obj.real, obj.imag))
            else:
                raise FreezeError(self, 'freezing of object %r is not implemented' % (obj,))

        # generate constant table, if it has any entries
        const_table_len = len(self.qstrs) + len(self.objs) + len(self.raw_codes)
        if const_table_len:
            print('STATIC const mp_rom_obj_t const_table_data_%s[%u] = {'
                % (self.escaped_name, const_table_len))
            for qst in self.qstrs:
                print('    MP_ROM_QSTR(%s),' % global_qstrs[qst].qstr_id)
            for i in range(len(self.objs)):
                if self.objs[i] is MPFunTable:
                    print('    mp_fun_table,')
                elif type(self.objs[i]) is float:
                    print('#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B')
                    print('    MP_ROM_PTR(&const_obj_%s_%u),' % (self.escaped_name, i))
                    print('#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C')
                    n = struct.unpack('<I', struct.pack('<f', self.objs[i]))[0]
                    n = ((n & ~0x3) | 2) + 0x80800000
                    print('    (mp_rom_obj_t)(0x%08x),' % (n,))
                    print('#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D')
                    n = struct.unpack('<Q', struct.pack('<d', self.objs[i]))[0]
                    n += 0x8004000000000000
                    print('    (mp_rom_obj_t)(0x%016x),' % (n,))
                    print('#endif')
                else:
                    print('    MP_ROM_PTR(&const_obj_%s_%u),' % (self.escaped_name, i))
            for rc in self.raw_codes:
                print('    MP_ROM_PTR(&raw_code_%s),' % rc.escaped_name)
            print('};')

    def freeze_module(self, qstr_links=(), type_sig=0):
        # generate module
        if self.simple_name.str != '<module>':
            print('STATIC ', end='')
        print('const mp_raw_code_t raw_code_%s = {' % self.escaped_name)
        print('    .kind = %s,' % RawCode.code_kind_str[self.code_kind])
        print('    .scope_flags = 0x%02x,' % self.prelude[2])
        print('    .n_pos_args = %u,' % self.prelude[3])
        print('    .fun_data = fun_data_%s,' % self.escaped_name)
        if len(self.qstrs) + len(self.objs) + len(self.raw_codes):
            print('    .const_table = (mp_uint_t*)const_table_data_%s,' % self.escaped_name)
        else:
            print('    .const_table = NULL,')
        print('    #if MICROPY_PERSISTENT_CODE_SAVE')
        print('    .fun_data_len = %u,' % len(self.bytecode))
        print('    .n_obj = %u,' % len(self.objs))
        print('    .n_raw_code = %u,' % len(self.raw_codes))
        if self.code_kind == MP_CODE_BYTECODE:
            print('    #if MICROPY_PY_SYS_SETTRACE')
            print('    .prelude = {')
            print('        .n_state = %u,' % self.prelude[0])
            print('        .n_exc_stack = %u,' % self.prelude[1])
            print('        .scope_flags = %u,' % self.prelude[2])
            print('        .n_pos_args = %u,' % self.prelude[3])
            print('        .n_kwonly_args = %u,' % self.prelude[4])
            print('        .n_def_pos_args = %u,' % self.prelude[5])
            print('        .qstr_block_name = %s,' % self.simple_name.qstr_id)
            print('        .qstr_source_file = %s,' % self.source_file.qstr_id)
            print('        .line_info = fun_data_%s + %u,' % (self.escaped_name, 0)) # TODO
            print('        .locals = fun_data_%s + %u,' % (self.escaped_name, 0)) # TODO
            print('        .opcodes = fun_data_%s + %u,' % (self.escaped_name, self.ip))
            print('    },')
            print('    .line_of_definition = %u,' % 0) # TODO
            print('    #endif')
        print('    #if MICROPY_EMIT_MACHINE_CODE')
        print('    .prelude_offset = %u,' % self.prelude_offset)
        print('    .n_qstr = %u,' % len(qstr_links))
        print('    .qstr_link = NULL,') # TODO
        print('    #endif')
        print('    #endif')
        print('    #if MICROPY_EMIT_MACHINE_CODE')
        print('    .type_sig = %u,' % type_sig)
        print('    #endif')
        print('};')

class RawCodeBytecode(RawCode):
    def __init__(self, bytecode, qstrs, objs, raw_codes):
        super(RawCodeBytecode, self).__init__(MP_CODE_BYTECODE, bytecode, 0, qstrs, objs, raw_codes)

    def freeze(self, parent_name):
        self.freeze_children(parent_name)

        # generate bytecode data
        print()
        print('// frozen bytecode for file %s, scope %s%s' % (self.source_file.str, parent_name, self.simple_name.str))
        print('STATIC ', end='')
        if not config.MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE:
            print('const ', end='')
        print('byte fun_data_%s[%u] = {' % (self.escaped_name, len(self.bytecode)))
        print('   ', end='')
        for i in range(self.ip2):
            print(' 0x%02x,' % self.bytecode[i], end='')
        print()
        print('   ', self.simple_name.qstr_id, '& 0xff,', self.simple_name.qstr_id, '>> 8,')
        print('   ', self.source_file.qstr_id, '& 0xff,', self.source_file.qstr_id, '>> 8,')
        print('   ', end='')
        for i in range(self.ip2 + 4, self.ip):
            print(' 0x%02x,' % self.bytecode[i], end='')
        print()
        ip = self.ip
        while ip < len(self.bytecode):
            f, sz = mp_opcode_format(self.bytecode, ip, True)
            if f == 1:
                qst = self._unpack_qstr(ip + 1).qstr_id
                extra = '' if sz == 3 else ' 0x%02x,' % self.bytecode[ip + 3]
                print('   ', '0x%02x,' % self.bytecode[ip], qst, '& 0xff,', qst, '>> 8,', extra)
            else:
                print('   ', ''.join('0x%02x, ' % self.bytecode[ip + i] for i in range(sz)))
            ip += sz
        print('};')

        self.freeze_constants()
        self.freeze_module()

class RawCodeNative(RawCode):
    def __init__(self, code_kind, fun_data, prelude_offset, prelude, qstr_links, qstrs, objs, raw_codes, type_sig):
        super(RawCodeNative, self).__init__(code_kind, fun_data, prelude_offset, qstrs, objs, raw_codes)
        self.prelude = prelude
        self.qstr_links = qstr_links
        self.type_sig = type_sig
        if config.native_arch in (MP_NATIVE_ARCH_X86, MP_NATIVE_ARCH_X64):
            self.fun_data_attributes = '__attribute__((section(".text,\\"ax\\",@progbits # ")))'
        else:
            self.fun_data_attributes = '__attribute__((section(".text,\\"ax\\",%progbits @ ")))'

        # Allow single-byte alignment by default for x86/x64/xtensa, but on ARM we need halfword- or word- alignment.
        if config.native_arch == MP_NATIVE_ARCH_ARMV6:
            # ARMV6 -- four byte align.
            self.fun_data_attributes += ' __attribute__ ((aligned (4)))'
        elif MP_NATIVE_ARCH_ARMV6M <= config.native_arch <= MP_NATIVE_ARCH_ARMV7EMDP:
            # ARMVxxM -- two byte align.
            self.fun_data_attributes += ' __attribute__ ((aligned (2)))'

    def _asm_thumb_rewrite_mov(self, pc, val):
        print('    (%u & 0xf0) | (%s >> 12),' % (self.bytecode[pc], val), end='')
        print(' (%u & 0xfb) | (%s >> 9 & 0x04),' % (self.bytecode[pc + 1], val), end='')
        print(' (%s & 0xff),' % (val,), end='')
        print(' (%u & 0x07) | (%s >> 4 & 0x70),' % (self.bytecode[pc + 3], val))

    def _link_qstr(self, pc, kind, qst):
        if kind == 0:
            # Generic 16-bit link
            print('    %s & 0xff, %s >> 8,' % (qst, qst))
            return 2
        else:
            # Architecture-specific link
            is_obj = kind == 2
            if is_obj:
                qst = '((uintptr_t)MP_OBJ_NEW_QSTR(%s))' % qst
            if config.native_arch in (MP_NATIVE_ARCH_X86, MP_NATIVE_ARCH_X64):
                print('    %s & 0xff, %s >> 8, 0, 0,' % (qst, qst))
                return 4
            elif MP_NATIVE_ARCH_ARMV6M <= config.native_arch <= MP_NATIVE_ARCH_ARMV7EMDP:
                if is_obj:
                    # qstr object, movw and movt
                    self._asm_thumb_rewrite_mov(pc, qst)
                    self._asm_thumb_rewrite_mov(pc + 4, '(%s >> 16)' % qst)
                    return 8
                else:
                    # qstr number, movw instruction
                    self._asm_thumb_rewrite_mov(pc, qst)
                    return 4
            else:
                assert 0

    def freeze(self, parent_name):
        self.freeze_children(parent_name)

        # generate native code data
        print()
        if self.code_kind == MP_CODE_NATIVE_PY:
            print('// frozen native code for file %s, scope %s%s' % (self.source_file.str, parent_name, self.simple_name.str))
        elif self.code_kind == MP_CODE_NATIVE_VIPER:
            print('// frozen viper code for scope %s' % (parent_name,))
        else:
            print('// frozen assembler code for scope %s' % (parent_name,))
        print('STATIC const byte fun_data_%s[%u] %s = {' % (self.escaped_name, len(self.bytecode), self.fun_data_attributes))

        if self.code_kind == MP_CODE_NATIVE_PY:
            i_top = self.prelude_offset
        else:
            i_top = len(self.bytecode)
        i = 0
        qi = 0
        while i < i_top:
            if qi < len(self.qstr_links) and i == self.qstr_links[qi][0]:
                # link qstr
                qi_off, qi_kind, qi_val = self.qstr_links[qi]
                qst = global_qstrs[qi_val].qstr_id
                i += self._link_qstr(i, qi_kind, qst)
                qi += 1
            else:
                # copy machine code (max 16 bytes)
                i16 = min(i + 16, i_top)
                if qi < len(self.qstr_links):
                    i16 = min(i16, self.qstr_links[qi][0])
                print('   ', end='')
                for ii in range(i, i16):
                    print(' 0x%02x,' % self.bytecode[ii], end='')
                print()
                i = i16

        if self.code_kind == MP_CODE_NATIVE_PY:
            print('   ', end='')
            for i in range(self.prelude_offset, self.ip2):
                print(' 0x%02x,' % self.bytecode[i], end='')
            print()

            print('   ', self.simple_name.qstr_id, '& 0xff,', self.simple_name.qstr_id, '>> 8,')
            print('   ', self.source_file.qstr_id, '& 0xff,', self.source_file.qstr_id, '>> 8,')

            print('   ', end='')
            for i in range(self.ip2 + 4, self.ip):
                print(' 0x%02x,' % self.bytecode[i], end='')
            print()

        print('};')

        self.freeze_constants()
        self.freeze_module(self.qstr_links, self.type_sig)

class BytecodeBuffer:
    def __init__(self, size):
        self.buf = bytearray(size)
        self.idx = 0

    def is_full(self):
        return self.idx == len(self.buf)

    def append(self, b):
        self.buf[self.idx] = b
        self.idx += 1

def read_byte(f, out=None):
    b = bytes_cons(f.read(1))[0]
    if out is not None:
        out.append(b)
    return b

def read_uint(f, out=None):
    i = 0
    while True:
        b = read_byte(f, out)
        i = (i << 7) | (b & 0x7f)
        if b & 0x80 == 0:
            break
    return i

def read_qstr(f, qstr_win):
    ln = read_uint(f)
    if ln == 0:
        # static qstr
        return bytes_cons(f.read(1))[0]
    if ln & 1:
        # qstr in table
        return qstr_win.access(ln >> 1)
    ln >>= 1
    data = str_cons(f.read(ln), 'utf8')
    global_qstrs.append(QStrType(data))
    qstr_win.push(len(global_qstrs) - 1)
    return len(global_qstrs) - 1

def read_obj(f):
    obj_type = f.read(1)
    if obj_type == b'e':
        return Ellipsis
    else:
        buf = f.read(read_uint(f))
        if obj_type == b's':
            return str_cons(buf, 'utf8')
        elif obj_type == b'b':
            return bytes_cons(buf)
        elif obj_type == b'i':
            return int(str_cons(buf, 'ascii'), 10)
        elif obj_type == b'f':
            return float(str_cons(buf, 'ascii'))
        elif obj_type == b'c':
            return complex(str_cons(buf, 'ascii'))
        else:
            assert 0

def read_prelude(f, bytecode):
    n_state = read_uint(f, bytecode)
    n_exc_stack = read_uint(f, bytecode)
    scope_flags = read_byte(f, bytecode)
    n_pos_args = read_byte(f, bytecode)
    n_kwonly_args = read_byte(f, bytecode)
    n_def_pos_args = read_byte(f, bytecode)
    l1 = bytecode.idx
    code_info_size = read_uint(f, bytecode)
    l2 = bytecode.idx
    for _ in range(code_info_size - (l2 - l1)):
        read_byte(f, bytecode)
    while read_byte(f, bytecode) != 255:
        pass
    return l2, (n_state, n_exc_stack, scope_flags, n_pos_args, n_kwonly_args, n_def_pos_args, code_info_size)

def read_qstr_and_pack(f, bytecode, qstr_win):
    qst = read_qstr(f, qstr_win)
    bytecode.append(qst & 0xff)
    bytecode.append(qst >> 8)

def read_bytecode(file, bytecode, qstr_win):
    while not bytecode.is_full():
        op = read_byte(file, bytecode)
        f, sz = mp_opcode_format(bytecode.buf, bytecode.idx - 1, False)
        sz -= 1
        if f == MP_OPCODE_QSTR:
            read_qstr_and_pack(file, bytecode, qstr_win)
            sz -= 2
        elif f == MP_OPCODE_VAR_UINT:
            while read_byte(file, bytecode) & 0x80:
                pass
        for _ in range(sz):
            read_byte(file, bytecode)

def read_raw_code(f, qstr_win):
    kind_len = read_uint(f)
    kind = (kind_len & 3) + MP_CODE_BYTECODE
    fun_data_len = kind_len >> 2
    fun_data = BytecodeBuffer(fun_data_len)

    if kind == MP_CODE_BYTECODE:
        name_idx, prelude = read_prelude(f, fun_data)
        read_bytecode(f, fun_data, qstr_win)
    else:
        fun_data.buf[:] = f.read(fun_data_len)

        qstr_links = []
        if kind in (MP_CODE_NATIVE_PY, MP_CODE_NATIVE_VIPER):
            # load qstr link table
            n_qstr_link = read_uint(f)
            for _ in range(n_qstr_link):
                off = read_uint(f)
                qst = read_qstr(f, qstr_win)
                qstr_links.append((off >> 2, off & 3, qst))

        type_sig = 0
        if kind == MP_CODE_NATIVE_PY:
            prelude_offset = read_uint(f)
            _, name_idx, prelude = extract_prelude(fun_data.buf, prelude_offset)
        else:
            prelude_offset = None
            scope_flags = read_uint(f)
            n_pos_args = 0
            if kind == MP_CODE_NATIVE_ASM:
                n_pos_args = read_uint(f)
                type_sig = read_uint(f)
            prelude = (None, None, scope_flags, n_pos_args, 0)

    if kind in (MP_CODE_BYTECODE, MP_CODE_NATIVE_PY):
        fun_data.idx = name_idx # rewind to where qstrs are in prelude
        read_qstr_and_pack(f, fun_data, qstr_win) # simple_name
        read_qstr_and_pack(f, fun_data, qstr_win) # source_file

    qstrs = []
    objs = []
    raw_codes = []
    if kind != MP_CODE_NATIVE_ASM:
        # load constant table
        n_obj = read_uint(f)
        n_raw_code = read_uint(f)
        qstrs = [read_qstr(f, qstr_win) for _ in range(prelude[3] + prelude[4])]
        if kind != MP_CODE_BYTECODE:
            objs.append(MPFunTable)
        objs.extend([read_obj(f) for _ in range(n_obj)])
        raw_codes = [read_raw_code(f, qstr_win) for _ in range(n_raw_code)]

    if kind == MP_CODE_BYTECODE:
        return RawCodeBytecode(fun_data.buf, qstrs, objs, raw_codes)
    else:
        return RawCodeNative(kind, fun_data.buf, prelude_offset, prelude, qstr_links, qstrs, objs, raw_codes, type_sig)

def read_mpy(filename):
    with open(filename, 'rb') as f:
        header = bytes_cons(f.read(4))
        if header[0] != ord('M'):
            raise Exception('not a valid .mpy file')
        if header[1] != config.MPY_VERSION:
            raise Exception('incompatible .mpy version')
        feature_byte = header[2]
        qw_size = read_uint(f)
        config.MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE = (feature_byte & 1) != 0
        config.MICROPY_PY_BUILTINS_STR_UNICODE = (feature_byte & 2) != 0
        mpy_native_arch = feature_byte >> 2
        if mpy_native_arch != MP_NATIVE_ARCH_NONE:
            if config.native_arch == MP_NATIVE_ARCH_NONE:
                config.native_arch = mpy_native_arch
            elif config.native_arch != mpy_native_arch:
                raise Exception('native architecture mismatch')
        config.mp_small_int_bits = header[3]
        qstr_win = QStrWindow(qw_size)
        return read_raw_code(f, qstr_win)

def dump_mpy(raw_codes):
    for rc in raw_codes:
        rc.dump()

def freeze_mpy(base_qstrs, raw_codes):
    # add to qstrs
    new = {}
    for q in global_qstrs:
        # don't add duplicates
        if q is None or q.qstr_esc in base_qstrs or q.qstr_esc in new:
            continue
        new[q.qstr_esc] = (len(new), q.qstr_esc, q.str)
    new = sorted(new.values(), key=lambda x: x[0])

    print('#include "py/mpconfig.h"')
    print('#include "py/objint.h"')
    print('#include "py/objstr.h"')
    print('#include "py/emitglue.h"')
    print()

    print('#if MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE != %u' % config.MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE)
    print('#error "incompatible MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE"')
    print('#endif')
    print()

    print('#if MICROPY_LONGINT_IMPL != %u' % config.MICROPY_LONGINT_IMPL)
    print('#error "incompatible MICROPY_LONGINT_IMPL"')
    print('#endif')
    print()

    if config.MICROPY_LONGINT_IMPL == config.MICROPY_LONGINT_IMPL_MPZ:
        print('#if MPZ_DIG_SIZE != %u' % config.MPZ_DIG_SIZE)
        print('#error "incompatible MPZ_DIG_SIZE"')
        print('#endif')
        print()


    print('#if MICROPY_PY_BUILTINS_FLOAT')
    print('typedef struct _mp_obj_float_t {')
    print('    mp_obj_base_t base;')
    print('    mp_float_t value;')
    print('} mp_obj_float_t;')
    print('#endif')
    print()

    print('#if MICROPY_PY_BUILTINS_COMPLEX')
    print('typedef struct _mp_obj_complex_t {')
    print('    mp_obj_base_t base;')
    print('    mp_float_t real;')
    print('    mp_float_t imag;')
    print('} mp_obj_complex_t;')
    print('#endif')
    print()

    if len(new) > 0:
        print('enum {')
        for i in range(len(new)):
            if i == 0:
                print('    MP_QSTR_%s = MP_QSTRnumber_of,' % new[i][1])
            else:
                print('    MP_QSTR_%s,' % new[i][1])
        print('};')

    # As in qstr.c, set so that the first dynamically allocated pool is twice this size; must be <= the len
    qstr_pool_alloc = min(len(new), 10)

    print()
    print('extern const qstr_pool_t mp_qstr_const_pool;');
    print('const qstr_pool_t mp_qstr_frozen_const_pool = {')
    print('    (qstr_pool_t*)&mp_qstr_const_pool, // previous pool')
    print('    MP_QSTRnumber_of, // previous pool size')
    print('    %u, // allocated entries' % qstr_pool_alloc)
    print('    %u, // used entries' % len(new))
    print('    {')
    for _, _, qstr in new:
        print('        %s,'
            % qstrutil.make_bytes(config.MICROPY_QSTR_BYTES_IN_LEN, config.MICROPY_QSTR_BYTES_IN_HASH, qstr))
    print('    },')
    print('};')

    for rc in raw_codes:
        rc.freeze(rc.source_file.str.replace('/', '_')[:-3] + '_')

    print()
    print('const char mp_frozen_mpy_names[] = {')
    for rc in raw_codes:
        module_name = rc.source_file.str
        print('"%s\\0"' % module_name)
    print('"\\0"};')

    print('const mp_raw_code_t *const mp_frozen_mpy_content[] = {')
    for rc in raw_codes:
        print('    &raw_code_%s,' % rc.escaped_name)
    print('};')

def main():
    import argparse
    cmd_parser = argparse.ArgumentParser(description='A tool to work with MicroPython .mpy files.')
    cmd_parser.add_argument('-d', '--dump', action='store_true',
        help='dump contents of files')
    cmd_parser.add_argument('-f', '--freeze', action='store_true',
        help='freeze files')
    cmd_parser.add_argument('-q', '--qstr-header',
        help='qstr header file to freeze against')
    cmd_parser.add_argument('-mlongint-impl', choices=['none', 'longlong', 'mpz'], default='mpz',
        help='long-int implementation used by target (default mpz)')
    cmd_parser.add_argument('-mmpz-dig-size', metavar='N', type=int, default=16,
        help='mpz digit size used by target (default 16)')
    cmd_parser.add_argument('files', nargs='+',
        help='input .mpy files')
    args = cmd_parser.parse_args()

    # set config values relevant to target machine
    config.MICROPY_LONGINT_IMPL = {
        'none':config.MICROPY_LONGINT_IMPL_NONE,
        'longlong':config.MICROPY_LONGINT_IMPL_LONGLONG,
        'mpz':config.MICROPY_LONGINT_IMPL_MPZ,
    }[args.mlongint_impl]
    config.MPZ_DIG_SIZE = args.mmpz_dig_size
    config.native_arch = MP_NATIVE_ARCH_NONE

    # set config values for qstrs, and get the existing base set of qstrs
    if args.qstr_header:
        qcfgs, base_qstrs = qstrutil.parse_input_headers([args.qstr_header])
        config.MICROPY_QSTR_BYTES_IN_LEN = int(qcfgs['BYTES_IN_LEN'])
        config.MICROPY_QSTR_BYTES_IN_HASH = int(qcfgs['BYTES_IN_HASH'])
    else:
        config.MICROPY_QSTR_BYTES_IN_LEN = 1
        config.MICROPY_QSTR_BYTES_IN_HASH = 1
        base_qstrs = {}

    raw_codes = [read_mpy(file) for file in args.files]

    if args.dump:
        dump_mpy(raw_codes)
    elif args.freeze:
        try:
            freeze_mpy(base_qstrs, raw_codes)
        except FreezeError as er:
            print(er, file=sys.stderr)
            sys.exit(1)

if __name__ == '__main__':
    main()
