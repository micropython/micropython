:mod:`framebuf` --- frame buffer manipulation
=============================================

.. module:: framebuf
   :synopsis: Frame buffer manipulation

This module provides a general frame buffer which can be used to create
bitmap images, which can then be sent to a display.

class FrameBuffer
-----------------

The FrameBuffer class provides a pixel buffer which can be drawn upon with
pixels, lines, rectangles, ellipses, polygons, text and even other
FrameBuffers. It is useful when generating output for displays.

For example::

    import framebuf

    # FrameBuffer needs 2 bytes for every RGB565 pixel
    fbuf = framebuf.FrameBuffer(bytearray(100 * 10 * 2), 100, 10, framebuf.RGB565)

    fbuf.fill(0)
    fbuf.text('MicroPython!', 0, 0, 0xffff)
    fbuf.hline(0, 9, 96, 0xffff)

Constructors
------------

.. class:: FrameBuffer(buffer, width, height, format, stride=width, /)

    Construct a FrameBuffer object.  The parameters are:

        - *buffer* is an object with a buffer protocol which must be large
          enough to contain every pixel defined by the width, height and
          format of the FrameBuffer.
        - *width* is the width of the FrameBuffer in pixels
        - *height* is the height of the FrameBuffer in pixels
        - *format* specifies the type of pixel used in the FrameBuffer;
          permissible values are listed under Constants below. These set the
          number of bits used to encode a color value and the layout of these
          bits in *buffer*.
          Where a color value c is passed to a method, c is a small integer
          with an encoding that is dependent on the format of the FrameBuffer.
        - *stride* is the number of pixels between each horizontal line
          of pixels in the FrameBuffer. This defaults to *width* but may
          need adjustments when implementing a FrameBuffer within another
          larger FrameBuffer or screen. The *buffer* size must accommodate
          an increased step size.

    One must specify valid *buffer*, *width*, *height*, *format* and
    optionally *stride*.  Invalid *buffer* size or dimensions may lead to
    unexpected errors.

Drawing primitive shapes
------------------------

The following methods draw shapes onto the FrameBuffer.

.. method:: FrameBuffer.fill(c)

    Fill the entire FrameBuffer with the specified color.

.. method:: FrameBuffer.pixel(x, y[, c, alpha=0xFF])

    If *c* is not given, get the color value of the specified pixel.
    If *c* is given, set the specified pixel to the given color.

    The optional *alpha* parameter is a value from 0 to 255 which
    indicates the transparency to use when setting the pixel (0 is
    completely transparent, 255 is completely opaque). The if not
    specified, the pixel is drawn opaquely.

.. method:: FrameBuffer.hline(x, y, w, c[, alpha=0xFF])
.. method:: FrameBuffer.vline(x, y, h, c[, alpha=0xFF])
.. method:: FrameBuffer.line(x1, y1, x2, y2, c[, alpha=0xFF])

    Draw a line from a set of coordinates using the given color and
    a thickness of 1 pixel. The `line` method draws the line up to
    a second set of coordinates whereas the `hline` and `vline`
    methods draw horizontal and vertical lines respectively up to
    a given length.

    The optional *alpha* parameter is a value from 0 to 255 which
    indicates the transparency to use when drawing the line. The
    default is 255 (completely opaque).

.. method:: FrameBuffer.rect(x, y, w, h, c[, f, alpha=0xFF])

    Draw a rectangle at the given location, size, color and transparency.

    The optional *f* parameter can be set to ``True`` to fill the rectangle.
    Otherwise just a one pixel outline is drawn.

    The optional *alpha* parameter is a value from 0 to 255 which
    indicates the transparency to use when drawing the rectangle. The
    default is 255 (completely opaque).

.. method:: FrameBuffer.ellipse(x, y, xr, yr, c[, f, m, alpha=0xFF])

    Draw an ellipse at the given location. Radii *xr* and *yr* define the
    geometry; equal values cause a circle to be drawn. The *c* parameter
    defines the color.

    The optional *f* parameter can be set to ``True`` to fill the ellipse.
    Otherwise just a one pixel outline is drawn.

    The optional *m* parameter enables drawing to be restricted to certain
    quadrants of the ellipse. The LS four bits determine which quadrants are
    to be drawn, with bit 0 specifying Q1, b1 Q2, b2 Q3 and b3 Q4. Quadrants
    are numbered counterclockwise with Q1 being top right.

    The optional *alpha* parameter is a value from 0 to 255 which
    indicates the transparency to use when drawing the ellipse. The
    default is 255 (completely opaque).

.. method:: FrameBuffer.poly(x, y, coords, c[, f, alpha=0xFF])

    Given a list of coordinates, draw an arbitrary (convex or concave) closed
    polygon at the given x, y location using the given color.

    The *coords* must be specified as a :mod:`array` of integers, e.g.
    ``array('h', [x0, y0, x1, y1, ... xn, yn])``.

    The optional *f* parameter can be set to ``True`` to fill the polygon.
    Otherwise just a one pixel outline is drawn.

    The optional *alpha* parameter is a value from 0 to 255 which
    indicates the transparency to use when drawing the polygon. The
    default is 255 (completely opaque).

Drawing text
------------

.. method:: FrameBuffer.text(s, x, y[, c, alpha=0xFF])

    Write text to the FrameBuffer using the coordinates as the upper-left
    corner of the text. The color of the text can be defined by the optional
    argument but is otherwise a default value of 1. All characters have
    dimensions of 8x8 pixels and there is currently no way to change the font.

    The optional *alpha* parameter is a value from 0 to 255 which
    indicates the transparency to use when drawing the text. The
    default is 255 (completely opaque).

Other methods
-------------

.. method:: FrameBuffer.scroll(xstep, ystep)

    Shift the contents of the FrameBuffer by the given vector. This may
    leave a footprint of the previous colors in the FrameBuffer.

.. method:: FrameBuffer.blit(fbuf, x, y, key=-1, palette=None, alpha=0xFF)

    Draw another FrameBuffer on top of the current one at the given coordinates.

    *fbuf* can be another FrameBuffer instance, or a tuple or list of the form::

        (buffer, width, height, format)

    or::

        (buffer, width, height, format, stride)

    This matches the signature of the FrameBuffer constructor, and the elements
    of the tuple/list are the same as the arguments to the constructor except that
    the *buffer* here can be read-only.

    If *key* is specified then it should be a color integer and the
    corresponding color will be considered transparent: all pixels with that
    color value will not be drawn. (If the *palette* is specified then the *key*
    is compared to the value from *palette*, not to the value directly from
    *fbuf*.)

    The *palette* argument enables blitting between FrameBuffers with differing
    formats. Typical usage is to render a monochrome or grayscale glyph/icon to
    a color display. The *palette* is a FrameBuffer instance whose format is
    that of the current FrameBuffer. The *palette* height is one pixel and its
    pixel width is the number of colors in the source FrameBuffer. The *palette*
    for an N-bit source needs 2**N pixels; the *palette* for a monochrome source
    would have 2 pixels representing background and foreground colors. The
    application assigns a color to each pixel in the *palette*. The color of the
    current pixel will be that of that *palette* pixel whose x position is the
    color of the corresponding source pixel.

    The *alpha* parameter is either a value from 0 to 255 which indicates the
    transparency to use when overlaying the buffer, or is a monochrome or
    grayscale mask buffer of the same size as the buffer being drawn, indicating
    the transparency for each pixel. The mask buffer can either be another
    FrameBuffer or a tuple as described above for the *fbuf* parameter. If both
    *key* and *alpha* are used, any pixels of the *key* color will be transparent
    and the *alpha* value will be ignored for those pixels.

    An alpha mask buffer cannot have the RGB565 pixel format.

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
    Each byte occupies 8 horizontal pixels with bit 7 being the leftmost.
    Subsequent bytes appear at successive horizontal locations until the
    rightmost edge is reached. Further bytes are rendered on the next row, one
    pixel lower.

.. data:: framebuf.MONO_HMSB

    Monochrome (1-bit) color format
    This defines a mapping where the bits in a byte are horizontally mapped.
    Each byte occupies 8 horizontal pixels with bit 0 being the leftmost.
    Subsequent bytes appear at successive horizontal locations until the
    rightmost edge is reached. Further bytes are rendered on the next row, one
    pixel lower.

.. data:: framebuf.RGB565

    Red Green Blue (16-bit, 5+6+5, native) color format in native byte-order.

.. data:: framebuf.RGB565_BS

    Red Green Blue (16-bit, 5+6+5, non-native) color format. This format uses
    color values supplied with a non-native bit-pattern (ie. instead of
    ``rrrrrggggggbbbbb`` the values are byte-swapped ``gggbbbbbrrrrrggg`` where
    the least-significant bits of the green channel occur first). This is a
    legacy format to ease migration for the common case of systems which used
    displays with an opposite byte-order to microcontroller.

.. data:: framebuf.RGB565_LE

    Red Green Blue (16-bit, 5+6+5, little-endian) color format in little-endian
    byte order.  This defines a 16-bit format where the bytes are stored in
    little-endian order. If the system is little-endian, this is the same as
    ``RGB565``.

.. data:: framebuf.RGB565_BE

    Red Green Blue (16-bit, 5+6+5, big-endian) color format in big-endian byte
    order.  This defines a 16-bit format where the bytes are stored in
    big-endian order. If the system is big-endian, this is the same as ``RGB565``.

.. data:: framebuf.GS2_HMSB

    Grayscale (2-bit) color format

.. data:: framebuf.GS4_HMSB

    Grayscale (4-bit) color format

.. data:: framebuf.GS8

    Grayscale (8-bit) color format

.. data:: framebuf.ALPHA

    Whether or not the framebuf module was compiled with support for alpha
    values.

    If this is False, all alpha parameters will be ignored, lines and
    polygons will be rendered with non-anti-aliasing algorithms, and drawing
    will be done full opaquely. In the ``framebuf.blit`` method a monochrome
    buffer can be used as the *alpha* parameter to provide a binary mask (0
    is transparent, 1 is opaque), but other values are ignored or are errors.

    If this is True, alpha parameters will act as specified in the descriptions,
    anti-aliased algorithms will be used for rendering lines and polygons, and
    ``framebuf.blit`` can use grayscale masks.

    There is currently no support for rendering antialiased ellipses.

.. note::
    In Micropython 1.26 and earlier, the RGB565 format did not need
    to know aout the internal color channels within each 16-bit value.
    As a result, it did not care about the byte-order of the values stored
    as pixels.  Many display devices use big-endian RGB565, and so code that
    used them from little-endian microcontrollers would simply provide colors
    as big-endian RGB565 values (ie. using ``0b00000000_11111000`` for red
    instead of ``0b11111000_00000000``).

    The introduction of support for alpha blending means that the RGB565 format
    is assuming native byte-order for the layout of color channels within the
    16-bits of a pixel.  This breaks code that uses byte-swapped color values,
    but it can be adapted either by:

    * using firmware compiled with alpha support disabled via the
      ``MICROPY_PY_FRAMEBUF_ALPHA`` flag; or

    * replacing the use of ``RGB565`` format with the byte-swapped
      ``RGB565_BS`` format; or

    * using ``RGB565_BE`` or ``RGB565_LE`` as appropriate for the target
      hardware and change color values to use native byte-order.

    New code which needs to support buffers of a particular byte-order should
    use the last option.
