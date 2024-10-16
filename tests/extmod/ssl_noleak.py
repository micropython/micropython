# Ensure that SSL sockets can be allocated sequentially
# without running out of available memory.
try:
    import io
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit


class TestSocket(io.IOBase):
    def write(self, buf):
        return len(buf)

    def readinto(self, buf):
        return 0

    def ioctl(self, cmd, arg):
        return 0

    def setblocking(self, value):
        pass


ITERS = 64

# Wrap sockets without a SSLContext
for n in range(ITERS):
    # print(n)
    s = TestSocket()
    s = ssl.wrap_socket(s, do_handshake=False)

# Single SSLContext, multiple sockets
ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
for n in range(ITERS):
    # print(n)
    s = TestSocket()
    s = ctx.wrap_socket(s, do_handshake_on_connect=False)

print("done")
