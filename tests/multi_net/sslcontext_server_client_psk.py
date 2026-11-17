# Test TCP server and client using TLS-PSK (pre-shared key).  The client
# presents an identity/key via the psk_identity/psk_key attributes, and the
# server looks the identity up in its server_psk_keys mapping.

try:
    import socket
    import tls

    # PSK support is optional; psk_identity only exists when it's enabled.
    tls.SSLContext(tls.PROTOCOL_TLS_CLIENT).psk_identity
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

PORT = 8000

# The identity the client presents and the key shared by both ends.  Both are
# byte strings (the identity is an opaque octet string on the wire); readable
# ones are used here for clarity.
PSK_IDENTITY = b"micropython"
PSK_KEY = b"secret-shared-key"

# The server maps identities to keys.  A plain dict covers the common case;
# any object with a get() method can be used to look keys up on demand.
SERVER_KEYS = {PSK_IDENTITY: PSK_KEY}


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
    server_ctx.server_psk_keys = SERVER_KEYS
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
