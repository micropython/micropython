:mod:`uefi.guid` --- GUIDs and the GUID registry
================================================

.. module:: uefi.guid
    :synopsis: the GUID type and a registry of well-known UEFI GUIDs

The ``uefi.guid`` submodule provides the :class:`GUID` type and a broad registry
of well-known UEFI GUIDs (protocols, variable namespaces and configuration
tables), named per the module's convention (the redundant trailing ``_GUID`` is
dropped).

:class:`GUID` is re-exported at the top level as :class:`uefi.GUID`.

.. class:: GUID(value)

    An ``EFI_GUID`` (16 bytes, mixed-endian).  *value* may be:

    * a canonical string ``"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"``,
    * a 16-byte ``bytes``/``bytearray`` (the packed form), or
    * another :class:`GUID`.

    .. attribute:: bytes

        The 16-byte packed ``EFI_GUID``, ready to hand to :mod:`uefi.raw` calls.

        .. note::

            Use the ``bytes`` **attribute**, not ``bytes(guid)`` -- MicroPython's
            ``bytes(obj)`` does not honour ``__bytes__`` (it iterates the object
            instead).

    :class:`GUID` supports ``==``, ``hash()`` (so GUIDs are usable as dict keys
    and set members), ``str()`` (the canonical text form) and ``repr()``.

Registry
--------

The module defines a large set of module-level :class:`GUID` constants grouped
into: console/input/graphics (e.g. ``GRAPHICS_OUTPUT_PROTOCOL``,
``SIMPLE_TEXT_OUTPUT_PROTOCOL``, ``SERIAL_IO_PROTOCOL``); storage and filesystem
(``BLOCK_IO_PROTOCOL``, ``SIMPLE_FILE_SYSTEM_PROTOCOL``, ``FILE_INFO``, ...);
buses and devices (``DEVICE_PATH_PROTOCOL``, ``PCI_IO_PROTOCOL``,
``USB2_HC_PROTOCOL``, ...); the image and driver model (``LOADED_IMAGE_PROTOCOL``,
``DRIVER_BINDING_PROTOCOL``, ``COMPONENT_NAME2_PROTOCOL``,
``DEVICE_PATH_TO_TEXT_PROTOCOL``, ...); the full UEFI 2.x network stack
(``SIMPLE_NETWORK_PROTOCOL``, ``IP4``/``IP6``, ``TCP4``/``TCP6``,
``UDP4``/``UDP6``, ``DHCP4``/``DHCP6``, ``DNS4``/``DNS6``, ``HTTP``, ``TLS``, ...,
each with its ``*_SERVICE_BINDING_PROTOCOL``); security and crypto
(``RNG_PROTOCOL``, ``HASH2_PROTOCOL``); HII and shell; variable namespaces and
policy (``GLOBAL_VARIABLE``, ``VARIABLE_POLICY_PROTOCOL``); and System-Table
configuration tables (``ACPI_20_TABLE_GUID``, ``SMBIOS3_TABLE_GUID``,
``DTB_TABLE_GUID``, ...).

.. function:: registry()

    Return ``{name: GUID}`` for every well-known GUID defined in the module.

.. function:: name_for(value)

    Return the registry name for a GUID (e.g. ``'BLOCK_IO_PROTOCOL'``), or
    ``None`` if unknown.  *value* may be a :class:`GUID` or anything the
    :class:`GUID` constructor accepts.  Handy for labelling handles in discovery
    output without needing the firmware's Component Name protocols.

.. note::

    The GUIDs are transcribed from the UEFI 2.x specification / EDK2 MdePkg.  A
    typo only makes a lookup silently miss (it never crashes), so spot-check
    against your firmware's headers if a lookup unexpectedly returns ``None``.

Example
-------

::

    from uefi import guid

    guid.name_for(guid.GRAPHICS_OUTPUT_PROTOCOL)   # 'GRAPHICS_OUTPUT_PROTOCOL'
    len(guid.registry())                           # number of known GUIDs
    my = guid.GUID('11111111-2222-3333-4444-555555555555')
