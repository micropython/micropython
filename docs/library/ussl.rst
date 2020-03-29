:mod:`ussl` -- SSL/TLS module
=============================

.. module:: ussl
   :synopsis: TLS/SSL wrapper for socket objects

|see_cpython_module| :mod:`python:ssl`.

This module provides access to Transport Layer Security (previously and
widely known as “Secure Sockets Layer”) encryption and peer authentication
facilities for network sockets, both client-side and server-side.

Functions
---------

.. function:: ussl.wrap_socket(sock, server_side=False, keyfile=None, certfile=None, cert_reqs=CERT_NONE, ca_certs=None, server_hostname=None, do_handshake=True)

   Takes a `stream` *sock* (usually usocket.socket instance of ``SOCK_STREAM`` type),
   and returns an instance of ssl.SSLSocket, which wraps the underlying stream in
   an SSL context. The returned object has the usual `stream` interface methods like
   ``read()``, ``write()``, etc.
   A server-side SSL socket should be created from a normal socket returned from
   :meth:`~usocket.socket.accept()` on a non-SSL listening server socket.

   Parameters:

   - ``server_side``: creates a server connection if True, else client connection. A
     server connection requires a ``keyfile`` and a ``certfile``.
   - ``cert_reqs``: specifies the level of certificate checking to be performed.
   - ``ca_certs``: root certificates to use for certificate checking.
   - ``server_hostname``: specifies the hostname of the server for verification purposes
     as well for SNI (Server Name Identification).
   - ``do_handshake``: determines whether the handshake is done as part of the ``wrap_socket``
     or whether it is deferred to be done as part of the initial reads or writes
     (there is no ``do_handshake`` method as in CPython).
     For blocking sockets doing the handshake immediately is standard. For non-blocking
     sockets (i.e. when the *sock* passed into ``wrap_socket`` is in non-blocking mode)
     the handshake should generally be deferred because otherwise ``wrap_socket`` blocks
     until it completes. Note that in AXTLS the handshake can be deferred until the first
     read or write but it then blocks until completion.

   Depending on the underlying module implementation in a particular
   :term:`MicroPython port`, some or all keyword arguments above may be not supported.

.. warning::

   Some implementations of ``ussl`` module do NOT validate server certificates,
   which makes an SSL connection established prone to man-in-the-middle attacks.

   CPython's ``wrap_socket`` returns an ``SSLSocket`` object which has methods typical
   for sockets, such as ``send``, ``recv``, etc. MicroPython's ``wrap_socket``
   returns an object more similar to CPython's ``SSLObject`` which does not have
   these socket methods.

Exceptions
----------

.. data:: ssl.SSLError

   This exception does NOT exist. Instead its base class, OSError, is used.

Constants
---------

.. data:: ussl.CERT_NONE
          ussl.CERT_OPTIONAL
          ussl.CERT_REQUIRED

    Supported values for *cert_reqs* parameter.

    - CERT_NONE: in client mode accept just about any cert, in server mode do not
      request a cert from the client.
    - CERT_OPTIONAL: in client mode behaves the same as CERT_REQUIRED and in server
      mode requests an optional cert from the client for authentication.
    - CERT_REQUIRED: in client mode validates the server's cert and
      in server mode requires the client to send a cert for authentication. Note that
      ussl does not actually support client authentication.
