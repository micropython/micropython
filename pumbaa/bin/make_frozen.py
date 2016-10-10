#!/usr/bin/env python
#
# Create frozen modules structure for MicroPython.
#
# Usage:
#
# Have a list of modules to be frozen:
#
# Run script, passing path to the directory above:
#
# ./make_frozen.py frozen1.py frozen2.py > frozen.c
#
# Include frozen.c in your build, having defined
# MICROPY_MODULE_FROZEN_STR in config.
#
from __future__ import print_function

import os
import argparse
import glob
import sys

def module_name(module):
    return os.path.basename(module)


def generate_frozen(paths):
    """Genrate the frozen module file.

    """      

    frozen = []
    modules = []
    
    for module in paths:
        if os.path.isdir(module):
            modules += glob.glob(os.path.join(module, "*.py"))
        else:
            modules.append(module)

    frozen.append("#include <stdint.h>")
    frozen.append("const char mp_frozen_str_names[] = {")

    for module in modules:
        frozen.append('"%s\\0"' % module_name(module))

    frozen.append('"\\0"};')

    frozen.append("const uint32_t mp_frozen_str_sizes[] = {")

    for module in modules:
        frozen.append("%d," % os.stat(module).st_size)

    frozen.append("};")

    frozen.append("const char mp_frozen_str_content[] = {")

    for module in modules:
        data = open(module, "rb").read()

        # We need to properly escape the script data to create a C string.
        # When C parses hex characters of the form \x00 it keeps parsing the hex
        # data until it encounters a non-hex character.  Thus one must create
        # strings of the form "data\x01" "abc" to properly encode this kind of
        # data.  We could just encode all characters as hex digits but it's nice
        # to be able to read the resulting C code as ASCII when possible.

        data = bytearray(data) # so Python2 extracts each byte as an integer
        esc_dict = {
            ord('\n'): '\\n',
            ord('\r'): '\\r',
            ord('"'): '\\"',
            ord('\\'): '\\\\'
        }
        chrs = ['"']
        break_str = False
        
        for c in data:
            try:
                chrs.append(esc_dict[c])
            except KeyError:
                if 32 <= c <= 126:
                    if break_str:
                        chrs.append('" "')
                        break_str = False
                    chrs.append(chr(c))
                else:
                    chrs.append('\\x%02x' % c)
                    break_str = True

        chrs.append('\\0"')
        frozen.append(''.join(chrs))

    frozen.append("};")

    return '\n'.join(frozen)

    
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-file")
    parser.add_argument("modules", nargs="*")
    args = parser.parse_args()

    frozen = generate_frozen(args.modules)
    
    if args.output_file:
        with open(args.output_file, "w") as fout:
            fout.write(frozen)
    else:
        sys.stdout.write(frozen)


if __name__ == '__main__':
    main()
