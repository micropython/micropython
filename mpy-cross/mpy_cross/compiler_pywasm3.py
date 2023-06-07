# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
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
import struct
import sys

from .compiler import (
    CrossCompileError,
    CompilerNotFoundError,
    Compiler,
    NATIVE_ARCH_NONE,
    EMIT_BYTECODE,
    native_arch_id,
    emit_id,
)

try:
    import wasm3
except ImportError:
    raise CompilerNotFoundError


class CompilerPywasm3(Compiler):
    def __init__(self, binary=None):
        with open(self._find_binary(binary, ext=".wasm", build_dir="build-wasm"), "rb") as f:
            self._wasm = f.read()

    def mpy_version(self):
        runtime, mem, _ = self._create_runtime()

        mpycross_bytecode_version = runtime.find_function("mpycross_bytecode_version")
        mpycross_bytecode_sub_version = runtime.find_function("mpycross_bytecode_sub_version")

        return (mpycross_bytecode_version(), mpycross_bytecode_sub_version())

    def version(self):
        runtime, mem, _ = self._create_runtime()

        mpycross_malloc = runtime.find_function("mpycross_malloc")
        mpycross_version = runtime.find_function("mpycross_version")

        version_len = mpycross_malloc(4)
        version_str = mpycross_version(version_len)
        (version_len,) = struct.unpack("<I", mem[version_len : version_len + 4])
        return mem[version_str : version_str + version_len].tobytes().decode()

    def compile(
        self, src, dest=None, src_path=None, opt=0, march=NATIVE_ARCH_NONE, emit=EMIT_BYTECODE
    ):
        with open(src, "rb") as f:
            src_contents = f.read()

        src_path = (src_path or src).encode() + b"\x00"
        dest = dest or (os.path.splitext(src)[0] + ".mpy")
        march = native_arch_id(march)
        emit = emit_id(emit)

        runtime, mem, mod = self._create_runtime(100 * 1024)
        mpycross_malloc = runtime.find_function("mpycross_malloc")
        mpycross_compile = runtime.find_function("mpycross_compile")

        def mpycross_error(buf, len):
            raise CrossCompileError(bytes(mem[buf : buf + len]).decode())

        mod.link_function("env", "mpycross_error", "v(ii)", mpycross_error)

        def proc_exit(status):
            raise RuntimeError("Unhandled exit({}) in mpy-cross.".format(status))

        mod.link_function("wasi_snapshot_preview1", "proc_exit", "v(i)", proc_exit)

        input_name = mpycross_malloc(len(src_path))
        mem[input_name : input_name + len(src_path)] = src_path

        input_len = len(src_contents)
        input_data = mpycross_malloc(input_len)
        mem[input_data : input_data + input_len] = src_contents

        output_len = mpycross_malloc(4)
        result = mpycross_malloc(4)

        small_int_bits = 0  # default (i.e. 31)
        try:
            output_data = mpycross_compile(
                input_name,
                input_data,
                input_len,
                opt,
                emit,
                march,
                small_int_bits,
                output_len,
                result,
            )
        except CrossCompileError as e:
            raise e
        (result,) = struct.unpack("<I", mem[result : result + 4])
        (output_len,) = struct.unpack("<I", mem[output_len : output_len + 4])
        mpy = mem[output_data : output_data + output_len]

        with open(dest, "wb") as f:
            f.write(mpy)

    def description(self):
        return "pywasm3 " + wasm3.M3_VERSION

    def _create_runtime(self, stack_size=10 * 1024):
        env = wasm3.Environment()
        rt = env.new_runtime(stack_size)
        mod = env.parse_module(self._wasm)
        rt.load(mod)
        mem = rt.get_memory(0)
        return rt, mem, mod
