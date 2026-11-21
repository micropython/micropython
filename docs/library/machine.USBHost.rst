.. currentmodule:: machine
.. _machine.USBHost:

class USBHost -- USB Host driver
================================

USBHost allows MicroPython to act as a USB host, enumerating devices plugged into
the board's USB host port.  It uses the TinyUSB host stack, on a USB controller that
isn't used by the USB device port that provides the REPL, so both can be used at the
same time.

Availability: boards that enable it, which have a USB host port.  On stm32 these
include the STM32N6570-DK, and the board configuration chooses the controller (see
``MICROPY_HW_USB_HOST`` in ``ports/stm32/mpconfigboard_common.h``).

Class drivers are provided for:

- USB Video Class (UVC) cameras, see `USBHost.UVC`.

Lifecycle
---------

- The host port starts off inactive.  Call ``USBHost().active(True)`` to start it,
  after which attached devices are enumerated in the background.

- A MicroPython :ref:`soft reset <soft_reset>` deactivates the host port, stopping
  any video stream and disconnecting all devices.  The host must be activated again
  after a soft reset.

Constructors
------------

.. class:: USBHost()

   Construct a USBHost object.

   .. note:: This object is a singleton, each call to this constructor
             returns the same object reference.

Methods
-------

.. method:: USBHost.active([value])

   Returns the current active state of the USB host as a boolean.

   If the optional ``value`` argument is set to a truthy value, the host port is
   started and attached devices will be enumerated.  If set to a falsey value, any
   video stream is stopped, devices are disconnected and the port is shut down.

.. method:: USBHost.devices()

   Returns a list of ``(address, vid, pid)`` tuples, one for each enumerated device.

class USBHost.UVC -- USB camera
-------------------------------

Captures video from a USB Video Class (UVC) camera, ie a standard USB webcam.

Video frames are received in the background into a set of three frame buffers: one
being filled, one holding the latest complete frame, and one handed out by `UVC.read()`.
Frames that arrive while the previous one is still being read replace the older
frame, so `UVC.read()` always returns the most recent frame and frames are dropped
rather than queued if they are read too slowly.

Each frame is the complete image in the camera's format, with the UVC protocol
headers removed.  For MJPEG this is a JPEG file, which can be saved, sent over the
network (eg as a ``multipart/x-mixed-replace`` HTTP stream) or decoded.

Limitations:

- One camera is supported at a time.
- Video is streamed over an isochronous endpoint serviced directly by the host
  controller's interrupt handler, which is currently only implemented for the
  Synopsys DWC2 controller in DMA mode.  On stm32 this means a high speed capable
  controller (such as either controller on STM32N6, or OTG_HS on other MCUs), as
  the full speed only OTG_FS controller has no DMA.  `UVC.start()` raises
  ``OSError(EOPNOTSUPP)`` on other controllers.
- Cameras that stream over a bulk endpoint (rather than isochronous) are not
  supported.
- A full speed camera connected through a high speed hub is not supported.

Example::

    from machine import USBHost

    host = USBHost()
    host.active(True)

    cam = USBHost.UVC()  # wait for a camera to be attached
    print(cam.formats())
    print(cam.start("MJPG", width=640, height=480, fps=30))

    frame = cam.read()
    with open("frame.jpg", "wb") as f:
        f.write(frame)

    cam.stop()

.. class:: USBHost.UVC(*, timeout=3000)

   Return the object for the attached camera, waiting up to ``timeout`` milliseconds
   for one to be attached and enumerated.  Raises ``OSError(ENODEV)`` if there is no
   camera.  The host must be active.

   .. note:: This object is a singleton, each call to this constructor
             returns the same object reference.

.. method:: UVC.formats()

   Returns a list of the frame sizes supported by the camera, as
   ``(format, width, height, fps)`` tuples.  ``format`` is a four character
   string such as ``"MJPG"``, ``"YUY2"`` or ``"H264"``, and ``fps`` is a tuple of the
   supported frame rates.

.. method:: UVC.start(format=None, *, width=0, height=0, fps=None, bufsize=0)

   Start streaming video.  If a stream is already running it is stopped first.

   - ``format`` is the format to use, as given by `formats()`.  The default is
     ``"MJPG"`` if the camera supports it, otherwise the camera's first format.

   - ``width`` and ``height`` select the frame size.  If neither is given the
     format's default frame size is used.

   - ``fps`` is the frame rate.  The closest supported rate is used, and if not
     given the camera's default rate for the frame size is used.

   - ``bufsize`` is the size of each of the three frame buffers in bytes.  For
     uncompressed formats it defaults to the frame size, and for compressed formats
     to half the number of pixels (or the camera's maximum frame size if that is
     smaller), which is plenty for typical JPEG frames.  Frames that don't fit in
     the buffer are dropped and counted in `stats()`.

   Returns the format, width, height and frame rate agreed with the camera as a
   ``(format, width, height, fps)`` tuple.

   .. note:: Many cameras lower their frame rate in poor lighting, regardless of
             the requested rate.

.. method:: UVC.stop()

   Stop streaming, and release the frame buffers.

.. method:: UVC.read(timeout=1000)

   Wait for a new frame and return it as a memoryview, or return ``None`` if no frame
   arrives within ``timeout`` milliseconds.  A negative ``timeout`` waits forever.

   The memoryview refers to one of the frame buffers, and its contents are only
   valid until the next call to `read()`, `readinto()` or `stop()`.

   Raises ``OSError(ENODEV)`` if the camera is disconnected, and ``OSError(EPERM)``
   if streaming hasn't been started.

.. method:: UVC.readinto(buf, timeout=1000)

   Like `read()`, but copies the frame into ``buf`` and returns its length, or
   ``None`` on timeout.  Raises ``ValueError`` if the frame doesn't fit in ``buf``.

.. method:: UVC.stats()

   Returns a dictionary of statistics for the current stream:

   - ``frames``: number of complete frames received.
   - ``dropped``: number of frames dropped because they were corrupt or incomplete.
   - ``errors``: number of (micro)frames lost to USB bus errors.
   - ``overflows``: number of frames dropped because they didn't fit in the frame buffer.
   - ``alt``: the alternate setting selected for the stream's USB bandwidth.
   - ``xfer_size``: the maximum number of bytes per USB (micro)frame of that setting.

.. method:: UVC.controls()

   Returns a dictionary of the controls the camera supports, such as focus and
   exposure, mapping the name of each control to a ``(min, max, step, default)`` tuple.
   The possible controls are:

   - From the camera: ``"exposure_auto"``, ``"exposure_priority"``, ``"exposure"``
     (in units of 100us), ``"focus"``, ``"focus_auto"``, ``"iris"``, ``"zoom"`` and
     ``"privacy"``.
   - From the image processing: ``"backlight_compensation"``, ``"brightness"``,
     ``"contrast"``, ``"gain"``, ``"power_line_frequency"`` (0 for off, 1 for 50Hz,
     2 for 60Hz), ``"hue"``, ``"hue_auto"``, ``"saturation"``, ``"sharpness"``,
     ``"gamma"``, ``"white_balance"`` (colour temperature in kelvin) and
     ``"white_balance_auto"``.

   Controls ending ``_auto``, and ``"exposure_priority"``, are on (1) or off (0).  A
   manual setting usually only takes effect when the matching automatic control is off,
   eg ``"focus"`` needs ``"focus_auto"`` to be off.  With ``"exposure_priority"`` on,
   the camera may lower its frame rate to give longer exposures in low light.

   The controls can be used whether or not the camera is streaming.

.. method:: UVC.control(name, [value])

   Get the current value of the control called *name*, or set it if *value* is given.
   Raises ``ValueError`` for an unknown name, and ``OSError(EOPNOTSUPP)`` if the
   camera doesn't support the control.

   Example::

       cam.control("focus_auto", 0)
       cam.control("focus", 100)
