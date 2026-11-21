#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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

"""Compile a model (.tflite or .onnx) into a relocatable model for the npu module.

Usage: compile_model.py MODEL [OUTPUT] [STEDGEAI_OPTIONS...]

OUTPUT defaults to the model's name with a .bin extension.  Any further options are
passed to "stedgeai generate", for example "--input-data-type uint8" to give a quantised
image model 8-bit RGB pixels as its input.

Needs ST Edge AI Core, with the same NPU runtime version as the firmware, installed at
STEDGEAI_DIR (default ~/STEdgeAI/4.0, or C:/ST/STEdgeAI/4.0 on Windows), and make and the
Arm GNU toolchain (arm-none-eabi-gcc) on the PATH.
"""

import os
import platform
import re
import subprocess
import sys
import tempfile

CONFIG_DIR = os.path.dirname(os.path.abspath(__file__))


def stedgeai_path():
    if sys.platform in ("win32", "cygwin", "msys"):
        default_dir, plat, exe = "C:/ST/STEdgeAI/4.0", "windows", "stedgeai.exe"
    elif sys.platform == "darwin":
        default_dir, exe = "~/STEdgeAI/4.0", "stedgeai"
        plat = "macarm" if platform.machine().lower() == "arm64" else "mac"
    else:
        default_dir, plat, exe = "~/STEdgeAI/4.0", "linux", "stedgeai"
    stedgeai_dir = os.path.expanduser(os.environ.get("STEDGEAI_DIR", default_dir))
    path = os.path.join(stedgeai_dir, "Utilities", plat, exe)
    if not os.path.isfile(path):
        raise SystemExit("compile_model: {} not found, set STEDGEAI_DIR".format(path))
    return path


def compile_model(model, options=()):
    """Compile a model, returning the relocatable model and a summary of what it needs."""
    with tempfile.TemporaryDirectory() as work_dir:
        # stm32n6.mpool describes the memory the npu module gives models (see
        # ports/stm32/modnpu.c): AXISRAM3-6 for activations, overflowing into PSRAM.
        cmd = [
            stedgeai_path(),
            "generate",
            "--model",
            model,
            "--target",
            "stm32n6",
            "--st-neural-art",
            "micropython@" + os.path.join(CONFIG_DIR, "neural_art.json"),
            "--relocatable",
            "--workspace",
            os.path.join(work_dir, "workspace"),
            "--output",
            os.path.join(work_dir, "output"),
        ] + list(options)
        proc = subprocess.run(
            cmd, stdin=subprocess.DEVNULL, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
        )
        log = proc.stdout.decode(errors="replace")
        if proc.returncode != 0:
            sys.stdout.write(log)
            raise SystemExit("compile_model: compiling {} failed".format(model))
        with open(os.path.join(work_dir, "output", "network_rel.bin"), "rb") as f:
            data = f.read()

    # The runtime version the model needs, and the memory needed: the file (in PSRAM,
    # unless it's used in place from ROMFS), the activations that overflow into PSRAM
    # ("ext_ram_sz") and the model's data in the heap ("XIP size").
    pattern = (
        r'rt_version_desc="[^"]*"'
        r"|(?:binary file size|params size|acts size|XIP size) *= *[0-9,]+"
        r"|ext_ram_sz=[0-9,]+"
    )
    summary = list(dict.fromkeys(re.findall(pattern, log)))
    return data, summary


def main():
    args = sys.argv[1:]
    if not args or args[0] in ("-h", "--help"):
        print(__doc__)
        sys.exit(1)
    model = args.pop(0)
    output = os.path.splitext(model)[0] + ".bin"
    if args and not args[0].startswith("-"):
        output = args.pop(0)
    data, summary = compile_model(model, args)
    for line in summary:
        print(line)
    with open(output, "wb") as f:
        f.write(data)
    print("Wrote " + output)


if __name__ == "__main__":
    main()
