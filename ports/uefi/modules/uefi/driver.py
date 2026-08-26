# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.driver — image loading and controller binding.
#
# Image wraps the load/start/unload lifecycle of a UEFI image (a driver or an
# application); connect()/disconnect() drive the UEFI driver model, binding
# drivers to controllers (or tearing the binding down).
#
# Pitfalls (see README.md):
#   * A *driver* image's start() returns immediately (it just registers its
#     Driver Binding); an *application* image's start() runs to completion and the
#     firmware unloads it on Exit — don't unload() a started application.
#   * unload() may return EFI_UNSUPPORTED if the image has no Unload entry point;
#     that is surfaced, not swallowed.
#   * load may return EFI_SECURITY_VIOLATION under Secure Boot — surfaced as
#     uefi.Error so the caller can decide.

import uctypes

from . import raw
from . import status
from . import utf16
from . import coerce
from . import device_path as _dp
from .buffer import PoolBuffer, LOADER_DATA
from .handle import Handle
from .protocols import LOADED_IMAGE


class Image:
    """A loaded UEFI image (driver or application). `.ptr` is its EFI_HANDLE."""

    def __init__(self, image_handle):
        self.ptr = int(image_handle)
        self._loaded = True
        self._options = None  # PoolBuffer backing LoadOptions, kept alive here

    def __int__(self):
        # The `.ptr` contract in integer form (see uefi.handle.Handle).
        return self.ptr

    @classmethod
    def load_from_file(cls, path, *, boot_policy=False):
        """Load an .efi from the VFS (a memory-to-memory load), without starting it."""
        with open(path, "rb") as f:
            data = f.read()
        # LoadImage copies/relocates `data` into fresh pages, so it need only live
        # for the call; a named local keeps it pinned across it.
        st, img = raw.load_image(
            boot_policy, raw.image_handle(), 0, uctypes.addressof(data), len(data)
        )
        status.check(st)
        return cls(img)

    @classmethod
    def load_from_device_path(cls, dp, *, boot_policy=False):
        """Load an image named by a device path (the firmware reads it off its volume).

        `dp` is any device-path spelling: a DevicePath, its text form, packed
        bytes, or a raw EFI_DEVICE_PATH_PROTOCOL pointer.
        """
        packed = _dp.as_device_path(dp).to_bytes()  # named local: outlives the call
        st, img = raw.load_image(boot_policy, raw.image_handle(), uctypes.addressof(packed), 0, 0)
        status.check(st)
        return cls(img)

    def loaded_image(self):
        """Open EFI_LOADED_IMAGE_PROTOCOL on this image (a ProtocolWrapper)."""
        return LOADED_IMAGE.open(Handle(self.ptr))

    def set_load_options(self, data):
        """Set the LoadOptions passed to the image at start() (its 'command line').

        `data` may be a str (encoded UTF-16LE with a NUL terminator, the UEFI
        convention for a textual option string) or any byte block, used verbatim.
        The backing buffer is a pool allocation held by this Image so it outlives
        start(). Call after load, before start().
        """
        if isinstance(data, str):
            data = utf16.encode(data)
        else:
            data = coerce.as_buffer(data)
        self._options = PoolBuffer.from_bytes(data, memory_type=LOADER_DATA)
        li = self.loaded_image()
        li.load_options = self._options.ptr
        li.load_options_size = len(data)
        li.close()

    def start(self):
        """Run the image via StartImage; return (exit_status, exit_data_bytes).

        For an application this runs to completion; for a driver it returns once
        the driver has registered its binding. exit_status is the image's own exit
        code (not raised — a non-SUCCESS code can be a normal application result).
        """
        st, data_ptr, data_size = raw.start_image(self.ptr)
        # The firmware unloads an application on Exit(); after start() its handle
        # may be invalid, so don't auto-unload it later.
        self._loaded = False
        exit_data = b""
        if data_ptr and data_size:
            exit_data = uctypes.bytes_at(data_ptr, data_size)
        if data_ptr:
            raw.free_pool(data_ptr)
        return st, exit_data

    def unload(self):
        """Unload the image (UnloadImage). Surfaces EFI_UNSUPPORTED if unsupported."""
        if self._loaded and self.ptr:
            status.check(raw.unload_image(self.ptr))
            self._loaded = False

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        # Best-effort cleanup: an image that was start()ed (an application) has
        # already been unloaded by the firmware, so _loaded is False and we skip it.
        if self._loaded:
            try:
                self.unload()
            except status.Error:
                pass

    def __repr__(self):
        return "Image(ptr=0x%x, loaded=%s)" % (self.ptr, self._loaded)


def connect(handle, *, driver=None, remaining_path=None, recursive=False):
    """Connect one or more drivers to a controller (ConnectController).

    `handle` and `driver` are any handle spelling (a Handle, a bare EFI_HANDLE
    int, an Image); driver=None lets the firmware choose applicable drivers.
    remaining_path is an optional device path — in any spelling — restricting how
    far down the bus to connect; recursive connects the whole subtree.
    EFI_NOT_FOUND (nothing to connect) is not an error.
    """
    d = coerce.as_ptr(driver)
    # A named local: the packed bytes must stay referenced for the whole call, or
    # the address we hand to the firmware can be collected out from under it.
    packed = _dp.as_device_path(remaining_path).to_bytes() if remaining_path is not None else None
    rp = uctypes.addressof(packed) if packed is not None else 0
    st = raw.connect_controller(coerce.as_ptr(handle), d, rp, bool(recursive))
    if st != status.NOT_FOUND:
        status.check(st)


def driver_name(handle, language="en"):
    """The human-readable name of the driver on `handle` (via ComponentName2), or None.

    This is what the shell's `drivers`/`devtree` show. Returns None if the driver
    provides no ComponentName2 or doesn't support `language`.
    """
    from .protocols import COMPONENT_NAME2

    try:
        cn = COMPONENT_NAME2.open(handle)
    except status.Error:
        return None
    try:
        st, ptr = cn.get_driver_name(language)
        if st != status.SUCCESS or not ptr:
            return None
        return raw.wstr_at(ptr)
    finally:
        cn.close()


def controller_name(driver_handle, controller, child=None, language="en"):
    """The driver's name for `controller` (and optional `child`), or None."""
    from .protocols import COMPONENT_NAME2

    try:
        cn = COMPONENT_NAME2.open(driver_handle)
    except status.Error:
        return None
    try:
        st, ptr = cn.get_controller_name(coerce.as_ptr(controller), coerce.as_ptr(child), language)
        if st != status.SUCCESS or not ptr:
            return None
        return raw.wstr_at(ptr)
    finally:
        cn.close()


def disconnect(handle, *, driver=None, child=None):
    """Disconnect drivers from a controller (DisconnectController).

    Handles take any spelling (see connect). driver=None disconnects all drivers;
    child=None disconnects all children of the selected driver.
    """
    status.check(
        raw.disconnect_controller(
            coerce.as_ptr(handle), coerce.as_ptr(driver), coerce.as_ptr(child)
        )
    )
