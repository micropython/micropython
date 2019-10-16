#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2019 Damien P. George
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

from __future__ import print_function
import sys
import os
import subprocess


###########################################################################
# Public functions to be used in the manifest

def include(manifest):
    """Include another manifest.

    The manifest argument can be a string (filename) or an iterable of
    strings.
    """

    if not isinstance(manifest, str):
        for m in manifest:
            include(m)
    else:
        with open(manifest) as f:
            exec(f.read())

def freeze(path, script=None, opt=0):
    """Freeze the input, automatically determining its type.  A .py script
    will be compiled to a .mpy first then frozen, and a .mpy file will be
    frozen directly.

    `path` must be a directory, which is the base directory to search for
    files from.  When importing the resulting frozen modules, the name of
    the module will start after `path`, ie `path` is excluded from the
    module name.

    If `script` is None all files in `path` will be frozen.

    If `script` is an iterable then freeze() is called on all items of the
    iterable (with the same `path` and `opt` passed through).

    If `script` is a string then it specifies the filename to freeze, and
    can include extra directories before the file.  The file will be
    searched for in `path`.

    `opt` is the optimisation level to pass to mpy-cross when compiling .py
    to .mpy.
    """

    freeze_internal(KIND_AUTO, path, script, opt)

def freeze_as_str(path):
    """Freeze the given `path` and all .py scripts within it as a string,
    which will be compiled upon import.
    """

    freeze_internal(KIND_AS_STR, path, None, 0)

def freeze_as_mpy(path, script=None, opt=0):
    """Freeze the input (see above) by first compiling the .py scripts to
    .mpy files, then freezing the resulting .mpy files.
    """

    freeze_internal(KIND_AS_MPY, path, script, opt)

def freeze_mpy(path, script=None, opt=0):
    """Freeze the input (see above), which must be .mpy files that are
    frozen directly.
    """

    freeze_internal(KIND_MPY, path, script, opt)


###########################################################################
# Internal implementation

KIND_AUTO = 0
KIND_AS_STR = 1
KIND_AS_MPY = 2
KIND_MPY = 3

manifest_list = []

class FreezeError(Exception):
    pass

def system(cmd):
    try:
        output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
        return 0, output
    except subprocess.CalledProcessError as er:
        return -1, er.output

def convert_path(path):
    return path.replace('$(MPY)', TOP)

def get_timestamp(path, default=None):
    try:
        stat = os.stat(path)
        return stat.st_mtime
    except OSError:
        if default is None:
            raise FreezeError('cannot stat {}'.format(path))
        return default

def get_timestamp_newest(path):
    ts_newest = 0
    for dirpath, dirnames, filenames in os.walk(path):
        for f in filenames:
            ts_newest = max(ts_newest, get_timestamp(os.path.join(dirpath, f)))
    return ts_newest

def mkdir(path):
    cur_path = ''
    for p in path.split('/')[:-1]:
        cur_path += p + '/'
        try:
            os.mkdir(cur_path)
        except OSError as er:
            if er.args[0] == 17: # file exists
                pass
            else:
                raise er

def freeze_internal(kind, path, script, opt):
    path = convert_path(path)
    if script is None and kind == KIND_AS_STR:
        if any(f[0] == KIND_AS_STR for f in manifest_list):
            raise FreezeError('can only freeze one str directory')
        manifest_list.append((KIND_AS_STR, path, script, opt))
    elif script is None:
        for dirpath, dirnames, filenames in os.walk(path):
            for f in filenames:
                freeze_internal(kind, path, (dirpath + '/' + f)[len(path) + 1:], opt)
    elif not isinstance(script, str):
        for s in script:
            freeze_internal(kind, path, s, opt)
    else:
        extension_kind = {KIND_AS_MPY: '.py', KIND_MPY: '.mpy'}
        if kind == KIND_AUTO:
            for k, ext in extension_kind.items():
                if script.endswith(ext):
                    kind = k
                    break
            else:
                raise FreezeError('unsupported file type {}'.format(script))
        wanted_extension = extension_kind[kind]
        if not script.endswith(wanted_extension):
            raise FreezeError('expecting a {} file, got {}'.format(wanted_extension, script))
        manifest_list.append((kind, path, script, opt))

def main():
    global TOP

    # Parse arguments
    assert sys.argv[1] == '-o'
    output_file = sys.argv[2]
    TOP = sys.argv[3]
    BUILD = sys.argv[4]
    mpy_cross_flags = sys.argv[5]
    input_manifest_list = sys.argv[6:]

    # Get paths to tools
    MAKE_FROZEN = TOP + '/tools/make-frozen.py'
    MPY_CROSS = TOP + '/mpy-cross/mpy-cross'
    MPY_TOOL = TOP + '/tools/mpy-tool.py'

    # Include top-level inputs, to generate the manifest
    for input_manifest in input_manifest_list:
        try:
            if input_manifest.endswith('.py'):
                include(input_manifest)
            else:
                exec(input_manifest)
        except FreezeError as er:
            print('freeze error executing "{}": {}'.format(input_manifest, er.args[0]))
            sys.exit(1)

    # Process the manifest
    str_paths = []
    mpy_files = []
    ts_newest = 0
    for kind, path, script, opt in manifest_list:
        if kind == KIND_AS_STR:
            str_paths.append(path)
            ts_outfile = get_timestamp_newest(path)
        elif kind == KIND_AS_MPY:
            infile = '{}/{}'.format(path, script)
            outfile = '{}/frozen_mpy/{}.mpy'.format(BUILD, script[:-3])
            ts_infile = get_timestamp(infile)
            ts_outfile = get_timestamp(outfile, 0)
            if ts_infile >= ts_outfile:
                print('MPY', script)
                mkdir(outfile)
                res, out = system([MPY_CROSS] + mpy_cross_flags.split() + ['-o', outfile, '-s', script, '-O{}'.format(opt), infile])
                if res != 0:
                    print('error compiling {}: {}'.format(infile, out))
                    raise SystemExit(1)
                ts_outfile = get_timestamp(outfile)
            mpy_files.append(outfile)
        else:
            assert kind == KIND_MPY
            infile = '{}/{}'.format(path, script)
            mpy_files.append(infile)
            ts_outfile = get_timestamp(infile)
        ts_newest = max(ts_newest, ts_outfile)

    # Check if output file needs generating
    if ts_newest < get_timestamp(output_file, 0):
        # No files are newer than output file so it does not need updating
        return

    # Freeze paths as strings
    _, output_str = system([MAKE_FROZEN] + str_paths)

    # Freeze .mpy files
    _, output_mpy = system([MPY_TOOL, '-f', '-q', BUILD + '/genhdr/qstrdefs.preprocessed.h'] + mpy_files)

    # Generate output
    print('GEN', output_file)
    mkdir(output_file)
    with open(output_file, 'wb') as f:
        f.write(b'//\n// Content for MICROPY_MODULE_FROZEN_STR\n//\n')
        f.write(output_str)
        f.write(b'//\n// Content for MICROPY_MODULE_FROZEN_MPY\n//\n')
        f.write(output_mpy)

if __name__ == '__main__':
    main()
