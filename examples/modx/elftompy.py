"""
This tool is ultimately intended to convert a .elf to a .mpy file.
But for now all it does is patch the text size within a .mpy file.

Usage (after building modx.mpy):

    $ python elftompy.py modx.mpy

This will make modx.mpy ready for importing.
"""

import sys

def process_file(filename):
    with open(filename, 'rb') as f:
        data = bytearray(f.read())
    size = len(data) - 16
    data[8] = size & 0xff
    data[9] = size >> 8
    with open(filename, 'wb') as f:
        f.write(data)

if __name__ == '__main__':
    process_file(sys.argv[1])
