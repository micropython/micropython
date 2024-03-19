# Test creating an SSL connection with certificates as bytes objects.

try:
    from io import IOBase
    import os
    import socket
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit

if not hasattr(ssl, "SSLSession"):
    print("SKIP")
    raise SystemExit

PORT = 8000

# These are test certificates. See tests/README.md for details.
certfile = "ec_cert.der"
keyfile = "ec_key.der"

try:
    os.stat(certfile)
    os.stat(keyfile)
except OSError:
    print("SKIP")
    raise SystemExit

with open(certfile, "rb") as cf:
    cert = cadata = cf.read()

with open(keyfile, "rb") as kf:
    key = kf.read()


# Helper class to count number of bytes going over a TCP socket
class CountingStream(IOBase):
    def __init__(self, stream):
        self.stream = stream
        self.count = 0

    def readinto(self, buf, nbytes=None):
        result = self.stream.readinto(buf) if nbytes is None else self.stream.readinto(buf, nbytes)
        self.count += result
        return result

    def write(self, buf):
        self.count += len(buf)
        return self.stream.write(buf)

    def ioctl(self, req, arg):
        if hasattr(self.stream, "ioctl"):
            return self.stream.ioctl(req, arg)
        return 0


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen(1)
    multitest.next()
    server_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    server_ctx.load_cert_chain(cert, key)
    for i in range(7):
        s2, _ = s.accept()
        s2 = server_ctx.wrap_socket(s2, server_side=True)
        print(s2.read(18))
        s2.write(b"server to client {}".format(i))
        s2.close()
    s.close()


# Client
def instance1():
    multitest.next()

    def connect_and_count(i, session, set_method="wrap_socket"):
        s = socket.socket()
        s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
        s = CountingStream(s)
        client_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
        client_ctx.verify_mode = ssl.CERT_REQUIRED
        client_ctx.load_verify_locations(cadata=cadata)
        wrap_socket_kwargs = {}
        if set_method == "wrap_socket":
            wrap_socket_kwargs = {"session": session}
        elif set_method == "socket_attr":
            wrap_socket_kwargs = {"do_handshake_on_connect": False}
        s2 = client_ctx.wrap_socket(s, server_hostname="micropython.local", **wrap_socket_kwargs)
        if set_method == "socket_attr" and session is not None:
            s2.session = session
        s2.write(b"client to server {}".format(i))
        print(s2.read(18))
        session = s2.session
        print(type(session))
        s2.close()
        return session, s.count

    # No session reuse
    session, count_without_reuse = connect_and_count(0, None)

    # Direct session reuse
    session, count = connect_and_count(1, session, "wrap_socket")
    print(count < count_without_reuse)

    # Serialized session reuse
    session = ssl.SSLSession(session.serialize())
    session, count = connect_and_count(2, session, "wrap_socket")
    print(count < count_without_reuse)

    # Serialized session reuse (using buffer protocol)
    session = ssl.SSLSession(bytes(session))
    session, count = connect_and_count(3, session, "wrap_socket")
    print(count < count_without_reuse)

    # Direct session reuse
    session, count = connect_and_count(4, session, "socket_attr")
    print(count < count_without_reuse)

    # Serialized session reuse
    session = ssl.SSLSession(session.serialize())
    session, count = connect_and_count(5, session, "socket_attr")
    print(count < count_without_reuse)

    # Serialized session reuse (using buffer protocol)
    session = ssl.SSLSession(bytes(session))
    session, count = connect_and_count(6, session, "socket_attr")
    print(count < count_without_reuse)
