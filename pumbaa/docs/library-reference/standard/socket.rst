:mod:`socket` --- Low-level networking interface
================================================

.. module:: socket
   :synopsis: Low-level networking interface.

This module provides access to the BSD socket interface.

----------------------------------------------

.. data:: socket.AF_INET

   This constant represent the address (and protocol) family, used for
   the first argument to :func:`socket.socket()`.


.. data:: socket.SOCK_STREAM
.. data:: socket.SOCK_DGRAM

   These constants represent the socket types, used for the second
   argument to :func:`socket.socket()`.


.. function:: socket.getaddrinfo(host, port[, family[, socktype[, proto[, flags]]]])

   Translate the host/port argument into a sequence of 5-tuples that
   contain all the necessary arguments for creating a socket connected
   to that service. `host` is a domain name, a string representation
   of an IPv4/v6 address or ``None``. `port` is a string service name
   such as 'http', a numeric port number or ``None``. By passing
   ``None`` as the value of `host` and `port`, you can pass NULL to
   the underlying C API.

   The function returns a list of 5-tuples with the following
   structure:

   ``(family, socktype, proto, canonname, sockaddr)``

   In these tuples, `family`, `socktype`, `proto` are all integers and
   are meant to be passed to :func:`socket.socket()`. `canonname` will
   be a string representing the canonical name of the host if
   AI_CANONNAME is part of the flags argument; else canonname will be
   empty. sockaddr is a tuple describing a socket address, whose
   format depends on the returned family (a (address, port) 2-tuple
   for AF_INET, a (address, port, flow info, scope id) 4-tuple for
   AF_INET6), and is meant to be passed to the socket.connect()
   method.

   The following example fetches address information for a
   hypothetical TCP connection to example.org on port 80 (results may
   differ on your system if IPv6 isn’t enabled):

   >>> socket.getaddrinfo("example.org", 80, 0, 0, socket.IPPROTO_TCP)
   [(2, 1, 6, '', ('93.184.216.34', 80))]


.. function:: socket.socket([family[, type[, proto]]])

   Create a new socket using the given address `family`, socket `type`
   and protocol number. The address family should be
   :data:`socket.AF_INET`. The socket `type` should be
   :data:`socket.SOCK_STREAM` (the default),
   :data:`socket.SOCK_DGRAM`. The protocol number is usually zero and
   may be omitted in that case.


.. function:: socket.inet_aton(ip_string)

   Convert an IPv4 address from dotted-quad string format (for
   example, ‘123.45.67.89’) to 32-bit packed binary format, as a
   string four characters in length. This is useful when conversing
   with a program that uses the standard C library and needs objects
   of type struct in_addr, which is the C type for the 32-bit packed
   binary this function returns.

   If the IPv4 address string passed to this function is invalid,
   ``socket.error`` will be raised. Note that exactly what is valid
   depends on the underlying C implementation of ``inet_aton()``.

   ``inet_aton()`` does not support IPv6.


.. function:: socket.inet_ntoa(packed_ip)

   Convert a 32-bit packed IPv4 address (a string four characters in
   length) to its standard dotted-quad string representation (for
   example, ‘123.45.67.89’). This is useful when conversing with a
   program that uses the standard C library and needs objects of type
   struct in_addr, which is the C type for the 32-bit packed binary
   data this function takes as an argument.

   If the string passed to this function is not exactly 4 bytes in
   length, ``socket.error`` will be raised. ``inet_ntoa()`` does not
   support IPv6.


.. class:: socket.SocketType

   This is a Python type object that represents the socket object
   type. It is the same as ``type(socket(...))``.

   Note that there are no methods ``read()`` or ``write()``; use
   ``recv()`` and ``send()`` without flags argument instead.


   .. method:: accept()

      Accept a connection. The socket must be bound to an address and
      listening for connections. The return value is a pair ``(conn,
      address)`` where conn is a new socket object usable to send and
      receive data on the connection, and address is the address bound
      to the socket on the other end of the connection.


   .. method:: bind(address)

      Bind the socket to address. The socket must not already be
      bound. The format is of `address` is ``(ip_address, port)``.


   .. method:: close()

      Close the socket. All future operations on the socket object
      will fail. The remote end will receive no more data (after
      queued data is flushed). Sockets are automatically closed when
      they are garbage-collected.

      Note ``close()`` releases the resource associated with a
      connection but does not necessarily close the connection
      immediately. If you want to close the connection in a timely
      fashion, call ``shutdown()`` before ``close()``.


   .. method:: connect(address)

      Connect to a remote socket at address. The format is of
      `address` is ``(ip_address, port)``.


   .. method:: listen(backlog)

      Listen for connections made to the socket. The backlog argument
      specifies the maximum number of queued connections and should be
      at least 0; the maximum value is system-dependent (usually 5),
      the minimum value is forced to 0.


   .. method:: recv(bufsize)

      Receive data from the socket. The return value is a string
      representing the data received. The maximum amount of data to be
      received at once is specified by `bufsize`.


   .. method:: recvfrom(bufsize)

      Receive data from the socket. The return value is a pair
      ``(string, address)`` where string is a string representing the
      data received and address is the address of the socket sending
      the data.


   .. method:: recv_into(buffer[, nbytes])

      Receive up to nbytes bytes from the socket, storing the data
      into a buffer rather than creating a new string. If nbytes is
      not specified (or 0), receive up to the size available in the
      given buffer. Returns the number of bytes received.


   .. method:: recvfrom_into(buffer[, nbytes])

      Receive data from the socket, writing it into `buffer` instead
      of creating a new string. The return value is a pair ``(nbytes,
      address)`` where nbytes is the number of bytes received and
      address is the address of the socket sending the data.


   .. method:: send(string)

      Send data to the socket. The socket must be connected to a
      remote socket. The optional flags argument has the same meaning
      as for ``recv()`` above. Returns the number of bytes
      sent. Applications are responsible for checking that all data
      has been sent; if only some of the data was transmitted, the
      application needs to attempt delivery of the remaining data.


   .. method:: sendall(string)

      Send data to the socket. The socket must be connected to a
      remote socket. The optional flags argument has the same meaning
      as for ``recv()`` above. Unlike ``send()``, this method
      continues to send data from string until either all data has
      been sent or an error occurs. None is returned on success. On
      error, an exception is raised, and there is no way to determine
      how much data, if any, was successfully sent.


   .. method:: sendto(string, address)
   .. method:: sendto(string, flags, address)

      Send data to the socket. The socket should not be connected to a
      remote socket, since the destination socket is specified by
      address. The optional flags argument has the same meaning as for
      ``recv()`` above. Return the number of bytes sent. (The format
      of address depends on the address family — see above.)


   .. method:: shutdown(how)

      Shut down one or both halves of the connection.
