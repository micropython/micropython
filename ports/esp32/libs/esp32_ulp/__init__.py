from .util import garbage_collect

from .preprocess import preprocess
from .assemble import Assembler
from .link import make_binary
garbage_collect('after import')


def src_to_binary(src):
    assembler = Assembler()
    src = preprocess(src)
    assembler.assemble(src, remove_comments=False)  # comments already removed by preprocessor
    garbage_collect('before symbols export')
    addrs_syms = assembler.symbols.export()
    for addr, sym in addrs_syms:
        print('%04d %s' % (addr, sym))

    text, data, bss_len = assembler.fetch()
    return make_binary(text, data, bss_len)


def assemble_file(filename):
    with open(filename) as f:
        src = f.read()

    binary = src_to_binary(src)

    if filename.endswith('.s') or filename.endswith('.S'):
        filename = filename[:-2]
    with open(filename + '.ulp', 'wb') as f:
        f.write(binary)

