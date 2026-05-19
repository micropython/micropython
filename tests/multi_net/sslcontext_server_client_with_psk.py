# Test asyncio TCP server and client with TLS-PSK, using set_psk_sever_callback() and set_psk_client_callback().

try:
    import socket
    import tls
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000


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

    def psk_server_callback(identity):
        psk_dict = {
            b"PSK-Identity-1": bytes.fromhex("c0ffee"),
            b"PSK-Identity-2": bytes.fromhex("facade"),
        }
        return psk_dict[identity]

    server_ctx.set_psk_server_callback(psk_server_callback)
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

    def psk_client_callback(identity):
        return (b"PSK-Identity-1", bytes.fromhex("c0ffee"))

    client_ctx.set_psk_client_callback(psk_client_callback)
    s = client_ctx.wrap_socket(s, server_hostname="micropython.local")
    s.write(b"client to server")
    print(s.read(16))
    s.close()
