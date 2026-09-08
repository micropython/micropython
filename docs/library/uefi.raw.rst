:mod:`uefi.raw` --- low-level UEFI services
===========================================

.. module:: uefi.raw
    :synopsis: thin C wrappers around Boot/Runtime Services and the EFIAPI trampoline

The ``uefi.raw`` submodule is the low-level, C-native surface of the :mod:`uefi`
module.  It provides thin wrappers around Boot Services and Runtime Services
table calls, plus the :func:`call` trampoline that performs indirect EFIAPI calls
through a protocol's function pointers.

Every function returns a raw ``int`` :ref:`EFI_STATUS <uefi_status>` (or a tuple
whose first element is a status), and **none of them raise** -- several callers
treat statuses such as ``BUFFER_TOO_SMALL`` or ``NOT_FOUND`` as control flow.
Use :func:`uefi.status.check` to convert the error range into a raised
:class:`uefi.Error`.

Integer and pointer arguments coerce with the usual rules: Python ints pass
straight through, and any wrapper object exposing ``.ptr`` (see the
:ref:`.ptr contract <uefi_ptr_contract>`) is accepted where a pointer is
expected.

The trampoline
--------------

.. function:: call(fn_ptr, *args)

    Perform an indirect EFIAPI call through the function pointer *fn_ptr* with up
    to 16 integer/pointer arguments, returning the ``UINTN`` result (typically an
    ``EFI_STATUS``).

    This is the one operation that cannot be done in Python -- :mod:`uctypes`
    can overlay a struct on a pointer but cannot call a function pointer.  It is
    compiled for the firmware calling convention (``ms_abi`` on x86-64, AAPCS64
    on aarch64).  **Integer/pointer arguments only** (UEFI protocol methods never
    take floating-point or by-value-struct arguments).

    Higher-level code rarely calls this directly; the
    :class:`~uefi.protocols.ProtocolDescriptor` machinery generates method stubs
    that use it.

Table pointers
--------------

.. function:: system_table_ptr()
              boot_services_ptr()
              runtime_services_ptr()
              image_handle()

    Return, as integers, the ``EFI_SYSTEM_TABLE``, ``EFI_BOOT_SERVICES``,
    ``EFI_RUNTIME_SERVICES`` and the application's image handle, respectively.

.. function:: boot_services_active()

    Return ``True`` while Boot Services are usable (before
    ``ExitBootServices``).  All Boot Services calls become invalid once this
    returns ``False``.

Handle database
---------------

.. function:: locate_handle_buffer(search_type, guid_or_none)

    Return ``(status, [handle, ...])``.  *search_type* is ``0`` for all handles
    or ``2`` to search by protocol (in which case *guid_or_none* is the 16-byte
    packed GUID).  The firmware buffer is copied into a Python list and freed
    internally; the raw pointer is never exposed.

.. function:: locate_protocol(guid, registration=0)

    Return ``(status, iface_ptr)`` for an arbitrary instance of *guid*.

.. function:: open_protocol(handle, guid, agent, controller, attributes)

    Return ``(status, iface_ptr)``.

.. function:: close_protocol(handle, guid, agent, controller)

    Return a status.

.. function:: protocols_per_handle(handle)

    Return ``(status, [guid_bytes_16, ...])`` -- the protocols installed on
    *handle*.

.. function:: open_protocol_information(handle, guid)

    Return ``(status, [(agent, controller, attributes, open_count), ...])`` --
    who has *guid* open on *handle* and how.  The firmware entry buffer is freed
    internally.

.. function:: register_protocol_notify(guid, event)

    Return ``(status, registration_ptr)``.  Asks the firmware to signal *event*
    whenever an instance of *guid* is (re)installed -- the basis of hot-plug
    detection.

.. function:: locate_handle_notify(registration)

    Return ``(status, handle)`` for the next handle that arrived on
    *registration* (one at a time, as ``LocateHandle(ByRegisterNotify)``
    requires).

.. function:: install_protocol_interface(handle_or_0, guid, iface_ptr)

    Return ``(status, handle)``.

.. function:: uninstall_protocol_interface(handle, guid, iface_ptr)

    Return a status.

Images and drivers
------------------

.. function:: load_image(boot_policy, parent_handle, device_path_ptr, src_buffer_ptr, src_size)

    Load a PE/COFF image and return ``(status, image_handle)``.  Pass a source
    buffer (with *device_path_ptr* ``0``) for a memory-to-memory load, or a device
    path (with *src_buffer_ptr* and *src_size* ``0``) to load from a volume.
    ``LoadImage`` copies and relocates the source into fresh pages, so the source
    buffer need only live for the duration of the call.

.. function:: start_image(image_handle)

    Transfer control to a loaded image; return ``(status, exit_data_ptr,
    exit_data_size)`` where *status* is the image's exit code.  The exit-data
    buffer is a firmware pool allocation (the Python layer copies it out and frees
    it).

.. function:: unload_image(image_handle)

    Unload an image that was loaded but not (successfully) started; return a
    status.

.. function:: connect_controller(handle, driver_image=0, remaining_dp=0, recursive=False)

    Connect drivers to a controller *handle*; return a status.  *driver_image*
    ``0`` lets the firmware choose drivers, otherwise it is a single driver image
    handle.  *recursive* connects the whole sub-tree.

.. function:: disconnect_controller(handle, driver=0, child=0)

    Disconnect drivers from a controller *handle*; return a status.  *driver*
    ``0`` disconnects all drivers; *child* ``0`` disconnects all children of the
    selected driver.

.. function:: load_file_install(dp_ptr, dp_len, data_ptr, data_len, is_v2)

    Install an ``EFI_LOAD_FILE(2)_PROTOCOL`` provider on a fresh handle that serves
    the *data_len* bytes at *data_ptr* on demand at the device path *dp_ptr*.
    Return ``(status, handle, token)``; *token* is an opaque value for
    :func:`load_file_uninstall`.  *is_v2* selects ``EFI_LOAD_FILE2_PROTOCOL`` (the
    protocol an EFI-stub Linux uses to fetch its initrd).  The data buffer is
    **borrowed, not copied** -- keep it alive until uninstall.

.. function:: load_file_uninstall(token)

    Remove a provider installed by :func:`load_file_install` and free its record;
    return a status.

Memory
------

.. function:: allocate_pool(memory_type, size)

    Return ``(status, ptr)`` for a Boot Services pool allocation.

.. function:: free_pool(ptr)

    Free a pool allocation; return a status.

Variables
---------

.. function:: get_variable(name, guid)

    Return ``(status, attributes, data_bytes)``.  *name* is a Python ``str``
    (marshalled to ``CHAR16`` internally); *guid* is 16 packed bytes.  The
    two-phase size probe is handled internally.

.. function:: set_variable(name, guid, attributes, data)

    Create, replace or (with *data* of ``None``) delete a variable; return a
    status.

.. function:: get_next_variable_name(name, guid)

    Return ``(status, next_name, next_guid_bytes)`` -- one step of a variable
    enumeration walk.

Events and timers
-----------------

.. function:: set_timer(event, type, trigger_100ns)

    Arm or cancel a timer *event*.  *type* is ``0`` to cancel, ``1`` for
    periodic, ``2`` for relative (one-shot); *trigger_100ns* is the interval in
    100-nanosecond units.  Return a status.

    Event *creation* is done via the C :class:`uefi.Event` type, not a ``raw``
    function.

.. function:: wait_for_event(events)

    Block until one of the events in the *events* list is signalled; return
    ``(status, signalled_index)``.  Valid only at ``TPL_APPLICATION`` and **not**
    for ``EVT_NOTIFY_SIGNAL`` events -- poll those with the :class:`uefi.Event`
    type instead.

Time and monotonic count
------------------------

.. function:: get_time()

    Read the platform real-time clock; return ``(status, fields)`` where *fields*
    is ``(year, month, day, hour, minute, second, nanosecond, timezone,
    daylight)`` (or ``None`` on error).  *timezone* is signed minutes from UTC, or
    ``0x07FF`` for "unspecified/local".

.. function:: set_time(year, month, day, hour, minute, second, nanosecond=0, timezone=0x07FF, daylight=0)

    Set the real-time clock; return a status.  ``0x07FF`` is
    ``EFI_UNSPECIFIED_TIMEZONE`` (store as local time).

.. function:: get_next_monotonic_count()

    Return ``(status, count)`` from the platform's monotonic counter (never
    repeats, survives across calls) -- the shell's ``getmtc``.

Strings
-------

.. function:: wstr_at(ptr)

    Decode a NUL-terminated firmware ``CHAR16*`` (UTF-16LE) at address *ptr* into
    a Python ``str``.

Filesystems
-----------

.. function:: open_volume(fs_iface_ptr)

    Open the root directory of an ``EFI_SIMPLE_FILE_SYSTEM_PROTOCOL`` interface
    (address *fs_iface_ptr*) and return a **VFS object** ready to hand to
    ``os.mount()``.  This is the primitive behind :mod:`uefi.fs`, which mounts
    volumes other than the auto-mounted boot volume; use that higher-level module
    rather than calling this directly.

System and firmware tables
--------------------------

.. function:: reset_system(type, status=0, data=None)

    Reset the platform via ``RuntimeServices->ResetSystem``.  *type* is ``0`` cold,
    ``1`` warm, ``2`` shutdown, ``3`` platform-specific.  **Does not return** for
    cold/warm/shutdown.  (Prefer :func:`machine.reset` for an ordinary reboot.)

.. function:: set_watchdog_timer(timeout_s, code=0)

    Arm (or, with *timeout_s* ``0``, disable) the platform watchdog; return a
    status.  A boot manager disables the watchdog before handing off to an OS
    loader.

.. function:: get_memory_map()

    Return ``(status, map_key, [(type, phys_start, num_pages, attribute), ...])``
    -- the UEFI memory map.  The two-phase size probe is handled internally and
    the firmware buffer is freed before returning.

.. function:: configuration_tables()

    Return ``[(guid_bytes_16, table_ptr), ...]`` for the System Table's vendor
    configuration tables (ACPI RSDP, SMBIOS, device tree, ...), read directly from
    the system table (no Boot Services call).

Graphics
--------

.. function:: convert_rgb565(src_ptr, dst_ptr, count)

    Expand *count* pixels from an RGB565 buffer at address *src_ptr* into a 32-bit
    BGRx buffer at address *dst_ptr* (the pixel format ``Blt`` expects).  This is
    the fast inner loop behind :meth:`uefi.display.Display.show`; both pointers are
    integer addresses (for example from ``uctypes.addressof``).

.. note::

    Most programs never touch ``raw`` directly -- the higher-level :mod:`uefi`
    wrappers (and the standard :mod:`time`, :mod:`machine` and :mod:`os` modules)
    cover the common cases -- but every primitive above is available for cases a
    wrapper does not cover.  A few Boot Services helpers (``Stall``, TPL control)
    are surfaced through :mod:`time` and :mod:`machine` rather than here.
