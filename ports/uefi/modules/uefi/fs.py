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

# uefi.fs — mount filesystems other than the boot volume.
#
# The port mounts the boot volume at "/" at startup. This module reaches *other*
# EFI_SIMPLE_FILE_SYSTEM volumes (USB sticks, other partitions — the shell's
# `map`/`FSx:`) by wrapping each as a VFS and handing it to os.mount, so `os` and
# `open()` reach them by path (e.g. "/vol1/..."). This is the "extra VFS mounts"
# model: one file API (os/open), many volumes.

import os

from . import raw
from . import status
from . import guid
from . import handle as _handle


def volumes():
    """Every handle exposing EFI_SIMPLE_FILE_SYSTEM_PROTOCOL (a mountable volume)."""
    return _handle.locate_handles(guid.SIMPLE_FILE_SYSTEM_PROTOCOL)


def open_volume(handle):
    """Open `handle`'s volume and return a VFS object (mount it with os.mount)."""
    ph = handle.open(guid.SIMPLE_FILE_SYSTEM_PROTOCOL)  # GET_PROTOCOL (untracked)
    try:
        return raw.open_volume(ph.ptr)  # OpenVolume → VfsEfi
    finally:
        ph.close()


def mount(handle, point, readonly=False):
    """Mount `handle`'s volume at `point` (e.g. '/usb'); returns `point`.

    Afterwards os.listdir(point), open(point + '/f'), etc. work as for the boot
    volume. Unmount with uefi.fs.umount(point).
    """
    os.mount(open_volume(handle), point, readonly=readonly)
    return point


def umount(point):
    """Unmount a volume previously mounted at `point`."""
    os.umount(point)


def mount_all(prefix="/vol"):
    """Mount every volume at prefix0, prefix1, … ; return [(point, handle), ...].

    Volumes that fail to open (unformatted/again) are skipped. Handy to make all
    storage reachable at once; unmount each returned point when done.
    """
    out = []
    for i, h in enumerate(volumes()):
        point = prefix + str(i)
        try:
            mount(h, point)
            out.append((point, h))
        except (OSError, status.Error):
            pass
    return out
