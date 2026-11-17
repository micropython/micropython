# Test when client does a TCP RST on an open connection

import struct, time, socket, select

try:
    import errno
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000


def convert_poll_list(l):
    # To be compatible across all ports/targets
    return [ev for _, ev in l]


# Server
def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(socket.getaddrinfo("0.0.0.0", PORT)[0][-1])
    s.listen(1)
    multitest.next()

    for iteration in range(2):
        print("server iteration", iteration)

        # Accept a connection from the client.
        s2, _ = s.accept()
        s2.setblocking(False)

        # Create a poller for the connected socket.
        poll = select.poll()
        poll.register(s2, select.POLLIN)

        # Wait for the client to send data and issue a TCP RST.
        for _ in range(10):
            if select.POLLIN | select.POLLERR | select.POLLHUP in convert_poll_list(
                poll.poll(1000)
            ):
                break
            time.sleep(0.1)
        print(convert_poll_list(poll.poll(1000)))

        # On the second test, drain the incoming data.
        if iteration == 1:
            for _ in range(5):
                try:
                    print(s2.recv(10))
                except OSError as er:
                    # This error should only happen on the 3rd recv.
                    print("ECONNRESET:", er.errno == errno.ECONNRESET)
                print(convert_poll_list(poll.poll(1000)))

        # Close the connection to the client.
        s2.close()
        print(convert_poll_list(poll.poll(1000)))

        # Try to read more data from the closed socket, to make sure the error code is correct.
        try:
            print(s2.recv(10))
        except OSError as er:
            print("EBADF:", er.errno == errno.EBADF)
        print(convert_poll_list(poll.poll(1000)))

    # Close the listening socket.
    s.close()


# Client
def instance1():
    if not hasattr(socket, "SO_LINGER"):
        multitest.skip()
    multitest.next()
    for iteration in range(2):
        print("client iteration", iteration)
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
        s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
        lgr_onoff = 1
        lgr_linger = 0
        s.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack("ii", lgr_onoff, lgr_linger))
        s.send(b"GET / HTTP/1.0\r\n\r\n")
        time.sleep(0.2)
        s.close()  # This issues a TCP RST since we've set the linger option
