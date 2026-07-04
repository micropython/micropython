:mod:`uefi.load_file` --- present files to the firmware
=======================================================

.. module:: uefi.load_file
    :synopsis: provide EFI_LOAD_FILE2_PROTOCOL files from memory

Most of the :mod:`uefi` package *consumes* firmware protocols.  This submodule is
the exception the boot path needs: it lets Python *provide* one.  It implements
``EFI_LOAD_FILE2_PROTOCOL`` (and, optionally, the v1 ``EFI_LOAD_FILE_PROTOCOL``)
so the firmware — or an operating system being booted — can load a file that is
not sitting on a filesystem.  The canonical use is handing an EFI-stub Linux
kernel its ``initramfs`` entirely from memory (for example, one just downloaded
over the network), without staging it to a disk.

The protocol is a single method, ``LoadFile(This, FilePath, BootPolicy, &Size,
Buffer)`` — a two-phase "how big?" / "copy it out" call.  It is backed by a native
shim (:func:`uefi.raw.load_file_install`), so the buffer is served from C when the
firmware loads the device path; **no Python runs inside the firmware's call**.  The
buffer is *borrowed*, not copied, so keep the :class:`FileProvider` (and the buffer
it wraps) alive until the firmware has finished with the file — in practice, across
the :meth:`Image.start` that boots the kernel.

.. class:: FileProvider(data, device_path, *, load_file2=True)

    Present *data* to the firmware as a loadable file at *device_path*.  Whenever
    the firmware/OS loads that device path, *data* is returned verbatim.

    *data* may be a :class:`uefi.buffer.PoolBuffer` (the :ref:`.ptr contract
    <uefi_ptr_contract>`) or any bytes-like object; it is borrowed, not copied.
    *device_path* is a :class:`uefi.device_path.DevicePath`.  *load_file2* selects
    ``EFI_LOAD_FILE2_PROTOCOL`` (``True``, the default — initrd and other non-boot
    loads) or the v1 ``EFI_LOAD_FILE_PROTOCOL`` (``False``).

    Raises :class:`uefi.Error` if installation fails.  :class:`FileProvider` is
    re-exported at the top level as :class:`uefi.FileProvider`.

    .. attribute:: handle

        The ``EFI_HANDLE`` the protocol was installed on (``0`` after
        :meth:`close`).

    .. attribute:: size

        The presented size in bytes.

    .. method:: close()

        Uninstall the protocol and release the handle.  Safe to call more than
        once.

    A :class:`FileProvider` is a context manager (``with``) and uninstalls itself
    in its finaliser (``__del__``).  Note that a kernel booted via the EFI stub
    calls ``ExitBootServices`` itself and never returns, so in a real boot the
    provider simply lives until the machine leaves boot services.

.. function:: present_initrd(data)

    Convenience wrapper: present *data* as the ``initramfs`` for an EFI-stub Linux
    kernel, i.e. ``FileProvider(data, linux_initrd_path())``.  Returns the
    :class:`FileProvider`; keep it alive until after you :meth:`Image.start` the
    kernel.

.. function:: linux_initrd_path()

    The :class:`uefi.device_path.DevicePath` the Linux EFI stub looks up (via
    ``LoadFile2``) to obtain its initrd — a vendor-media node carrying
    ``LINUX_EFI_INITRD_MEDIA_GUID``.

Example
-------

A minimal in-memory boot: download a kernel and initramfs, present the initrd, and
start the kernel (see ``ports/uefi/samples/net_boot.py`` for the full loader)::

    import uefi
    from uefi import load_file

    kernel = download(KERNEL_URL)   # a PoolBuffer
    initrd = download(INITRD_URL)   # a PoolBuffer

    provider = load_file.present_initrd(initrd)   # keep alive across start()

    st, handle = uefi.raw.load_image(
        False, uefi.raw.image_handle(), 0, kernel.ptr, kernel.size)
    uefi.status.check(st)
    image = uefi.Image(handle)
    image.set_load_options("console=ttyS0")
    image.start()                                 # -> Linux
