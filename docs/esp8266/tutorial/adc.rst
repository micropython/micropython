Analog to Digital Conversion
============================

The ESP8266 has a single pin (separate to the GPIO pins) which can be used to
read analog voltages and convert them to a digital value.  You can construct
such an ADC pin object using::

    >>> import machine
    >>> adc = machine.ADC(0)

Then read its value with::

    >>> adc.read()
    58

The values returned from the ``read()`` function are between 0 (for 0.0 volts)
and 1024 (for 1.0 volts).  Please note that this input can only tolerate a
maximum of 1.0 volts and you must use a voltage divider circuit to measure
larger voltages.
