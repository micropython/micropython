:mod:`uefi.buffer` --- pool-backed memory
=========================================

.. module:: uefi.buffer
    :synopsis: UEFI-allocator-backed memory buffers

The ``uefi.buffer`` submodule provides :class:`PoolBuffer`, memory allocated from
the UEFI pool allocator (``BootServices->AllocatePool`` / ``FreePool``), and
:class:`PageBuffer`, whole pages from ``AllocatePages`` / ``FreePages``.

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

.. class:: PageBuffer(size, memory_type=BOOT_SERVICES_DATA, max_address=None, address=None)

    Allocate *size* bytes, rounded up to whole :data:`PAGE_SIZE` pages, so the
    buffer is always page-aligned.  By default the pages may be anywhere;
    *max_address* keeps the whole buffer at or below that address (for example
    ``0xFFFF_FFFF`` for a device limited to 32-bit DMA), and *address* asks for
    exactly that physical address.  Giving both is a :exc:`ValueError`.  Raises
    :class:`uefi.Error` if the firmware cannot satisfy the request.

    It behaves like a :class:`PoolBuffer` (``ptr``, ``size``, ``close()``, item
    access, ``write``/``bytes``, ``from_bytes``, ``with``), and adds:

    .. attribute:: pages

        The number of pages allocated.

.. note::

    Memory handed to a device -- a DMA ring, a descriptor, a buffer the device
    writes into -- must come from :class:`PoolBuffer`, :class:`PageBuffer` or
    ``PCI_IO.allocate_buffer``, never a Python ``bytes`` or ``bytearray``: the
    garbage collector can move or reuse those while the device still holds the
    address.  For real bus-master DMA prefer ``PCI_IO.allocate_buffer`` followed
    by ``PCI_IO.map``, which apply any IOMMU or bounce-buffer translation and give
    you the *device* address; a :class:`PageBuffer`'s ``ptr`` is the CPU address.

Constants
---------

.. data:: PAGE_SIZE

    The UEFI page size, 4096 bytes.

.. data:: ALLOCATE_ANY_PAGES
          ALLOCATE_MAX_ADDRESS
          ALLOCATE_ADDRESS

    ``EFI_ALLOCATE_TYPE`` values, for :func:`uefi.raw.allocate_pages`.

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
