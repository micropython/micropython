Making the pyboard act as a USB mouse
=====================================

The pyboard is a USB device, and can configured to act as a mouse instead
of the default USB flash drive.

To do this we must first edit the ``boot.py`` file to change the USB
configuration.  If you have not yet touched your ``boot.py`` file then it
will look something like this::

    # boot.py -- run on boot to configure USB and filesystem
    # Put app code in main.py

    import pyb
    #pyb.main('main.py') # main script to run after this one
    #pyb.usb_mode('VCP+MSC') # act as a serial and a storage device
    #pyb.usb_mode('VCP+HID') # act as a serial device and a mouse

To enable the mouse mode, uncomment the last line of the file, to
make it look like::

    pyb.usb_mode('VCP+HID') # act as a serial device and a mouse

If you already changed your ``boot.py`` file, then the minimum code it
needs to work is::

    import pyb
    pyb.usb_mode('VCP+HID')

This tells the pyboard to configure itself as a VCP (Virtual COM Port,
ie serial port) and HID (human interface device, in our case a mouse)
USB device when it boots up.

Eject/unmount the pyboard drive and reset it using the RST switch.
Your PC should now detect the pyboard as a mouse!

Sending mouse events by hand
----------------------------

To get the py-mouse to do anything we need to send mouse events to the PC.
We will first do this manually using the REPL prompt.  Connect to your
pyboard using your serial program and type the following (no need to type
the ``#`` and text following it)::

    >>> hid = pyb.USB_HID()
    >>> hid.send((0, 100, 0, 0)) # (button status, x-direction, y-direction, scroll)

Your mouse should move 100 pixels to the right!  In the command above you
are sending 4 pieces of information: **button status**, **x-direction**, **y-direction**, and **scroll**.  The
number 100 is telling the PC that the mouse moved 100 pixels in the x direction.

Let's make the mouse oscillate left and right::

    >>> import math
    >>> def osc(n, d):
    ...   for i in range(n):
    ...     hid.send((0, int(20 * math.sin(i / 10)), 0, 0))
    ...     pyb.delay(d)
    ...
    >>> osc(100, 50)

The first argument to the function ``osc`` is the number of mouse events to send,
and the second argument is the delay (in milliseconds) between events.  Try
playing around with different numbers.

**Exercise: make the mouse go around in a circle.**

Making a mouse with the accelerometer
-------------------------------------

Now lets make the mouse move based on the angle of the pyboard, using the
accelerometer.  The following code can be typed directly at the REPL prompt,
or put in the ``main.py`` file.  Here, we'll put in in ``main.py`` because to do
that we will learn how to go into safe mode.

At the moment the pyboard is acting as a serial USB device and an HID (a mouse).
So you cannot access the filesystem to edit your ``main.py`` file.

You also can't edit your ``boot.py`` to get out of HID-mode and back to normal
mode with a USB drive...

To get around this we need to go into *safe mode*.  This was described in
the [safe mode tutorial](tut-reset), but we repeat the instructions here:

1. Hold down the USR switch.
2. While still holding down USR, press and release the RST switch.
3. The LEDs will then cycle green to orange to green+orange and back again.
4. Keep holding down USR until *only the orange LED is lit*, and then let
   go of the USR switch.
5. The orange LED should flash quickly 4 times, and then turn off.
6. You are now in safe mode.

In safe mode, the ``boot.py`` and ``main.py`` files are not executed, and so
the pyboard boots up with default settings.  This means you now have access
to the filesystem (the USB drive should appear), and you can edit ``main.py``.
(Leave ``boot.py`` as-is, because we still want to go back to HID-mode after
we finish editing ``main.py``.)

In ``main.py`` put the following code::

    import pyb

    switch = pyb.Switch()
    accel = pyb.Accel()
    hid = pyb.USB_HID()

    while not switch():
        hid.send((0, accel.x(), accel.y(), 0))
        pyb.delay(20)

Save your file, eject/unmount your pyboard drive, and reset it using the RST
switch.  It should now act as a mouse, and the angle of the board will move
the mouse around.  Try it out, and see if you can make the mouse stand still!

Press the USR switch to stop the mouse motion.

You'll note that the y-axis is inverted.  That's easy to fix: just put a
minus sign in front of the y-coordinate in the ``hid.send()`` line above.

Restoring your pyboard to normal
--------------------------------

If you leave your pyboard as-is, it'll behave as a mouse everytime you plug
it in.  You probably want to change it back to normal.  To do this you need
to first enter safe mode (see above), and then edit the ``boot.py`` file.
In the ``boot.py`` file, comment out (put a # in front of) the line with the
``VCP+HID`` setting, so it looks like::

    #pyb.usb_mode('VCP+HID') # act as a serial device and a mouse

Save your file, eject/unmount the drive, and reset the pyboard.  It is now
back to normal operating mode.
