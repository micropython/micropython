.. _usbd_zephyr:

USB Device
==========

USB device classes can be instantiated at buildtime as documented by Zephyr,
then configured and accessed at runtime.

USB will be enabled on boot to allow CDC ACM interfaces to be used for logs and REPL access,
which prevents customization of USB descriptors.
To avoid this, set the ``MICROPY_ZEPHYR_USBD_RUNTIME`` Kconfig option.

Then, USB support can be controlled using a similar API to other ports:

.. code-block::

   from zephyr import USBDevice
   usbd = USBDevice()

Before calling ``usbd.active(True)`` USB classes can be configured which will affect the USB descriptors.

To customize USB descriptors, ``usbd.configure()`` needs to be called before ``usbd.active(True)``.

UVC Support
-----------

Configuring UVC requires to prepare a list of formats and a video device to use.

.. code-block::

   from zephyr import Video
   cam = Video("video_sw_generator")

For adding a format table that will be listed by UVC:

.. code-block::

   usbd.config(uvc=True, uvc_source="video_sw_generator", uvc_formats=(('YUYV', 640, 480),))

Then the USB device stack can be activated:

.. code-block::

   usbd.active(True)

A camera device should now be detected.

You can now start to read frames from this device using a webcam viewer application.

UVC itself is a video device with the name ``uvc0``:

.. code-block::

   uvc = Video("uvc0")

Querying the formats will return an empty format, until the host selects one:

.. code-block::

   uvc.format()

Then, it is possible to use this format to configure the video back-end of choice:

.. code-block::

   cam.format(*uvc.format())
   cam.frmival(uvc.frmival())
   cam.start()

Then capturing frames from the camera and sending then happen in a loop:

.. code-block::

   while True:
       cam.capture()
       uvc.submit(cam)
