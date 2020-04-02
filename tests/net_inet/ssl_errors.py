# test that socket.connect() on a non-blocking socket raises EINPROGRESS
# and that an immediate write/send/read/recv does the right thing

try:
    import usocket as socket, ussl as ssl, sys
except:
    import socket, ssl, sys


def test(addr, hostname, block=True):
    print("---", hostname or addr)
    s = socket.socket()
    s.setblocking(block)
    try:
        s.connect(addr)
        print("connected")
    except OSError as e:
        if e.args[0] != 115:  # 115 == EINPROGRESS
            raise

    try:
        s = ssl.wrap_socket(s)
        print("wrap: True")
    except OSError as e:
        print("wrap:", e)

    if not block:
        try:
            while s.write(b"0") is None:
                pass
        except OSError as e:
            print("write:", e)
    s.close()


if __name__ == "__main__":
    # connect to plain HTTP port, oops!
    addr = socket.getaddrinfo("micropython.org", 80)[0][-1]
    test(addr, None)
    # connect to plain HTTP port, oops!
    addr = socket.getaddrinfo("micropython.org", 80)[0][-1]
    test(addr, None, False)
    # connect to server with self-signed cert, oops!
    addr = socket.getaddrinfo("test.mosquitto.org", 8883)[0][-1]
    test(addr, "test.mosquitto.org")
