:mod:`esp` --- functions related to the ESP8266
===============================================

.. module:: esp
    :synopsis: functions related to the ESP8266

The ``esp`` module contains specific functions related to the ESP8266 module.


Functions
---------

.. function:: scan(cb)

    Initiate scanning for the available wireless networks.

    Scanning is only possible if the radio is in station or station+AP mode; if
    called while in AP only mode, an exception will be raised.

    Once the scanning is complete, the provided callback function ``cb`` will
    be called once for each network found, and passed a tuple with information
    about that network:

        (ssid, bssid, channel, RSSI, authmode, hidden)

    There are five values for authmode:

        * 0 -- open
        * 1 -- WEP
        * 2 -- WPA-PSK
        * 3 -- WPA2-PSK
        * 4 -- WPA/WPA2-PSK

    and two for hidden:

        * 0 -- visible
        * 1 -- hidden

.. function:: status()

    Return the current status of the wireless connection.

    The possible statuses are defined as constants:

        * ``STAT_IDLE`` -- no connection and no activity,
        * ``STAT_CONNECTING`` -- connecting in progress,
        * ``STAT_WRONG_PASSWORD`` -- failed due to incorrect password,
        * ``STAT_NO_AP_FOUND`` -- failed because no access point replied,
        * ``STAT_CONNECT_FAIL`` -- failed due to other problems,
        * ``STAT_GOT_IP`` -- connection susccessful.

.. function:: getaddrinfo((hostname, port, lambda))

    Initiate resolving of the given hostname.

    When the hostname is resolved, the provided ``lambda`` callback will be
    called with two arguments, first being the hostname being resolved,
    second a tuple with information about that hostname.

Classes
-------

.. toctree::
    :maxdepth: 1

    esp.socket.rst
