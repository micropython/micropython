# test that socket.connect() on a non-blocking socket raises EINPROGRESS
# and that an immediate write/send/read/recv does the right thing

try:
    import usocket as socket, ussl as ssl, sys, time
except:
    import socket, ssl, sys, time


def dp(e):
    # print(e)  # uncomment this line for dev&test to print the actual exceptions
    pass


# do_connect establishes the socket and wraps it if requested
def do_connect(peer_addr, tls, handshake):
    s = socket.socket()
    s.setblocking(False)
    try:
        s.connect(peer_addr)
    except OSError as er:
        print("connect:", er.args[0] == 115)  # 115 is EINPROGRESS
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
    # if handshake is set, we wait after connect() so it has time to actually happen
    if handshake and not tls:  # with tls the handshake does it
        time.sleep(0.2)
    return s


def test(peer_addr, tls=False, handshake=False):
    # a fresh socket is opened for each combination because MP on linux is too fast

    # hasRW is false in CPython for sockets: they don't have read or write methods
    hasRW = sys.implementation.name == "micropython" or tls

    # connect + send
    s = do_connect(peer_addr, tls, handshake)
    # send -> 4 or EAGAIN
    try:
        ret = s.send(b"1234")
        print("send:", handshake and ret == 4)
    except OSError as er:
        dp(er)
        print("send:", er.args[0] == 11)  # 11 is EAGAIN
    s.close()

    # connect + write
    if hasRW:
        s = do_connect(peer_addr, tls, handshake)
        # write -> None
        try:
            ret = s.write(b"1234")
            print("write:", ret is (4 if handshake else None))
        except OSError as er:
            dp(er)
            print("write:", False)  # should not raise
        except ValueError as er:  # CPython
            dp(er)
            print("write:", er.args[0] == "Write on closed or unwrapped SSL socket.")
        s.close()
    else:  # fake it...
        print("connect:", True)
        print("write:", True)

    # connect + recv
    s = do_connect(peer_addr, tls, handshake)
    # recv -> EAGAIN
    try:
        print("recv:", s.recv(10))
    except OSError as er:
        dp(er)
        print("recv:", er.args[0] == 11)  # 11 is EAGAIN
    s.close()

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
        print("read:", True)


if __name__ == "__main__":
    # these tests use an non-existant test IP address, this way the connect takes forever and
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
