# Test TCP server and client with TLS-PSK, using set_psk_identity(), 
# set_psk_key(), and set_ciphers("TLS-PSK-WITH-AES-128-CBC-SHA256").

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
    
    # Configure PSK with specific ciphersuite
    server_ctx.set_psk_identity("PSK-Identity-1")
    server_ctx.set_psk_key(bytes.fromhex("c0ffee"))
    server_ctx.set_ciphers("TLS-PSK-WITH-AES-128-CBC-SHA256")
    
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
    
    # Configure PSK with specific ciphersuite
    client_ctx.set_psk_identity("PSK-Identity-1")
    client_ctx.set_psk_key(bytes.fromhex("c0ffee"))
    client_ctx.set_ciphers("TLS-PSK-WITH-AES-128-CBC-SHA256")
    
    s = client_ctx.wrap_socket(s, server_hostname="micropython.local")
    s.write(b"client to server")
    print(s.read(16))
    s.close()
    