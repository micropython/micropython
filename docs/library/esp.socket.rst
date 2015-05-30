class socket -- network socket
==============================

``socket`` is an object that represents a network socket. Example usage::

    socket = esp.socket()
    socket.onrecv(print)
    socket.connect(('207.58.139.247', 80))
    socket.send('GET /testwifi/index.html HTTP/1.0\r\n\r\n')

Constructors
------------

.. class:: esp.socket()

    Create and return a socket object.


TCP Methods
-----------

.. method:: socket.connect(addr)

    Connect to the adress and port specified in the ``addr`` tuple.

.. method:: socket.close()

    Close the connection.

.. method:: socket.accept()

    Accept a single connection from the connection queue.

.. method:: socket.listen(backlog)

    Start listening for incoming connections.

    Note: Only one socket can be listening for connections at a time.

.. method:: socket.bind(addr)

    Bind the socket to the address and port specified by the ``addr`` tuple.

.. method:: socket.send(buf)

    Send the bytes from ``buf``.

.. method:: socket.recv()

    Receive and return bytes from the socket.


UDP Methods
-----------

.. method:: socket.sendto(data, addr)

    Placeholder for UDP support, not implemented yet.

.. method:: socket.recvfrom(addr)

    Placeholder for UDP support, not implemented yet.


Callback Setter Methods
-----------------------

.. method:: onconnect(lambda)::

    When connection is established, call the callback ``lambda``.

.. method:: onrecv(lambda)::

    When data is received, call the callback ``lambda``.

.. method:: onsent(lamda)::

    What data is finished sending, call the callback ``lambda``.

.. method:: ondisconnect(lambda)::

    Call the callback ``lambda`` when the connection is closed.
