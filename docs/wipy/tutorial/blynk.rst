Getting started with Blynk and the WiPy
---------------------------------------

Blynk is a platform with iOS and Android apps to control
Arduino, Raspberry Pi and the likes over the Internet.
You can easily build graphic interfaces for all your
projects by simply dragging and dropping widgets.

There are several examples available that work out-of-the-box with
the WiPy. Before anything else, make sure that your WiPy is running
the latest software, check :ref:`OTA How-To <wipy_firmware_upgrade>` for instructions.

1. Get the `Blynk library <https://github.com/wipy/wipy/blob/master/lib/blynk/BlynkLib.py>`_ and put it in ``/flash/lib/`` via FTP.
2. Get the `Blynk examples <https://github.com/wipy/wipy/tree/master/examples/blynk>`_, edit the network settings, and afterwards
   upload them to ``/flash/lib/`` via FTP as well.
3. Follow the instructions on each example to setup the Blynk dashboard on your smartphone or tablet.
4. Give it a try, for instance::

   >>> execfile('01_simple.py')
