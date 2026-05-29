# Adversarial test: a PSK client must not connect to a non-PSK (certificate)
# server.  Configuring PSK restricts the client to PSK cipher suites, so it has
# no cipher suite in common with a certificate-only server and the handshake
# fails on both ends (rather than silently falling back to a non-PSK suite).

try:
    import socket
    import tls

    # PSK support is optional; psk_identity only exists when it's enabled.
    tls.SSLContext(tls.PROTOCOL_TLS_CLIENT).psk_identity
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

PORT = 8000

with open("ec_cert.der", "rb") as f:
    cert = f.read()
with open("ec_key.der", "rb") as f:
    key = f.read()


# Server: ordinary certificate-based server, no PSK.
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen(1)
    multitest.next()
    s2, _ = s.accept()
    server_ctx = tls.SSLContext(tls.PROTOCOL_TLS_SERVER)
    server_ctx.load_cert_chain(cert, key)
    try:
        server_ctx.wrap_socket(s2, server_side=True)
        print("server: connected")
    except OSError:
        print("server: handshake failed")
    multitest.broadcast("finished")
    s2.close()
    s.close()


# Client: PSK configured, so only PSK cipher suites are offered.
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    client_ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
    client_ctx.psk_identity = b"micropython"
    client_ctx.psk_key = b"secret-shared-key"
    try:
        client_ctx.wrap_socket(s, server_hostname="micropython.local")
        print("client: connected")
    except OSError:
        print("client: handshake failed")
    # Don't close until the server has seen the failed handshake.
    multitest.wait("finished")
    s.close()
