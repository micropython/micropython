:mod:`uefi.driver` --- image loading and the driver model
=========================================================

.. module:: uefi.driver
    :synopsis: load/start/unload UEFI images and bind drivers to controllers

The ``uefi.driver`` submodule wraps the UEFI **image lifecycle** (load, start,
unload) and the **driver model** (connect/disconnect drivers to controllers).
:class:`Image` handles a loaded driver or application; :func:`connect` and
:func:`disconnect` drive controller binding, and :func:`driver_name` /
:func:`controller_name` read the human-readable names the shell's ``drivers`` and
``devtree`` show.

:class:`Image` is re-exported at the top level as :class:`uefi.Image`.

.. class:: Image(image_handle)

    A loaded UEFI image.  ``.ptr`` is its ``EFI_HANDLE`` (the
    :ref:`.ptr contract <uefi_ptr_contract>`).  Usually created via the class
    methods below rather than directly.

    .. classmethod:: load_from_file(path, *, boot_policy=False)

        Load an ``.efi`` from the VFS (a memory-to-memory load) without starting
        it.  Raises :class:`uefi.Error` on failure -- including
        ``EFI_SECURITY_VIOLATION`` under Secure Boot, so the caller can decide.

    .. classmethod:: load_from_device_path(dp, *, boot_policy=False)

        Load an image named by a :class:`~uefi.device_path.DevicePath` (the
        firmware reads it from its volume).

    .. method:: loaded_image()

        Open ``EFI_LOADED_IMAGE_PROTOCOL`` on this image and return a
        :class:`~uefi.protocols.ProtocolWrapper`.

    .. method:: set_load_options(data)

        Set the ``LoadOptions`` passed to the image at :meth:`start` (its "command
        line").  *data* may be ``bytes`` (used verbatim) or ``str`` (encoded
        UTF-16LE with a NUL terminator, the UEFI convention).  Call after load and
        before start; the backing buffer is held by the :class:`Image` so it
        outlives the call.

    .. method:: start()

        Run the image via ``StartImage``; return ``(exit_status, exit_data_bytes)``.
        For an application this runs to completion; for a driver it returns once
        the driver has registered its binding.  *exit_status* is the image's own
        exit code and is **not** raised (a non-``SUCCESS`` code can be a normal
        application result).

        .. note::

            The firmware unloads an application on ``Exit()``, so after
            :meth:`start` an application's handle may be invalid -- do not
            :meth:`unload` a started application.

    .. method:: unload()

        Unload the image (``UnloadImage``).  Surfaces ``EFI_UNSUPPORTED`` if the
        image has no unload entry point rather than swallowing it.

    :class:`Image` is also a context manager: on exit it best-effort unloads an
    image that was loaded but never started.

.. function:: connect(handle, *, driver=None, remaining_path=None, recursive=False)

    Connect one or more drivers to a controller *handle* (``ConnectController``).
    *driver* ``None`` lets the firmware choose applicable drivers; *remaining_path*
    is an optional :class:`~uefi.device_path.DevicePath` restricting how far down
    the bus to connect; *recursive* connects the whole sub-tree.
    ``EFI_NOT_FOUND`` (nothing to connect) is not treated as an error.

.. function:: disconnect(handle, *, driver=None, child=None)

    Disconnect drivers from a controller *handle* (``DisconnectController``).
    *driver* ``None`` disconnects all drivers; *child* ``None`` disconnects all
    children of the selected driver.

.. function:: driver_name(handle, language="en")

    The human-readable name of the driver on *handle* (via ``ComponentName2``), or
    ``None`` if it provides no ``ComponentName2`` or does not support *language*.

.. function:: controller_name(driver_handle, controller, child=None, language="en")

    The driver's name for *controller* (and optional *child*), or ``None``.

Example
-------

::

    from uefi.driver import Image

    # Load and run a UEFI application, passing it a command line
    with Image.load_from_file('/tools/memtest.efi') as img:
        img.set_load_options('--quick')
        exit_status, exit_data = img.start()
        print('exited with', hex(exit_status))

.. seealso::

    :mod:`uefi.boot` builds on :class:`Image` to implement a boot manager
    (``BootOrder`` / ``Boot####``), and :func:`uefi.raw.load_image` /
    :func:`uefi.raw.start_image` are the underlying primitives.
