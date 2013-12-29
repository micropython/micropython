import argparse, dis, marshal, struct, sys, time, types

def show_file(fname):
    f = open(fname, "rb")
    magic = f.read(4)
    moddate = f.read(4)
    modtime = time.asctime(time.localtime(struct.unpack('I', moddate)[0]))
    f.read(4) # don't know what these 4 bytes are
    #print("magic %s" % (bytes_to_hex(magic)))
    #print("moddate %s (%s)" % (bytes_to_hex(moddate), modtime))
    code = marshal.load(f)
    to_show_code = [code]
    for c in to_show_code:
        show_code(c, to_show_code)
            
def show_code(code, to_show_code):
    print("code {}".format(code.co_name))
    indent = '     '
    print("%sflags %04x" % (indent, code.co_flags))
    print("%sargcount %d" % (indent, code.co_argcount))
    print("%snlocals %d" % (indent, code.co_nlocals))
    print("%sstacksize %d" % (indent, code.co_stacksize))
    #show_hex("code", code.co_code, indent=indent)
    disassemble(code)
    #print("%sconsts" % indent)
    for const in code.co_consts:
        if type(const) == types.CodeType:
    #        print("   {}code at {:#x}".format(indent, id(const)))
            to_show_code.append(const)
    #    else:
    #        print("   %s%r" % (indent, const))
    #print("%snames %r" % (indent, code.co_names))
    #print("%svarnames %r" % (indent, code.co_varnames))
    #print("%sfreevars %r" % (indent, code.co_freevars))
    #print("%scellvars %r" % (indent, code.co_cellvars))
    #print("%sfilename %r" % (indent, code.co_filename))
    #print("%sname %r" % (indent, code.co_name))
    #print("%sfirstlineno %d" % (indent, code.co_firstlineno))
    #show_hex("lnotab", code.co_lnotab, indent=indent)
                        
def show_hex(label, h, indent):
    h = bytes_to_hex(h)
    if len(h) < 60:
        print("%s%s %s" % (indent, label, h))
    else:
        print("%s%s" % (indent, label))
        for i in range(0, len(h), 60):
            print("%s   %s" % (indent, h[i:i+60]))

def bytes_to_hex(bs):
    h = []
    for b in bs:
        h.append("{:02x}".format(b))
    return ''.join(h)

# taken from python library
import opcode
def disassemble(co):
    """Disassemble a code object."""
    code = co.co_code
    num_bytes = len(code)
    num_ops = 0
    i = 0
    extended_arg = 0
    free = None
    while i < num_bytes:
        op = code[i]
        print(repr(i).rjust(4), end=' ')
        num_ops += 1
        i = i+1
        if op < opcode.HAVE_ARGUMENT:
            print(opcode.opname[op])
        else:
            print(opcode.opname[op], end=' ')
            oparg = code[i] + code[i+1]*256 + extended_arg
            extended_arg = 0
            i = i+2
            if op == opcode.EXTENDED_ARG:
                extended_arg = oparg*65536
            #print(repr(oparg).rjust(5))
            if op in opcode.hasconst:
                if type(co.co_consts[oparg]) == types.CodeType:
                    print('code', co.co_consts[oparg].co_name)
                else:
                    print(repr(co.co_consts[oparg]))
            elif op in opcode.hasname:
                print(co.co_names[oparg])
            elif op in opcode.hasjrel:
                print(repr(i + oparg))
            elif op in opcode.haslocal:
                print(oparg, co.co_varnames[oparg])
            elif op in opcode.hascompare:
                print(opcode.cmp_op[oparg])
            elif op in opcode.hasfree:
                if free is None:
                    free = co.co_cellvars + co.co_freevars
                print(oparg, free[oparg])
            elif op in opcode.hasnargs:
                print('{}, {}'.format(code[i-2], code[i-1]))
            else:
                print(repr(oparg))
    # accounting output for bytes per opcode
    #print('{} bytes / {} opcodes = {} bytes per opcode'.format(num_bytes, num_ops, num_bytes / num_ops))
    #print('{} {} {} # bpo'.format(num_bytes, num_ops, num_bytes / num_ops))

if __name__ == "__main__":
    cmd_parser = argparse.ArgumentParser(description='Uncompile .pyc files')
    cmd_parser.add_argument('files', nargs='+', help='input files')
    args = cmd_parser.parse_args()

    for fname in args.files:
        show_file(fname)
