#
# @file main.py
#
# @section License
# Copyright (C) 2016, Erik Moqvist
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# This file is part of the Pumbaa project.
#

import os
import harness
from harness import assert_raises


def test_format():
    """Format the file system.

    """

    with assert_raises(OSError):
        os.format("apa")

    try:
        os.format("/fs")
    except:
        print("Failed to format /fs.")


def test_missing_file():
    """Try to open a non-existing file.

    """

    with assert_raises(OSError):
        open("missing.txt", "r")


def test_create():
    """Create, write and read. Recreate, write and read.

    """

    # Create, write and read.
    with open("create.txt", "w") as fout:
        fout.write("test")

    with open("create.txt", "rt") as fin:
        assert fin.read() == "test"

    # Recreate the file, write and read.
    with open("create.txt", "w") as fout:
        fout.write("foo")

    with open("create.txt", "r") as fin:
        assert fin.read() == "foo"


def test_append():
    """Test the append functionality.

    """

    with open("append.txt", "w") as fout:
        fout.write("foo")

    with open("append.txt", "a") as fout:
        fout.write("bar")

    with open("append.txt", "r") as fin:
        assert fin.read() == "foobar"


def test_read_write():
    """Test the read + write mode.

    """

    with open("rw.txt", "w+") as f:
        f.write("foo")
        f.seek(0)
        assert f.read() == "foo"

    with open("rw.txt", "r") as fin:
        assert fin.read() == "foo"

    with open("rw.txt", "r") as fin:
        with assert_raises(OSError):
            fin.write('foo')

    with open("rw.txt", "w") as fout:
        with assert_raises(OSError):
            fout.read()


def test_seek_tell():
    """Seek in a file.

    """

    with open("seek.txt", "w") as fout:
        fout.write("12345678")
        assert fout.tell() == 8

    with open("seek.txt", "r") as fin:
        # Offset relative to the beginning of the file.
        assert fin.seek(1) == 1
        assert fin.tell() == 1
        assert fin.read(1) == "2"
        assert fin.seek(1, 0) == 1
        assert fin.read(1) == "2"
        # Offset relative to the current cursor position.
        assert fin.seek(1, 1) == 3
        assert fin.read(1) == "4"
        # Offset relative to the end of the file.
        assert fin.seek(-1, 2) == 7
        assert fin.read(1) == "8"
        assert fin.tell() == 8
        # Seek outside of the file.
        with assert_raises(OSError):
            fin.seek(-1)
        with assert_raises(OSError):
            fin.seek(1, 2)


def test_stat():
    """Test to stat files.

    """

    print(os.stat("."))

    with open("stat.txt", "w") as fout:
        fout.write("12345678")

    assert os.stat("stat.txt") == (0, 0, 0, 0, 0, 0, 8, 0, 0, 0)

    try:
        os.stat("stat2.txt")
    except OSError as e:
        print(e)
    else:
        assert False


def test_listdir():
    assert os.listdir() == ['CREATE.TXT',
                            'APPEND.TXT',
                            'RW.TXT',
                            'SEEK.TXT',
                            'STAT.TXT']

    assert os.listdir('.') == ['CREATE.TXT',
                               'APPEND.TXT',
                               'RW.TXT',
                               'SEEK.TXT',
                               'STAT.TXT']

    try:
        os.listdir('non-existing')
    except OSError as e:
        assert str(e) == "No such file or directory: 'non-existing'"
    else:
        assert False

def test_print():
    with open("print.txt", "w") as fout:
        print(fout)
        fout.write("")
        print(fout)


def test_flush():
    """Flush a file.

    """

    with assert_raises(NotImplementedError, "file_obj_flush"):
        with open("flush.txt", "w") as fout:
            fout.write('')
            fout.flush()

def main():
    testcases = [
        (test_format, "test_format"),
        (test_missing_file, "test_missing_file"),
        (test_create, "test_create"),
        (test_append, "test_append"),
        (test_read_write, "test_read_write"),
        (test_seek_tell, "test_seek_tell"),
        (test_stat, "test_stat"),
        (test_listdir, "test_listdir"),
        (test_flush, "test_flush"),
        (test_print, "test_print")
    ]
    harness.run(testcases)


if __name__ == '__main__':
    main()
