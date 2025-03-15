# Test creating an SSL connection and getting the peer certificate.

try:
    import io
    import os
    import socket
    import tls
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000

# These are test certificates. See tests/README.md for details.
cert = cafile = "ec_cert.der"
key = "ec_key.der"

try:
    with open(cafile, "rb") as f:
        cadata = f.read()
    with open(key, "rb") as f:
        key = f.read()
except OSError:
    print("SKIP")
    raise SystemExit


def verify_callback(cert, depth):
    print(cert.hex())
    return 0


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen(1)
    multitest.next()
    s2, _ = s.accept()
    server_ctx = tls.SSLContext(tls.PROTOCOL_TLS_SERVER)
    server_ctx.load_cert_chain(cadata, key)
    s2 = server_ctx.wrap_socket(s2, server_side=True)
    print(s2.read(16))
    s2.write(b"server to client")
    s2.close()
    s.close()


# Client
def instance1():
    s_test = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
    if not hasattr(s_test, "verify_callback"):
        print("SKIP")
        raise SystemExit

    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    client_ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
    client_ctx.verify_mode = tls.CERT_REQUIRED
    client_ctx.verify_callback = verify_callback
    client_ctx.load_verify_locations(cadata)
    s = client_ctx.wrap_socket(s, server_hostname="micropython.local")
    s.write(b"client to server")
    print(s.read(16))
    s.close()
