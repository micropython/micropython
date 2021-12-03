.. _intro_zephyr:

Getting started with MicroPython on the Zephyr port
===================================================

Let’s get started!

Requirements
------------

To use the MicroPython Zephyr port, you will need a Zephyr supported board (for a list of acceptable
boards see :ref:`zephyr_general`).

Powering up
-----------

If your board has a USB connector on it then most likely it is powered
through this when connected to your PC. Otherwise you will need to power
it directly. Please refer to the documentation for your board for
further details.

Getting and deploying the firmware
----------------------------------

The first step you will need to do is either clone the `MicroPython repository <https://github.com/micropython/micropython.git>`_
or download it from the `MicroPython downloads page <http://micropython.org/download>`_. If you are an end user of MicroPython,
it is recommended to start with the stable firmware builds. If you would like to work on development, you may follow the daily
builds on git.

Next, follow the Zephyr port readme document (``ports/zephyr/README.md``) to build and run the application on your board.
