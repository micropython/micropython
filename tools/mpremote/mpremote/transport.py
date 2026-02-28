#!/usr/bin/env python3
#
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

import ast, binascii, errno, hashlib, os, re, sys
from collections import namedtuple
from .compression_utils import (
    compress_chunk,
    test_compression_ratio,
    MIN_COMPRESS_SIZE,
    MIN_COMPRESS_RATIO,
    DEFLATE_CHUNK_SIZE,
    DEFLATE_WBITS,
    BASE64_CHUNK_SIZE,
)
from .mp_errno import MP_ERRNO_TABLE


def stdout_write_bytes(b):
    b = b.replace(b"\x04", b"")
    if hasattr(sys.stdout, "buffer"):
        sys.stdout.buffer.write(b)
        sys.stdout.buffer.flush()
    else:
        text = b.decode(sys.stdout.encoding, "strict")
        sys.stdout.write(text)


class TransportError(Exception):
    pass


class TransportExecError(TransportError):
    def __init__(self, status_code, error_output):
        self.status_code = status_code
        self.error_output = error_output
        super().__init__(error_output)


listdir_result = namedtuple("dir_result", ["name", "st_mode", "st_ino", "st_size"])


# Takes a Transport error (containing the text of an OSError traceback) and
# raises it as the corresponding OSError-derived exception.
def _convert_filesystem_error(e, info):
    if "OSError" in e.error_output:
        for code, estr in [
            *errno.errorcode.items(),
            (errno.EOPNOTSUPP, "EOPNOTSUPP"),
        ]:
            if estr in e.error_output:
                return OSError(code, info)

        # Some targets don't render OSError with the name of the errno, so in these
        # cases support an explicit mapping of errnos to known numeric codes.
        error_lines = e.error_output.splitlines()
        match = re.match(r"OSError: (\d+)$", error_lines[-1])
        if match:
            value = int(match.group(1), 10)
            if value in MP_ERRNO_TABLE:
                return OSError(MP_ERRNO_TABLE[value], info)

    return e


class Transport:
    def fs_listdir(self, src=""):
        buf = bytearray()

        def repr_consumer(b):
            buf.extend(b.replace(b"\x04", b""))

        cmd = "import os\nfor f in os.ilistdir(%s):\n print(repr(f), end=',')" % (
            ("'%s'" % src) if src else ""
        )
        try:
            buf.extend(b"[")
            self.exec(cmd, data_consumer=repr_consumer)
            buf.extend(b"]")
        except TransportExecError as e:
            raise _convert_filesystem_error(e, src) from None

        return [
            listdir_result(*f) if len(f) == 4 else listdir_result(*(f + (0,)))
            for f in ast.literal_eval(buf.decode())
        ]

    def fs_stat(self, src):
        try:
            self.exec("import os")
            return os.stat_result(self.eval("os.stat(%s)" % ("'%s'" % src)))
        except TransportExecError as e:
            raise _convert_filesystem_error(e, src) from None

    def fs_exists(self, src):
        try:
            self.fs_stat(src)
            return True
        except OSError:
            return False

    def fs_isdir(self, src):
        try:
            mode = self.fs_stat(src).st_mode
            return (mode & 0x4000) != 0
        except OSError:
            # Match CPython, a non-existent path is not a directory.
            return False

    def fs_printfile(self, src, chunk_size=256):
        cmd = (
            "with open('%s') as f:\n while 1:\n"
            "  b=f.read(%u)\n  if not b:break\n  print(b,end='')" % (src, chunk_size)
        )
        try:
            self.exec(cmd, data_consumer=stdout_write_bytes)
        except TransportExecError as e:
            raise _convert_filesystem_error(e, src) from None

    def fs_readfile(self, src, chunk_size=256, progress_callback=None):
        if progress_callback:
            src_size = self.fs_stat(src).st_size

        contents = bytearray()

        try:
            self.exec("f=open('%s','rb')\nr=f.read" % src)
            while True:
                chunk = self.eval("r({})".format(chunk_size))
                if not chunk:
                    break
                contents.extend(chunk)
                if progress_callback:
                    progress_callback(len(contents), src_size)
            self.exec("f.close()")
        except TransportExecError as e:
            raise _convert_filesystem_error(e, src) from None

        return contents

    def detect_encoding_capabilities(self):
        """Detect available encoding methods on device. Cached after first call."""
        if hasattr(self, "_fs_encoding_caps"):
            return self._fs_encoding_caps

        # Probe base64 and bytesio independently from deflate, so that a missing
        # deflate module doesn't prevent base64 from being detected.
        try:
            caps = self.eval(
                "{"
                "'bytesio':hasattr(__import__('io'),'BytesIO'),"
                "'base64':hasattr(__import__('binascii'),'a2b_base64'),"
                "}"
            )
        except TransportExecError:
            caps = {}

        try:
            has_deflate = self.eval("hasattr(__import__('deflate'),'DeflateIO')")
        except TransportExecError:
            has_deflate = False

        self._fs_encoding_caps = {
            "deflate": has_deflate and caps.get("bytesio") and caps.get("base64"),
            "base64": caps.get("base64", False),
        }

        return self._fs_encoding_caps

    def _choose_encoding_for_data(self, data):
        """Choose best encoding based on device capabilities and data compressibility.

        Three-tier fallback:
        1. deflate+base64 - if device has deflate/io/binascii and data compresses well
        2. base64 - if device has binascii.a2b_base64
        3. repr - universal fallback

        Returns: (encoding, ratio) where encoding is 'deflate', 'base64', or 'repr',
            and ratio is the compression ratio (float) for deflate, or None otherwise.
        """
        caps = self.detect_encoding_capabilities()

        if caps.get("deflate") and len(data) > MIN_COMPRESS_SIZE:
            ratio = test_compression_ratio(data)
            if ratio < MIN_COMPRESS_RATIO:
                return "deflate", ratio

        if caps.get("base64"):
            return "base64", None

        return "repr", None

    _VALID_ENCODINGS = ("deflate", "base64", "repr")

    def fs_writefile(self, dest, data, chunk_size=None, progress_callback=None, encoding=None):
        """Write data to a file on the device.

        Automatically selects the best encoding based on device capabilities and
        data compressibility, with dynamic chunk sizing per encoding method.

        Args:
            dest: Destination path on device
            data: Bytes to write
            chunk_size: Chunk size in bytes, or None for encoding-appropriate default.
            progress_callback: Optional callback(written, total)
            encoding: Force encoding: 'deflate', 'base64', 'repr', or None (auto-select)
        """
        if progress_callback:
            src_size = len(data)
            written = 0

        # Auto-select encoding based on data compressibility
        if encoding is None:
            encoding, _ratio = self._choose_encoding_for_data(data)

        if encoding not in self._VALID_ENCODINGS:
            raise ValueError(
                "encoding must be one of %s, got %r" % (self._VALID_ENCODINGS, encoding)
            )

        # Dynamic chunk sizing: larger chunks = fewer exec() round trips.
        # Only auto-size when caller didn't specify.
        if chunk_size is None:
            if encoding == "deflate":
                chunk_size = DEFLATE_CHUNK_SIZE
            elif encoding == "base64":
                chunk_size = BASE64_CHUNK_SIZE
            else:
                chunk_size = 256

        try:
            # Setup imports and file handle on device
            if encoding == "deflate":
                self.exec(
                    "from binascii import a2b_base64\n"
                    "from io import BytesIO\n"
                    "from deflate import DeflateIO,RAW\n"
                    "f=open('%s','wb')\nw=f.write" % dest
                )
            elif encoding == "base64":
                self.exec("from binascii import a2b_base64\nf=open('%s','wb')\nw=f.write" % dest)
            else:
                self.exec("f=open('%s','wb')\nw=f.write" % dest)

            while data:
                chunk = data[:chunk_size]
                if encoding == "deflate":
                    compressed = compress_chunk(chunk)
                    b64 = binascii.b2a_base64(compressed).strip()
                    self.exec(
                        "w(DeflateIO(BytesIO(a2b_base64(%s)),RAW,%d).read())"
                        % (b64, DEFLATE_WBITS)
                    )
                elif encoding == "base64":
                    b64 = binascii.b2a_base64(chunk).strip()
                    self.exec("w(a2b_base64(%s))" % b64)
                else:
                    self.exec("w(" + repr(chunk) + ")")
                data = data[len(chunk) :]
                if progress_callback:
                    written += len(chunk)
                    progress_callback(written, src_size)
            self.exec("f.close()")
        except TransportExecError as e:
            raise _convert_filesystem_error(e, dest) from None

    def fs_mkdir(self, path):
        try:
            self.exec("import os\nos.mkdir('%s')" % path)
        except TransportExecError as e:
            raise _convert_filesystem_error(e, path) from None

    def fs_rmdir(self, path):
        try:
            self.exec("import os\nos.rmdir('%s')" % path)
        except TransportExecError as e:
            raise _convert_filesystem_error(e, path) from None

    def fs_rmfile(self, path):
        try:
            self.exec("import os\nos.remove('%s')" % path)
        except TransportExecError as e:
            raise _convert_filesystem_error(e, path) from None

    def fs_touchfile(self, path):
        try:
            self.exec("f=open('%s','a')\nf.close()" % path)
        except TransportExecError as e:
            raise _convert_filesystem_error(e, path) from None

    def fs_hashfile(self, path, algo, chunk_size=256):
        try:
            self.exec("import hashlib\nh = hashlib.{algo}()".format(algo=algo))
        except TransportExecError:
            # hashlib (or hashlib.{algo}) not available on device. Do the hash locally.
            data = self.fs_readfile(path, chunk_size=chunk_size)
            return getattr(hashlib, algo)(data).digest()
        try:
            self.exec(
                "buf = memoryview(bytearray({chunk_size}))\nwith open('{path}', 'rb') as f:\n while True:\n  n = f.readinto(buf)\n  if n == 0:\n   break\n  h.update(buf if n == {chunk_size} else buf[:n])\n".format(
                    chunk_size=chunk_size, path=path
                )
            )
            return self.eval("h.digest()")
        except TransportExecError as e:
            raise _convert_filesystem_error(e, path) from None
