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
   an SSL context. Returned object has the usual `stream` interface methods like
   ``read()``, ``write()``, etc. In MicroPython, the returned object does not expose
   socket interface and methods like ``recv()``, ``send()``. In particular, a
   server-side SSL socket should be created from a normal socket returned from
   :meth:`~usocket.socket.accept()` on a non-SSL listening server socket.

   Parameters:
   - ``server_side``: creates a server connection if True, else client connection. A
     server connection requires a ``keyfile`` and a ``certfile``.
   - ``cert_reqs``: specifies the level of certificate checking to be performed.
     CERT_NONE: in client mode accept just about any cert, in server mode do not
     request a cert from the client. CERT_OPTIONAL: in client mode behaves the
     same as CERT_REQUIRED and in server mode requests an optional cert from the client
     for authentication. CERT_REQUIRED: in client mode validates the server's cert and
     in server mode requires the client to send a cert for authentication. Note that
     ussl does not actually support client authentication.
   - ``ca_certs``: root certificates to use for certificate checking.
   - ``server_hostname``: specifies the hostname of the server for verification purposes
     as well for SNI (Server Name Identification).
   - ``do_handshake``: initiates the TLS handshake if set to True, otherwise (???).

   Depending on the underlying module implementation in a particular
   `MicroPython port`, some or all keyword arguments above may be not supported.

.. warning::

   Some implementations of ``ussl`` module do NOT validate server certificates,
   which makes an SSL connection established prone to man-in-the-middle attacks.

   ESP32 implementation notes:
   - The esp32 implementation does not support cert_reqs: it never validates certs!
   - The esp32 implementation supports key-exchange and bidirectional authentication
     using Pre-Shared Keys. Use KW options ``psk_ident=<identity hint>`` and
     ``psk_key=binascii.unhexlify(b'<key in hex>')``. PSK ciphers are only supported
     for client-side connections. See below for more info about PSK ciphers.

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

Pre-Shared Key (PSK) cipher suites
----------------------------------

TLS supports authentication and encryption using a pre-shared key (i.e. a
key that both client and server know) as an alternative to the public
key cryptography commonly used on the web for HTTPS.  PSK is starting
to be used for MQTT, e.g. in mosquitto, to simplify the set-up and avoid
having to go through the whole CA, cert, and private key process.

A pre-shared key is a binary string of up to 32 bytes and is commonly
represented in hex form. In addition to the key, clients can also
present an id/ident and typically the server allows a different key
to be associated with each client id. In effect this is very similar
to username and password pairs, except that unlike a password the
key is not directly transmitted to the server, thus a connection to a
malicious server does not divulge the password. Plus the server is also
authenticated to the client.

To use PSK:
- Generate a random hex string (generating an MD5 or SHA on some random
  data is one way to do this)
- Come up with a string id for your client and configure your server to accept the id/key pair
- In ``ussl.wrap_socket`` use ``psk_ident`` and ``psk_key`` to set the id/key combo
- When the handshake with the server is performed it uses the id/key combo to authenticate the
  server (it must prove that it has the key too), authenticate the client and then negotiate
  encryption for the connection

An example can be found in ``examples/network/mqtt_psk.py``.
