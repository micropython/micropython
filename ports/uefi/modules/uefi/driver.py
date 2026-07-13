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
from .buffer import PoolBuffer, LOADER_DATA
from .handle import Handle
from .protocols import LOADED_IMAGE


class Image:
    """A loaded UEFI image (driver or application). `.ptr` is its EFI_HANDLE."""

    def __init__(self, image_handle):
        self.ptr = int(image_handle)
        self._loaded = True
        self._options = None  # PoolBuffer backing LoadOptions, kept alive here

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
        """Load an image named by a DevicePath (the firmware reads it off its volume)."""
        packed = dp.to_bytes()  # named local: must outlive the LoadImage call
        st, img = raw.load_image(boot_policy, raw.image_handle(), uctypes.addressof(packed), 0, 0)
        status.check(st)
        return cls(img)

    def loaded_image(self):
        """Open EFI_LOADED_IMAGE_PROTOCOL on this image (a ProtocolWrapper)."""
        return LOADED_IMAGE.open(Handle(self.ptr))

    def set_load_options(self, data):
        """Set the LoadOptions passed to the image at start() (its 'command line').

        `data` may be bytes (used verbatim) or a str (encoded UTF-16LE with a NUL
        terminator, the UEFI convention for a textual option string). The backing
        buffer is a pool allocation held by this Image so it outlives start().
        Call after load, before start().
        """
        if isinstance(data, str):
            data = utf16.encode(data)
        else:
            data = bytes(data)
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

    driver=None lets the firmware choose applicable drivers; remaining_path is an
    optional DevicePath restricting how far down the bus to connect; recursive
    connects the whole subtree. EFI_NOT_FOUND (nothing to connect) is not an error.
    """
    d = int(driver.ptr) if isinstance(driver, Handle) else int(driver or 0)
    rp = uctypes.addressof(remaining_path.to_bytes()) if remaining_path is not None else 0
    st = raw.connect_controller(int(handle.ptr), d, rp, bool(recursive))
    if st != status.NOT_FOUND:
        status.check(st)


def driver_name(handle, language="en"):
    """The human-readable name of the driver on `handle` (via ComponentName2), or None.

    This is what the shell's `drivers`/`devtree` show. Returns None if the driver
    provides no ComponentName2 or doesn't support `language`.
    """
    from .protocols import COMPONENT_NAME2

    try:
        cn = COMPONENT_NAME2.open(Handle(handle) if not isinstance(handle, Handle) else handle)
    except status.Error:
        return None
    try:
        out = bytearray(8)
        if cn.get_driver_name(language, out) != status.SUCCESS:
            return None
        ptr = int.from_bytes(out, "little")
        return raw.wstr_at(ptr) if ptr else None
    finally:
        cn.close()


def controller_name(driver_handle, controller, child=None, language="en"):
    """The driver's name for `controller` (and optional `child`), or None."""
    from .protocols import COMPONENT_NAME2

    dh = driver_handle if isinstance(driver_handle, Handle) else Handle(driver_handle)
    try:
        cn = COMPONENT_NAME2.open(dh)
    except status.Error:
        return None
    try:
        out = bytearray(8)
        c = controller.ptr if isinstance(controller, Handle) else int(controller)
        ch = child.ptr if isinstance(child, Handle) else int(child or 0)
        if cn.get_controller_name(c, ch, language, out) != status.SUCCESS:
            return None
        ptr = int.from_bytes(out, "little")
        return raw.wstr_at(ptr) if ptr else None
    finally:
        cn.close()


def disconnect(handle, *, driver=None, child=None):
    """Disconnect drivers from a controller (DisconnectController).

    driver=None disconnects all drivers; child=None disconnects all children of
    the selected driver.
    """
    d = int(driver.ptr) if isinstance(driver, Handle) else int(driver or 0)
    c = int(child.ptr) if isinstance(child, Handle) else int(child or 0)
    status.check(raw.disconnect_controller(int(handle.ptr), d, c))
