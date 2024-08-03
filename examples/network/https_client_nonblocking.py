# Example of a HTTPS client working with non-blocking sockets.
#
# Non-blocking SSL streams works differently in MicroPython compared to CPython.  In
# CPython a write to an SSLSocket may raise ssl.SSLWantReadError.  In MicroPython an
# SSLSocket behaves like a normal socket/stream and can be polled for reading/writing.

from errno import EINPROGRESS
import select
import socket
import ssl


def connect_nonblocking(sock, addr):
    sock.setblocking(False)
    try:
        sock.connect(addr)
    except OSError as er:
        if er.errno != EINPROGRESS:
            raise er


def write_nonblocking(poller, sock, data):
    poller.register(sock, select.POLLOUT)
    while data:
        poller.poll()
        n = sock.write(data)
        print("Wrote:", n)
        if n is not None:
            data = data[n:]


def read_nonblocking(poller, sock, n):
    poller.register(sock, select.POLLIN)
    poller.poll()
    data = sock.read(n)
    print("Read:", len(data))
    return data


def main(url, addr_family=0):
    # Split the given URL into components.
    proto, _, host, path = url.split(b"/", 3)
    assert proto == b"https:"

    # Note: this getaddrinfo() call is blocking!
    ai = socket.getaddrinfo(host, 443, addr_family, socket.SOCK_STREAM)[0]
    addr = ai[-1]
    print("Connect address:", addr)

    # Create a TCP socket and connect to the server in non-blocking mode.
    sock = socket.socket(ai[0], ai[1], ai[2])
    connect_nonblocking(sock, addr)

    # Wrap the TCP socket in an SSL stream in non-blocking mode.
    ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    sock = ctx.wrap_socket(sock, server_hostname=host, do_handshake_on_connect=False)
    sock.setblocking(False)

    # Create an object to poll the SSL stream for readability/writability.
    poller = select.poll()

    # Send the HTTP request on the SSL stream.
    request = b"GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n" % (path, host)
    write_nonblocking(poller, sock, request)

    # Receive the HTTP response from the SSL stream.
    response = read_nonblocking(poller, sock, 1000)
    for line in response.split(b"\n"):
        print(line)

    # Close the SSL stream.  This will also close the underlying TCP socket.
    sock.close()


main(b"https://micropython.org/ks/test.html")
