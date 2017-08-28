Select
=====

About
-----

Setup three channels, add them to a :class:`poll object<select.select.poll>` and wait for events to
occur.

Three channels are polled:

1. An :class:`event channel<sync.sync.Event>` that waits for a button
   to be pressed.
2. A :class:`queue channel<sync.sync.Queue>` that the string "foo" is
   written to in the script.
3. A :class:`socket channel<socket.socket.SocketType>` waiting for UDP
   packets.

NOTE: Change the UDP configuration to match your setup.

Source code
-----------

.. include:: select/source-code.rst

The source code can also be found on Github in the
:github-tree:`examples/select` folder.

Build and run
-------------

Build and upload the application.

.. code-block:: text

   $ cd examples/blink
   $ make -s BOARD=esp12e CDEFS_EXTRA="CONFIG_START_NETWORK_INTERFACE_WIFI_SSID=ssid CONFIG_START_NETWORK_INTERFACE_WIFI_PASSWORD=password" run
   ...
   queue: b'foo'

At this point the application is waiting for an event to occur. Send a
UDP packet to it from your PC using Python.

.. code-block:: python

   >>> import socket
   >>> udp = socket.socket(type=socket.SOCK_DGRAM)
   >>> udp.sendto('bar', ('192.168.1.103', 30303))

The written packet is received by the application and printed.
   
.. code-block:: text

   udp: b'bar'
