ubluepy libraries for nRF52832
*****

.. image:: img/nRF52.JPG
    :alt: nRF52832-Bluefruit52 board
    :width: 640px

ubluepy
================
.. py:module:: ubluepy

This is the ``ubluepy`` module. Micropython Bluetooth LE API for nrf5x device series. You can use it to connect a bluetooth device to your board. This module provide some BLE control functions.

To access this module you need to::

    import ubluepy

.. py:function:: constants

They are:

    * ``constants.EVT_GAP_CONNECTED`` - value is 16.
    * ``constants.EVT_GAP_DISCONNECTED`` - value is 17.
    * ``constants.EVT_GATTS_WRITE`` - value is 80.
    * ``constants.UUID_CCCD`` - value is 0x2902.
    * ``constants.ADDR_TYPE_PUBLIC`` - value is 0.
    * ``constants.ADDR_TYPE_RANDOM_STATIC`` - value is 1.

We assume you have done this for the examples below.

UUID Class
=========

Functions
-----------------

.. py:function:: UUID("0x181A")

    # Environmental Sensing service.

.. py:function:: UUID.binVal()

    Get binary value of the 16 or 128 bit UUID. Returned as bytearray type.

Example
-------

Examples for UUID::

    from ubluepy import Service, Characteristic, UUID, Peripheral, constants

    uuid_env_sense = UUID("0x181A") # Environmental Sensing service
    uuid_temp = UUID("0x2A6E") # Temperature characteristic


Service Class
=========

Functions
-----------------

.. py:function:: Service(UUID("0x181A"))

    New a Service object.

.. py:function:: Service.getCharacteristics()

    Return list with all characteristics registered in the Service.

.. py:function:: Service.getCharacteristic(UUID)

    Return Characteristic with the given UUID.

.. py:function:: Service.addCharacteristic(Characteristic)

    Add Characteristic to the Service.

.. py:function:: Service.uuid()
    
    Get UUID instance of the Service.

Constants they are:

    * ``Service.PRIMARY``.
    * ``Service.SECONDARY``

Example
-------

Examples for Service::

    from ubluepy import Service, Characteristic, UUID, Peripheral, constants

    uuid_env_sense = UUID("0x181A") # Environmental Sensing service
    uuid_temp = UUID("0x2A6E") # Temperature characteristic

    serv_env_sense = Service(uuid_env_sense)

    temp_props = Characteristic.PROP_NOTIFY | Characteristic.PROP_READ
    temp_attrs = Characteristic.ATTR_CCCD
    char_temp = Characteristic(uuid_temp, props = temp_props, attrs = temp_attrs)

    serv_env_sense.addCharacteristic(char_temp)

Scanner Class
=========

Functions
-----------------

.. py:function:: Scanner()

    New a Scanner object.

.. py:function:: Scanner.scan(time_out)

    Scan for devices. Timeout is in milliseconds and will set the duration of the scanning.

Example
-------

Examples for Scanner::

    from ubluepy import Scanner, Peripheral, constants

    s = Scanner()
    scan_res = s.scan(500)

ScanEntry Class
=========

Functions
-----------------

.. py:function:: ScanEntry()

    New a ScanEntry object.

.. py:function:: ScanEntry.addr()

    Return address as text string.

.. py:function:: ScanEntry.addr_type()

    Return address type value.

.. py:function:: ScanEntry.rssi()
    
    Return RSSI value.

.. py:function:: ScanEntry.getScanData()

    Return list of the scan data tupples (ad_type, description, value).

Peripheral Class
=========

Functions
-----------------


.. py:function:: Peripheral()

    New a Peripheral object.

.. py:function:: Peripheral.withDelegate(DefaultDelegate)

    Set delegate instance for handling Bluetooth LE events.

.. py:function:: Peripheral.setNotificationHandler(func)

    Set handler for Bluetooth LE notification events.

.. py:function:: Peripheral.setConnectionHandler(func)

    Set handler for Bluetooth LE connection events.

.. py:function:: Peripheral.advertise(device_name, [service=[service1, service2, ...]], [data=bytearray], [connectable=True])

    Start advertising. Connectable advertisment type by default.

.. py:function:: Peripheral.advertise_stop()

    Stop advertisment if any onging advertisment.

.. py:function:: Peripheral.disconnect()

    disconnect connection.

.. py:function:: Peripheral.addService(Service)

    Add service to the Peripheral.

.. py:function:: Peripheral.getServices()

    Return list with all service registered in the Peripheral.

.. py:function:: Peripheral.connect(device_address [, addr_type=ADDR_TYPE_PUBLIC])

    Connect to device peripheral with the given device address. addr_type can be either ADDR_TYPE_PUBLIC (default) or ADDR_TYPE_RANDOM_STATIC.

Example
-------

Examples for Peripheral::

    from ubluepy import Service, Characteristic, UUID, Peripheral, constants

    uuid_env_sense = UUID("0x181A") # Environmental Sensing service
    uuid_temp = UUID("0x2A6E") # Temperature characteristic

    serv_env_sense = Service(uuid_env_sense)

    temp_props = Characteristic.PROP_NOTIFY | Characteristic.PROP_READ
    temp_attrs = Characteristic.ATTR_CCCD
    char_temp = Characteristic(uuid_temp, props = temp_props, attrs = temp_attrs)

    serv_env_sense.addCharacteristic(char_temp)

    periph = Peripheral()
    periph.addService(serv_env_sense)
    periph.setConnectionHandler(event_handler)
    periph.advertise(device_name="micr_temp", services=[serv_env_sense])

DefaultDelegate Class
=========

Functions
-----------------

.. py:function:: DefaultDelegate()

    New a DefaultDelegate object.

.. py:function:: DefaultDelegate.handleConnection()

    Handle connection events.

.. py:function:: DefaultDelegate.handleNotification()

    Handle notification events.

Characteristic Class
=========

Functions
-----------------

.. py:function:: Characteristic(uuid,props,attrs)

    New a Characteristic object.

.. py:function:: Characteristic.read()

    Read Characteristic value.

.. py:function:: Characteristic.write(data, [with_response=False])

    Write Characteristic value.

.. py:function:: Characteristic.properties()

    Read Characteristic value properties.

.. py:function:: Characteristic.uuid()
    
    Get UUID instance of the characteristic.

Constants they are:

    * ``Characteristic.PROP_BROADCAST``
    * ``Characteristic.PROP_READ``
    * ``Characteristic.PROP_WRITE_WO_RESP``
    * ``Characteristic.PROP_WRITE``
    * ``Characteristic.PROP_NOTIFY``
    * ``Characteristic.PROP_INDICATE``
    * ``Characteristic.PROP_AUTH_SIGNED_WR``
    * ``Characteristic.ATTR_CCCD``
    * ``Characteristic.AUTH_SIGNED_WR``

Example
-------

Examples for Characteristic::

    from ubluepy import Service, Characteristic, UUID, Peripheral, constants

    uuid_env_sense = UUID("0x181A") # Environmental Sensing service
    uuid_temp = UUID("0x2A6E") # Temperature characteristic

    serv_env_sense = Service(uuid_env_sense)

    temp_props = Characteristic.PROP_NOTIFY | Characteristic.PROP_READ
    temp_attrs = Characteristic.ATTR_CCCD
    char_temp = Characteristic(uuid_temp, props = temp_props, attrs = temp_attrs)

    serv_env_sense.addCharacteristic(char_temp)

    periph = Peripheral()
    periph.addService(serv_env_sense)
    periph.setConnectionHandler(event_handler)
    periph.advertise(device_name="micr_temp", services=[serv_env_sense])

