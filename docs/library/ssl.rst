:mod:`ssl` -- SSL/TLS module
============================

.. module:: ssl
   :synopsis: TLS/SSL wrapper for socket objects

|see_cpython_module| :mod:`python:ssl`.

This module provides access to Transport Layer Security (previously and
widely known as “Secure Sockets Layer”) encryption and peer authentication
facilities for network sockets, both client-side and server-side.

Functions
---------

.. function:: ssl.wrap_socket(sock, server_side=False, key=None, cert=None, cert_reqs=CERT_NONE, cadata=None, server_hostname=None, do_handshake=True)

    Wrap the given *sock* and return a new wrapped-socket object.  The implementation
    of this function is to first create an `SSLContext` and then call the `SSLContext.wrap_socket`
    method on that context object.  The arguments *sock*, *server_side* and *server_hostname* are
    passed through unchanged to the method call.  The argument *do_handshake* is passed through as
    *do_handshake_on_connect*.  The remaining arguments have the following behaviour:

   - *cert_reqs* determines whether the peer (server or client) must present a valid certificate.
     Note that for mbedtls based ports, ``ssl.CERT_NONE`` and ``ssl.CERT_OPTIONAL`` will not
     validate any certificate, only ``ssl.CERT_REQUIRED`` will.

   - *cadata* is a bytes object containing the CA certificate chain (in DER format) that will
     validate the peer's certificate.  Currently only a single DER-encoded certificate is supported.

   Depending on the underlying module implementation in a particular
   :term:`MicroPython port`, some or all keyword arguments above may be not supported.

class SSLContext
----------------

.. class:: SSLContext(protocol, /)

    Create a new SSLContext instance.  The *protocol* argument must be one of the ``PROTOCOL_*``
    constants.

.. method:: SSLContext.load_cert_chain(certfile, keyfile)

   Load a private key and the corresponding certificate.  The *certfile* is a string
   with the file path of the certificate.  The *keyfile* is a string with the file path
   of the private key.

   .. admonition:: Difference to CPython
      :class: attention

      MicroPython extension: *certfile* and *keyfile* can be bytes objects instead of
      strings, in which case they are interpreted as the actual certificate/key data.

.. method:: SSLContext.load_verify_locations(cafile=None, cadata=None)

   Load the CA certificate chain that will validate the peer's certificate.
   *cafile* is the file path of the CA certificates.  *cadata* is a bytes object
   containing the CA certificates.  Only one of these arguments should be provided.

.. method:: SSLContext.get_ciphers()

   Get a list of enabled ciphers, returned as a list of strings.

.. method:: SSLContext.set_ciphers(ciphers)

   Set the available ciphers for sockets created with this context.  *ciphers* should be
   a list of strings in the `IANA cipher suite format <https://wiki.mozilla.org/Security/Cipher_Suites>`_ .

.. method:: SSLContext.wrap_socket(sock, *, server_side=False, do_handshake_on_connect=True, server_hostname=None)

   Takes a `stream` *sock* (usually socket.socket instance of ``SOCK_STREAM`` type),
   and returns an instance of ssl.SSLSocket, wrapping the underlying stream.
   The returned object has the usual `stream` interface methods like
   ``read()``, ``write()``, etc.

   - *server_side* selects whether the wrapped socket is on the server or client side.
     A server-side SSL socket should be created from a normal socket returned from
     :meth:`~socket.socket.accept()` on a non-SSL listening server socket.

   - *do_handshake_on_connect* determines whether the handshake is done as part of the ``wrap_socket``
     or whether it is deferred to be done as part of the initial reads or writes
     For blocking sockets doing the handshake immediately is standard. For non-blocking
     sockets (i.e. when the *sock* passed into ``wrap_socket`` is in non-blocking mode)
     the handshake should generally be deferred because otherwise ``wrap_socket`` blocks
     until it completes. Note that in AXTLS the handshake can be deferred until the first
     read or write but it then blocks until completion.

   - *server_hostname* is for use as a client, and sets the hostname to check against the received
     server certificate.  It also sets the name for Server Name Indication (SNI), allowing the server
     to present the proper certificate.

.. warning::

   Some implementations of ``ssl`` module do NOT validate server certificates,
   which makes an SSL connection established prone to man-in-the-middle attacks.

   CPython's ``wrap_socket`` returns an ``SSLSocket`` object which has methods typical
   for sockets, such as ``send``, ``recv``, etc. MicroPython's ``wrap_socket``
   returns an object more similar to CPython's ``SSLObject`` which does not have
   these socket methods.

.. attribute:: SSLContext.verify_mode

    Set or get the behaviour for verification of peer certificates.  Must be one of the
    ``CERT_*`` constants.

.. note::

   ``ssl.CERT_REQUIRED`` requires the device's date/time to be properly set, e.g. using
   `mpremote rtc --set <mpremote_command_rtc>` or ``ntptime``, and ``server_hostname``
   must be specified when on the client side.

Exceptions
----------

.. data:: ssl.SSLError

   This exception does NOT exist. Instead its base class, OSError, is used.

Constants
---------

.. data:: ssl.PROTOCOL_TLS_CLIENT
          ssl.PROTOCOL_TLS_SERVER

    Supported values for the *protocol* parameter.

.. data:: ssl.CERT_NONE
          ssl.CERT_OPTIONAL
          ssl.CERT_REQUIRED

    Supported values for *cert_reqs* parameter, and the :attr:`SSLContext.verify_mode`
    attribute.
