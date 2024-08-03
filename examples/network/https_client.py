# Very simple HTTPS client example:
# - Connects to a server.
# - Upgrades the connection to a TLS connection.
# - Sends a HTTP request.
# - Reads the result.
#
# This example works in both MicroPython and CPython.
#
# To implement an HTTPS client using less code, use mip to install the requests package:
# https://github.com/micropython/micropython-lib/tree/master/python-ecosys/requests

import socket
import ssl


# `addr_family` selects IPv4 vs IPv6: 0 means either, or use
# socket.AF_INET or socket.AF_INET6 to select a particular one.
def main(domain, addr_family=0, use_stream=True):
    # Lookup the server address, for the given family and socket type.
    ai = socket.getaddrinfo(domain, 443, addr_family, socket.SOCK_STREAM)
    print("Address infos:", ai)

    # Select the first address.
    ai = ai[0]

    # Create a socket with the server's family, type and proto.
    s = socket.socket(ai[0], ai[1], ai[2])

    # Connect to the server.
    addr = ai[-1]
    print("Connect address:", addr)
    s.connect(addr)

    # Upgrade the socket to a TLS connection.
    ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    s = ctx.wrap_socket(s)
    print(s)

    # Send request and read response.
    if use_stream:
        # Both CPython and MicroPython SSLSocket objects support read() and
        # write() methods.
        s.write(b"GET / HTTP/1.0\r\n\r\n")
        print(s.read(4096))
    else:
        # MicroPython SSLSocket objects implement only stream interface, not
        # socket interface
        s.send(b"GET / HTTP/1.0\r\n\r\n")
        print(s.recv(4096))

    # Close the socket.
    s.close()


main("google.com")
