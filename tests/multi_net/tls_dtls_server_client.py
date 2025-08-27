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

with open(certfile, "rb") as cf:
    cert = cadata = cf.read()
with open(keyfile, "rb") as kf:
    key = kf.read()


# DTLS server.
def instance0():
    multitest.globals(IP=multitest.get_network_ip())

    # Create a UDP socket and bind it to accept incoming connections.
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])

    multitest.next()

    ctx = tls.SSLContext(tls.PROTOCOL_DTLS_SERVER)
    ctx.load_cert_chain(cert, key)

    # Because of "hello verify required", we expect the peer
    # to connect twice: once to set the cookie, then second time
    # successfully.
    #
    # As this isn't a real server, we hard-code two connection attempts
    for _ in range(2):
        print("waiting")
        # Wait for the client to connect so we know their address
        _, client_addr = s.recvfrom(1, socket.MSG_PEEK)
        print("incoming connection")
        s.connect(client_addr)  # Connect back to the client

        # Wrap the UDP socket in server mode.
        try:
            s = ctx.wrap_socket(s, server_side=1, client_id=repr(client_addr).encode())
        except OSError as e:
            print(e)
            continue  # wait for second connection

        # Transfer some data.
        for i in range(4):
            print(s.recv(32))
            s.send(b"server to client " + str(i).encode())

        # Close the DTLS and UDP connection.
        s.close()
        break


# DTLS client.
def instance1():
    multitest.next()

    # Create a UDP socket and connect to the server.
    addr = socket.getaddrinfo(IP, PORT)[0][-1]
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print("connect")
    s.connect(addr)

    # Create a DTLS context and load the certificate.
    ctx = tls.SSLContext(tls.PROTOCOL_DTLS_CLIENT)
    ctx.verify_mode = tls.CERT_REQUIRED
    ctx.load_verify_locations(cadata)

    # Wrap the UDP socket.
    print("wrap socket")
    s = ctx.wrap_socket(s, server_hostname="micropython.local")

    # Transfer some data.
    for i in range(4):
        s.send(b"client to server " + str(i).encode())
        print(s.recv(32))

    # Close the DTLS and UDP connection.
    s.close()
