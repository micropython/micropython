:mod:`uefi.protocols` --- protocol descriptors
==============================================

.. module:: uefi.protocols
    :synopsis: descriptor-driven protocol overlays and pre-baked common protocols

The ``uefi.protocols`` package provides pre-baked
:class:`ProtocolDescriptor` objects for common UEFI protocols, and the
descriptor machinery (:class:`ProtocolDescriptor`, :class:`ProtocolWrapper`,
:class:`Field`, :class:`Method` and the type atoms) that declares a protocol
interface's layout and dispatches its methods.

A descriptor declares an interface struct as an ordered list of members.  From
that it builds a :mod:`uctypes` layout (so data fields can be read and written as
attributes) and generated method stubs (which read the function pointer at the
right slot and dispatch through :func:`uefi.raw.call`).

:class:`ProtocolDescriptor` and :class:`ProtocolWrapper` are re-exported at the
top level as :class:`uefi.ProtocolDescriptor` and :class:`uefi.ProtocolWrapper`.

Descriptors
-----------

.. class:: ProtocolDescriptor(name, guid, members)

    Declares a protocol interface.  *name* is a label, *guid* the protocol's
    :class:`~uefi.guid.GUID`, and *members* an ordered list of :class:`Field` and
    :class:`Method`.

    .. method:: open(handle, attributes=uefi.handle.GET_PROTOCOL)

        Open the protocol on *handle* and return a :class:`ProtocolWrapper` that
        owns the open.

    .. method:: locate()

        Locate an arbitrary instance and return a :class:`ProtocolWrapper`.

    .. method:: locate_all()

        Return a :class:`ProtocolWrapper` for every handle carrying the protocol.

    .. method:: wrap(iface_ptr, protocol_handle=None)

        Overlay the descriptor on an interface pointer you already hold.

.. class:: ProtocolWrapper

    A live overlay of a descriptor on an interface pointer.  Declared data fields
    are read and written as attributes (via :mod:`uctypes`); declared methods are
    callable and dispatch through the trampoline, raising :class:`uefi.Error` on a
    non-zero status unless the method was declared ``check=False``.

    .. attribute:: ptr

        The interface pointer as an integer (the
        :ref:`.ptr contract <uefi_ptr_contract>`).

    .. method:: struct_at(field, descriptor)

        Follow a pointer-typed *field* and overlay *descriptor* on the target,
        returning a new :class:`ProtocolWrapper`.  This is how nested structures
        (for example the graphics ``mode`` -> ``info`` chain) are reached.

    .. method:: close()

        Release the owned open, if any.  Also a context manager and finaliser.

Members and type atoms
----------------------

.. class:: Field(name, type)

    A data field of the given type atom.

.. class:: Method(name, arg_types=(), ret=Status, check=True, this=True)

    A method at the next function-pointer slot.  *arg_types* is a tuple of type
    atoms.  ``check=False`` returns the raw result instead of raising on error
    (used for methods that return a pointer rather than a status).  ``this=False``
    is for stateless "global" protocols whose methods take no leading interface
    pointer (e.g. ``DevicePathToText``/``FromText``).

The type atoms describe field and argument types:

``U8 U16 U32 U64 I8 I16 I32 I64`` (integers), ``Bool``, ``VPtr`` (``void*``),
``WStr`` (``CHAR16*``, marshalled to/from UTF-16LE), ``Str`` (``CHAR8*``),
``Ptr(target)`` (a typed pointer; an alias for ``VPtr``), and
``Status`` (a ``UINTN`` return, checked by default).

Pre-baked descriptors
---------------------

Importable from ``uefi.protocols``:

.. data:: LOADED_IMAGE

    ``EFI_LOADED_IMAGE_PROTOCOL``.

.. data:: BLOCK_IO
          BLOCK_IO_MEDIA

    ``EFI_BLOCK_IO_PROTOCOL`` and its media-geometry sub-structure.

.. data:: SIMPLE_FS

    ``EFI_SIMPLE_FILE_SYSTEM_PROTOCOL``.

.. data:: GOP
          GOP_MODE
          GOP_INFO

    ``EFI_GRAPHICS_OUTPUT_PROTOCOL`` and its ``mode`` / ``info``
    sub-structures.

.. data:: PCI_IO

    ``EFI_PCI_IO_PROTOCOL``.

.. data:: SERIAL_IO
          SERIAL_IO_MODE

    ``EFI_SERIAL_IO_PROTOCOL`` and its mode sub-structure.

.. data:: TIMESTAMP

    ``EFI_TIMESTAMP_PROTOCOL``.

.. data:: COMPONENT_NAME
          COMPONENT_NAME2

    ``EFI_COMPONENT_NAME_PROTOCOL`` and ``EFI_COMPONENT_NAME2_PROTOCOL`` -- the
    human-readable driver/controller names (see :func:`uefi.driver.driver_name`).

.. note::

    Method dispatch always goes through the C :func:`uefi.raw.call` trampoline
    (EFIAPI: ``ms_abi`` on x86-64, AAPCS64 on aarch64).  Do not attempt to call a
    function pointer from Python/:mod:`uctypes` directly -- there is no such
    facility, and a calling-convention mismatch would corrupt the stack on
    x86-64.  Also keep any ``CHAR16*``/buffer argument alive (a named local or a
    :class:`~uefi.buffer.PoolBuffer`) for the duration of the call.

Example
-------

Reading the current graphics resolution::

    from uefi.protocols import GOP, GOP_MODE, GOP_INFO

    g = GOP.locate()
    mode = g.struct_at('mode', GOP_MODE)
    info = mode.struct_at('info', GOP_INFO)
    print(info.horizontal_resolution, 'x', info.vertical_resolution)

Declaring a custom descriptor::

    from uefi.protocols import ProtocolDescriptor, Field, Method, U32, VPtr, Status
    from uefi import guid

    MY_PROTOCOL = ProtocolDescriptor('MY_PROTOCOL', guid.GUID('....'), [
        Field('revision', U32),
        Method('do_thing', (VPtr, U32)),   # (This, buf, len) -> EFI_STATUS
    ])
    p = MY_PROTOCOL.locate()
    p.do_thing(buf, len(buf))
