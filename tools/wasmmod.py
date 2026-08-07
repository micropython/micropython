#!/usr/bin/env python3

# This file is part of wasmmod, https://github.com/pymergetic/wasmmod
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Rouven Raudzus <raudzus@pymergetic.com>
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

"""
Thin host trampoline for the wasmmod CLI.

Experimental: the ``extmod/wasmmod`` submodule is pre-release (alpha). This
script only re-execs the submodule CLI; see that package README for status.

MetalPython (and other MicroPython trees) keep this script at ``tools/wasmmod.py``
so you can run the usual commands from the host tree without ``cd`` into the
submodule. It does not implement pack/sign/embed-ca/httpd itself — it locates
``extmod/wasmmod/tools/wasmmod.py`` and re-executes that entry point with the
same argv.

Requires the ``extmod/wasmmod`` submodule
(https://github.com/pymergetic/wasmmod). Init with::

    git submodule update --init --recursive extmod/wasmmod

Usage (from the host repo root)::

    python3 tools/wasmmod.py pack …
    python3 tools/wasmmod.py sign …
    python3 tools/wasmmod.py embed-ca …
    python3 tools/wasmmod.py httpd …

See the submodule README / tools docstring for full command help.
"""

import runpy
import sys
from pathlib import Path

t = Path(__file__).resolve().parents[1] / "extmod/wasmmod/tools/wasmmod.py"
if not t.is_file():
    sys.exit("error: needs extmod/wasmmod — git submodule update --init extmod/wasmmod")
sys.argv[0] = str(t)
runpy.run_path(str(t), run_name="__main__")
