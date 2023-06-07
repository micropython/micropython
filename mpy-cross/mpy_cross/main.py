# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2022 Andrew Leech
# Copyright (c) 2023 Jim Mussared
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

import argparse
import sys

from .compiler import (
    default_compiler,
    CrossCompileError,
    EMIT_BYTECODE,
    EMIT_NATIVE,
    EMIT_VIPER,
    NATIVE_ARCHS,
)


def main():
    march_opts = [v for v in NATIVE_ARCHS if v]
    emit_opts = [EMIT_BYTECODE, EMIT_NATIVE, EMIT_VIPER]

    cmd_parser = argparse.ArgumentParser(description="MicroPython cross-compiler")
    cmd_parser.add_argument(
        "-v", "--version", default=False, action="store_true", help="Display version information"
    )
    cmd_parser.add_argument(
        "-o",
        "--output",
        type=str,
        help="output file for compiled bytecode (defaults to input with .mpy extension)",
    )
    cmd_parser.add_argument(
        "-s",
        "--source",
        type=str,
        help="source filename to embed in the compiled bytecode (defaults to input file)",
    )
    cmd_parser.add_argument(
        "-march",
        "--march",
        type=str,
        choices=march_opts,
        metavar="MARCH",
        help="set architecture for native emitter; " + ", ".join(march_opts),
    )
    cmd_parser.add_argument(
        "-O",
        "--opt",
        type=int,
        choices=range(0, 4),
        metavar="N",
        help="apply bytecode optimizations of level N",
    )
    cmd_parser.add_argument(
        "-X",
        metavar="emit=EMIT",
        choices=["emit=" + x for x in emit_opts],
        help="set the default code emitter; " + ", ".join(emit_opts),
    )
    cmd_parser.add_argument("file", nargs="?", help="input filename")
    args = cmd_parser.parse_args()

    try:
        c = default_compiler()
    except RuntimeError as e:
        print("Error: " + str(e), file=sys.stderr)
        sys.exit(1)

    if args.version:
        print("{} ({})".format(c.version(), c.description()))
        v, s = c.mpy_version()
        print("mpy-cross emitting mpy v{}.{}".format(v, s))
        sys.exit(0)

    if not args.file:
        print("Error: Input filename is required", file=sys.stderr)
        sys.exit(1)

    try:
        kwargs = {}
        if args.march:
            kwargs.update(march=args.march)
        if args.X:
            kwargs.update(emit=args.X.split("=")[1])
        if args.opt:
            kwargs.update(opt=args.opt)
        c.compile(args.file, src_path=args.source, dest=args.output, **kwargs)
    except CrossCompileError as er:
        print(er.args[0], file=sys.stderr)
        raise SystemExit(1)
