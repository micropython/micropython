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
    import wasmtime
except ImportError:
    raise CompilerNotFoundError


class CompilerWasmTime(Compiler):
    def __init__(self, binary=None):
        with open(self._find_binary(binary, ext=".wasm", build_dir="build-wasm"), "rb") as f:
            self._wasm = f.read()

    def mpy_version(self):
        store, exports, mem = self._create_instance()

        mpycross_bytecode_version = exports["mpycross_bytecode_version"]
        mpycross_bytecode_sub_version = exports["mpycross_bytecode_sub_version"]

        return (mpycross_bytecode_version(store), mpycross_bytecode_sub_version(store))

    def version(self):
        store, exports, mem = self._create_instance()
        mpycross_malloc = exports["mpycross_malloc"]
        mpycross_version = exports["mpycross_version"]

        version_len = mpycross_malloc(store, 4)
        version_str = mpycross_version(store, version_len)
        (version_len,) = struct.unpack("<I", mem.read(store, version_len, version_len + 4))
        return mem.read(store, version_str, version_str + version_len).decode()

    def compile(
        self, src, dest=None, src_path=None, opt=0, march=NATIVE_ARCH_NONE, emit=EMIT_BYTECODE
    ):
        with open(src, "rb") as f:
            src_contents = f.read()

        src_path = (src_path or src).encode() + b"\x00"
        dest = dest or (os.path.splitext(src)[0] + ".mpy")
        march = native_arch_id(march)
        emit = emit_id(emit)

        store, exports, mem = self._create_instance()
        mpycross_malloc = exports["mpycross_malloc"]
        mpycross_compile = exports["mpycross_compile"]

        input_name = mpycross_malloc(store, len(src_path))
        mem.write(store, src_path, input_name)

        input_data = mpycross_malloc(store, len(src_contents))
        mem.write(store, src_contents, input_data)

        output_len = mpycross_malloc(store, 4)
        result = mpycross_malloc(store, 4)

        small_int_bits = 0  # default (i.e. 31)
        try:
            output_data = mpycross_compile(
                store,
                input_name,
                input_data,
                len(src_contents),
                opt,
                emit,
                march,
                small_int_bits,
                output_len,
                result,
            )
        except CrossCompileError as e:
            raise e
        (result,) = struct.unpack("<I", mem.read(store, result, result + 4))
        (output_len,) = struct.unpack("<I", mem.read(store, output_len, output_len + 4))
        mpy = mem.read(store, output_data, output_data + output_len)

        with open(dest, "wb") as f:
            f.write(mpy)

    def description(self):
        import pkg_resources

        return "wasmtime {}".format(pkg_resources.get_distribution("wasmtime").version)

    def _create_instance(self):
        engine = wasmtime.Engine()
        store = wasmtime.Store(engine)
        module = wasmtime.Module(engine, self._wasm)
        linker = wasmtime.Linker(engine)

        def mpycross_error(buf: int, len: int) -> None:
            raise CrossCompileError(mem.read(store, buf, buf + len).decode())

        linker.define_func(
            "env",
            "mpycross_error",
            wasmtime.FuncType([wasmtime.ValType.i32(), wasmtime.ValType.i32()], []),
            mpycross_error,
        )

        def proc_exit(status: int) -> None:
            raise RuntimeError("Unhandled exit({}) in mpy-cross.".format(status))

        def fd_close(i: int) -> int:
            return 0

        def fd_write(a: int, b: int, c: int, d: int) -> int:
            return 0

        # ruff: noqa: F821 - i64 type
        def fd_seek(a: int, b: "i64", c: int, d: int) -> int:
            return 0

        linker.define_func(
            "wasi_snapshot_preview1",
            "proc_exit",
            wasmtime.FuncType([wasmtime.ValType.i32()], []),
            proc_exit,
        ),
        linker.define_func(
            "wasi_snapshot_preview1",
            "fd_close",
            wasmtime.FuncType([wasmtime.ValType.i32()], [wasmtime.ValType.i32()]),
            fd_close,
        ),
        linker.define_func(
            "wasi_snapshot_preview1",
            "fd_write",
            wasmtime.FuncType(
                [
                    wasmtime.ValType.i32(),
                    wasmtime.ValType.i32(),
                    wasmtime.ValType.i32(),
                    wasmtime.ValType.i32(),
                ],
                [wasmtime.ValType.i32()],
            ),
            fd_write,
        ),
        linker.define_func(
            "wasi_snapshot_preview1",
            "fd_seek",
            wasmtime.FuncType(
                [
                    wasmtime.ValType.i32(),
                    wasmtime.ValType.i64(),
                    wasmtime.ValType.i32(),
                    wasmtime.ValType.i32(),
                ],
                [wasmtime.ValType.i32()],
            ),
            fd_seek,
        ),

        instance = linker.instantiate(store, module)
        exports = instance.exports(store)
        mem = exports["memory"]

        return store, exports, mem
