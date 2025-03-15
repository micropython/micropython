#
# MicroPython http_server_simplistic.py example
#
# This example shows how to write the smallest possible HTTP
# server in MicroPython. With comments and convenience code
# removed, this example can be compressed literally to ten
# lines. There's a catch though - read comments below for
# details, and use this code only for quick hacks, preferring
# http_server.py for "real thing".
#
import socket


CONTENT = b"""\
HTTP/1.0 200 OK

Hello #%d from MicroPython!
"""


def main():
    s = socket.socket()

    # Bind to (allow to be connected on ) all interfaces. This means
    # this server will be accessible to other hosts on your local
    # network, and if your server has direct (non-firewalled) connection
    # to the Internet, then to anyone on the Internet. We bind to all
    # interfaces to let this example work easily on embedded MicroPython
    # targets, which you will likely access from another machine on your
    # local network. Take care when running this on an Internet-connected
    # machine though! Replace "0.0.0.0" with "127.0.0.1" if in doubt, to
    # make the server accessible only on the machine it runs on.
    ai = socket.getaddrinfo("0.0.0.0", 8080)
    print("Bind address info:", ai)
    addr = ai[0][-1]

    # A port on which a socket listened remains inactive during some time.
    # This means that if you run this sample, terminate it, and run again
    # you will likely get an error. To avoid this timeout, set SO_REUSEADDR
    # socket option.
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    s.bind(addr)
    s.listen(5)
    print("Listening, connect your browser to http://<this_host>:8080/")

    counter = 0
    while True:
        res = s.accept()
        client_s = res[0]
        client_addr = res[1]
        print("Client address:", client_addr)
        print("Client socket:", client_s)
        # We assume here that .recv() call will read entire HTTP request
        # from client. This is usually true, at least on "big OS" systems
        # like Linux/MacOS/Windows. But that doesn't have to be true in
        # all cases, in particular on embedded systems, when there can
        # easily be "short recv", where it returns much less than requested
        # data size. That's why this example is called "simplistic" - it
        # shows that writing a web server in Python that *usually works* is
        # ten lines of code, and you can use this technique for quick hacks
        # and experimentation. But don't do it like that in production
        # applications - instead, parse HTTP request properly, as shown
        # by http_server.py example.
        req = client_s.recv(4096)
        print("Request:")
        print(req)
        client_s.send(CONTENT % counter)
        client_s.close()
        counter += 1
        print()


main()
