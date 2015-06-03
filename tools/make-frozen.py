#!/usr/bin/env python3
#
# Create frozen modules structure for MicroPython.
#
# Usage:
#
# Have a directory with modules to be frozen (only modules, not packages
# supported so far):
#
# frozen/foo.py
# frozen/bar.py
#
# Run script, passing path to the directory above:
#
# ./make-frozen.py frozen > frozen.c
#
# Include frozen.c in your build, having defined MICROPY_MODULE_FROZEN in
# config.
#
import sys
import os


def module_name(f):
    return f[:-len(".py")]

modules = []

root = sys.argv[1].rstrip("/")
root_len = len(root)

for dirpath, dirnames, filenames in os.walk(root):
    for f in filenames:
        fullpath = dirpath + "/" + f
        st = os.stat(fullpath)
        modules.append((fullpath[root_len + 1:], st))

print("#include <stdint.h>")
print("const uint16_t mp_frozen_sizes[] = {")

for f, st in modules:
    print("%d," % st.st_size)

print("0};")

print("const char mp_frozen_content[] = {")
for f, st in modules:
    m = module_name(f)
    print('"%s\\0"' % m)
    data = open(sys.argv[1] + "/" + f, "rb").read()
    data = repr(data)[2:-1]
    data = data.replace('"', '\\"')
    print('"%s"' % data)
print("};")
