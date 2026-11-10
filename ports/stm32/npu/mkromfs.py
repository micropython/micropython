#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

"""Build a ROMFS image for the npu module, compiling any models it contains.

Usage: mkromfs.py SRC_DIR [-o OUTPUT]

Copies SRC_DIR into a ROMFS image (default SRC_DIR.romfs).  Each model in it (a .tflite
or .onnx file) is compiled with compile_model.py into a relocatable model for the npu
module, and stored with a .bin extension instead.  Options for compiling a model, eg
"--input-data-type uint8", can be given in a file next to it with the same name plus
".args", eg "yolox.tflite.args".  Other files, including already compiled models, are
copied as they are.

The data of every file is aligned to 32 bytes, so the npu module can use models in place
from the memory mapped ROMFS.  Deploy the image to the device with:

    mpremote romfs deploy OUTPUT
"""

import argparse
import os
import shlex

from compile_model import compile_model

MODEL_EXTS = (".tflite", ".onnx")
ALIGN = 32

ROMFS_HEADER = b"\xd2\xcd\x31"
KIND_PADDING = 1
KIND_DATA_VERBATIM = 2
KIND_DIRECTORY = 4
KIND_FILE = 5

# Record lengths are written with a fixed number of bytes, so the position of every
# record's contents is known before the contents are written, which allows aligning them.
LEN_BYTES = 4


def encode_uint(value, min_len=1):
    encoded = [value & 0x7F]
    value >>= 7
    while value:
        encoded.insert(0, 0x80 | (value & 0x7F))
        value >>= 7
    # Leading zero groups (0x80) don't change the value.
    return bytes([0x80] * (min_len - len(encoded)) + encoded)


class RomfsWriter:
    def __init__(self):
        self.buf = bytearray()

    def begin(self, kind):
        # Start a record whose length is filled in by end().
        self.buf += ROMFS_HEADER if kind is None else encode_uint(kind)
        pos = len(self.buf)
        self.buf += bytes(LEN_BYTES)
        return pos

    def end(self, pos):
        length = len(self.buf) - pos - LEN_BYTES
        if length >= 1 << (7 * LEN_BYTES):
            raise ValueError("ROMFS record too big")
        self.buf[pos : pos + LEN_BYTES] = encode_uint(length, LEN_BYTES)

    def name(self, name):
        name = name.encode("ascii")
        self.buf += encode_uint(len(name)) + name

    def data(self, data):
        # A data record, padding its length so the data starts at an aligned offset.
        self.buf += encode_uint(KIND_DATA_VERBATIM)
        length = encode_uint(len(data))
        pad = -(len(self.buf) + len(length)) % ALIGN
        self.buf += encode_uint(len(data), len(length) + pad)
        self.buf += data

    def file(self, name, data):
        pos = self.begin(KIND_FILE)
        self.name(name)
        self.data(data)
        self.end(pos)

    def finish(self, pos):
        # The filesystem record's size must be even, so add a 3-byte padding record if
        # it's odd.
        if len(self.buf) % 2:
            self.buf += encode_uint(KIND_PADDING) + encode_uint(1) + b"\x00"
        self.end(pos)


def add_dir(w, src_dir, prefix):
    for name in sorted(os.listdir(src_dir)):
        path = os.path.join(src_dir, name)
        if os.path.isdir(path):
            print(prefix + name + "/")
            pos = w.begin(KIND_DIRECTORY)
            w.name(name)
            add_dir(w, path, prefix + "    ")
            w.end(pos)
        elif name.endswith(".args") and name[:-5].endswith(MODEL_EXTS):
            continue
        elif name.endswith(MODEL_EXTS):
            out_name = name.rsplit(".", 1)[0] + ".bin"
            print(prefix + name + " -> " + out_name, flush=True)
            options = []
            if os.path.isfile(path + ".args"):
                with open(path + ".args") as f:
                    options = shlex.split(f.read())
            data, summary = compile_model(path, options)
            for line in summary:
                print(prefix + "    " + line)
            w.file(out_name, data)
        else:
            print(prefix + name)
            with open(path, "rb") as f:
                w.file(name, f.read())


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("src_dir", help="directory to put in the ROMFS image")
    parser.add_argument("-o", "--output", help="output image (default SRC_DIR.romfs)")
    args = parser.parse_args()

    src_dir = args.src_dir.rstrip("/")
    output = args.output or src_dir + ".romfs"
    w = RomfsWriter()
    pos = w.begin(None)
    print("/")
    add_dir(w, src_dir, "    ")
    w.finish(pos)
    with open(output, "wb") as f:
        f.write(w.buf)
    print("Wrote {} ({} bytes)".format(output, len(w.buf)))


if __name__ == "__main__":
    main()
