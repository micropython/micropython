class CAN -- controller area network communication bus
======================================================

CAN implements the standard CAN communications protocol.  At
the physical level it consists of 2 lines: RX and TX.  Note that
to connect the pyboard to a CAN bus you must use a CAN transceiver
to convert the CAN logic signals from the pyboard to the correct
voltage levels on the bus.

Note that this driver does not yet support filter configuration
(it defaults to a single filter that lets through all messages),
or bus timing configuration (except for setting the prescaler).

Example usage (works without anything connected)::

    from pyb import CAN
    can = pyb.CAN(1, pyb.CAN.LOOPBACK)
    can.send('message!', 123)   # send message to id 123
    can.recv(0)                 # receive message on FIFO 0


Constructors
------------

.. class:: pyb.CAN(bus, ...)

   Construct a CAN object on the given bus.  ``bus`` can be 1-2, or 'YA' or 'YB'.
   With no additional parameters, the CAN object is created but not
   initialised (it has the settings from the last initialisation of
   the bus, if any).  If extra arguments are given, the bus is initialised.
   See ``init`` for parameters of initialisation.
   
   The physical pins of the CAN busses are:
   
     - ``CAN(1)`` is on ``YA``: ``(RX, TX) = (Y3, Y4) = (PB8, PB9)``
     - ``CAN(2)`` is on ``YB``: ``(RX, TX) = (Y5, Y6) = (PB12, PB13)``


Methods
-------

.. method:: can.init(mode, extframe=False, prescaler=100, \*, sjw=1, bs1=6, bs2=8)

   Initialise the CAN bus with the given parameters:
   
     - ``mode`` is one of:  NORMAL, LOOPBACK, SILENT, SILENT_LOOPBACK

   If ``extframe`` is True then the bus uses extended identifiers in the frames (29 bits).
   Otherwise it uses standard 11 bit identifiers.

.. method:: can.deinit()

   Turn off the CAN bus.

.. method:: can.any(fifo)

   Return ``True`` if any message waiting on the FIFO, else ``False``.

.. method:: can.recv(fifo, \*, timeout=5000)

   Receive data on the bus:
   
     - ``fifo`` is an integer, which is the FIFO to receive on
     - ``timeout`` is the timeout in milliseconds to wait for the receive.
   
   Return value: buffer of data bytes.

.. method:: can.send(send, addr, \*, timeout=5000)

   Send a message on the bus:
   
     - ``send`` is the data to send (an integer to send, or a buffer object).
     - ``addr`` is the address to send to
     - ``timeout`` is the timeout in milliseconds to wait for the send.
   
   Return value: ``None``.


Constants
---------

.. data:: CAN.NORMAL
.. data:: CAN.LOOPBACK
.. data:: CAN.SILENT
.. data:: CAN.SILENT_LOOPBACK

   the mode of the CAN bus
