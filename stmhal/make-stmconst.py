"""
Read in the cmsis/devinc/stm32f405xx.h header, extract relevant constants,
and create modstmconst.c.

This is not part of the automatic build process because stm32f405xx.h isn't
expected to change.  After generating the file, some manual intervention is
needed to copy the new qstr definitions to qstrdefsport.h.
"""

from __future__ import print_function

import argparse
import re

# given a list of (name,regex) pairs, find the first one that matches the given line
def re_match_first(regexs, line):
    for name, regex in regexs:
        match = re.match(regex, line)
        if match:
            return name, match
    return None, None

class LexerError(Exception):
    def __init__(self, line):
        self.line = line

class Lexer:
    re_io_reg = r'__IO uint(?P<bits>8|16|32)_t +(?P<reg>[A-Z0-9]+)'
    re_comment = r'(?P<comment>[A-Za-z0-9 \-/_()]+)'
    re_addr_offset = r'Address offset: (?P<offset>0x[0-9A-Z]{2,3})'
    regexs = (
        ('#define hex', re.compile(r'#define +(?P<id>[A-Z0-9_]+) +\(\(uint32_t\)(?P<hex>0x[0-9A-F]+)\)($| +/\*)')),
        ('#define X', re.compile(r'#define +(?P<id>[A-Z0-9_]+) +(?P<id2>[A-Z0-9_]+)($| +/\*)')),
        ('#define X+hex', re.compile(r'#define +(?P<id>[A-Za-z0-9_]+) +\((?P<id2>[A-Z0-9_]+) \+ (?P<hex>0x[0-9A-F]+)\)($| +/\*)')),
        ('#define typedef', re.compile(r'#define +(?P<id>[A-Z0-9_]+(ext)?) +\(\([A-Za-z0-9_]+_TypeDef \*\) (?P<id2>[A-Za-z0-9_]+)\)($| +/\*)')),
        ('typedef struct', re.compile(r'typedef struct$')),
        ('{', re.compile(r'{$')),
        ('}', re.compile(r'}$')),
        ('} TypeDef', re.compile(r'} *(?P<id>[A-Z][A-Za-z0-9_]+)_(?P<global>([A-Za-z0-9_]+)?)TypeDef;$')),
        ('IO reg', re.compile(re_io_reg + r'; +/\*!< ' + re_comment + r', +' + re_addr_offset + r' *\*/')),
        ('IO reg array', re.compile(re_io_reg + r'\[(?P<array>[2-8])\]; +/\*!< ' + re_comment + r', +' + re_addr_offset + r'-(0x[0-9A-Z]{2,3}) *\*/')),
    )

    def __init__(self, filename):
        self.file = open(filename, 'rt')
        self.line_number = 0

    def next_match(self, strictly_next=False):
        while True:
            line = self.file.readline()
            self.line_number += 1
            if len(line) == 0:
                return ('EOF', None)
            match = re_match_first(Lexer.regexs, line.strip())
            if strictly_next or match[0] is not None:
                return match

    def must_match(self, kind):
        match = self.next_match(strictly_next=True)
        if match[0] != kind:
            raise LexerError(self.line_number)
        return match

def parse_file(filename):
    lexer = Lexer(filename)

    reg_defs = {}
    consts = {}
    periphs = []
    while True:
        m = lexer.next_match()
        if m[0] == 'EOF':
            break
        elif m[0] == '#define hex':
            d = m[1].groupdict()
            consts[d['id']] = int(d['hex'], base=16)
        elif m[0] == '#define X':
            d = m[1].groupdict()
            if d['id2'] in consts:
                consts[d['id']] = consts[d['id2']]
        elif m[0] == '#define X+hex':
            d = m[1].groupdict()
            if d['id2'] in consts:
                consts[d['id']] = consts[d['id2']] + int(d['hex'], base=16)
        elif m[0] == '#define typedef':
            d = m[1].groupdict()
            if d['id2'] in consts:
                periphs.append((d['id'], consts[d['id2']]))
        elif m[0] == 'typedef struct':
            lexer.must_match('{')
            m = lexer.next_match()
            regs = []
            while m[0] in ('IO reg', 'IO reg array'):
                d = m[1].groupdict()
                reg = d['reg']
                offset = int(d['offset'], base=16)
                bits = int(d['bits'])
                comment = d['comment']
                if m[0] == 'IO reg':
                    regs.append((reg, offset, bits, comment))
                else:
                    for i in range(int(d['array'])):
                        regs.append((reg + str(i), offset + i * bits // 8, bits, comment))
                m = lexer.next_match()
            if m[0] == '}':
                pass
            elif m[0] == '} TypeDef':
                reg_defs[m[1].groupdict()['id']] = regs
            else:
                raise LexerError(lexer.line_number)

    return periphs, reg_defs

def print_periph(periph_name, periph_val, needed_qstrs):
    qstr = periph_name.upper()
    print('{ MP_OBJ_NEW_QSTR(MP_QSTR_%s), MP_OBJ_NEW_SMALL_INT(%#x) },' % (qstr, periph_val))
    needed_qstrs.add(qstr)

def print_regs(reg_name, reg_defs, needed_qstrs):
    reg_name = reg_name.upper()
    for r in reg_defs:
        qstr = reg_name + '_' + r[0]
        print('{ MP_OBJ_NEW_QSTR(MP_QSTR_%s), MP_OBJ_NEW_SMALL_INT(%#x) }, // %s-bits, %s' % (qstr, r[1], r[2], r[3]))
        needed_qstrs.add(qstr)

# This version of print regs groups registers together into submodules (eg GPIO submodule).
# This makes the qstrs shorter, and makes the list of constants more manageable (since
# they are not all in one big module) but it is then harder to compile the constants, and
# is more cumbersome to access.
# As such, we don't use this version.
# And for the number of constants we have, this function seems to use about the same amount
# of ROM as print_regs.
def print_regs_as_submodules(reg_name, reg_defs, modules, needed_qstrs):
    mod_name_lower = reg_name.lower() + '_'
    mod_name_upper = mod_name_lower.upper()
    modules.append((mod_name_lower, mod_name_upper))

    print("""
STATIC const mp_map_elem_t stm_%s_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_%s) },
""" % (mod_name_lower, mod_name_upper))
    needed_qstrs.add(mod_name_upper)

    for r in reg_defs:
        print('    { MP_OBJ_NEW_QSTR(MP_QSTR_%s), MP_OBJ_NEW_SMALL_INT(%#x) }, // %s-bits, %s' % (r[0], r[1], r[2], r[3]))
        needed_qstrs.add(r[0])

    print("""};

STATIC MP_DEFINE_CONST_DICT(stm_%s_globals, stm_%s_globals_table);

const mp_obj_module_t stm_%s_obj = {
    .base = { &mp_type_module },
    .name = MP_QSTR_%s,
    .globals = (mp_obj_dict_t*)&stm_%s_globals,
};
""" % (mod_name_lower, mod_name_lower, mod_name_lower, mod_name_upper, mod_name_lower))

def main():
    cmd_parser = argparse.ArgumentParser(description='Extract ST constants from a C header file.')
    cmd_parser.add_argument('file', nargs=1, help='input file')
    cmd_parser.add_argument('-q', '--qstr', dest='qstr_filename', default='build/stmconst_qstr.h',
                            help='Specified the name of the generated qstr header file')
    args = cmd_parser.parse_args()

    periphs, reg_defs = parse_file(args.file[0])

    modules = []
    needed_qstrs = set()

    print("// Automatically generated from %s by make-stmconst.py" % args.file[0])
    print("")

    for periph_name, periph_val in periphs:
        print_periph(periph_name, periph_val, needed_qstrs)

    for reg in (
        'ADC',
        #'ADC_Common',
        #'CAN_TxMailBox',
        #'CAN_FIFOMailBox',
        #'CAN_FilterRegister',
        #'CAN',
        'CRC',
        'DAC',
        'DBGMCU',
        'DMA_Stream',
        'DMA',
        'EXTI',
        'FLASH',
        'GPIO',
        'SYSCFG',
        'I2C',
        'IWDG',
        'PWR',
        'RCC',
        'RTC',
        #'SDIO',
        'SPI',
        'TIM',
        'USART',
        'WWDG',
        'RNG',
        ):
        if reg in reg_defs:
            print_regs(reg, reg_defs[reg], needed_qstrs)
        #print_regs_as_submodules(reg, reg_defs[reg], modules, needed_qstrs)

    #print("#define MOD_STM_CONST_MODULES \\")
    #for mod_lower, mod_upper in modules:
    #    print("    { MP_OBJ_NEW_QSTR(MP_QSTR_%s), (mp_obj_t)&stm_%s_obj }, \\" % (mod_upper, mod_lower))

    print("")

    with open(args.qstr_filename, 'wt') as qstr_file:
        for qstr in sorted(needed_qstrs):
            print('Q({})'.format(qstr), file=qstr_file)

if __name__ == "__main__":
    main()
