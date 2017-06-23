# test that socket.connect() on a non-blocking socket raises EINPROGRESS

try:
    import usocket as socket
except:
    import socket


def test(peer_addr):
    s = socket.socket()
    s.setblocking(False)
    try:
        s.connect(peer_addr)
    except OSError as er:
        print(er.args[0] == 115) # 115 is EINPROGRESS
    s.close()


if __name__ == "__main__":
    test(socket.getaddrinfo('micropython.org', 80)[0][-1])
