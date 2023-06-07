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


from .compiler import *


def mpy_version():
    return default_compiler().mpy_version()


def version():
    return default_compiler().version()


def compile(*args, **kwargs):
    compiler_kwargs = {}

    if "mpy_cross" in kwargs:
        compiler_kwargs.update(binary=kwargs["mpy_cross"])
        del kwargs["mpy_cross"]

    if "extra_args" in kwargs:
        for arg in kwargs["extra_args"]:
            if arg.startswith("-march="):
                kwargs.update(march=arg[7:])
            else:
                raise ValueError("Unknown mpy-cross arg: {}".format(arg))
        del kwargs["extra_args"]

    return default_compiler(**compiler_kwargs).compile(*args, **kwargs)


def description():
    return default_compiler().description()
