# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Nicko van Someren
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

# Minimal CPython-compatible gzip module for the UEFI port, implemented as a thin
# wrapper over the C `deflate` module (frozen into the firmware). Provides
# compress(), decompress(), and GzipFile()/open().

import deflate
import io
from builtins import open as _open

_GZIP = 3  # deflate format selector: gzip (DEFLATEIO_FORMAT_GZIP)


def GzipFile(filename=None, mode="rb", fileobj=None):
    if fileobj is None:
        fileobj = _open(filename, mode if "b" in mode else mode + "b")
    return deflate.DeflateIO(fileobj, _GZIP)


def open(filename, mode="rb"):
    return GzipFile(filename, mode)


def compress(data):
    stream = io.BytesIO()
    d = deflate.DeflateIO(stream, _GZIP)
    d.write(data)
    d.close()
    return stream.getvalue()


def decompress(data):
    return deflate.DeflateIO(io.BytesIO(data), _GZIP).read()
