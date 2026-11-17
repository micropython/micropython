# Test tls.load_verify_locations loading a .pem file.

import socket
import tls

if not hasattr(tls, "CERT_REQUIRED"):
    print("SKIP")
    raise SystemExit

# For details about this cert, see comment in test_sslcontext_client.py
root_cert_path = "isrgrootx1.pem"

with open(root_cert_path) as ca:
    cadata = ca.read()

context = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
context.verify_mode = tls.CERT_REQUIRED
context.load_verify_locations(cadata)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
addr = socket.getaddrinfo("micropython.org", 443)[0][-1]

s.connect(addr)
ssl_sock = context.wrap_socket(s, server_hostname="micropython.org")
ssl_sock.write(b"GET / HTTP/1.0\r\n\r\n")
print(ssl_sock.read(17))
ssl_sock.close()
