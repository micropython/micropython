# Test TLS-PSK where the server's server_psk_keys is a user object with a get()
# method (rather than a dict), so the key lookup is a callback into Python
# during the handshake.

try:
    import socket
    import tls

    # PSK support is optional; psk_identity only exists when it's enabled.
    tls.SSLContext(tls.PROTOCOL_TLS_CLIENT).psk_identity
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

PORT = 8000

PSK_IDENTITY = b"micropython"
PSK_KEY = b"secret-shared-key"


# A mapping-like object whose get() is called with the client's identity during
# the server handshake; it returns the key, or None to reject the identity.
class PSKStore:
    def get(self, identity):
        print("server: lookup", identity)
        if identity == PSK_IDENTITY:
            return PSK_KEY
        return None


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
    server_ctx.server_psk_keys = PSKStore()
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
    client_ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
    client_ctx.psk_identity = PSK_IDENTITY
    client_ctx.psk_key = PSK_KEY
    s = client_ctx.wrap_socket(s, server_hostname="micropython.local")
    s.write(b"client to server")
    print(s.read(16))
    s.close()
