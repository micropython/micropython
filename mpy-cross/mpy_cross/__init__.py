#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2022 Andrew Leech
# Copyright (c) 2022 Jim Mussared
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

from __future__ import print_function
import os
import stat
import subprocess

NATIVE_ARCH_X86 = "x86"
NATIVE_ARCH_X64 = "x64"
NATIVE_ARCH_ARMV6 = "armv6"
NATIVE_ARCH_ARMV6M = "armv6m"
NATIVE_ARCH_ARMV7M = "armv7m"
NATIVE_ARCH_ARMV7EM = "armv7em"
NATIVE_ARCH_ARMV7EMSP = "armv7emsp"
NATIVE_ARCH_ARMV7EMDP = "armv7emdp"
NATIVE_ARCH_XTENSA = "xtensa"
NATIVE_ARCH_XTENSAWIN = "xtensawin"

NATIVE_ARCHS = [
    NATIVE_ARCH_X86,
    NATIVE_ARCH_X64,
    NATIVE_ARCH_ARMV6,
    NATIVE_ARCH_ARMV6M,
    NATIVE_ARCH_ARMV7M,
    NATIVE_ARCH_ARMV7EM,
    NATIVE_ARCH_ARMV7EMSP,
    NATIVE_ARCH_ARMV7EMDP,
    NATIVE_ARCH_XTENSA,
    NATIVE_ARCH_XTENSAWIN,
]

__all__ = ["compile", "run", "CrossCompileError"]


class CrossCompileError(Exception):
    pass


def find_mpy_cross_binary(mpy_cross):
    if mpy_cross:
        return mpy_cross
    return os.path.abspath(os.path.join(os.path.dirname(__file__), "../mpy-cross"))


def compile(src, dest=None, src_path=None, opt=None, march=None, mpy_cross=None, extra_args=None):
    if not src:
        raise ValueError("src is required")
    if not os.path.exists(src):
        raise CrossCompileError("Input .py file not found: {}.".format(src_py))

    args = []

    if src_path:
        args += ["-s", src_path]

    if dest:
        args += ["-o", dest]

    if march:
        args += ["-march", march]

    if opt is not None:
        args += ["-O{}".format(opt)]

    if extra_args:
        args += extra_args

    args += [src]

    run(args, mpy_cross)


def run(args, mpy_cross=None):
    mpy_cross = find_mpy_cross_binary(mpy_cross)

    if not os.path.exists(mpy_cross):
        raise CrossCompileError("mpy-cross binary not found at {}.".format(mpy_cross))

    try:
        st = os.stat(mpy_cross)
        os.chmod(mpy_cross, st.st_mode | stat.S_IEXEC)
    except OSError:
        pass

    try:
        subprocess.check_output([mpy_cross] + args, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as er:
        raise CrossCompileError(er.output)
