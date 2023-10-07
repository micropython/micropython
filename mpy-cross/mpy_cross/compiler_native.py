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

import os
import re
import stat
import subprocess

from .compiler import CrossCompileError, Compiler, NATIVE_ARCH_NONE, EMIT_BYTECODE

_BYTECODE_VERSION_RE = re.compile("mpy-cross emitting mpy v([0-9]+)(?:.([0-9]+))?")


class CompilerNative(Compiler):
    def __init__(self, binary=None):
        self._binary = self._find_binary(binary)
        self._cached_version_output = None

    def mpy_version(self):
        match = re.search(_BYTECODE_VERSION_RE, self._query_version_info())
        mpy_version, mpy_sub_version = int(match.group(1)), int(match.group(2) or "0")
        return (
            mpy_version,
            mpy_sub_version,
        )

    def version(self):
        return self._exec(["--version"]).split(";")[0].strip()

    def compile(
        self, src, dest=None, src_path=None, opt=0, march=NATIVE_ARCH_NONE, emit=EMIT_BYTECODE
    ):
        if not src:
            raise ValueError("src is required")

        if not os.path.exists(src):
            raise CrossCompileError("Input .py file not found: {}.".format(src))

        args = []

        if src_path is not None:
            args += ["-s", src_path]

        if dest is not None:
            args += ["-o", dest]

        if opt is not None:
            args += ["-O{}".format(opt)]

        if march:
            args += ["-march=" + march]

        if emit:
            args += ["-X", "emit=" + emit]

        args += [src]

        self._exec(args)

    def description(self):
        return "native"

    def _query_version_info(self):
        if not self._cached_version_output:
            self._cached_version_output = self._exec(["--version"])
        return self._cached_version_output

    def _exec(self, args):
        try:
            st = os.stat(self._binary)
            os.chmod(self._binary, st.st_mode | stat.S_IEXEC)
        except OSError:
            pass

        try:
            return subprocess.check_output(
                [self._binary] + args, stderr=subprocess.STDOUT
            ).decode()
        except subprocess.CalledProcessError as er:
            raise CrossCompileError(er.output.decode())
