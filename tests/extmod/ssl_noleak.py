# Ensure that SSLSockets can be allocated sequentially
# without running out of available memory.
try:
    import io
    import tls
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest


class TestSocket(io.IOBase):
    def write(self, buf):
        return len(buf)

    def readinto(self, buf):
        return 0

    def ioctl(self, cmd, arg):
        return 0

    def setblocking(self, value):
        pass


ITERS = 128


class TLSNoLeaks(unittest.TestCase):
    def test_unique_context(self):
        for n in range(ITERS):
            print(n)
            s = TestSocket()
            ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
            ctx.verify_mode = tls.CERT_NONE
            s = ctx.wrap_socket(s, do_handshake_on_connect=False)

    def test_shared_context(self):
        # Single SSLContext, multiple sockets
        ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
        ctx.verify_mode = tls.CERT_NONE
        for n in range(ITERS):
            print(n)
            s = TestSocket()
            s = ctx.wrap_socket(s, do_handshake_on_connect=False)


if __name__ == "__main__":
    unittest.main()
