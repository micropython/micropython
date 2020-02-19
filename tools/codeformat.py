#!/usr/bin/env python3

import sys, os, glob

FILES = [
    'extmod/*.[ch]',
    'lib/netutils/*.[ch]',
    'lib/timeutils/*.[ch]',
    'lib/utils/*.[ch]',
    'ports/*/*.[ch]',
    'py/*.[ch]',
]

EXCLUSIONS = [
    'ports/*/qstrdefsport.h',
    'py/grammar.h',
    'py/qstrdefs.h',
    'py/vm.c',
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
        os.stat('.astylerc')
    except OSError:
        print('Run this from the top-level directory, e.g. ./tools/codeformat.py', file=sys.stderr)
        sys.exit(1)

    os.system('astyle --project ' + ' '.join(list_files()))

if __name__ == '__main__':
    main()
