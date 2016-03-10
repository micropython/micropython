#!/usr/bin/env python
"""Convert a file to a C char array representation.

Outputs a C header on standard output.

"""

from sys import argv, exit, stdout
from os.path import basename, isfile


BYTES_PER_LINE = 12
INDENT = 2

# for Python 2 backwards compatibility
if isinstance(b'', str):
    toint = ord
else:
    toint = lambda x: x


def read_file(fobj, chunksize):
    while True:
        chunk = fobj.read(chunksize)
        if not chunk:
            break
        yield chunk


def fn2identifier(fname):
    res = []
    for c in basename(fname).lower():
        res.append(c if c.isalnum() else '_')

    if not (res[0].isalpha() or res[0] == '_'):
        res.insert(0, '_')

    return "".join(res)


def process_file(inf, outf, varname, bytes_per_line=BYTES_PER_LINE,
                 indent=INDENT, lenvar=True, guard=True):
    size = 0

    if guard:
        outf.write("#ifndef {}_H\n".format(varname.upper()))
        outf.write("#define {}_H\n".format(varname.upper()))

    outf.write("static const char {}[] = {{\n".format(varname + '_data'))

    for bytes in read_file(inf, bytes_per_line):
        size = size + len(bytes)
        outf.write(" " * indent)
        outf.write(", ".join("0x{:02x}".format(toint(i)) for i in bytes))
        outf.write(",\n" if len(bytes) == bytes_per_line else '\n')

    outf.write("};\n")

    if lenvar:
        outf.write("const size_t {} = {:d};\n".format(varname + '_len', size))

    if guard:
        outf.write("#endif\n")


def main(args=None):
    if not args:
        return "Usage: file2h.py <file>"
    else:
        infn = args[0]
        if not isfile(infn):
            return "{} is not a file.".format(infn)
        else:
            varname = fn2identifier(infn)
            with open(infn, "rb") as infile:
                process_file(infile, stdout, varname)


if __name__ == '__main__':
    exit(main(argv[1:]) or 0)
