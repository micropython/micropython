:mod:`ussl` -- ssl module
===============================

.. module:: ussl
   :synopsis: TLS/SSL wrapper for socket objects

This module provides access to Transport Layer Security (often known as 
“Secure Sockets Layer”) encryption and peer authentication facilities for
network sockets, both client-side and server-side.

.. only:: not port_wipy

    Functions
    ---------

    .. function:: ssl.wrap_socket(sock, server_side=False)

       Takes a stream `sock` (usually usocket.socket instance of ``SOCK_STREAM`` type),
       and returns an instance of ssl.SSLSocket, which wraps the underlying stream in
       an SSL context. Returned object has the usual stream interface methods like
       `read()`, `write()`, etc. In MicroPython, the returned object does not expose
       socket interface and methods like `recv()`, `send()`. In particular, a
       server-side SSL socket should be created from a normal socket returned from
       `accept()` on a non-SSL listening server socket.

    .. warning::

       Currently, this function does NOT validate server certificates, which makes
       an SSL connection established prone to man-in-the-middle attacks.


.. only:: port_wipy

    Functions
    ---------

    .. function:: ssl.wrap_socket(sock, keyfile=None, certfile=None, server_side=False, cert_reqs=CERT_NONE, ca_certs=None)

       Takes an instance sock of socket.socket, and returns an instance of ssl.SSLSocket, a subtype of 
       ``socket.socket``, which wraps the underlying socket in an SSL context. sock must be a ``SOCK_STREAM``
       socket and protocol number ``socket.IPPROTO_SEC``; other socket types are unsupported. Example::

          import socket
          import ssl
          s = socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_SEC)
          ss = ssl.wrap_socket(s)
          ss.connect(socket.getaddrinfo('www.google.com', 443)[0][-1])

       Certificates must be used in order to validate the other side of the connection, and also to
       authenticate ourselves with the other end. Such certificates must be stored as files using the
       FTP server, and they must be placed in specific paths with specific names.

       - The certificate to validate the other side goes in: **'/flash/cert/ca.pem'**
       - The certificate to authenticate ourselves goes in: **'/flash/cert/cert.pem'**
       - The key for our own certificate goes in: **'/flash/cert/private.key'**

       .. note::

          When these files are stored, they are placed inside the internal **hidden** file system
          (just like firmware updates), and therefore they are never visible.

       For instance to connect to the Blynk servers using certificates, take the file ``ca.pem`` located
       in the `blynk examples folder <https://github.com/wipy/wipy/tree/master/examples/blynk>`_ 
       and put it in '/flash/cert/'. Then do::

          import socket
          import ssl
          s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_SEC)
          ss = ssl.wrap_socket(s, cert_reqs=ssl.CERT_REQUIRED, ca_certs='/flash/cert/ca.pem')
          ss.connect(socket.getaddrinfo('cloud.blynk.cc', 8441)[0][-1])

       SSL sockets inherit all methods and from the standard sockets, see the :mod:`usocket` module.

    Exceptions
    ----------

    .. data:: ssl.SSLError

    Constants
    ---------

    .. data:: ssl.CERT_NONE
    .. data:: ssl.CERT_OPTIONAL
    .. data:: ssl.CERT_REQUIRED

        supported values in ``cert_reqs``
