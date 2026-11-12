:mod:`jpeg` --- JPEG decoder
============================

.. module:: jpeg
   :synopsis: JPEG decoder

This module decodes JPEG images.  The image is decoded directly into a buffer using
the same pixel layout as a `framebuf.FrameBuffer`, so it can be decoded straight
into a display's frame buffer, or wrapped in a ``FrameBuffer`` to be drawn on.

Availability: ports that provide a decoder and boards that enable it.  On stm32 the
decoder is the hardware JPEG codec of the STM32F7, STM32H7 and STM32N6, enabled with
``MICROPY_PY_JPEG``.

On the STM32N6, the DMA2D converts the decoded image to RGB while the codec decodes,
which takes about 5ms for an 800x448 camera frame.  The DMA2D is used when all of the
following are true, and otherwise the CPU converts the colours, which takes about 32ms
for the same frame:

- ``format`` is `RGB565` or `RGB888`;
- ``scale`` is 1;
- the image is in colour, not greyscale;
- the whole image fits in the destination at ``x`` and ``y``, so nothing is clipped;
- for `RGB565` with ``swap=True``, the width of the image is even.

On the STM32F7 and STM32H7 the CPU always converts the colours.

Chroma is upsampled by repeating samples rather than interpolating, which is fast but
can make the edges of strongly coloured areas slightly blocky in 4:2:2 and 4:2:0
images.

Baseline JPEG images are supported, which includes the frames from MJPEG webcams
(see `machine.USBHost.UVC`).  Images without Huffman tables, which some webcams send,
are decoded using the standard tables.  Greyscale images and YCbCr images with 4:4:4,
4:2:2 or 4:2:0 chroma subsampling are supported.

Example::

    import jpeg, framebuf

    data = open("photo.jpg", "rb").read()
    width, height = jpeg.info(data)

    buf = bytearray(width * height * 2)
    jpeg.decode(data, buf, width, height, jpeg.RGB565)
    fb = framebuf.FrameBuffer(buf, width, height, framebuf.RGB565)

Functions
---------

.. function:: info(data)

   Parse the header of the JPEG image in ``data``, and return its size as a
   ``(width, height)`` tuple.  Raises ``ValueError`` if ``data`` isn't a supported
   JPEG image.

.. function:: decode(data, buf, width, height, format=RGB565, *, x=0, y=0, scale=1, swap=False)

   Decode the JPEG image in ``data`` into ``buf``.

   ``buf``, ``width``, ``height`` and ``format`` describe the destination in the same
   way as for `framebuf.FrameBuffer`.  ``format`` can be `RGB565`, `GS8` or `RGB888`,
   and the ``framebuf`` constants of the same names can be used for the first two.

   - ``x`` and ``y`` give the position of the top left corner of the image in the
     destination.  They can be negative, and any part of the image outside the
     destination is clipped.

   - ``scale`` is 1, 2, 4 or 8, to reduce the size of the image by that factor.
     Pixels are sampled rather than averaged, which is fast but can show aliasing.

   - ``swap`` swaps the bytes of each `RGB565` pixel, as needed by many SPI displays,
     or for `RGB888` swaps the red and blue bytes, to give BGR order.

   Returns the size of the decoded image, after scaling, as a ``(width, height)``
   tuple.  Raises ``ValueError`` if ``data`` isn't a supported JPEG image, and
   ``OSError(ETIMEDOUT)`` if the image is truncated.

Constants
---------

.. data:: RGB565
          GS8
          RGB888

   Output formats: 16-bit RGB565 colour, 8-bit greyscale, and 24-bit colour with a
   byte each for red, green and blue.  `RGB888` isn't a ``framebuf`` format, but is
   the usual input format of image models.
