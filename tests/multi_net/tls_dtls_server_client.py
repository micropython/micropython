# Test DTLS server and client, sending a small amount of data between them.

try:
    import socket
    import tls
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000

# These are test certificates. See tests/README.md for details.
certfile = "ec_cert.der"
keyfile = "ec_key.der"

try:
    with open(certfile, "rb") as cf:
        cert = cadata = cf.read()
    with open(keyfile, "rb") as kf:
        key = kf.read()
except OSError:
    print("SKIP")
    raise SystemExit


# DTLS server.
def instance0():
    multitest.globals(IP=multitest.get_network_ip())

    # Create a UDP socket and bind it to accept incoming connections.
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])

    multitest.next()

    # Wait for the client to connect.
    data, client_addr = s.recvfrom(1)
    print("incoming connection", data)

    # Connect back to the client, so the UDP socket can be used like a stream.
    s.connect(client_addr)

    # Create the DTLS context and load the certificate.
    ctx = tls.SSLContext(tls.PROTOCOL_DTLS_SERVER)
    ctx.load_cert_chain(cert, key)

    # Wrap the UDP socket in server mode.
    print("wrap socket")
    s = ctx.wrap_socket(s, server_side=1)

    # Transfer some data.
    for _ in range(4):
        print(s.recv(16))
        s.send(b"server to client")

    # Close the DTLS and UDP connection.
    s.close()


# DTLS client.
def instance1():
    multitest.next()

    # Create a UDP socket and connect to the server.
    addr = socket.getaddrinfo(IP, PORT)[0][-1]
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print("connect")
    s.connect(addr)

    # Send one byte to indicate a connection, and so the server can obtain our address.
    s.write("X")

    # Create a DTLS context and load the certificate.
    ctx = tls.SSLContext(tls.PROTOCOL_DTLS_CLIENT)
    ctx.verify_mode = tls.CERT_REQUIRED
    ctx.load_verify_locations(cadata)

    # Wrap the UDP socket.
    print("wrap socket")
    s = ctx.wrap_socket(s, server_hostname="micropython.local")

    # Transfer some data.
    for _ in range(4):
        s.send(b"client to server")
        print(s.recv(16))

    # Close the DTLS and UDP connection.
    s.close()
