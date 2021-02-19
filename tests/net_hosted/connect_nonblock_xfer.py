# test that socket.connect() on a non-blocking socket raises EINPROGRESS
# and that an immediate write/send/read/recv does the right thing

try:
    import sys, time
    import uerrno as errno, usocket as socket, ussl as ssl
except:
    import socket, errno, ssl
isMP = sys.implementation.name == "micropython"


def dp(e):
    # uncomment next line for development and testing, to print the actual exceptions
    # print(repr(e))
    pass


# do_connect establishes the socket and wraps it if tls is True.
# If handshake is true, the initial connect (and TLS handshake) is
# allowed to be performed before returning.
def do_connect(peer_addr, tls, handshake):
    s = socket.socket()
    s.setblocking(False)
    try:
        # print("Connecting to", peer_addr)
        s.connect(peer_addr)
    except OSError as er:
        print("connect:", er.args[0] == errno.EINPROGRESS)
        if er.args[0] != errno.EINPROGRESS:
            print("  got", er.args[0])
    # wrap with ssl/tls if desired
    if tls:
        try:
            if sys.implementation.name == "micropython":
                s = ssl.wrap_socket(s, do_handshake=handshake)
            else:
                s = ssl.wrap_socket(s, do_handshake_on_connect=handshake)
            print("wrap: True")
        except Exception as e:
            dp(e)
            print("wrap:", e)
    elif handshake:
        # just sleep a little bit, this allows any connect() errors to happen
        time.sleep(0.2)
    return s


# test runs the test against a specific peer address.
def test(peer_addr, tls=False, handshake=False):
    # MicroPython plain sockets have read/write, but CPython's don't
    # MicroPython TLS sockets and CPython's have read/write
    # hasRW captures this wonderful state of affairs
    hasRW = isMP or tls

    # MicroPython plain sockets and CPython's have send/recv
    # MicroPython TLS sockets don't have send/recv, but CPython's do
    # hasSR captures this wonderful state of affairs
    hasSR = not (isMP and tls)

    # connect + send
    if hasSR:
        s = do_connect(peer_addr, tls, handshake)
        # send -> 4 or EAGAIN
        try:
            ret = s.send(b"1234")
            print("send:", handshake and ret == 4)
        except OSError as er:
            #
            dp(er)
            print("send:", er.args[0] in (errno.EAGAIN, errno.EINPROGRESS))
        s.close()
    else:  # fake it...
        print("connect:", True)
        if tls:
            print("wrap:", True)
        print("send:", True)

    # connect + write
    if hasRW:
        s = do_connect(peer_addr, tls, handshake)
        # write -> None
        try:
            ret = s.write(b"1234")
            print("write:", ret in (4, None))  # SSL may accept 4 into buffer
        except OSError as er:
            dp(er)
            print("write:", False)  # should not raise
        except ValueError as er:  # CPython
            dp(er)
            print("write:", er.args[0] == "Write on closed or unwrapped SSL socket.")
        s.close()
    else:  # fake it...
        print("connect:", True)
        if tls:
            print("wrap:", True)
        print("write:", True)

    if hasSR:
        # connect + recv
        s = do_connect(peer_addr, tls, handshake)
        # recv -> EAGAIN
        try:
            print("recv:", s.recv(10))
        except OSError as er:
            dp(er)
            print("recv:", er.args[0] == errno.EAGAIN)
        s.close()
    else:  # fake it...
        print("connect:", True)
        if tls:
            print("wrap:", True)
        print("recv:", True)

    # connect + read
    if hasRW:
        s = do_connect(peer_addr, tls, handshake)
        # read -> None
        try:
            ret = s.read(10)
            print("read:", ret is None)
        except OSError as er:
            dp(er)
            print("read:", False)  # should not raise
        except ValueError as er:  # CPython
            dp(er)
            print("read:", er.args[0] == "Read on closed or unwrapped SSL socket.")
        s.close()
    else:  # fake it...
        print("connect:", True)
        if tls:
            print("wrap:", True)
        print("read:", True)


if __name__ == "__main__":
    # these tests use a non-existent test IP address, this way the connect takes forever and
    # we can see EAGAIN/None (https://tools.ietf.org/html/rfc5737)
    print("--- Plain sockets to nowhere ---")
    test(socket.getaddrinfo("192.0.2.1", 80)[0][-1], False, False)
    print("--- SSL sockets to nowhere ---")
    # this test fails with AXTLS because do_handshake=False blocks on first read/write and
    # there it times out until the connect is aborted
    test(socket.getaddrinfo("192.0.2.1", 443)[0][-1], True, False)
    print("--- Plain sockets ---")
    test(socket.getaddrinfo("micropython.org", 80)[0][-1], False, True)
    print("--- SSL sockets ---")
    test(socket.getaddrinfo("micropython.org", 443)[0][-1], True, True)
