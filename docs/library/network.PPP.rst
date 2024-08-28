.. currentmodule:: network
.. _network.PPP:

class PPP -- create network connections over serial PPP
=======================================================

This class allows you to create a network connection over a serial port using
the PPP protocol.  It is only available on selected ports and boards.

Example usage::

    import network

    ppp = network.PPP(uart)
    ppp.connect()

    while not ppp.isconnected():
        pass

    print(ppp.ipconfig("addr4"))

    # use the socket module as usual, etc

    ppp.disconnect()

Constructors
------------

.. class:: PPP(stream)

   Create a PPP driver object.

   Arguments are:

     - *stream* is any object that supports the stream protocol, but is most commonly a
       :class:`machine.UART` instance.  This stream object must have an ``irq()`` method
       and an ``IRQ_RXIDLE`` constant, for use by `PPP.connect`.

Methods
-------

.. method:: PPP.connect(security=SEC_NONE, user=None, key=None)

   Initiate a PPP connection with the given parameters:

     - *security* is the type of security, either ``PPP.SEC_NONE``, ``PPP.SEC_PAP``,
       or ``PPP.SEC_CHAP``.
     - *user* is an optional user name to use with the security mode.
     - *key* is an optional password to use with the security mode.

   When this method is called the underlying stream has its interrupt configured to call
   `PPP.poll` via ``stream.irq(ppp.poll, stream.IRQ_RXIDLE)``.  This makes sure the
   stream is polled, and data passed up the PPP stack, wheverver data becomes available
   on the stream.

   The connection proceeds asynchronously, in the background.

.. method:: PPP.disconnect()

   Terminate the connection.  This must be called to cleanly close the PPP connection.

.. method:: PPP.isconnected()

   Returns ``True`` if the PPP link is connected and up.
   Returns ``False`` otherwise.

.. method:: PPP.status()

   Returns the PPP status.

.. method:: PPP.config(config_parameters)

   Sets or gets parameters of the PPP interface. There are currently no parameter that
   can be set or retrieved.

.. method:: PPP.ipconfig('param')
            PPP.ipconfig(param=value, ...)

   See `AbstractNIC.ipconfig`.

.. method:: PPP.ifconfig([(ip, subnet, gateway, dns)])

   See `AbstractNIC.ifconfig`.

.. method:: PPP.poll()

   Poll the underlying stream for data, and pass it up the PPP stack.
   This is called automatically if the stream is a UART with a RXIDLE interrupt,
   so it's not usually necessary to call it manually.

Constants
---------

.. data:: PPP.SEC_NONE
          PPP.SEC_PAP
          PPP.SEC_CHAP

    The type of connection security.
