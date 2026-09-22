.. _uefi_introspection:

Firmware introspection with the ``uefi`` module
===============================================

The port-specific :mod:`uefi` module is what makes this port more than "Python in
firmware": it exposes the UEFI protocol/handle database, Boot and Runtime
Services, NVRAM variables, events and timers, and device paths as Pythonic
objects.  This chapter is a tour; see :mod:`uefi` for the full reference.

The handle database
-------------------

Every device and service in UEFI is a *handle* carrying one or more *protocols*
(identified by GUID).  This is the same information the UEFI shell's ``dh``
command shows::

    import uefi

    for h in uefi.handle.all_handles():
        names = [uefi.guid.name_for(g) or str(g) for g in h.protocols()]
        print(h, names)

To find handles that support a particular protocol::

    from uefi import guid
    disks = uefi.handle.locate_handles(guid.BLOCK_IO_PROTOCOL)

Opening a protocol
------------------

The pre-baked descriptors in :mod:`uefi.protocols` wrap common protocols so their
fields and methods are directly usable from Python.  For example, reading the
graphics mode via the Graphics Output Protocol::

    from uefi.protocols import GOP, GOP_MODE, GOP_INFO

    g = GOP.locate()                          # first GOP instance
    mode = g.struct_at('mode', GOP_MODE)      # follow the `mode` pointer
    info = mode.struct_at('info', GOP_INFO)   # ... and its `info`
    print(info.horizontal_resolution, 'x', info.vertical_resolution)

Field access goes through a ``uctypes`` overlay; method calls (like ``set_mode``
or ``blt``) are dispatched through the low-level trampoline for you, and non-zero
statuses are raised as :class:`uefi.Error`.

NVRAM variables
---------------

Reading and writing firmware variables::

    from uefi import variable, guid

    # A well-known variable in the global namespace
    data = variable.get('BootOrder', guid.GLOBAL_VARIABLE)

    # A private, *volatile* variable (no flash wear)
    ns = uefi.GUID('11111111-2222-3333-4444-555555555555')
    variable.set('scratch', ns, b'hello',
                 attributes=variable.BOOTSERVICE_ACCESS)
    print(variable.get('scratch', ns))
    variable.delete('scratch', ns)

.. warning::

   ``variable.set`` on a non-volatile variable writes SPI flash.  Never do it in
   a loop.  See :mod:`uefi.variable` for the attribute flags and the Variable
   Policy interface.

Events, timers and hot-plug
---------------------------

:class:`uefi.Event` is a pollable UEFI event that integrates with
:mod:`select`/:mod:`asyncio`.  :class:`uefi.event.Timer` is a UEFI timer with an
awaitable interface, and :class:`uefi.event.ProtocolWatch` fires whenever a new
instance of a protocol is installed -- i.e. when a device of that class is
hot-plugged::

    import asyncio
    from uefi import guid
    from uefi.event import ProtocolWatch

    async def watch_disks():
        watch = ProtocolWatch(guid.BLOCK_IO_PROTOCOL)
        async for h in watch:
            print('new block device:', h)

    asyncio.run(watch_disks())

Where to go next
----------------

* :mod:`uefi` -- module overview and the two-layer design.
* :mod:`uefi.handle` -- handles, protocols and descriptors.
* :mod:`uefi.raw` -- the low-level Boot/Runtime Services surface, for when you
  need a protocol that isn't pre-baked.
