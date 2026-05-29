# Adversarial test: a PSK client must not fall back to a non-PSK (certificate)
# cipher suite, even when it would otherwise trust the server's certificate.
# Configuring PSK restricts the client to PSK cipher suites, so it has no cipher
# suite in common with a certificate-only server and the handshake fails.
#
# Unlike sslcontext_server_client_psk_client_only_error.py (where the client
# rejects the server's certificate), here the client is given the CA so the
# certificate would verify.  The only thing preventing a connection is the PSK
# cipher-suite restriction, so this test fails if that restriction is not
# applied (i.e. the client would negotiate a certificate suite and connect).

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
    cert = cadata = f.read()
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


# Client: PSK configured (so only PSK suites are offered) but also trusts the
# server's certificate, so the only reason the handshake fails is the PSK
# cipher-suite restriction.
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    client_ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
    client_ctx.verify_mode = tls.CERT_REQUIRED
    client_ctx.load_verify_locations(cadata)
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
