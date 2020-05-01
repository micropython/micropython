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

.. function:: ussl.wrap_socket(sock, server_side=False, keyfile=None, certfile=None, cert_reqs=CERT_NONE, ca_certs=None)

   Takes a `stream` *sock* (usually usocket.socket instance of ``SOCK_STREAM`` type),
   and returns an instance of ssl.SSLSocket, which wraps the underlying stream in
   an SSL context. Returned object has the usual `stream` interface methods like
   ``read()``, ``write()``, etc. In MicroPython, the returned object does not expose
   socket interface and methods like ``recv()``, ``send()``. In particular, a
   server-side SSL socket should be created from a normal socket returned from
   :meth:`~usocket.socket.accept()` on a non-SSL listening server socket.

   Depending on the underlying module implementation in a particular
   `MicroPython port`, some or all keyword arguments above may be not supported.

.. warning::

   Some implementations of ``ussl`` module do NOT validate server certificates,
   which makes an SSL connection established prone to man-in-the-middle attacks.

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

MBEDTLS Based Ports
-------------------
This applies for the esp32

.. function:: ussl.wrap_socket(sock, server_side=False, key=None, cert=None, server_hostname=None, cert_reqs=0, ca_certs=None, do_handshake=True)

   * *sock* the socket object which gets wrapped
   * *server_side* if this socket is used as a server
   * *key* server key
   * *cert* server cert
   * *server_hostname* for SNI
   * *cert_reqs* Or-ed flags from x509.h https://tls.mbed.org/api/x509_8h_source.html which errors NOT to tolerate. If set to zero all cert validation errors are accapted, if set to 0xffffff all errors will raise.
   * *ca_certs* ca certificates
   * *do_handshake* if the handshake should be performed

.. warning::

   key, cert and ca_certs are byte objects which include one DER-encoded or one or more concatenated PEM-encoded certificates.