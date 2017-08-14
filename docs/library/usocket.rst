*******************************
:mod:`usocket` -- socket module
*******************************

.. module:: usocket
   :synopsis: socket module

This module provides access to the BSD socket interface.

See corresponding `CPython module <https://docs.python.org/3/library/socket.html>`_ for
comparison.

Socket address format(s)
------------------------

Functions below which expect a network address, accept it in the format of
`(ipv4_address, port)`, where `ipv4_address` is a string with dot-notation numeric
IPv4 address, e.g. ``"8.8.8.8"``, and port is integer port number in the range
1-65535. Note the domain names are not accepted as `ipv4_address`, they should be
resolved first using ``socket.getaddrinfo()``.

Functions
---------

.. function:: socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)

   Create a new socket using the given address family, socket type and protocol number.

    .. only:: port_wipy

        .. note::

           SSL sockets need to be created the following way before wrapping them with 
           ``ssl.wrap_socket``::

              import socket
              import ssl
              s = socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_SEC)
              ss = ssl.wrap_socket(s)

.. function:: socket.getaddrinfo(host, port)

   Translate the host/port argument into a sequence of 5-tuples that contain all the 
   necessary arguments for creating a socket connected to that service. The list of 
   5-tuples has following structure::

      (family, type, proto, canonname, sockaddr)

   The following example shows how to connect to a given url::

      s = socket.socket()
      s.connect(socket.getaddrinfo('www.micropython.org', 80)[0][-1])

.. only:: port_wipy

    Exceptions
    ----------

    .. data:: socket.error
    .. data:: socket.timeout

Constants
---------

.. data:: socket.AF_INET

   family types

.. data:: socket.SOCK_STREAM
.. data:: socket.SOCK_DGRAM

   socket types

.. data:: socket.IPPROTO_UDP
.. data:: socket.IPPROTO_TCP
.. only:: port_wipy

   .. data:: socket.IPPROTO_SEC

      protocol numbers

class socket
============

Methods
-------

    .. method:: socket.close

       Mark the socket closed. Once that happens, all future operations on the socket 
       object will fail. The remote end will receive no more data (after queued data is flushed).

       Sockets are automatically closed when they are garbage-collected, but it is recommended 
       to close() them explicitly, or to use a with statement around them.

    .. method:: socket.bind(address)

       Bind the socket to address. The socket must not already be bound.

    .. method:: socket.listen([backlog])

       Enable a server to accept connections. If backlog is specified, it must be at least 0 
       (if it's lower, it will be set to 0); and specifies the number of unaccepted connections
       that the system will allow before refusing new connections. If not specified, a default
       reasonable value is chosen.

    .. method:: socket.accept()

       Accept a connection. The socket must be bound to an address and listening for connections.
       The return value is a pair (conn, address) where conn is a new socket object usable to send
       and receive data on the connection, and address is the address bound to the socket on the
       other end of the connection.

    .. method:: socket.connect(address)

       Connect to a remote socket at address.

    .. method:: socket.send(bytes)

       Send data to the socket. The socket must be connected to a remote socket.

    .. method:: socket.sendall(bytes)

       Send data to the socket. The socket must be connected to a remote socket.

    .. method:: socket.recv(bufsize)

       Receive data from the socket. The return value is a bytes object representing the data
       received. The maximum amount of data to be received at once is specified by bufsize.

    .. method:: socket.sendto(bytes, address)

       Send data to the socket. The socket should not be connected to a remote socket, since the
       destination socket is specified by `address`.

    .. method:: socket.recvfrom(bufsize)

      Receive data from the socket. The return value is a pair (bytes, address) where bytes is a
      bytes object representing the data received and address is the address of the socket sending
      the data.

    .. method:: socket.setsockopt(level, optname, value)

       Set the value of the given socket option. The needed symbolic constants are defined in the
       socket module (SO_* etc.). The value can be an integer or a bytes-like object representing
       a buffer.

    .. method:: socket.settimeout(value)

       Set a timeout on blocking socket operations. The value argument can be a nonnegative floating
       point number expressing seconds, or None. If a non-zero value is given, subsequent socket operations
       will raise a timeout exception if the timeout period value has elapsed before the operation has
       completed. If zero is given, the socket is put in non-blocking mode. If None is given, the socket
       is put in blocking mode.

    .. method:: socket.setblocking(flag)

       Set blocking or non-blocking mode of the socket: if flag is false, the socket is set to non-blocking,
       else to blocking mode.

       This method is a shorthand for certain ``settimeout()`` calls::

          sock.setblocking(True) is equivalent to sock.settimeout(None)
          sock.setblocking(False) is equivalent to sock.settimeout(0.0)

    .. method:: socket.makefile(mode='rb')

       Return a file object associated with the socket. The exact returned type depends on the arguments
       given to makefile(). The support is limited to binary modes only ('rb' and 'wb').
       CPython's arguments: ``encoding``, ``errors`` and ``newline`` are not supported.

       The socket must be in blocking mode; it can have a timeout, but the file object’s internal buffer
       may end up in a inconsistent state if a timeout occurs.

       .. admonition:: Difference to CPython
          :class: attention

          Closing the file object returned by makefile() WILL close the
          original socket as well.

    .. method:: socket.read(size)

       Read up to size bytes from the socket. Return a bytes object. If ``size`` is not given, it
       behaves just like ``socket.readall()``, see below.

    .. method:: socket.readall()

       Read all data available from the socket until ``EOF``. This function will not return until
       the socket is closed.

    .. method:: socket.readinto(buf[, nbytes])

       Read bytes into the ``buf``.  If ``nbytes`` is specified then read at most
       that many bytes.  Otherwise, read at most ``len(buf)`` bytes.

       Return value: number of bytes read and stored into ``buf``.

    .. method:: socket.readline()

       Read a line, ending in a newline character.

       Return value: the line read.

    .. method:: socket.write(buf)

       Write the buffer of bytes to the socket.

       Return value: number of bytes written.
