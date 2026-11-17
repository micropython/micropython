# Adversarial test: a PSK server must not fall back to a non-PSK (certificate)
# cipher suite, even when it has a certificate loaded that the client trusts.
# Configuring server_psk_keys restricts the server to PSK cipher suites, so it
# has no cipher suite in common with a certificate-only client and the handshake
# fails.
#
# This is the server-side counterpart to
# sslcontext_server_client_psk_no_fallback_error.py: the server is given both a
# certificate (which the client trusts) and PSK keys, so the only thing
# preventing a connection is the PSK cipher-suite restriction.  This test fails
# if that restriction is not applied (i.e. the server would negotiate a
# certificate suite and connect).

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

SERVER_KEYS = {b"micropython": b"secret-shared-key"}


# Server: has a certificate the client trusts, but also configures PSK, which
# restricts it to PSK suites only.
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
    server_ctx.server_psk_keys = SERVER_KEYS
    try:
        server_ctx.wrap_socket(s2, server_side=True)
        print("server: connected")
    except OSError:
        print("server: handshake failed")
    multitest.broadcast("finished")
    s2.close()
    s.close()


# Client: ordinary certificate client with no PSK, trusting the server's cert.
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    client_ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
    client_ctx.verify_mode = tls.CERT_REQUIRED
    client_ctx.load_verify_locations(cadata)
    try:
        client_ctx.wrap_socket(s, server_hostname="micropython.local")
        print("client: connected")
    except OSError:
        print("client: handshake failed")
    # Don't close until the server has seen the failed handshake.
    multitest.wait("finished")
    s.close()
