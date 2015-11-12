:mod:`esp` --- functions related to the ESP8266
===============================================

.. module:: esp
    :synopsis: functions related to the ESP8266

The ``esp`` module contains specific functions related to the ESP8266 module.


Functions
---------

.. function:: mac([address])

    Get or set the network interface's MAC address.

    If the ``address`` parameter is provided, sets the address to its value. If
    the function is called wihout parameters, returns the current address.

.. function:: getaddrinfo((hostname, port, lambda))

    Initiate resolving of the given hostname.

    When the hostname is resolved, the provided ``lambda`` callback will be
    called with two arguments, first being the hostname being resolved,
    second a tuple with information about that hostname.

.. function:: wifi_mode([mode])

    Get or set the wireless network operating mode.

    If the ``mode`` parameter is provided, sets the mode to its value. If
    the function is called wihout parameters, returns the current mode.

    The possible modes are defined as constants:

        * ``STA_MODE`` -- station mode,
        * ``AP_MODE`` -- software access point mode,
        * ``STA_AP_MODE`` -- mixed station and software access point mode.

.. function:: phy_mode([mode])

    Get or set the network interface mode.

    If the ``mode`` parameter is provided, sets the mode to its value. If
    the function is called wihout parameters, returns the current mode.

    The possible modes are defined as constants:
        * ``MODE_11B`` -- IEEE 802.11b,
        * ``MODE_11G`` -- IEEE 802.11g,
        * ``MODE_11N`` -- IEEE 802.11n.

.. function:: sleep_type([sleep_type])

    Get or set the sleep type.

    If the ``sleep_type`` parameter is provided, sets the sleep type to its
    value. If the function is called wihout parameters, returns the current
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

Classes
-------

.. toctree::
    :maxdepth: 1

    esp.socket.rst
