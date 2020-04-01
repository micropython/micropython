LiteX (FPGA)
============

`LiteX <https://github.com/enjoy-digital/litex>`_ is a Python-based System on a Chip (SoC) designer
for open source supported Field Programmable Gate Array (FPGA) chips. This means that the CPU
core(s) and peripherals are not defined by the physical chip. Instead, they are loaded as separate
"gateware". Once this gateware is loaded, CircuitPython can be loaded on top of it to work as
expected.

Installation
-------------

You'll need ``dfu-util`` to install CircuitPython on the Fomu.

Make sure the foboot bootloader is updated. Instructions are here: https://github.com/im-tomu/fomu-workshop/blob/master/docs/bootloader.rst

Once you've updated the bootloader, you should know how to use ``dfu-util``. It's pretty easy!

To install CircuitPython do:

.. code-block:: shell

  dfu-util -D adafruit-circuitpython-fomu-en_US-<version>.dfu

It will install and then restart. CIRCUITPY should appear as it usually does and work the same.
