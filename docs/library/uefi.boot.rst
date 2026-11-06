:mod:`uefi.boot` --- the boot-manager layer
===========================================

.. module:: uefi.boot
    :synopsis: read/parse/write the standard boot variables and launch boot options

The ``uefi.boot`` submodule is the substance of a Python **boot loader / boot
manager**.  It reads, parses and writes the standard boot variables --
``BootOrder``, ``Boot####`` and ``BootNext`` -- and launches a boot option the way
the firmware's own boot manager does: ``LoadImage`` its device path, hand its
optional data to the image as ``LoadOptions``, and ``StartImage`` it.

Chainloading an OS this way needs no ``ExitBootServices`` from the port itself: the
OS loader that is started (shim / GRUB / systemd-boot / ``bootmgfw`` / an EFI-stub
kernel) calls it.

:class:`LoadOption` is re-exported at the top level as :class:`uefi.LoadOption`.

.. class:: LoadOption(description, device_path, optional_data=b"", attributes=ACTIVE)

    An ``EFI_LOAD_OPTION``: an OS or application to boot -- a *description*, a
    :class:`~uefi.device_path.DevicePath`, some *optional_data* (passed to the
    image as its load options), and *attributes*.

    .. attribute:: active

        ``True`` if the :data:`ACTIVE` flag is set (read-only property).

    .. classmethod:: parse(data)

        Parse the packed ``EFI_LOAD_OPTION`` bytes of a ``Boot####`` variable into
        a :class:`LoadOption`.

    .. method:: to_bytes()

        Serialise to the packed ``EFI_LOAD_OPTION`` form for ``SetVariable``.

    .. method:: load(*, boot_policy=True)

        ``LoadImage`` this option's device path and return an unstarted
        :class:`uefi.driver.Image` (with the option's optional data already set as
        its load options).

    .. method:: boot(*, boot_policy=True)

        Load and ``StartImage`` this option; return ``(exit_status, exit_data)``.

.. data:: ACTIVE
          FORCE_RECONNECT
          HIDDEN
          CATEGORY_APP

    ``LOAD_OPTION_*`` attribute flags for ``EFI_LOAD_OPTION.Attributes``.

Boot variables
--------------

.. function:: boot_order()

    The ``BootOrder`` list -- boot-option numbers in priority order (``[]`` if
    unset).

.. function:: set_boot_order(nums)

    Write ``BootOrder`` from a sequence of option numbers.

.. function:: load_option(num)

    Parse ``Boot####`` into a :class:`LoadOption` (raises :class:`uefi.Error` if it
    does not exist).

.. function:: set_option(num, option)

    Write a :class:`LoadOption` to ``Boot####``.

.. function:: options()

    List ``(num, LoadOption)`` for every entry in ``BootOrder`` (skipping
    unreadable ones).

.. function:: set_boot_next(num)

    Set ``BootNext`` so the firmware boots option *num* on the next reset.

.. function:: boot(num, *, boot_policy=True)

    Load and start boot option *num*; return ``(exit_status, exit_data)``.

Example
-------

::

    import uefi.boot as boot

    # List the firmware boot menu
    for num, opt in boot.options():
        print('Boot%04X' % num, opt.description, '->', opt.device_path.to_text())

    # Boot the first entry in BootOrder now
    order = boot.boot_order()
    if order:
        boot.boot(order[0])

.. warning::

    Writing boot variables writes flash (see :mod:`uefi.variable`); do not do it in
    a loop.  Boot variables are written with the non-volatile, boot-service and
    runtime access attributes so they persist and are visible to the OS.
