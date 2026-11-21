.. currentmodule:: stm

:mod:`stm` --- functionality specific to STM32 MCUs
===================================================

.. module:: stm
    :synopsis: functionality specific to STM32 MCUs

This module provides functionality specific to STM32 microcontrollers, including
direct access to peripheral registers.

Memory access
-------------

The module exposes three objects used for raw memory access.

.. data:: mem8

    Read/write 8 bits of memory.

.. data:: mem16

    Read/write 16 bits of memory.

.. data:: mem32

    Read/write 32 bits of memory.

Use subscript notation ``[...]`` to index these objects with the address of
interest.

These memory objects can be used in combination with the peripheral register
constants to read and write registers of the MCU hardware peripherals, as well
as all other areas of address space.


Peripheral register constants
-----------------------------

The module defines constants for registers which are generated from CMSIS header
files, and the constants available depend on the microcontroller series that is
being compiled for.  Examples of some constants include:

.. data:: GPIOA

    Base address of the GPIOA peripheral.

.. data:: GPIOB

    Base address of the GPIOB peripheral.

.. data:: GPIO_BSRR

    Offset of the GPIO bit set/reset register.

.. data:: GPIO_IDR

    Offset of the GPIO input data register.

.. data:: GPIO_ODR

    Offset of the GPIO output data register.

Constants that are named after a peripheral, like ``GPIOA``, are the absolute
address of that peripheral.  Constants that have a prefix which is the name of a
peripheral, like ``GPIO_BSRR``, are relative offsets of the register.  Accessing
peripheral registers requires adding the absolute base address of the peripheral
and the relative register offset.  For example ``GPIOA + GPIO_BSRR`` is the
full, absolute address of the ``GPIOA->BSRR`` register.

Example use:

.. code-block:: python3

    # set PA2 high
    stm.mem32[stm.GPIOA + stm.GPIO_BSRR] = 1 << 2

    # read PA3
    value = (stm.mem32[stm.GPIOA + stm.GPIO_IDR] >> 3) & 1


Functions specific to STM32WBxx MCUs
------------------------------------

These functions are available on STM32WBxx microcontrollers, and interact with
the second CPU, the RF core.

.. function:: rfcore_status()

    Returns the status of the second CPU as an integer (the first word of device
    info table).

.. function:: rfcore_fw_version(id)

    Get the version of the firmware running on the second CPU.  Pass in 0 for
    *id* to get the FUS version, and 1 to get the WS version.

    Returns a 5-tuple with the full version number.

.. function:: rfcore_sys_hci(ogf, ocf, data, timeout_ms=0)

    Execute a HCI command on the SYS channel.  The execution is synchronous.

    Returns a bytes object with the result of the SYS command.

Functions specific to STM32WLxx MCUs
------------------------------------

These functions are available on STM32WLxx microcontrollers, and interact with
the integrated "SUBGHZ" radio modem peripheral.

.. function:: subghz_cs(level)

   Sets the internal SPI CS pin attached to the radio peripheral. The ``level``
   argument is active-low: a truthy value means "CS pin high" and de-asserts the
   signal, a falsey value means "CS pin low" and asserts the signal.

   The internal-only SPI bus corresponding to this CS signal can be instantiated
   using :ref:`machine.SPI()<machine.SPI>` ``id`` value ``"SUBGHZ"``.

.. function:: subghz_irq(handler)

   Sets the internal SUBGHZ radio interrupt handler to the provided
   function. The handler function is called as a "hard" interrupt in response to
   radio peripheral interrupts. See :ref:`isr_rules` for more information about
   interrupt handlers in MicroPython.

   Calling this function with the handler argument set to None disables the IRQ.

   Due to a hardware limitation, each time this IRQ fires MicroPython disables
   it before calling the handler. In order to receive another interrupt, Python
   code should call ``subghz_irq()`` to set the handler again. This has the side
   effect of re-enabling the IRQ.

.. function:: subghz_is_busy()

   Return a ``bool`` corresponding to the internal "RFBUSYS" signal from the
   radio peripheral. Before sending a new command to the radio over SPI then
   this function should be polled until it returns ``False``, to confirm the
   busy signal is de-asserted.

The LTDC display controller
---------------------------

On boards with a parallel RGB display, the ``LTDC`` class drives it from a frame
buffer in memory.  The board sets the panel's size, timing and pins.

.. class:: LTDC(*, format=LTDC.RGB565)

   Turn on the display, which shows black until the first call to ``show()``.
   There is only one display, so this always returns the same object, and
   calling it again re-initialises the display.

   *format* is the pixel format of the buffers passed to ``show()``, one of the
   :ref:`formats <stm_ltdc_formats>` listed below.

.. method:: LTDC.show(buf, *, wait=True)

   Display the contents of *buf*, which must hold at least ``width * height``
   pixels in the configured format.  The LTDC keeps reading *buf* directly,
   without copying it, and driving the display continuously until another
   buffer is shown.  *buf* can be modified after calling ``show()``, but some
   changes may stay in the CPU cache and appear late or not at all; calling
   ``show()`` again flushes the cache.

   To update the display without showing partly drawn changes, use two buffers
   (double buffering): draw into the one that isn't shown, then show it.  The
   display switches to *buf* at the start of the next frame, so a frame is
   never drawn partly from the old buffer and partly from the new one.

   With *wait* set to ``True``, ``show()`` waits for the switch, after which
   the previous buffer is no longer in use and can be drawn into.  With *wait*
   set to ``False``, ``show()`` returns straight away, but the switch still
   happens at the start of the next frame.  Until then the display continues to
   read the previous buffer, so drawing into it may show partly drawn changes.
   If ``show()`` is called again before the switch, it waits for the switch
   first.

   ``show()`` keeps a reference to each buffer the display may still be reading,
   so it isn't freed by the garbage collector even if the program no longer
   refers to it.  With *wait* set to ``True``, the previous buffer is released
   when ``show()`` returns.  With *wait* set to ``False``, the previous buffer
   is released at the next call to ``show()``, because only then is the switch
   known to be done.

   If *buf* is ``None``, the display shows black, as it does before the first
   call to ``show()``, without needing a buffer.  The switch to black happens at
   the start of the next frame, and the previous buffer is released in the same
   way as above: when ``show()`` returns with *wait* set to ``True``, or at the
   next call to ``show()`` with *wait* set to ``False``.  After that, ``show()``
   holds no references to any buffer.

   Raises ``ValueError`` if *buf* is too small, and ``OSError`` if the display
   has been turned off by ``deinit()`` or the hardware fails to respond.

.. method:: LTDC.deinit()

   Turn off the display, and release the references that ``show()`` keeps to
   the buffers it was given, so they can be freed by the garbage collector if
   the program no longer refers to them.  This also happens on a soft reset.
   After ``deinit()``, ``show()`` raises ``OSError`` until the display is turned
   on again with ``LTDC()``.

.. attribute:: LTDC.width
               LTDC.height

   The size of the display in pixels.

.. _stm_ltdc_formats:

.. data:: LTDC.RGB565
          LTDC.RGB888
          LTDC.ARGB8888
          LTDC.GS8

   Pixel formats.  Each pixel is stored in the buffer as:

   - ``RGB565``: 2 bytes, a little-endian 16-bit value with red in the top 5
     bits, green in the middle 6 and blue in the bottom 5.  This matches
     ``framebuf.RGB565``.
   - ``RGB888``: 3 bytes, in the order blue, green, red.
   - ``ARGB8888``: 4 bytes, in the order blue, green, red, alpha.  The alpha
     byte is ignored.
   - ``GS8``: 1 byte, a grey level from 0 (black) to 255 (white).  This matches
     ``framebuf.GS8``.

For example::

    import stm, framebuf

    lcd = stm.LTDC()
    buf = bytearray(lcd.width * lcd.height * 2)
    fb = framebuf.FrameBuffer(buf, lcd.width, lcd.height, framebuf.RGB565)
    fb.fill(0)
    fb.text("Hello!", 10, 10, 0xffff)
    lcd.show(buf)
