# Simple test creating an SSL connection and transferring some data
# This test won't run under CPython because CPython doesn't have key/cert

try:
    import binascii, os, socket, ssl
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000

# These are test certificates. See tests/README.md for details.
certfile = "rsa_cert.der"
keyfile = "rsa_key.der"

try:
    os.stat(certfile)
    os.stat(keyfile)
except OSError:
    print("SKIP")
    raise SystemExit

with open(certfile, "rb") as cf:
    cert = cadata = cf.read()

with open(keyfile, "rb") as kf:
    key = kf.read()


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket(multitest.AF_FAMILY, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo(multitest.BIND_ADDR, PORT, multitest.AF_FAMILY)[0][-1])
    s.listen(1)
    multitest.next()
    s2, _ = s.accept()
    s2 = ssl.wrap_socket(s2, server_side=True, key=key, cert=cert)
    print(s2.read(16))
    s2.write(b"server to client")
    s2.close()
    s.close()


# Client
def instance1():
    multitest.next()
    s = socket.socket(multitest.AF_FAMILY, socket.SOCK_STREAM)
    s.connect(socket.getaddrinfo(IP, PORT, multitest.AF_FAMILY)[0][-1])
    s = ssl.wrap_socket(
        s, cert_reqs=ssl.CERT_REQUIRED, server_hostname="micropython.local", cadata=cadata
    )
    s.write(b"client to server")
    print(s.read(16))
    s.close()
