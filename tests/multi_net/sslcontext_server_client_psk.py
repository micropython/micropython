# Test creating an SSL connection using PSK (Pre-Shared Key).

try:
    import socket
    import tls
    import multitest
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000

# PSK and identity values
psk = b"micropython-psk-secret"
psk_identity = b"micropython-client"

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
    
    # Set PSK and identity for server
    server_ctx.set_psk(psk, psk_identity)
    
    # Configure PSK ciphersuites
    server_ctx.set_psk_ciphersuites(["TLS-PSK-WITH-AES-128-CBC-SHA256"])
    
    s2 = server_ctx.wrap_socket(s2, server_side=True)
    assert isinstance(s2.cipher(), tuple)
    cipher_info = s2.cipher()
    # PSK cipher should be used
    assert "PSK" in cipher_info[0], f"Expected PSK cipher, got {cipher_info[0]}"
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
    
    # Set PSK and identity for client
    client_ctx.set_psk(psk, psk_identity)
    
    # Configure PSK ciphersuites
    client_ctx.set_psk_ciphersuites(["TLS-PSK-WITH-AES-128-CBC-SHA256"])
    
    s = client_ctx.wrap_socket(s, server_hostname="micropython.local")
    cipher_info = s.cipher()
    # PSK cipher should be used
    assert "PSK" in cipher_info[0], f"Expected PSK cipher, got {cipher_info[0]}"
    s.write(b"client to server")
    print(s.read(16))
    s.close()