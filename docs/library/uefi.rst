:mod:`uefi` --- access to the UEFI firmware environment
=======================================================

.. module:: uefi
    :synopsis: access to UEFI Boot/Runtime Services, protocols, variables and events

.. note::

    ``uefi`` is specific to the :ref:`UEFI port <uefi_general>`.

The ``uefi`` module gives MicroPython first-class access to the UEFI firmware it
runs inside: the protocol/handle database, Boot Services and Runtime Services,
NVRAM variables, events and timers, and device paths.  It turns the ad-hoc work
usually done with UEFI-shell scripts into a programmable, resource-safe Python
API.

Two layers
----------

The module is structured in two layers:

* :mod:`uefi.raw` -- a **low-level, C-native** surface.  Thin, policy-free
  wrappers around Boot/Runtime Services table calls, plus a **call-through
  trampoline** (``uefi.raw.call``) that invokes a protocol's function pointers
  with the firmware calling convention (EFIAPI).  Functions return raw ``int``
  status values and **never raise**.
* :mod:`uefi` (this module and its Python submodules) -- a **descriptor-driven,
  resource-safe** layer built on ``raw``.  It uses :mod:`uctypes` to overlay
  struct layouts on interface pointers for field access, routes method dispatch
  through the ``raw`` trampoline, and presents protocols, handles, events and
  variables as Pythonic objects with lifecycle management.  Errors become raised
  :class:`uefi.Error` exceptions.

Why the low level is C, not :mod:`uctypes`: MicroPython's :mod:`uctypes` does
struct *layout / memory overlay only* -- it can read and write fields of a struct
overlaid on a pointer, but it has **no way to call a function pointer**.  Every
UEFI protocol method is an indirect call through a function pointer using the
EFIAPI convention (``ms_abi`` on x86-64, AAPCS64 on aarch64), so method
invocation must go through the C ``uefi.raw.call`` trampoline.

Submodules
----------

.. toctree::
    :maxdepth: 1

    uefi.raw.rst
    uefi.guid.rst
    uefi.status.rst
    uefi.buffer.rst
    uefi.handle.rst
    uefi.variable.rst
    uefi.event.rst
    uefi.device_path.rst
    uefi.fs.rst
    uefi.load_file.rst
    uefi.driver.rst
    uefi.boot.rst
    uefi.system.rst
    uefi.console.rst
    uefi.protocols.rst
    uefi.display.rst

Top-level names
---------------

For convenience the most commonly used types are re-exported at the top level of
the ``uefi`` package (verbs stay in their submodules):

.. class:: GUID

    The :class:`uefi.guid.GUID` type.  See :mod:`uefi.guid`.

.. class:: Error

    The :class:`uefi.status.Error` exception (a subclass of ``OSError``).  See
    :mod:`uefi.status`.

.. class:: PoolBuffer

    The :class:`uefi.buffer.PoolBuffer` type.  See :mod:`uefi.buffer`.

.. class:: Handle

    The :class:`uefi.handle.Handle` type.  See :mod:`uefi.handle`.

.. class:: ProtocolHandle

    The :class:`uefi.handle.ProtocolHandle` type.  See :mod:`uefi.handle`.

.. class:: ProtocolDescriptor

    The descriptor type used to declare a protocol interface's layout.  See
    :mod:`uefi.protocols`.

.. class:: ProtocolWrapper

    A live overlay of a :class:`ProtocolDescriptor` on an interface pointer.  See
    :mod:`uefi.protocols`.

.. class:: Event

    A pollable UEFI event.  This is a C type (from the built-in ``_uefi``
    module).  See :mod:`uefi.event`.

.. class:: Timer

    The :class:`uefi.event.Timer` type.  See :mod:`uefi.event`.

.. class:: ProtocolWatch

    The :class:`uefi.event.ProtocolWatch` type (device hot-plug).  See
    :mod:`uefi.event`.

.. class:: VariablePolicy

    The :class:`uefi.variable.VariablePolicy` type.  See :mod:`uefi.variable`.

.. class:: DevicePath
           DevicePathNode

    The device-path types.  See :mod:`uefi.device_path`.

.. class:: Image

    The :class:`uefi.driver.Image` type -- a loaded UEFI image (driver or
    application).  See :mod:`uefi.driver`.

.. class:: LoadOption

    The :class:`uefi.boot.LoadOption` type -- an ``EFI_LOAD_OPTION`` boot entry.
    See :mod:`uefi.boot`.

.. class:: Console

    The :class:`uefi.console.Console` type -- text console (colour/cursor/keys).
    See :mod:`uefi.console`.

.. class:: FileProvider

    The :class:`uefi.load_file.FileProvider` type -- present an in-memory file to
    the firmware via ``EFI_LOAD_FILE2_PROTOCOL`` (e.g. a kernel's initrd).  See
    :mod:`uefi.load_file`.

.. class:: Display

    The :class:`uefi.display.Display` type -- the GOP display as a framebuffer.
    See :mod:`uefi.display`.

.. _uefi_ptr_contract:

The ``.ptr`` contract
---------------------

Any object that can be handed to a ``uefi.raw`` call exposes its firmware token
as an integer attribute ``.ptr``.  This is uniform across :class:`PoolBuffer`,
:class:`Handle`, :class:`Event`, :class:`ProtocolHandle` and
:class:`ProtocolWrapper` (whose ``.ptr`` is the interface pointer used for method
dispatch).  Because of this, wrapper objects can be passed directly as pointer
arguments to ``raw`` functions.

Initialisation
--------------

No explicit initialisation step is required.  The C layer reads the live UEFI
tables directly from the port's start-up globals, exposing them via
:func:`uefi.raw.system_table_ptr`, :func:`uefi.raw.boot_services_ptr`,
:func:`uefi.raw.runtime_services_ptr` and :func:`uefi.raw.image_handle`.

All Boot Services calls become invalid after ``ExitBootServices``;
:func:`uefi.raw.boot_services_active` gates them (the port is a
pre-``ExitBootServices`` application, so it is currently always true).

Example
-------

::

    import uefi
    from uefi import guid

    # Enumerate the whole handle database, labelling known protocols
    for h in uefi.handle.all_handles():
        names = [guid.name_for(g) or str(g) for g in h.protocols()]
        print(h, names)

    # Open a well-known protocol via a pre-baked descriptor
    from uefi.protocols import LOADED_IMAGE
    li = LOADED_IMAGE.open(uefi.Handle(uefi.raw.image_handle()))

Other volumes can be mounted with :mod:`uefi.fs`, and images loaded and started
through :func:`uefi.raw.load_image` / :func:`uefi.raw.start_image`.  Networking is
available through the standard :mod:`network`, :mod:`socket` and :mod:`ssl`/``tls``
modules (see the :ref:`UEFI quick reference <uefi_quickref>`).

Pitfalls
--------

.. list-table::
    :header-rows: 1
    :widths: 25 75

    * - Area
      - Pitfall
    * - Calling convention
      - Protocol methods must be called via the C ``uefi.raw.call`` trampoline
        (EFIAPI). :mod:`uctypes` cannot call function pointers.
    * - Buffer lifetime
      - Argument buffers (especially ``CHAR16*``) must outlive the call -- use a
        :class:`PoolBuffer` or a named local, never an unnamed temporary.
    * - Exclusive opens
      - ``handle.EXCLUSIVE`` disconnects the current driver and can destabilise
        the system (e.g. kill the console). Prefer ``handle.GET_PROTOCOL``.
    * - Variable enumeration
      - Don't create/delete variables while enumerating; collect the list first.
    * - Variable wear
      - ``variable.set`` writes flash; never do it in a loop.
    * - ``WaitForEvent``
      - Invalid for ``EVT_NOTIFY_SIGNAL`` events and above ``TPL_APPLICATION``;
        use the pollable :class:`Event`.
    * - Device path END node
      - Constructed paths must be terminated with the End node (``to_bytes``
        does this for you).
    * - ExitBootServices
      - All Boot Services calls are invalid after exit; gate with
        :func:`uefi.raw.boot_services_active`.
