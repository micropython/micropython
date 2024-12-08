# Ensure that SSL sockets can be allocated from multiple
# threads without thread safety issues
try:
    import _thread
    import io
    import ssl
    import sys
    import time
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


class DoneFlag:
    def __init__(self):
        self.value = False

    def set(self):
        self.value = True


def test_case(flag):
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

    if flag:
        flag.set()


# Only do this in two concurrent threads, as RAM demand is too high
# for some boards otherwise
done_flag = DoneFlag()
_thread.start_new_thread(test_case, (done_flag,))
test_case(None)

while not done_flag.value:
    time.sleep(0.1)
print("done")
