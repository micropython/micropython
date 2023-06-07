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
    import wasmer
except:
    raise CompilerNotFoundError

try:
    import wasmer_compiler_llvm as wasmer_compiler

    wasmer_compiler_type = "llvm"
except ImportError:
    try:
        import wasmer_compiler_cranelift as wasmer_compiler

        wasmer_compiler_type = "cranelift"
    except ImportError:
        try:
            import wasmer_compiler_singlepass as wasmer_compiler

            wasmer_compiler_type = "singlepass"
        except:
            raise CompilerNotFoundError


class CompilerWasmer(Compiler):
    def __init__(self, binary=None):
        with open(self._find_binary(binary, ext=".wasm", build_dir="build-wasm"), "rb") as f:
            self._wasm = f.read()

    def mpy_version(self):
        instance, mem = self._create_instance()

        mpycross_bytecode_version = instance.exports.mpycross_bytecode_version
        mpycross_bytecode_sub_version = instance.exports.mpycross_bytecode_sub_version

        return (mpycross_bytecode_version(), mpycross_bytecode_sub_version())

    def version(self):
        instance, mem = self._create_instance()

        mpycross_malloc = instance.exports.mpycross_malloc
        mpycross_version = instance.exports.mpycross_version

        version_len = mpycross_malloc(4)
        version_str = mpycross_version(version_len)
        (version_len,) = struct.unpack("<I", bytes(mem[version_len : version_len + 4]))
        return bytes(mem[version_str : version_str + version_len]).decode()

    def compile(
        self, src, dest=None, src_path=None, opt=0, march=NATIVE_ARCH_NONE, emit=EMIT_BYTECODE
    ):
        with open(src, "rb") as f:
            src_contents = f.read()

        src_path = (src_path or src).encode() + b"\x00"
        dest = dest or (os.path.splitext(src)[0] + ".mpy")
        march = native_arch_id(march)
        emit = emit_id(emit)

        instance, mem = self._create_instance()

        mpycross_compile = instance.exports.mpycross_compile
        mpycross_malloc = instance.exports.mpycross_malloc

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
        (result,) = struct.unpack("<I", bytes(mem[result : result + 4]))
        (output_len,) = struct.unpack("<I", bytes(mem[output_len : output_len + 4]))
        mpy = bytes(mem[output_data : output_data + output_len])

        with open(dest, "wb") as f:
            f.write(mpy)

    def description(self):
        return "wasmer " + wasmer.__version__ + " " + wasmer_compiler_type

    def _create_instance(self):
        engine = wasmer.engine.Universal(wasmer_compiler.Compiler)
        store = wasmer.Store(engine)

        def mpycross_error(buf: int, len: int) -> None:
            raise CrossCompileError(bytes(mem[buf : buf + len]).decode())

        def proc_exit(status: int) -> None:
            raise RuntimeError("Unhandled exit({}) in mpy-cross.".format(status))

        def fd_close(i: int) -> int:
            return 0

        def fd_write(a: int, b: int, c: int, d: int) -> int:
            return 0

        # ruff: noqa: F821 - i64 type
        def fd_seek(a: int, b: "i64", c: int, d: int) -> int:
            return 0

        import_object = wasmer.ImportObject()
        import_object.register(
            "env",
            {
                "mpycross_error": wasmer.Function(store, mpycross_error),
            },
        )
        import_object.register(
            "wasi_snapshot_preview1",
            {
                "proc_exit": wasmer.Function(store, proc_exit),
                "fd_close": wasmer.Function(store, fd_close),
                "fd_write": wasmer.Function(store, fd_write),
                "fd_seek": wasmer.Function(store, fd_seek),
            },
        )

        module = wasmer.Module(store, self._wasm)
        instance = wasmer.Instance(module, import_object)
        mem = instance.exports.memory.uint8_view(0)

        return instance, mem
