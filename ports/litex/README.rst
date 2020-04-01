You'll need `dfu-util` to install CircuitPython on the Fomu.

Make sure the foboot bootloader is updated. Instructions are here: https://github.com/im-tomu/fomu-workshop/blob/master/docs/bootloader.rst

Once you've updated the bootloader, you should know how to use `dfu-util`. It's pretty easy!

To install CircuitPython do:

.. code-block:: shell

  dfu-util -D adafruit-circuitpython-fomu-en_US-<version>.dfu

It will install and then restart. CIRCUITPY should appear as it usually does and work the same.
