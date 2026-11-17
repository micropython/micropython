# test that socket.connect() on a non-blocking socket raises EINPROGRESS
# and that an immediate write/send/read/recv does the right thing

import unittest
import errno
import select
import socket
import ssl

# only mbedTLS supports non-blocking mode
ssl_supports_nonblocking = hasattr(ssl, "MBEDTLS_VERSION")


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
def do_connect(self, peer_addr, tls, handshake):
    s = socket.socket()
    s.setblocking(False)
    try:
        print("Connecting to", peer_addr)
        s.connect(peer_addr)
        self.fail()
    except OSError as er:
        print("connect:", errno_name(er.errno))
        self.assertEqual(er.errno, errno.EINPROGRESS)

    # wrap with ssl/tls if desired
    if tls:
        print("wrap socket")
        ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
        s = ssl_context.wrap_socket(s, do_handshake_on_connect=handshake)

    return s


# poll a socket and check the result
def poll(self, s, expect_writable):
    poller = select.poll()
    poller.register(s)
    result = poller.poll(0)
    print("poll:", result)
    if expect_writable:
        self.assertEqual(len(result), 1)
        self.assertEqual(result[0][1], select.POLLOUT)
    else:
        self.assertEqual(result, [])


# do_test runs the test against a specific peer address.
def do_test(self, peer_addr, tls, handshake):
    print()

    # MicroPython plain and TLS sockets have read/write
    hasRW = True

    # MicroPython plain sockets have send/recv
    # MicroPython TLS sockets don't have send/recv
    hasSR = not tls

    # connect + send
    # non-blocking send should raise EAGAIN
    if hasSR:
        s = do_connect(self, peer_addr, tls, handshake)
        poll(self, s, False)
        with self.assertRaises(OSError) as ctx:
            ret = s.send(b"1234")
        print("send error:", errno_name(ctx.exception.errno))
        self.assertEqual(ctx.exception.errno, errno.EAGAIN)
        s.close()

    # connect + write
    # non-blocking write should return None
    if hasRW:
        s = do_connect(self, peer_addr, tls, handshake)
        poll(self, s, tls and handshake)
        ret = s.write(b"1234")
        print("write:", ret)
        if tls and handshake:
            self.assertEqual(ret, 4)
        else:
            self.assertIsNone(ret)
        s.close()

    # connect + recv
    # non-blocking recv should raise EAGAIN
    if hasSR:
        s = do_connect(self, peer_addr, tls, handshake)
        poll(self, s, False)
        with self.assertRaises(OSError) as ctx:
            ret = s.recv(10)
        print("recv error:", errno_name(ctx.exception.errno))
        self.assertEqual(ctx.exception.errno, errno.EAGAIN)
        s.close()

    # connect + read
    # non-blocking read should return None
    if hasRW:
        s = do_connect(self, peer_addr, tls, handshake)
        poll(self, s, tls and handshake)
        ret = s.read(10)
        print("read:", ret)
        self.assertIsNone(ret)
        s.close()


class Test(unittest.TestCase):
    # these tests use a non-existent test IP address, this way the connect takes forever and
    # we can see EAGAIN/None (https://tools.ietf.org/html/rfc5737)
    def test_plain_sockets_to_nowhere(self):
        do_test(self, socket.getaddrinfo("192.0.2.1", 80)[0][-1], False, False)

    @unittest.skipIf(not ssl_supports_nonblocking, "SSL doesn't support non-blocking")
    def test_ssl_sockets_to_nowhere(self):
        do_test(self, socket.getaddrinfo("192.0.2.1", 443)[0][-1], True, False)

    def test_plain_sockets(self):
        do_test(self, socket.getaddrinfo("micropython.org", 80)[0][-1], False, False)

    @unittest.skipIf(not ssl_supports_nonblocking, "SSL doesn't support non-blocking")
    def test_ssl_sockets(self):
        do_test(self, socket.getaddrinfo("micropython.org", 443)[0][-1], True, True)


if __name__ == "__main__":
    unittest.main()
