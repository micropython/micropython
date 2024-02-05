Network basics
==============

The network module is used to configure the WiFi connection.  There are two WiFi
interfaces, one for the station (when the ESP8266 connects to a router) and one
for the access point (for other devices to connect to the ESP8266).  Create
instances of these objects using::

    >>> import network
    >>> sta_if = network.WLAN(network.STA_IF)
    >>> ap_if = network.WLAN(network.AP_IF)

You can check if the interfaces are active by::

    >>> sta_if.active()
    False
    >>> ap_if.active()
    True

You can also check the network settings of the interface by::

    >>> ap_if.ifconfig()
    ('192.168.4.1', '255.255.255.0', '192.168.4.1', '8.8.8.8')

The returned values are: IP address, netmask, gateway, DNS.

Configuration of the WiFi
-------------------------

Upon a fresh install the ESP8266 is configured in access point mode, so the
AP_IF interface is active and the STA_IF interface is inactive.  You can
configure the module to connect to your own network using the STA_IF interface.

First activate the station interface::

    >>> sta_if.active(True)

Then connect to your WiFi network::

    >>> sta_if.connect('<your SSID>', '<your key>')

To check if the connection is established use::

    >>> sta_if.isconnected()

Once established you can check the IP address::

    >>> sta_if.ifconfig()
    ('192.168.0.2', '255.255.255.0', '192.168.0.1', '8.8.8.8')

You can then disable the access-point interface if you no longer need it::

    >>> ap_if.active(False)

Here is a function you can run (or put in your boot.py file) to automatically
connect to your WiFi network::

    def do_connect():
        import network
        sta_if = network.WLAN(network.STA_IF)
        if not sta_if.isconnected():
            print('connecting to network...')
            sta_if.active(True)
            sta_if.connect('<ssid>', '<key>')
            while not sta_if.isconnected():
                pass
        print('network config:', sta_if.ifconfig())

Sockets
-------

Once the WiFi is set up the way to access the network is by using sockets.
A socket represents an endpoint on a network device, and when two sockets are
connected together communication can proceed.
Internet protocols are built on top of sockets, such as email (SMTP), the web
(HTTP), telnet, ssh, among many others.  Each of these protocols is assigned
a specific port, which is just an integer.  Given an IP address and a port
number you can connect to a remote device and start talking with it.

The next part of the tutorial discusses how to use sockets to do some common
and useful network tasks.
