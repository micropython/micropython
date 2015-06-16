:mod:`esp` --- functions related to the ESP8266
===============================================

.. module:: esp
    :synopsis: functions related to the ESP8266

The ``esp`` module contains specific functions related to the ESP8266 module.


Functions
---------

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
