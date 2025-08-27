# Test TCP server and client with TLS-PSK, using set_psk_identity(),
# set_psk_key(), and set_ciphers("PSK").

try:
    import socket
    import tls
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000

PSK_ID = "PSK-Identity-1"
PSK_KEY = "c0ffee"
PSK_CIPHER = "PSK"


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
    # Configure PSK
    server_ctx.set_psk_identity(PSK_ID)
    server_ctx.set_psk_key(bytes.fromhex(PSK_KEY))
    server_ctx.set_ciphers(PSK_CIPHER)
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
    # Configure PSK
    client_ctx.set_psk_identity(PSK_ID)
    client_ctx.set_psk_key(bytes.fromhex(PSK_KEY))
    client_ctx.set_ciphers(PSK_CIPHER)
    s = client_ctx.wrap_socket(s, server_hostname="micropython.local")
    s.write(b"client to server")
    print(s.read(16))
    s.close()
