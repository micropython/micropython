:mod:`uefi.fs` --- mounting other filesystem volumes
====================================================

.. module:: uefi.fs
    :synopsis: mount EFI_SIMPLE_FILE_SYSTEM volumes beyond the boot volume

The port auto-mounts the **boot volume** (the volume the application was launched
from) at ``/`` during start-up.  The ``uefi.fs`` submodule reaches *other*
``EFI_SIMPLE_FILE_SYSTEM_PROTOCOL`` volumes -- USB sticks, additional partitions,
the volumes the UEFI shell shows as ``FS0:``/``FS1:``/... -- by wrapping each as a
VFS and handing it to :func:`os.mount`.  Afterwards the standard :mod:`os`
functions and :func:`open` reach them by path.

This is the "extra VFS mounts" model: one file API (:mod:`os` / :func:`open`),
many volumes.

.. function:: volumes()

    Return the list of :class:`~uefi.handle.Handle` objects that expose
    ``EFI_SIMPLE_FILE_SYSTEM_PROTOCOL`` -- i.e. every mountable volume the firmware
    currently knows about.

.. function:: open_volume(handle)

    Open *handle*'s volume and return a VFS object.  Pass it to :func:`os.mount`
    to make the volume reachable by path (this is what :func:`mount` does for you).
    Raises :class:`uefi.Error` / ``OSError`` if the volume cannot be opened.

.. function:: mount(handle, point, readonly=False)

    Mount *handle*'s volume at *point* (for example ``'/usb'``) and return *point*.
    Afterwards ``os.listdir(point)``, ``open(point + '/f')`` and the rest work just
    as for the boot volume.  Unmount with :func:`umount`.

.. function:: umount(point)

    Unmount a volume previously mounted at *point* (a thin wrapper over
    :func:`os.umount`).

.. function:: mount_all(prefix="/vol")

    Mount every available volume at ``prefix0``, ``prefix1``, ... and return
    ``[(point, handle), ...]`` for the volumes that mounted.  Volumes that fail to
    open (unformatted, or already mounted) are skipped.  Handy to make all storage
    reachable at once; unmount each returned *point* when done.

Example
-------

::

    import os
    import uefi.fs as fs

    # Mount the first non-boot volume at /usb and read from it
    vols = fs.volumes()
    fs.mount(vols[0], '/usb')
    print(os.listdir('/usb'))
    fs.umount('/usb')

    # Or bring every volume online at once
    for point, handle in fs.mount_all():
        print(point, os.listdir(point))

.. note::

    Volumes are opened with ``GET_PROTOCOL`` (an untracked open), so mounting does
    not disturb any driver already using the volume.  The boot volume is already
    mounted at ``/`` and does not appear as something you need to mount yourself.
