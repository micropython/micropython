import errno
import socket


def test(peer_addr):
    s = socket.socket()
    s.settimeout(1)
    try:
        s.connect(peer_addr)
    except OSError as er:
        print(er.errno == errno.ETIMEDOUT or str(er) == "timed out")
    s.close()


if __name__ == "__main__":
    test(socket.getaddrinfo("192.0.0.0", 8888)[0][-1])
