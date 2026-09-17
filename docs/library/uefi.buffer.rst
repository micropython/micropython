:mod:`uefi.buffer` --- pool-backed memory
=========================================

.. module:: uefi.buffer
    :synopsis: UEFI-allocator-backed memory buffers

The ``uefi.buffer`` submodule provides :class:`PoolBuffer`, memory allocated from
the UEFI pool allocator (``BootServices->AllocatePool`` / ``FreePool``).

Use a :class:`PoolBuffer` when memory must:

* survive Python garbage collection without being pinned,
* be handed to firmware that retains the pointer past the call, or
* back a two-phase size-probe.

Because it exposes ``.ptr`` (see the :ref:`.ptr contract <uefi_ptr_contract>`), a
:class:`PoolBuffer` can be passed straight to :mod:`uefi.raw` calls where a
pointer is expected.  :class:`PoolBuffer` is re-exported at the top level as
:class:`uefi.PoolBuffer`.

.. class:: PoolBuffer(size, memory_type=BOOT_SERVICES_DATA)

    Allocate *size* bytes from the UEFI pool.  *memory_type* is an
    ``EFI_MEMORY_TYPE`` value; the module defines :data:`LOADER_DATA` and
    :data:`BOOT_SERVICES_DATA`.  Raises :class:`uefi.Error` if the allocation
    fails.

    .. attribute:: ptr

        The allocated address as an integer (``0`` after :meth:`close`).

    .. attribute:: size

        The allocation size in bytes (``0`` after :meth:`close`).

    .. method:: close()

        Free the allocation.  Safe to call more than once.

    A :class:`PoolBuffer` is a context manager (``with``) and frees itself in its
    finaliser (``__del__``), so the allocation is released deterministically on
    block exit or, as a backstop, at collection.

Constants
---------

.. data:: LOADER_DATA

    ``EfiLoaderData`` memory type.

.. data:: BOOT_SERVICES_DATA

    ``EfiBootServicesData`` memory type (the default).

Example
-------

::

    from uefi.buffer import PoolBuffer

    with PoolBuffer(256) as buf:
        st, iface = uefi.raw.open_protocol(h, g.bytes, agent, 0, attrs)
        # buf.ptr can be passed to a raw call that fills it
        ...
    # buffer freed here
