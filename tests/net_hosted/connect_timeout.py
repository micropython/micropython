# Test that socket.connect() on a socket with timeout raises EINPROGRESS or ETIMEDOUT appropriately.

import errno
import socket


def test(peer_addr, timeout, expected_exc):
    s = socket.socket()
    s.settimeout(timeout)
    try:
        s.connect(peer_addr)
        print("OK")
    except OSError as er:
        print(er.args[0] in expected_exc)
    s.close()


if __name__ == "__main__":
    # This test needs an address that doesn't respond to TCP connections.
    # 1.1.1.1:8000 seem to reliably timeout, so use that.
    addr = socket.getaddrinfo("1.1.1.1", 8000)[0][-1]

    test(addr, 0, (errno.EINPROGRESS,))
    test(addr, 1, (errno.ETIMEDOUT, "timed out"))  # CPython uses a string instead of errno
