:mod:`uefi.handle` --- handles and protocol opens
=================================================

.. module:: uefi.handle
    :synopsis: EFI_HANDLE wrappers and protocol open/close ownership

The ``uefi.handle`` submodule wraps the UEFI handle database.  A :class:`Handle`
wraps a raw ``EFI_HANDLE`` and is the entry point for enumerating and opening the
protocols on a device; a :class:`ProtocolHandle` owns a firmware protocol *open*
and releases it deterministically.

:class:`Handle` and :class:`ProtocolHandle` are re-exported at the top level as
:class:`uefi.Handle` and :class:`uefi.ProtocolHandle`.

Discovery helpers
-----------------

.. function:: all_handles()

    Return a list of every :class:`Handle` in the handle database (the shell's
    ``dh``).

.. function:: locate_handles(guid)

    Return a list of :class:`Handle` that support the protocol *guid*.

.. function:: locate_protocol(guid)

    Return an (unowned) :class:`ProtocolHandle` around the firmware's arbitrary
    instance of *guid*.  Use :func:`locate_handles` when several instances may
    exist.

.. function:: all_protocols()

    Return the ``set`` of every protocol :class:`~uefi.guid.GUID` installed
    anywhere in the database -- the global protocol inventory.  ``len()`` of it is
    the distinct-protocol count.

Handle
------

.. class:: Handle(ptr)

    A wrapper around a raw ``EFI_HANDLE`` (an integer *ptr*).  Supports ``==``
    and ``hash()``.

    .. attribute:: ptr

        The handle as an integer (the :ref:`.ptr contract <uefi_ptr_contract>`).

    .. method:: protocols()

        Return the list of protocol :class:`~uefi.guid.GUID` installed on this
        handle.

    .. method:: supports(guid)

        Return ``True`` if *guid* is installed on this handle.

    .. method:: open(guid, attributes=GET_PROTOCOL)

        Open *guid* on this handle and return a :class:`ProtocolHandle`.  See the
        open-attribute constants below.

    .. method:: open_info(guid)

        Return a list of :class:`ProtocolOpenInfo` describing who has *guid* open
        on this handle and how (the usage view behind the shell's ``dh -d`` /
        ``devices`` / ``devtree``).  An empty list if the protocol is present but
        unopened.

    .. method:: device_path()

        Return this handle's :class:`~uefi.device_path.DevicePath`, or ``None`` if
        it has no Device Path protocol.

ProtocolHandle
--------------

.. class:: ProtocolHandle

    Owns a firmware protocol open.  Obtained from :meth:`Handle.open`,
    :func:`locate_protocol`, or the descriptor methods in :mod:`uefi.protocols`.

    .. attribute:: ptr

        The interface pointer as an integer -- ready to hand to
        :func:`uefi.raw.call` for method dispatch.

    .. method:: close()

        Release the open (``CloseProtocol`` for tracked opens).  Also invoked by
        the context-manager protocol and the finaliser.

    A :class:`ProtocolHandle` is a context manager and closes itself in its
    finaliser.

ProtocolOpenInfo
----------------

.. class:: ProtocolOpenInfo

    One ``EFI_OPEN_PROTOCOL_INFORMATION_ENTRY``: an agent's open of a protocol on
    a handle.  Returned by :meth:`Handle.open_info`.

    .. attribute:: agent

        The driver/application image :class:`Handle` that opened the protocol, or
        ``None``.

    .. attribute:: controller

        The controller :class:`Handle` (for driver/child opens), or ``None``.

    .. attribute:: attributes

        The raw open-attribute flags.

    .. attribute:: open_count

        The firmware's open count for this entry.

    .. attribute:: by_driver
                    by_child
                    exclusive

        Booleans decoded from :attr:`attributes` -- whether the open was
        ``BY_DRIVER``, ``BY_CHILD_CONTROLLER`` or ``EXCLUSIVE``.

Open-attribute constants
------------------------

For :meth:`Handle.open` (and the :mod:`uefi.protocols` descriptor ``open``):

.. data:: BY_HANDLE_PROTOCOL
.. data:: GET_PROTOCOL

    The default; an untracked open suitable for most uses.

.. data:: TEST_PROTOCOL
.. data:: BY_CHILD_CONTROLLER
.. data:: BY_DRIVER
.. data:: EXCLUSIVE

    .. warning::

        ``EXCLUSIVE`` forces the firmware to ``DisconnectController`` on the
        current user, which can kill the console driver.  Prefer
        ``GET_PROTOCOL``.

Shell-style discovery
---------------------

These primitives reproduce the UEFI shell's discovery commands: ``dh`` is
:func:`all_handles` + :meth:`Handle.protocols`; ``drivers`` is
``locate_handles(guid.DRIVER_BINDING_PROTOCOL)``; ``dh -d`` / ``devices`` /
``devtree`` is :meth:`Handle.open_info`, whose :class:`ProtocolOpenInfo` entries
expose the driver-to-controller relationships.

Example
-------

::

    import uefi
    from uefi import guid

    for h in uefi.handle.all_handles():
        if h.supports(guid.BLOCK_IO_PROTOCOL):
            print('block device:', h, h.device_path())
