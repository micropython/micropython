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

# uefi.load_file — present in-memory files to the firmware/OS.
#
# Most of the uefi package *consumes* firmware protocols. This module is the
# exception the boot path needs: it lets Python *provide* EFI_LOAD_FILE2_PROTOCOL
# (and the v1 EFI_LOAD_FILE_PROTOCOL) — the mechanism by which an EFI-stub OS pulls
# a file that isn't sitting on a filesystem. The canonical use is handing a Linux
# kernel its initramfs from a buffer you downloaded over the network.
#
# The protocol is a single method, LoadFile(This, FilePath, BootPolicy, &Size,
# Buffer): a two-phase "how big?" / "here it is" copy-out. We back it with a fixed
# buffer served by a native shim (uefi.raw.load_file_install), so there is no
# Python callback running inside the firmware's call — you register bytes, the
# firmware copies them out when it loads the device path. Keep the FileProvider
# (and its buffer) alive until the firmware is finished, i.e. across the
# StartImage() that boots the kernel.

import uctypes

from . import raw
from . import status
from .device_path import DevicePath

# LINUX_EFI_INITRD_MEDIA_GUID, packed (mixed-endian EFI_GUID) — the vendor-media
# device path the Linux EFI stub looks up (via LoadFile2) to fetch its initramfs.
_LINUX_INITRD_GUID = b"\x27\xe4\x68\x55\xfc\x68\x3d\x4f\xac\x74\xca\x55\x52\x31\xcc\x68"


def linux_initrd_path():
    """The device path an EFI-stub Linux kernel loads to obtain its initrd."""
    # MEDIA_DEVICE_PATH(0x04) / MEDIA_VENDOR_DP(0x03), length 20, then the End node.
    node = b"\x04\x03\x14\x00" + _LINUX_INITRD_GUID
    return DevicePath.from_bytes(node + b"\x7f\xff\x04\x00")


def _addr_len(data):
    # Accept a PoolBuffer (the .ptr/.size contract) or any bytes-like buffer.
    if hasattr(data, "ptr"):
        return int(data.ptr), int(data.size)
    return uctypes.addressof(data), len(data)


class FileProvider:
    """Presents a byte buffer to the firmware as a loadable file at a device path.

    Whenever the firmware/OS loads `device_path`, the buffer is returned verbatim.
    `data` may be a PoolBuffer or any bytes-like object; it is borrowed, not copied,
    so it (and this FileProvider) must stay alive until the firmware has fetched the
    file. `load_file2=True` installs EFI_LOAD_FILE2_PROTOCOL (initrd and other
    non-boot loads); `False` installs the v1 EFI_LOAD_FILE_PROTOCOL.
    """

    def __init__(self, data, device_path, *, load_file2=True):
        self._data = data  # keep the source alive (borrowed by C)
        addr, size = _addr_len(data)
        dp = device_path.to_bytes()  # named local: outlive the install call
        st, handle, token = raw.load_file_install(
            uctypes.addressof(dp), len(dp), addr, size, bool(load_file2)
        )
        status.check(st)
        self.handle = handle
        self.size = size
        self.load_file2 = bool(load_file2)
        self._token = token

    def close(self):
        """Uninstall the protocol and release the handle."""
        if self._token:
            status.check(raw.load_file_uninstall(self._token))
            self._token = 0
            self.handle = 0

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    def __del__(self):
        try:
            self.close()
        except Exception:
            pass

    def __repr__(self):
        return "FileProvider(handle=0x%x, size=%d, v2=%s)" % (
            self.handle or 0,
            self.size,
            self.load_file2,
        )


def present_initrd(data):
    """Present `data` as the initramfs for an EFI-stub Linux kernel.

    Returns a FileProvider; keep it alive until after you StartImage the kernel.
    Equivalent to FileProvider(data, linux_initrd_path()).
    """
    return FileProvider(data, linux_initrd_path())
