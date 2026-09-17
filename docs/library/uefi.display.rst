:mod:`uefi.display` --- the GOP display as a framebuffer
========================================================

.. module:: uefi.display
    :synopsis: the UEFI graphics output as a framebuf.FrameBuffer

The ``uefi.display`` submodule exposes the UEFI Graphics Output Protocol (GOP) as
an ordinary :mod:`framebuf` ``FrameBuffer``, so you can draw with all the usual
framebuf primitives and push the result to the screen.

:class:`Display` is re-exported at the top level as :class:`uefi.Display`.

.. note::

    :mod:`framebuf`'s only colour format is RGB565 (16 bits per pixel), while GOP
    expects 32-bit BGRx.  A :class:`Display` therefore keeps an RGB565
    back-buffer (the one framebuf draws into) and, on :meth:`Display.show`,
    expands it to BGRx (via the C :func:`uefi.raw.convert_rgb565`) and ``Blt``\ s
    it to the screen.

.. class:: Display(mode=None)

    Locate the GOP display and wrap it as a framebuffer.  If *mode* is given, the
    display is first switched to that mode number.  Raises :class:`uefi.Error` if
    no GOP display is present.

    .. attribute:: fb

        The :mod:`framebuf` ``FrameBuffer`` (RGB565).  Draw on it with any
        framebuf method, then call :meth:`show`.

    .. attribute:: width
                   height

        The current mode's resolution in pixels.

    .. attribute:: gop

        The underlying :class:`~uefi.protocols.ProtocolWrapper` for the GOP
        interface, for direct access if needed.

    .. method:: modes()

        Return ``[(mode_number, width, height), ...]`` for every supported
        display mode.

    .. method:: set_mode(mode)

        Switch to the given *mode* number, rebuilding the back-buffers for the new
        resolution.  (This invalidates the previous :attr:`fb`; re-read it after
        switching.)

    .. method:: show()

        Push the framebuffer to the screen.

.. function:: rgb565(r, g, b)

    Pack 8-bit *r*, *g*, *b* components into a :mod:`framebuf` RGB565 colour
    value.

Example
-------

::

    import uefi
    from uefi.display import rgb565

    d = uefi.Display()
    fb, W, H = d.fb, d.width, d.height

    fb.fill(0)                                  # black
    fb.rect(10, 10, W - 20, H - 20, rgb565(255, 255, 255))
    fb.text('hello UEFI', 20, 20, rgb565(255, 60, 60))
    d.show()

See the ``samples/`` directory in the port (for example ``clock.py`` and
``draw.py``) for complete graphics programs; run them with ``make run-gfx``.
