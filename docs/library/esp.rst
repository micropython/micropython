:mod:`esp` --- functions related to the ESP8266
===============================================

.. module:: esp
    :synopsis: functions related to the ESP8266

The ``esp`` module contains specific functions related to the ESP8266 module.


Functions
---------

.. function:: sleep_type([sleep_type])

    Get or set the sleep type.

    If the ``sleep_type`` parameter is provided, sets the sleep type to its
    value. If the function is called without parameters, returns the current
    sleep type.

    The possible sleep types are defined as constants:

        * ``SLEEP_NONE`` -- all functions enabled,
        * ``SLEEP_MODEM`` -- modem sleep, shuts down the WiFi Modem circuit.
        * ``SLEEP_LIGHT`` -- light sleep, shuts down the WiFi Modem circuit
          and suspends the processor periodically.

    The system enters the set sleep mode automatically when possible.

.. function:: deepsleep(time=0)

    Enter deep sleep.

    The whole module powers down, except for the RTC clock circuit, which can
    be used to restart the module after the specified time if the pin 16 is
    connected to the reset pin. Otherwise the module will sleep until manually
    reset.

.. function:: flash_id()

    Read the device ID of the flash memory.

.. function:: flash_read(byte_offset, length_or_buffer)

.. function:: flash_write(byte_offset, bytes)

.. function:: flash_erase(sector_no)
