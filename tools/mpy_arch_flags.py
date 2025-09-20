#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2025 Alessandro Gatti
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


MPY_VERSION = 6
MPY_SUB_VERSION = 3

MP_NATIVE_ARCH_FLAGS_PRESENT = 0x40


def mp_encode_uint(val, signed=False):
    encoded = bytearray([val & 0x7F])
    val >>= 7
    while val != 0 and val != -1:
        encoded.insert(0, 0x80 | (val & 0x7F))
        val >>= 7
    if signed:
        if val == -1 and encoded[0] & 0x40 == 0:
            encoded.insert(0, 0xFF)
        elif val == 0 and encoded[0] & 0x40 != 0:
            encoded.insert(0, 0x80)
    return encoded


def read_uint(source):
    i = 0
    while True:
        b = source.read(1)[0]
        i = (i << 7) | (b & 0x7F)
        if b & 0x80 == 0:
            break
    return i


def main(args=None):
    import argparse

    cmd_parser = argparse.ArgumentParser(
        description="A tool to manipulate the architecture flags of .mpy files."
    )
    cmd_parser.add_argument(
        "-march-flags",
        metavar="F",
        type=int,
        help="architecture flags value to set in the output file (default 0)",
    )
    cmd_parser.add_argument(
        "source", metavar="SOURCE", type=argparse.FileType("rb"), help="input .mpy file"
    )
    cmd_parser.add_argument(
        "target", metavar="TARGET", type=argparse.FileType("wb"), help="output .mpy file"
    )
    args = cmd_parser.parse_args(args)

    header = bytearray(args.source.read(4))
    if header[0] != ord("M") or header[1] != MPY_VERSION or (header[2] & 3) != MPY_SUB_VERSION:
        raise Exception("unsupported MPY file version or corrupted file")
    if header[2] & MP_NATIVE_ARCH_FLAGS_PRESENT != 0:
        read_uint(args.source)
    arch_flags = args.march_flags if args.march_flags is not None else 0
    if arch_flags != 0:
        header[2] |= MP_NATIVE_ARCH_FLAGS_PRESENT
    else:
        header[2] &= ~MP_NATIVE_ARCH_FLAGS_PRESENT
    args.target.write(header)
    if arch_flags != 0:
        args.target.write(mp_encode_uint(arch_flags))
    args.target.write(args.source.read())


if __name__ == "__main__":
    main()
