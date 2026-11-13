:mod:`uefi.device_path` --- device paths
========================================

.. module:: uefi.device_path
    :synopsis: parse, format and build UEFI device paths

The ``uefi.device_path`` submodule parses, formats and builds UEFI device paths.
A device path is a packed sequence of nodes -- each a 4-byte header (Type,
SubType, Length) followed by node data -- terminated by an End node.

:class:`DevicePath` and :class:`DevicePathNode` are re-exported at the top level
as :class:`uefi.DevicePath` and :class:`uefi.DevicePathNode`.

.. class:: DevicePath(nodes=None)

    A device path as a list of :class:`DevicePathNode`.

    .. classmethod:: from_ptr(ptr)

        Parse a device path from a raw ``EFI_DEVICE_PATH_PROTOCOL`` pointer.

    .. classmethod:: from_bytes(b)

        Parse a device path from a ``bytes`` buffer.

    .. classmethod:: from_text(text)

        Parse text such as ``'PciRoot(0x0)/Pci(0x1,0x0)'`` via the firmware's
        ``DevicePathFromText`` protocol.  Raises :class:`uefi.Error` if that
        protocol is absent, or ``ValueError`` if the text cannot be parsed.

    .. method:: to_bytes()

        Serialise to ``bytes``, **including the terminating End node** (you never
        need to add it yourself).

    .. method:: to_text(display_only=False, allow_shortcuts=False)

        Format as text via the firmware's ``DevicePathToText`` protocol, falling
        back to a minimal built-in ``type/subtype`` formatter if that protocol is
        absent.

    .. method:: file_path()

        Return the path string from the first Media/FilePath node, or ``None``.

    .. method:: pci_node()

        Return ``(device, function)`` from the first Hardware/PCI node, or
        ``None``.

    .. method:: acpi_node()

        Return ``(HID, UID)`` from the first ACPI node, or ``None``.

    :class:`DevicePath` is iterable (over its nodes) and supports ``len()``.

.. class:: DevicePathNode(type, subtype, data=b"")

    A single device-path node.

    .. attribute:: type
                   subtype
                   data

        The node's type, sub-type and payload ``bytes``.

    .. method:: to_bytes()

        Serialise this node (header + data).  ``len(node)`` gives its length in
        bytes.

Node-type constants
-------------------

Node types:

.. data:: HARDWARE
          ACPI
          MESSAGING
          MEDIA
          END

Selected sub-types:

.. data:: HW_PCI
          ACPI_ACPI
          MEDIA_HARDDRIVE
          MEDIA_FILEPATH
          END_INSTANCE
          END_ENTIRE

.. note::

    Every device path ends with an End node (type ``0x7F``, sub-type ``0xFF``).
    When building a path, :meth:`DevicePath.to_bytes` appends it for you -- do not
    add it manually, but never strip it either, as firmware reads until it finds
    the End node.

Example
-------

::

    import uefi

    dp = uefi.Handle(uefi.raw.image_handle()).device_path()
    print(dp.to_text())        # e.g. PciRoot(0x0)/Pci(0x1,0x0)/.../\EFI\BOOT\BOOTAA64.EFI
    print(dp.file_path())      # \EFI\BOOT\BOOTAA64.EFI
    assert dp.to_bytes() == uefi.DevicePath.from_bytes(dp.to_bytes()).to_bytes()
