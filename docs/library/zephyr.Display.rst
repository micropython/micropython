.. currentmodule:: zephyr
.. _zephyr.Display:

class Display -- access to Zephyr Displays
==========================================

Uses the `Zephyr Display API <https://docs.zephyrproject.org/latest/doxygen/html/group__display__interface.html>`_.

This class allows access to Zephyr-provided Displays (`zephyr,display chosen Node <https://docs.zephyrproject.org/latest/build/dts/api/api.html#zephyr-specific-chosen-nodes>`_ and `zephyr,displays Node <https://docs.zephyrproject.org/latest/build/dts/api/bindings/display/zephyr%2Cdisplays.html>`_) via an API reproducing the Zephyr one.

Constructors
------------

.. class:: Display(id)

   Gets an object for accessing a Display identified by ``id``.

   ``id`` can be an integer (``0``, ``1``...) or a string (``"ssd1306@3c"``) identifying a display node by its position or by its node identifiers.

Methods
-------

.. method:: Display.write(buf[, x, y[, size_x[, size_y]]])

            Write a buffer-protocol object in the Display's Pixel Format to the display.

            Optionally x and y position, x size, and y size can be specified.

.. method:: Display.rgb(r, g, b)

            Convert a RGB color to the Display's Pixel Format.

.. method:: Display.framebuf_current_format()

            Retrieve the Display's current Pixel Format as a framebuf Pixel Format Integer.

.. method:: Display.capabilities()

            Retrieve a tuple describing the display in the format:

            ``(X Size, Y Size, Supported PFs, Current PF, Current Orientation, Misc Characteristics)``

.. method:: Display.format([format])

            Get and set the Pixel Format of the Display.

.. method:: Display.blanking_on()

            Enable blanking.

.. method:: Display.blanking_off()

            Disable blanking.

.. method:: Display.clear()

            Clear the Display.

.. method:: Display.set_brightness(value)

            Set the Display's brightness from ``0`` to ``255``.

.. method:: Display.set_contrast(value)

            Set the Display's contrast from ``0`` to ``255``.

.. method:: Display.orientation([orientation])

            Get and set the Orientation of the Display.

.. method:: Display.as_framebuf()

            If :mod:`framebuf` is enabled, generate a :class:`framebuf.FrameBuffer` instance augmented with a ``show()`` function that directly maps to the display with the currently configured settings.
