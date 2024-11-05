# test that socket.connect() on a non-blocking socket raises EINPROGRESS
# and that an immediate write/send/read/recv does the right thing

import errno
import select
import socket
import ssl

# only mbedTLS supports non-blocking mode
if not hasattr(ssl, "MBEDTLS_VERSION"):
    print("SKIP")
    raise SystemExit


# get the name of an errno error code
def errno_name(er):
    if er == errno.EAGAIN:
        return "EAGAIN"
    if er == errno.EINPROGRESS:
        return "EINPROGRESS"
    return er


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
        print("connect:", errno_name(er.errno))
    # wrap with ssl/tls if desired
    if tls:
        ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
        try:
            s = ssl_context.wrap_socket(s, do_handshake_on_connect=handshake)
            print("wrap ok: True")
        except Exception as e:
            print("wrap er:", e)
    return s


# poll a socket and print out the result
def poll(s):
    poller = select.poll()
    poller.register(s)
    print("poll:   ", poller.poll(0))


# test runs the test against a specific peer address.
def test(peer_addr, tls, handshake):
    # MicroPython plain and TLS sockets have read/write
    hasRW = True

    # MicroPython plain sockets have send/recv
    # MicroPython TLS sockets don't have send/recv
    hasSR = not tls

    # connect + send
    # non-blocking send should raise EAGAIN
    if hasSR:
        s = do_connect(peer_addr, tls, handshake)
        poll(s)
        try:
            ret = s.send(b"1234")
            print("send ok:", ret)  # shouldn't get here
        except OSError as er:
            print("send er:", errno_name(er.errno))
        s.close()

    # connect + write
    # non-blocking write should return None
    if hasRW:
        s = do_connect(peer_addr, tls, handshake)
        poll(s)
        ret = s.write(b"1234")
        print("write:  ", ret)
        s.close()

    # connect + recv
    # non-blocking recv should raise EAGAIN
    if hasSR:
        s = do_connect(peer_addr, tls, handshake)
        poll(s)
        try:
            ret = s.recv(10)
            print("recv ok:", ret)  # shouldn't get here
        except OSError as er:
            print("recv er:", errno_name(er.errno))
        s.close()

    # connect + read
    # non-blocking read should return None
    if hasRW:
        s = do_connect(peer_addr, tls, handshake)
        poll(s)
        ret = s.read(10)
        print("read:   ", ret)
        s.close()


if __name__ == "__main__":
    # these tests use a non-existent test IP address, this way the connect takes forever and
    # we can see EAGAIN/None (https://tools.ietf.org/html/rfc5737)
    print("--- Plain sockets to nowhere ---")
    test(socket.getaddrinfo("192.0.2.1", 80)[0][-1], False, False)
    print("--- SSL sockets to nowhere ---")
    test(socket.getaddrinfo("192.0.2.1", 443)[0][-1], True, False)
    print("--- Plain sockets ---")
    test(socket.getaddrinfo("micropython.org", 80)[0][-1], False, False)
    print("--- SSL sockets ---")
    test(socket.getaddrinfo("micropython.org", 443)[0][-1], True, True)
