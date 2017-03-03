#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2016 Damien P. George
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

sys.path.append('../py')
import makeqstrdata as qstrutil

class FreezeError(Exception):
    def __init__(self, rawcode, msg):
        self.rawcode = rawcode
        self.msg = msg

    def __str__(self):
        return 'error while freezing %s: %s' % (self.rawcode.source_file, self.msg)

class Config:
    MPY_VERSION = 1
    MICROPY_LONGINT_IMPL_NONE = 0
    MICROPY_LONGINT_IMPL_LONGLONG = 1
    MICROPY_LONGINT_IMPL_MPZ = 2
config = Config()

MP_OPCODE_BYTE = 0
MP_OPCODE_QSTR = 1
MP_OPCODE_VAR_UINT = 2
MP_OPCODE_OFFSET = 3

# extra bytes:
MP_BC_MAKE_CLOSURE = 0x62
MP_BC_MAKE_CLOSURE_DEFARGS = 0x63
MP_BC_RAISE_VARARGS = 0x5c
# extra byte if caching enabled:
MP_BC_LOAD_NAME = 0x1c
MP_BC_LOAD_GLOBAL = 0x1d
MP_BC_LOAD_ATTR = 0x1e
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
    OC4(B, U, V, V), # 0x18-0x1b
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
    OC4(B, B, O, U), # 0x44-0x47
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
    OC4(B, B, B, B), # 0xd4-0xd7
    OC4(B, B, B, B), # 0xd8-0xdb
    OC4(B, B, B, B), # 0xdc-0xdf

    OC4(B, B, B, B), # 0xe0-0xe3
    OC4(B, B, B, B), # 0xe4-0xe7
    OC4(B, B, B, B), # 0xe8-0xeb
    OC4(B, B, B, B), # 0xec-0xef

    OC4(B, B, B, B), # 0xf0-0xf3
    OC4(B, B, B, B), # 0xf4-0xf7
    OC4(B, B, B, U), # 0xf8-0xfb
    OC4(U, U, U, U), # 0xfc-0xff
    ))

# this function mirrors that in py/bc.c
def mp_opcode_format(bytecode, ip, opcode_format=make_opcode_format()):
    opcode = bytecode[ip]
    ip_start = ip
    f = (opcode_format[opcode >> 2] >> (2 * (opcode & 3))) & 3
    if f == MP_OPCODE_QSTR:
        ip += 3
    else:
        extra_byte = (
            opcode == MP_BC_RAISE_VARARGS
            or opcode == MP_BC_MAKE_CLOSURE
            or opcode == MP_BC_MAKE_CLOSURE_DEFARGS
            or config.MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE and (
                opcode == MP_BC_LOAD_NAME
                or opcode == MP_BC_LOAD_GLOBAL
                or opcode == MP_BC_LOAD_ATTR
                or opcode == MP_BC_STORE_ATTR
            )
        )
        ip += 1
        if f == MP_OPCODE_VAR_UINT:
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

def extract_prelude(bytecode):
    ip = 0
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

class RawCode:
    # a set of all escaped names, to make sure they are unique
    escaped_names = set()

    def __init__(self, bytecode, qstrs, objs, raw_codes):
        # set core variables
        self.bytecode = bytecode
        self.qstrs = qstrs
        self.objs = objs
        self.raw_codes = raw_codes

        # extract prelude
        self.ip, self.ip2, self.prelude = extract_prelude(self.bytecode)
        self.simple_name = self._unpack_qstr(self.ip2)
        self.source_file = self._unpack_qstr(self.ip2 + 2)

    def _unpack_qstr(self, ip):
        qst = self.bytecode[ip] | self.bytecode[ip + 1] << 8
        return global_qstrs[qst]

    def dump(self):
        # dump children first
        for rc in self.raw_codes:
            rc.freeze()
        # TODO

    def freeze(self, parent_name):
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

        # generate bytecode data
        print()
        print('// frozen bytecode for file %s, scope %s%s' % (self.source_file.str, parent_name, self.simple_name.str))
        print('STATIC ', end='')
        if not config.MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE:
            print('const ', end='')
        print('byte bytecode_data_%s[%u] = {' % (self.escaped_name, len(self.bytecode)))
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
            f, sz = mp_opcode_format(self.bytecode, ip)
            if f == 1:
                qst = self._unpack_qstr(ip + 1).qstr_id
                print('   ', '0x%02x,' % self.bytecode[ip], qst, '& 0xff,', qst, '>> 8,')
            else:
                print('   ', ''.join('0x%02x, ' % self.bytecode[ip + i] for i in range(sz)))
            ip += sz
        print('};')

        # generate constant objects
        for i, obj in enumerate(self.objs):
            obj_name = 'const_obj_%s_%u' % (self.escaped_name, i)
            if is_str_type(obj) or is_bytes_type(obj):
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
                        '{.neg=%u, .fixed_dig=1, .alloc=%u, .len=%u, .dig=(uint%u_t[]){%s}}};'
                        % (obj_name, neg, ndigs, ndigs, bits_per_dig, digs))
            elif type(obj) is float:
                print('#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B')
                print('STATIC const mp_obj_float_t %s = {{&mp_type_float}, %.16g};'
                    % (obj_name, obj))
                print('#endif')
            elif type(obj) is complex:
                print('STATIC const mp_obj_complex_t %s = {{&mp_type_complex}, %.16g, %.16g};'
                    % (obj_name, obj.real, obj.imag))
            else:
                # TODO
                raise FreezeError(self, 'freezing of object %r is not implemented' % (obj,))

        # generate constant table
        print('STATIC const mp_uint_t const_table_data_%s[%u] = {'
            % (self.escaped_name, len(self.qstrs) + len(self.objs) + len(self.raw_codes)))
        for qst in self.qstrs:
            print('    (mp_uint_t)MP_OBJ_NEW_QSTR(%s),' % global_qstrs[qst].qstr_id)
        for i in range(len(self.objs)):
            if type(self.objs[i]) is float:
                print('#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B')
                print('    (mp_uint_t)&const_obj_%s_%u,' % (self.escaped_name, i))
                print('#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C')
                n = struct.unpack('<I', struct.pack('<f', self.objs[i]))[0]
                n = ((n & ~0x3) | 2) + 0x80800000
                print('    (mp_uint_t)0x%08x,' % (n,))
                print('#else')
                print('#error "MICROPY_OBJ_REPR_D not supported with floats in frozen mpy files"')
                print('#endif')
            else:
                print('    (mp_uint_t)&const_obj_%s_%u,' % (self.escaped_name, i))
        for rc in self.raw_codes:
            print('    (mp_uint_t)&raw_code_%s,' % rc.escaped_name)
        print('};')

        # generate module
        if self.simple_name.str != '<module>':
            print('STATIC ', end='')
        print('const mp_raw_code_t raw_code_%s = {' % self.escaped_name)
        print('    .kind = MP_CODE_BYTECODE,')
        print('    .scope_flags = 0x%02x,' % self.prelude[2])
        print('    .n_pos_args = %u,' % self.prelude[3])
        print('    .data.u_byte = {')
        print('        .bytecode = bytecode_data_%s,' % self.escaped_name)
        print('        .const_table = const_table_data_%s,' % self.escaped_name)
        print('        #if MICROPY_PERSISTENT_CODE_SAVE')
        print('        .bc_len = %u,' % len(self.bytecode))
        print('        .n_obj = %u,' % len(self.objs))
        print('        .n_raw_code = %u,' % len(self.raw_codes))
        print('        #endif')
        print('    },')
        print('};')

def read_uint(f):
    i = 0
    while True:
        b = bytes_cons(f.read(1))[0]
        i = (i << 7) | (b & 0x7f)
        if b & 0x80 == 0:
            break
    return i

global_qstrs = []
qstr_type = namedtuple('qstr', ('str', 'qstr_esc', 'qstr_id'))
def read_qstr(f):
    ln = read_uint(f)
    data = str_cons(f.read(ln), 'utf8')
    qstr_esc = qstrutil.qstr_escape(data)
    global_qstrs.append(qstr_type(data, qstr_esc, 'MP_QSTR_' + qstr_esc))
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

def read_qstr_and_pack(f, bytecode, ip):
    qst = read_qstr(f)
    bytecode[ip] = qst & 0xff
    bytecode[ip + 1] = qst >> 8

def read_bytecode_qstrs(file, bytecode, ip):
    while ip < len(bytecode):
        f, sz = mp_opcode_format(bytecode, ip)
        if f == 1:
            read_qstr_and_pack(file, bytecode, ip + 1)
        ip += sz

def read_raw_code(f):
    bc_len = read_uint(f)
    bytecode = bytearray(f.read(bc_len))
    ip, ip2, prelude = extract_prelude(bytecode)
    read_qstr_and_pack(f, bytecode, ip2) # simple_name
    read_qstr_and_pack(f, bytecode, ip2 + 2) # source_file
    read_bytecode_qstrs(f, bytecode, ip)
    n_obj = read_uint(f)
    n_raw_code = read_uint(f)
    qstrs = [read_qstr(f) for _ in range(prelude[3] + prelude[4])]
    objs = [read_obj(f) for _ in range(n_obj)]
    raw_codes = [read_raw_code(f) for _ in range(n_raw_code)]
    return RawCode(bytecode, qstrs, objs, raw_codes)

def read_mpy(filename):
    with open(filename, 'rb') as f:
        header = bytes_cons(f.read(4))
        if header[0] != ord('M'):
            raise Exception('not a valid .mpy file')
        if header[1] != config.MPY_VERSION:
            raise Exception('incompatible .mpy version')
        feature_flags = header[2]
        config.MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE = (feature_flags & 1) != 0
        config.MICROPY_PY_BUILTINS_STR_UNICODE = (feature_flags & 2) != 0
        config.mp_small_int_bits = header[3]
        return read_raw_code(f)

def dump_mpy(raw_codes):
    for rc in raw_codes:
        rc.dump()

def freeze_mpy(base_qstrs, raw_codes):
    # add to qstrs
    new = {}
    for q in global_qstrs:
        # don't add duplicates
        if q.qstr_esc in base_qstrs or q.qstr_esc in new:
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

    print('enum {')
    for i in range(len(new)):
        if i == 0:
            print('    MP_QSTR_%s = MP_QSTRnumber_of,' % new[i][1])
        else:
            print('    MP_QSTR_%s,' % new[i][1])
    print('};')

    print()
    print('extern const qstr_pool_t mp_qstr_const_pool;');
    print('const qstr_pool_t mp_qstr_frozen_const_pool = {')
    print('    (qstr_pool_t*)&mp_qstr_const_pool, // previous pool')
    print('    MP_QSTRnumber_of, // previous pool size')
    print('    %u, // allocated entries' % len(new))
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
