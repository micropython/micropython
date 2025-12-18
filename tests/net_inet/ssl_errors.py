# test that socket.connect() on a non-blocking socket raises EINPROGRESS
# and that an immediate write/send/read/recv does the right thing

import sys, errno, select, socket, ssl

if not hasattr(ssl, "CERT_REQUIRED"):
    print("SKIP")
    raise SystemExit


def test(addr, hostname, block=True):
    print("---", hostname)
    s = socket.socket(socket.AF_INET)
    s.setblocking(block)
    try:
        s.connect(addr)
        print("connected")
    except OSError as e:
        if e.errno != errno.EINPROGRESS:
            raise
        print("EINPROGRESS")

    if sys.implementation.name != "micropython":
        # in CPython we have to wait, otherwise wrap_socket is not happy
        select.select([], [s], [])

    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    ssl_context.verify_mode = ssl.CERT_REQUIRED

    try:
        s = ssl_context.wrap_socket(s, do_handshake_on_connect=block, server_hostname=hostname)
        print("wrap: True")
    except OSError:
        print("wrap: error")

    if not block:
        try:
            while s.write(b"0") is None:
                pass
        except (ValueError, OSError):  # CPython raises ValueError, MicroPython raises OSError
            print("write: error")
    s.close()


if __name__ == "__main__":
    # connect to plain HTTP port, oops!
    addr = socket.getaddrinfo("micropython.org", 80, socket.AF_INET)[0][-1]
    test(addr, "micropython.org")
    # connect to plain HTTP port, oops!
    addr = socket.getaddrinfo("micropython.org", 80, socket.AF_INET)[0][-1]
    test(addr, "micropython.org", False)
    # connect to server with self-signed cert, oops!
    addr = socket.getaddrinfo("test.mosquitto.org", 8883, socket.AF_INET)[0][-1]
    test(addr, "test.mosquitto.org")
