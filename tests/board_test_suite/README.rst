
Introduction
============

Board test suite for CircuitPython. Run these tests to ensure that a CircuitPython port was created correctly, individual pin mappings are correct, and buses (e.g. SPI) work.

Tests can be run individually. Copy code found in each *<name>_test.py* module (found in *source* directory) to main.py in your CIRCUITPYTHON device drive.

Alternatively, tests can be imported as modules. Copy the *lib* directory to CIRCUITPYTHON device drive and import the test in your own code. Each test can be run with the `run_test(pins)` function.

The *main.py* example shows how to call tests from within a script. *main.py* runs the following tests:

 * LED Test
 * GPIO Test
 * Voltage Monitor Test
 * UART Test
 * SPI Test
 * I2C Test
 
Dependencies
=============

This test suite depends on:

* `Adafruit CircuitPython <https://github.com/adafruit/circuitpython>`_
* `SD Card <https://github.com/adafruit/Adafruit_CircuitPython_SD>`_
* `Bus Device <https://github.com/adafruit/Adafruit_CircuitPython_BusDevice>`_

Please ensure all dependencies are available on the CircuitPython filesystem.
This is easily achieved by downloading
`the Adafruit library and driver bundle <https://github.com/adafruit/Adafruit_CircuitPython_Bundle>`_.

Usage Example
=============

You will need the following components:

* Multimeter
* LED
* 1x 330 Ohm resistor
* 2x 4.7k Ohm resistor
* Microchip 25AA040A SPI EEPROM
* Microchip AT24HC04B I2C EEPROM
* Breadboard
* Wires

Connect the components as shown to your board.

![Test jig Fritzing diagram](doc/test_jig.png)

Copy the *lib* folder to the CIRCUITPYTHON drive. Copy *main.py* to the root directory of your CIRCUITPYTHON drive. Open a Serial terminal and connect to the board. Follow the directions given to run through the tests.

Building
========

Individual test modules can be built with the mpy-cross cross-compiler. This is required to save RAM space if you plan to run more than one test at a time. See [the mpy-cross directory in circuitpython](https://github.com/adafruit/circuitpython/tree/master/mpy-cross) to learn more.
