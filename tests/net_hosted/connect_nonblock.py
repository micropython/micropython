# test that socket.connect() on a non-blocking socket raises EINPROGRESS

import socket, errno


def test(peer_addr):
    s = socket.socket()
    s.setblocking(False)
    try:
        s.connect(peer_addr)
    except OSError as er:
        print(er.errno == errno.EINPROGRESS)
    s.close()


if __name__ == "__main__":
    test(socket.getaddrinfo("micropython.org", 80)[0][-1])
