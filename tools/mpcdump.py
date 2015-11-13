import sys

################################################################
# this stuff is copied from makeqstrdata.py

import re

# codepoint2name is different in Python 2 to Python 3
import platform
if platform.python_version_tuple()[0] == '2':
    from htmlentitydefs import codepoint2name
elif platform.python_version_tuple()[0] == '3':
    from html.entities import codepoint2name
codepoint2name[ord('-')] = 'hyphen';

# add some custom names to map characters that aren't in HTML
codepoint2name[ord(' ')] = 'space'
codepoint2name[ord('\'')] = 'squot'
codepoint2name[ord(',')] = 'comma'
codepoint2name[ord('.')] = 'dot'
codepoint2name[ord(':')] = 'colon'
codepoint2name[ord('/')] = 'slash'
codepoint2name[ord('%')] = 'percent'
codepoint2name[ord('#')] = 'hash'
codepoint2name[ord('(')] = 'paren_open'
codepoint2name[ord(')')] = 'paren_close'
codepoint2name[ord('[')] = 'bracket_open'
codepoint2name[ord(']')] = 'bracket_close'
codepoint2name[ord('{')] = 'brace_open'
codepoint2name[ord('}')] = 'brace_close'
codepoint2name[ord('*')] = 'star'
codepoint2name[ord('!')] = 'bang'
codepoint2name[ord('\\')] = 'backslash'
codepoint2name[ord('+')] = 'plus'

def qstr_escape(qst):
    return re.sub(r'[^A-Za-z0-9_]', lambda s: "_" + codepoint2name[ord(s.group(0))] + '_', qst)

def qstr_id(qst):
    return 'MP_QSTR_' + qstr_escape(qst)

################################################################

# extra bytes:
MP_BC_MAKE_CLOSURE = 0x62
MP_BC_MAKE_CLOSURE_DEFARGS = 0x63
MP_BC_RAISE_VARARGS = 0x5c

def make_opcode_format():
    def OC4(a, b, c, d):
        return a | (b << 2) | (c << 4) | (d << 6)
    U = 0
    B = 0
    Q = 1
    V = 2
    O = 3
    return bytes((
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
        OC4(B, B, O, B), # 0x44-0x47
        OC4(U, U, U, U), # 0x48-0x4b
        OC4(U, U, U, U), # 0x4c-0x4f
        OC4(V, V, V, V), # 0x50-0x53
        OC4(B, V, V, V), # 0x54-0x57
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
        OC4(B, B, U, U), # 0xf8-0xfb
        OC4(U, U, U, U), # 0xfc-0xff
    ))

def get_opcode_info(bytecode, ip, opcode_format=make_opcode_format()):
    opcode = bytecode[ip]
    ip_start = ip
    f = (opcode_format[opcode >> 2] >> (2 * (opcode & 3))) & 3
    if f == 1:
        # qstr
        ip += 3
    else:
        extra_byte = (opcode == MP_BC_RAISE_VARARGS
            or opcode == MP_BC_MAKE_CLOSURE
            or opcode == MP_BC_MAKE_CLOSURE_DEFARGS)
        ip += 1
        if f == 2:
            # var-uint
            while bytecode[ip] & 0x80 != 0:
                ip += 1
            ip += 1
        elif f == 3:
            # bc offset
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
    def __init__(self, bytecode, qstrs, objs, raw_codes):
        self.bytecode = bytecode
        self.qstrs = qstrs
        self.objs = objs
        self.raw_codes = raw_codes

    def _unpack_qstr(self, ip):
        qst = self.bytecode[ip] | self.bytecode[ip + 1] << 8
        return global_qstrs[qst]

    def dump_c(self):
        # emit children first
        child_names = []
        for rc in self.raw_codes:
            child_names.append(rc.dump_c())

        ip, ip2, prelude = extract_prelude(self.bytecode)
        simple_name = self._unpack_qstr(ip2)
        source_file = self._unpack_qstr(ip2 + 2)
        esc_name = source_file[:-3] + '_' + qstr_escape(simple_name)

        print()
        print('// frozen bytecode for file %s, scope %s' % (source_file, simple_name))
        print('STATIC const byte bytecode_data_%s[%u] = {' % (esc_name, len(self.bytecode)))
        print('   ', end='')
        for i in range(ip2):
            print(' 0x%02x,' % self.bytecode[i], end='')
        print()
        print('   ', qstr_id(simple_name), '& 0xff,', qstr_id(simple_name), '>> 8,')
        print('   ', qstr_id(source_file), '& 0xff,', qstr_id(source_file), '>> 8,')
        print('   ', end='')
        for i in range(ip2 + 4, ip):
            print(' 0x%02x,' % self.bytecode[i], end='')
        print()
        while ip < len(self.bytecode):
            f, sz = get_opcode_info(self.bytecode, ip)
            if f == 1:
                qst = self._unpack_qstr(ip + 1)
                print('   ', '0x%02x,' % self.bytecode[ip], qstr_id(qst), '& 0xff,', qstr_id(qst), '>> 8,')
            else:
                print('   ', ''.join('0x%02x, ' % self.bytecode[ip + i] for i in range(sz)))
            ip += sz
        print('};')
        print('STATIC const mp_uint_t const_table_data_%s[%u] = {' % (esc_name, len(self.qstrs) + len(self.objs) + len(self.raw_codes)))
        for qst in self.qstrs:
            print('    (mp_uint_t)MP_OBJ_NEW_QSTR(%s),' % qstr_id(global_qstrs[qst]))
        for obj in self.objs:
            assert 0
        for n in child_names:
            print('    (mp_uint_t)&raw_code_%s,' % n)
        print('};')
        if simple_name != '<module>':
            print('STATIC ', end='')
        print('const mp_raw_code_t raw_code_%s = {' % esc_name)
        print('    .kind = MP_CODE_BYTECODE,')
        print('    .scope_flags = 0x%02x,' % prelude[2])
        print('    .n_pos_args = %u,' % prelude[3])
        print('    .data.u_byte = {')
        print('        .bytecode = bytecode_data_%s,' % esc_name)
        print('        .const_table = const_table_data_%s,' % esc_name)
        print('        #if MICROPY_PORTABLE_CODE_SAVE')
        print('        .bc_len = %u,' % len(self.bytecode))
        print('        .n_obj = %u,' % len(self.objs))
        print('        .n_raw_code = %u,' % len(self.raw_codes))
        print('        #endif')
        print('    },')
        print('};')

        return esc_name

def read_uint(f):
    i = 0
    while True:
        b = f.read(1)[0]
        i = (i << 7) | (b & 0x7f)
        if b & 0x80 == 0:
            break
    return i

global_qstrs = []
def read_qstr(f):
    ln = read_uint(f)
    data = str(f.read(ln), 'utf8')
    global_qstrs.append(data)
    return len(global_qstrs) - 1

def read_obj(f):
    assert 0

def read_qstr_and_pack(f, bytecode, ip):
    qst = read_qstr(f)
    bytecode[ip] = qst & 0xff
    bytecode[ip + 1] = qst >> 8

def read_bytecode_qstrs(file, bytecode, ip):
    while ip < len(bytecode):
        f, sz = get_opcode_info(bytecode, ip)
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

def read_mpc(filename):
    with open(filename, 'rb') as f:
        header = f.read(6)
        if header[:3] != b'MPC':
            raise Exception('not an MPC file')
        if header[3:6] != b'001':
            raise Exception('incompatible version')
        return read_raw_code(f)

if __name__ == '__main__':
    mpc = read_mpc(sys.argv[1])
    print('#include "py/emitglue.h"')
    print()
    for q in global_qstrs:
        print('// Q(%s)' % q)
    mpc.dump_c()
