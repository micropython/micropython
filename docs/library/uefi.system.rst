:mod:`uefi.system` --- platform-wide services
=============================================

.. module:: uefi.system
    :synopsis: reset, watchdog, memory map, firmware tables, RTC and firmware identity

The ``uefi.system`` submodule gathers the non-device, **whole-machine**
operations: platform reset and power-off, the boot watchdog, the UEFI memory map,
the firmware configuration tables (ACPI / SMBIOS / device tree), the real-time
clock, and System Table identity.  These are the services an EFI-shell user
reaches for with ``reset``, ``memmap``, ``acpiview`` / ``smbiosview``, ``time`` and
``getmtc``.

Reset and power
---------------

.. data:: COLD
          WARM
          SHUTDOWN
          PLATFORM_SPECIFIC

    ``EFI_RESET_TYPE`` values for :func:`reset`.

.. function:: reset(mode=COLD, status_code=0, data=None)

    Reset the platform (``ResetSystem``).  **Does not return** for ``COLD``,
    ``WARM`` or ``SHUTDOWN``.  *data* is an optional reset payload (for example a
    NUL-terminated string for a ``PLATFORM_SPECIFIC`` reset).

    .. note::

        This is a deliberate platform reset.  The port's *own* clean exit instead
        returns ``EFI_SUCCESS`` to its caller; use :func:`machine.reset` /
        :func:`machine.deepsleep` for the ordinary lifecycle, and ``reset`` only
        when you specifically want to reboot or power down the machine.

.. function:: shutdown()

    Power the platform off (``ResetSystem`` with ``EfiResetShutdown``).  Does not
    return.

.. function:: set_watchdog(seconds, code=0)

    Arm the boot watchdog for *seconds* (``0`` disables it).  A boot manager
    disables the watchdog before handing off to an OS loader.

Memory map
----------

.. class:: MemoryDescriptor

    One ``EFI_MEMORY_DESCRIPTOR``: a run of *num_pages* 4 KiB pages of a given
    type.

    .. attribute:: type

        The ``EFI_MEMORY_TYPE`` of the run.

    .. attribute:: physical_start

        Physical base address.

    .. attribute:: num_pages

        Number of ``PAGE_SIZE`` (4096-byte) pages.

    .. attribute:: attribute

        The ``EFI_MEMORY_ATTRIBUTE`` bitfield.

    .. attribute:: size

        ``num_pages * PAGE_SIZE`` in bytes (read-only property).

    .. attribute:: physical_end

        ``physical_start + size`` (read-only property).

.. function:: memory_map()

    Return ``(map_key, [MemoryDescriptor, ...])`` -- the current UEFI memory map.

Configuration tables
--------------------

.. function:: configuration_tables()

    Return ``[(GUID, table_ptr), ...]`` for every System Table configuration
    table.

.. function:: configuration_table(which)

    Return the pointer for one configuration table named by GUID *which*, or ``0``
    if it is not present.

.. function:: acpi_rsdp()

    Pointer to the ACPI 2.0+ RSDP (falling back to ACPI 1.0), or ``0``.

.. function:: smbios()

    Pointer to the SMBIOS 3 entry point (falling back to SMBIOS 2.x), or ``0``.

.. function:: device_tree()

    Pointer to the flattened device-tree (DTB) blob, or ``0`` if absent.

Real-time clock and monotonic count
-----------------------------------

.. class:: Time

    A wall-clock reading (``EFI_TIME``).  *timezone* is signed minutes from UTC or
    :data:`UNSPECIFIED_TIMEZONE`; *daylight* is a bitfield.  Attributes: ``year``,
    ``month``, ``day``, ``hour``, ``minute``, ``second``, ``nanosecond``,
    ``timezone``, ``daylight``.

.. data:: UNSPECIFIED_TIMEZONE

    The ``EFI_TIME.TimeZone`` sentinel (``0x07FF``): the clock is kept as local
    time with an unknown zone.

.. function:: get_time()

    Read the platform real-time clock, returning a :class:`Time`.

.. function:: set_time(year, month, day, hour, minute, second, nanosecond=0, timezone=UNSPECIFIED_TIMEZONE, daylight=0)

    Set the platform real-time clock (the shell's ``time`` / ``date``).

.. function:: monotonic_count()

    Return the next value of the platform monotonic counter (the shell's
    ``getmtc``) -- a value that never repeats and survives across calls.

Firmware identity
-----------------

.. function:: firmware_vendor()

    The firmware vendor string (for example ``'EDK II'``).

.. function:: firmware_revision()

    The firmware revision, a vendor-defined 32-bit value.

Example
-------

::

    import uefi.system as sys_

    print(sys_.firmware_vendor(), hex(sys_.firmware_revision()))
    print(sys_.get_time())

    key, mmap = sys_.memory_map()
    total = sum(d.size for d in mmap)
    print('memory map:', len(mmap), 'entries,', total >> 20, 'MiB described')

    rsdp = sys_.acpi_rsdp()
    print('ACPI RSDP at', hex(rsdp))
