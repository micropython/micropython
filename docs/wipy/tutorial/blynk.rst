Getting started with Blynk and the WiPy
---------------------------------------

Blynk provides iOS and Android apps to control any hardware over the Internet
or directly using Bluetooth. You can easily build graphic interfaces for all
your projects by simply dragging and dropping widgets, right on your smartphone.

Before anything else, make sure that your WiPy is running
the latest software, check :ref:`OTA How-To <wipy_firmware_upgrade>` for instructions.

1. Get the `Blynk library <https://github.com/vshymanskyy/blynk-library-python/blob/master/BlynkLib.py>`_ and put it in ``/flash/lib/`` via FTP.
2. Get the `Blynk example for WiPy <https://github.com/vshymanskyy/blynk-library-python/blob/master/examples/hardware/PyCom_WiPy.py>`_, edit the network settings, and afterwards
   upload it to ``/flash/`` via FTP as well.
3. Follow the instructions on each example to setup the Blynk dashboard on your smartphone or tablet.
4. Give it a try, for instance::

   >>> execfile('sync_virtual.py')
