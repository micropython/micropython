# Test creating an SSL connection with certificates from a file.

try:
    import os
    import socket
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000

# These are test certificates. See tests/README.md for details.
cert = cafile = "ec_cert.der"
key = "ec_key.der"

try:
    os.stat(cafile)
    os.stat(key)
except OSError:
    print("SKIP")
    raise SystemExit


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen(1)
    multitest.next()
    s2, _ = s.accept()
    server_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    server_ctx.load_cert_chain(cert, key)
    s2 = server_ctx.wrap_socket(s2, server_side=True)
    print(s2.read(16))
    s2.write(b"server to client")
    s2.close()
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    client_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    client_ctx.verify_mode = ssl.CERT_REQUIRED
    client_ctx.load_verify_locations(cafile=cafile)
    s = client_ctx.wrap_socket(s, server_hostname="micropython.local")
    s.write(b"client to server")
    print(s.read(16))
    s.close()
