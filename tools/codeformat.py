#!/usr/bin/env python3

import sys, os, glob

FILES = [
    'extmod/*.[ch]',
    'lib/netutils/*.[ch]',
    'lib/timeutils/*.[ch]',
    'lib/utils/*.[ch]',
    'mpy-cross/*.[ch]',
    'ports/*/*.[ch]',
    'py/*.[ch]',
]

EXCLUSIONS = [
]

CFG = './tools/uncrustify.cfg'

def list_files():
    files = set()
    for pattern in FILES:
        files.update(glob.glob(pattern))
    for pattern in EXCLUSIONS:
        files.difference_update(glob.fnmatch.filter(files, pattern))
    return sorted(files)

def main():
    try:
        os.stat('./tools/uncrustify.cfg')
    except OSError:
        print('Run this from the top-level directory, e.g. ./tools/codeformat.py', file=sys.stderr)
        sys.exit(1)

    os.system('uncrustify -c {} -l c --no-backup {}'.format(CFG, ' '.join(list_files())))

if __name__ == '__main__':
    main()
