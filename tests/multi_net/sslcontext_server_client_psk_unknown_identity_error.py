# Adversarial test: a PSK client presenting an identity the server does not
# know must not connect.  The server's lookup (server_psk_keys.get(identity))
# returns None, which rejects the handshake.

try:
    import socket
    import tls

    # PSK support is optional; psk_identity only exists when it's enabled.
    tls.SSLContext(tls.PROTOCOL_TLS_CLIENT).psk_identity
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

PORT = 8000

SERVER_KEYS = {b"micropython": b"secret-shared-key"}


# Server: only knows the "micropython" identity.
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen(1)
    multitest.next()
    s2, _ = s.accept()
    server_ctx = tls.SSLContext(tls.PROTOCOL_TLS_SERVER)
    server_ctx.server_psk_keys = SERVER_KEYS
    try:
        server_ctx.wrap_socket(s2, server_side=True)
        print("server: connected")
    except OSError:
        print("server: handshake failed")
    multitest.broadcast("finished")
    s2.close()
    s.close()


# Client: presents an identity the server has no key for.
def instance1():
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    client_ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
    client_ctx.psk_identity = b"stranger"
    client_ctx.psk_key = b"secret-shared-key"
    try:
        client_ctx.wrap_socket(s, server_hostname="micropython.local")
        print("client: connected")
    except OSError:
        print("client: handshake failed")
    # Don't close until the server has seen the failed handshake.
    multitest.wait("finished")
    s.close()
