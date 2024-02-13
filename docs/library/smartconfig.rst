:mod:`smartconfig` --- support for the SmartConfig provisioning protocol
========================================================================

.. module:: smartconfig
    :synopsis: SmartConfig provisioning protocol support

This module provides an interface to the SmartConfig provisioning protocol
provided by Espressif on ESP32 devices (`API docs <https://docs.espressif.com/
projects/esp-idf/en/latest/esp32/api-reference/network/esp_smartconfig.html>`_).

Introduction
------------

The SmartConfig\ :sup:`TM` is a provisioning technology developed by TI to
connect a new Wi-Fi device to a Wi-Fi network. It uses a mobile application to
broadcast the network credentials from a smartphone, or a tablet, to an
un-provisioned Wi-Fi device.

The advantage of this technology is that the device does not need to directly
know SSID or password of an Access Point (AP). This information is provided
using the smartphone. This is particularly important to headless device and
systems, due to their lack of a user interface.

A simple example would be::

    import network, time
    import smartconfig

    # A WLAN interface must be instancing to start smartconfig progress
    sta = network.WLAN(network.STA_IF)
    sta.active(True)

    smartconfig.type(smartconfig.TYPE_ESPTOUCH_AIRKISS)

    smartconfig.start()

    while not smartconfig.done():
        time.sleep_ms(100)

    print(smartconfig.info())
    # (ssid, password, bssid, type[, rvd_data])

    smartconfig.stop()

Functions
---------

.. function:: start()

  Start the SmartConfig process.

  After received the broadcast data, it will try to use the data to connect
  to the AP, the ``smartconfig.done()`` function returns ``True`` regardless
  of whether the connection is successful or not.

  **Note:** This function automatically calls the ``smartconfig.stop()``
  function first.

.. function:: stop()

  Stop the SmartConfig process.

.. function:: done()

  Returns ``True`` if the SmartConfig process completed, and ``False`` for
  otherwise.

  **Note:** If returns ``True``, it will automatically calls the
  ``smartconfig.stop()`` function.

.. function:: info()

  Returns a 4 or 5-tuple ``(ssid, password, bssid, type[, rvd_data])``.

  **Note:** ``rvd_data`` is the custom data for EspTouch V2 protocol type.

.. function:: ssid()

  Returns the received ``ssid`` as ``str``.

.. function:: password()

  Returns the received ``password`` as ``str``.

.. function:: bssid()

  Returns the received ``bssid`` as ``bytes``.

.. function:: type([type])

  Get or set SmartConfig protocol *type*.

  You must set a protocol *type* for receiving data that broadcast by your phone
  over the same protocol, usually set to ``smartconfig.TYPE_ESPTOUCH_AIRKISS``,
  which can cover mostly of the application scenarios.

  When SmartConfig process done, you can get the protocol type used by the
  phone.

.. function:: rvd_data()

  Returns the received ``rvd_data`` as ``bytes``.

Constants
---------

.. data:: TYPE_ESPTOUCH
          TYPE_AIRKISS
          TYPE_ESPTOUCH_AIRKISS
          TYPE_ESPTOUCH_V2

   SmartConfig protocol types.
