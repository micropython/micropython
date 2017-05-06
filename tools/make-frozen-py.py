#!/usr/bin/env python
#
# Create Python source files from binary files to 'freeze' for MicroPython.
#
# Usage:
#
#   Have a directory with files to be frozen:
#
#   ```
#   resources/image.png
#   resources/favicon.ico
#   ```
#
#   Run the script, passing the path to the directory above:
#
#   `./tools/make-frozen-py.py resources > resources.py`
#
#   You can then include the resources.py in your normal `modules` directory
#   and it will be frozen along with the rest of the modules.
#
#   In your inisetup.py you can then do something like:
#
#   ```
#       from resources import resources
#       for resource in resources:
#           with open('.'.join(resource.rsplit('_', 1)), 'wb') as resource_file:
#               resource_file.write(resources[resource])
#   ```
#
# This file is adapted from, and is a companion to, `make-frozen.py`
#
from __future__ import print_function
import sys
import os

modules = []

root = sys.argv[1].rstrip("/")
root_len = len(root)

for dirpath, dirnames, filenames in os.walk(root):
    for f in filenames:
        fullpath = dirpath + "/" + f
        st = os.stat(fullpath)
        modules.append((fullpath[root_len + 1:], st))


for f, st in modules:
    data = open(sys.argv[1] + "/" + f, "rb").read()

    data = bytearray(data) # so Python2 extracts each byte as an integer

    print("_{} = b'".format(f.replace('.','_')), end="")
    chrs = ['']
    for c in data:
        chrs.append('\\x%02x' % c)
    chrs.append("'")
    print(''.join(chrs))

print("resources = {'", end="")
for i, (f, st) in enumerate(modules):
    m = f.replace('.','_')
    print("{}' : _{}".format(m,m), end="")
    if not len(modules)-1 == i:
        print(", '", end="")
print("}")
