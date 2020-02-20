#!/usr/bin/env python3

import sys, os, glob

FILES = [
    'extmod/*.[ch]',
    'ports/*/*.[ch]',
    'py/*.[ch]',
]

EXCLUSIONS = [
]

def list_files():
    files = set()
    for pattern in FILES:
        files.update(glob.glob(pattern))
    for pattern in EXCLUSIONS:
        files.difference_update(glob.fnmatch.filter(files, pattern))
    return sorted(files)

def main():
    try:
        os.stat('.clang-format')
    except OSError:
        print('Run this from the top-level directory, e.g. ./tools/codeformat.py', file=sys.stderr)
        sys.exit(1)

    os.system('clang-format --style=file -i ' + ' '.join(list_files()))
    # Add spaces inside struct initialisation: `    {MP_QSTR_name, foo, {.x = y}},` --> `    { MP_QSTR_name, foo, {.x = y} },`
    os.system(r'sed -r -i "s/^(\s+\{)(\S.*\S)(\},?)$/\1 \2 \3/g" ' + ' '.join(list_files()))

if __name__ == '__main__':
    main()
