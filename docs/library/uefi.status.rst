.. _uefi_status:

:mod:`uefi.status` --- EFI_STATUS codes and errors
==================================================

.. module:: uefi.status
    :synopsis: EFI_STATUS constants and the uefi.Error exception

The ``uefi.status`` submodule defines the ``EFI_STATUS`` constants and the
:class:`Error` exception.  An ``EFI_STATUS`` is a ``UINTN`` whose top bit marks an
error.  The Pythonic layer calls :func:`check` on the raw return values to turn
the error range into a raised :class:`Error`.

:class:`Error` and :func:`check` are re-exported at the top level as
:class:`uefi.Error` and ``uefi.check``.

.. exception:: Error(status)

    Raised when a UEFI call returns a non-``SUCCESS`` ``EFI_STATUS``.  Subclasses
    ``OSError`` so existing ``except OSError`` paths still catch it.

    .. attribute:: status

        The numeric ``EFI_STATUS``.

    .. attribute:: name

        The human-readable name, e.g. ``"EFI_NOT_FOUND"``.

.. function:: check(status)

    Raise :class:`Error` if *status* is in the error range (top bit set);
    otherwise return *status* unchanged.  This makes it convenient to both check
    and pass through non-error informational statuses.

.. function:: name(status)

    Return the human-readable name for *status* (e.g. ``"EFI_NOT_FOUND"``), or a
    ``"EFI_STATUS_0x..."`` form for unrecognised values.

Constants
---------

``SUCCESS`` is ``0``.  The error codes (top bit set) mirror the UEFI
specification, Appendix D, and include:

``LOAD_ERROR``, ``INVALID_PARAMETER``, ``UNSUPPORTED``, ``BAD_BUFFER_SIZE``,
``BUFFER_TOO_SMALL``, ``NOT_READY``, ``DEVICE_ERROR``, ``WRITE_PROTECTED``,
``OUT_OF_RESOURCES``, ``VOLUME_CORRUPTED``, ``VOLUME_FULL``, ``NO_MEDIA``,
``MEDIA_CHANGED``, ``NOT_FOUND``, ``ACCESS_DENIED``, ``NO_RESPONSE``,
``NO_MAPPING``, ``TIMEOUT``, ``NOT_STARTED``, ``ALREADY_STARTED``, ``ABORTED``,
``ICMP_ERROR``, ``TFTP_ERROR``, ``PROTOCOL_ERROR``, ``INCOMPATIBLE_VERSION``,
``SECURITY_VIOLATION``, ``CRC_ERROR``, ``END_OF_MEDIA``, ``END_OF_FILE``,
``INVALID_LANGUAGE`` and ``COMPROMISED_DATA``.

Example
-------

::

    from uefi import raw, status

    st, iface = raw.locate_protocol(some_guid.bytes)
    if st == status.NOT_FOUND:
        print('not present')
    else:
        status.check(st)   # raises uefi.Error on any other failure
