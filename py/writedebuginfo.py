# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Alessandro Gatti
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
Encode to STDOUT some configuration information used to build the binary.

The output is meant to be embedded in the final ELF binary (or platform-specific
equivalent), to be used by GDB.  Platform build scripts will take this output
and perform the actual binary merge operation.

More documentation on how this works can be found here:
https://sourceware.org/gdb/current/onlinedocs/gdb.html/dotdebug_005fgdb_005fscripts-section.html
"""

import io
import os
import shlex
import subprocess
import sys


DEBUG_INFO_TEMPLATE = """
#include "mpconfigport.h"
#include "../../py/mpconfig.h"

MPY_DEBUG_INFO_START
repr({
  "float_impl": MICROPY_FLOAT_IMPL,
  "longint_impl": MICROPY_LONGINT_IMPL,
  "mpz_dig_size": MPZ_DIG_SIZE,
  "obj_immediate_objs": MICROPY_OBJ_IMMEDIATE_OBJS,
  "obj_repr": MICROPY_OBJ_REPR,
  "timestamp_impl": MICROPY_TIMESTAMP_IMPL,
})
"""


def preprocess(source):
    preprocessor = os.environ.get("CPP", None)
    if not preprocessor:
        raise Exception("no preprocessor specified")
    cpp, *flags = shlex.split(preprocessor)
    command_line = [cpp]
    if flags:
        command_line.extend(flags)
    command_line.extend(shlex.split(os.environ.get("CFLAGS", "")))
    command_line.append("-")
    with subprocess.Popen(command_line, stdin=subprocess.PIPE, stdout=subprocess.PIPE) as process:
        process.stdin.write(DEBUG_INFO_TEMPLATE.encode())
        process.stdin.close()
        try:
            stdout, stderr = process.communicate(timeout=10)
        except subprocess.TimeoutExpired:
            process.kill()
            stdout, stderr = process.communicate()
    if stderr:
        raise Exception("preprocessing failed", stderr)
    debug_info = stdout.decode("utf-8")
    cut_index = debug_info.find("MPY_DEBUG_INFO_START")
    if cut_index == -1:
        raise Exception("incomplete debug info")
    debug_info = [line for line in debug_info[cut_index:].splitlines()[1:] if line]
    return "\n".join(debug_info).strip()


def main():
    # The C preprocessor won't remove extra brackets or evaluate expressions in
    # the first pass.  However the output should be "python-like" enough for
    # eval() to clean up.

    preprocessed = preprocess(DEBUG_INFO_TEMPLATE)
    debug_info = eval(preprocessed).strip()
    sys.stdout.write(f'\x04"gdb.inlinedscript"\nMPY_BINARY_DEBUG_INFO={debug_info}\x00')


if __name__ == "__main__":
    main()
