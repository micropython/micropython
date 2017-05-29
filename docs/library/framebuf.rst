:mod:`framebuf` --- Frame buffer manipulation
=============================================

.. module:: framebuf
   :synopsis: Frame buffer manipulation

This module provides a general frame buffer which can be used to create
bitmap images, which can then be sent to a display.

class FrameBuffer
-----------------

The FrameBuffer class provides a pixel buffer which can be drawn upon with
pixels, lines, rectangles, text and even other FrameBuffer's. It is useful
when generating output for displays.

For example::

    import framebuf

    # FrameBuffer needs 2 bytes for every RGB565 pixel
    fbuf = FrameBuffer(bytearray(10 * 100 * 2), 10, 100, framebuf.RGB565)

    fbuf.fill(0)
    fbuf.text('MicroPython!', 0, 0, 0xffff)
    fbuf.hline(0, 10, 96, 0xffff)

Constructors
------------

.. class:: FrameBuffer(buffer, width, height, format, stride=width)

    Construct a FrameBuffer object.  The parameters are:

        - `buffer` is an object with a buffer protocol which must be large
          enough to contain every pixel defined by the width, height and
          format of the FrameBuffer.
        - `width` is the width of the FrameBuffer in pixels
        - `height` is the height of the FrameBuffer in pixels
        - `format` specifies the type of pixel used in the FrameBuffer;
          valid values are ``framebuf.MVLSB``, ``framebuf.RGB565``
          and ``framebuf.GS4_HMSB``. MVLSB is monochrome 1-bit color,
          RGB565 is RGB 16-bit color, and GS4_HMSB is grayscale 4-bit color.
          Where a color value c is passed to a method, c is a small integer
          with an encoding that is dependent on the format of the FrameBuffer.
        - `stride` is the number of pixels between each horizontal line
          of pixels in the FrameBuffer. This defaults to `width` but may
          need adjustments when implementing a FrameBuffer within another
          larger FrameBuffer or screen. The `buffer` size must accommodate
          an increased step size.

    One must specify valid `buffer`, `width`, `height`, `format` and
    optionally `stride`.  Invalid `buffer` size or dimensions may lead to
    unexpected errors.

Drawing primitive shapes
------------------------

The following methods draw shapes onto the FrameBuffer.

.. method:: FrameBuffer.fill(c)

    Fill the entire FrameBuffer with the specified color.

.. method:: FrameBuffer.pixel(x, y[, c])

    If `c` is not given, get the color value of the specified pixel.
    If `c` is given, set the specified pixel to the given color.

.. method:: FrameBuffer.hline(x, y, w, c)
.. method:: FrameBuffer.vline(x, y, h, c)
.. method:: FrameBuffer.line(x1, y1, x2, y2, c)

    Draw a line from a set of coordinates using the given color and
    a thickness of 1 pixel. The `line` method draws the line up to
    a second set of coordinates whereas the `hline` and `vline`
    methods draw horizontal and vertical lines respectively up to
    a given length.

.. method:: FrameBuffer.rect(x, y, w, h, c)
.. method:: FrameBuffer.fill_rect(x, y, w, h, c)

    Draw a rectangle at the given location, size and color. The `rect`
    method draws only a 1 pixel outline whereas the `fill_rect` method
    draws both the outline and interior.

Drawing text
------------

.. method:: FrameBuffer.text(s, x, y[, c])

    Write text to the FrameBuffer using the the coordinates as the upper-left
    corner of the text. The color of the text can be defined by the optional
    argument but is otherwise a default value of 1. All characters have
    dimensions of 8x8 pixels and there is currently no way to change the font.


Other methods
-------------

.. method:: FrameBuffer.scroll(xstep, ystep)

    Shift the contents of the FrameBuffer by the given vector. This may
    leave a footprint of the previous colors in the FrameBuffer.

.. method:: FrameBuffer.blit(fbuf, x, y[, key])

    Draw another FrameBuffer on top of the current one at the given coordinates.
    If `key` is specified then it should be a color integer and the
    corresponding color will be considered transparent: all pixels with that
    color value will not be drawn.

    This method works between FrameBuffer's utilising different formats, but the
    resulting colors may be unexpected due to the mismatch in color formats.

Constants
---------

.. data:: framebuf.MONO_VLSB

    Monochrome (1-bit) color format
    This defines a mapping where the bits in a byte are vertically mapped with
    bit 0 being nearest the top of the screen. Consequently each byte occupies
    8 vertical pixels. Subsequent bytes appear at successive horizontal
    locations until the rightmost edge is reached. Further bytes are rendered
    at locations starting at the leftmost edge, 8 pixels lower.

.. data:: framebuf.MONO_HLSB

    Monochrome (1-bit) color format
    This defines a mapping where the bits in a byte are horizontally mapped.
    Each byte occupies 8 horizontal pixels with bit 0 being the leftmost.
    Subsequent bytes appear at successive horizontal locations until the
    rightmost edge is reached. Further bytes are rendered on the next row, one
    pixel lower.

.. data:: framebuf.MONO_HMSB

    Monochrome (1-bit) color format
    This defines a mapping where the bits in a byte are horizontally mapped.
    Each byte occupies 8 horizontal pixels with bit 7 being the leftmost.
    Subsequent bytes appear at successive horizontal locations until the
    rightmost edge is reached. Further bytes are rendered on the next row, one
    pixel lower.

.. data:: framebuf.RGB565

    Red Green Blue (16-bit, 5+6+5) color format

.. data:: framebuf.GS4_HMSB

    Grayscale (4-bit) color format
