.. _video_zephyr:

Video and cameras
=================

The video module give access to Zephyr video devices,
to control them and capture image buffers.

A video device existing on zephyr can be instantiated using its device name.

You can add the test video software generator by
appending ``-S video-sw-generator`` while building, then access it:

.. code-block::

   from zephyr import Video
   cam = Video("video_sw_generator")

Controlling the camera
----------------------

Framerate control is done via the frame interval:

.. code-block::

   # Turn 30 FPS to frame interval in microseconds
   cam.frmival(1_000_000 // 30) # set
   cam.frmival() # get

The format list is a 2-dimensional tuple, one row per format capability.

.. code-block::

   # Fields: pixelformat, width min/max/step, height min/max/step
   cam.format_caps()

Pixel format control is done via the four character code:

.. code-block::

   # In order: pixelformat, width, height
   cam.format('YUYV', 640, 480) # set
   cam.format() # get

Stream start/stop control currently only affects output buffers:

.. code-block::

   # Start and stop capture
   cam.start()
   cam.stop()

Video controls are currently limited and require entering the integers:

.. code-block::

   # Exposure control
   cam.control(0x00980900 + 17) # get
   cam.control(0x00980900 + 17, 100) # set

Capturing frames
----------------

The ``.capture()`` method allocates an internal video buffer and fills it with new content.

.. code-block::

   # Update the frame contained in cam
   cam.capture()

The ``.submit()``  method accepts an object with a buffer protocol, and be used to submit data
to input video devices (such as an USB link).

.. code-block::

   # Send data to an UVC device
   uvc.submit(b'\xff\xff\xff...')

It can be accessed through the "buffer protocol", which means that the video device such as ``cam`` can be
directly used as an array of bytes via ``memoryview()`` (zero-copy) or ``bytearray()`` (raw buffer):

.. code-block::

   # Access the first 128 bytes of the frame as a tuple
   memoryview(cam)[0:128]
   bytearray(cam)[0:128]

It is possible to combine ``.capture()`` and ``.submit()`` to forward a frame from a device to another:

.. code-block::

   # Forward the captured frame to another video device
   uvc.submit(cam)
