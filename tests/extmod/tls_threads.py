# Ensure that SSL sockets can be allocated from multiple
# threads without thread safety issues

try:
    import _thread
    import io
    import tls
    import time
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


ITERS = 256


class TLSThreads(unittest.TestCase):
    def test_sslsocket_threaded(self):
        self.done = False
        # only run in two threads: too much RAM demand otherwise, and rp2 only
        # supports two anyhow
        _thread.start_new_thread(self._alloc_many_sockets, (True,))
        self._alloc_many_sockets(False)
        while not self.done:
            time.sleep(0.1)
        print("done")

    def _alloc_many_sockets(self, set_done_flag):
        print("start", _thread.get_ident())
        ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
        ctx.verify_mode = tls.CERT_NONE
        for n in range(ITERS):
            s = TestSocket()
            s = ctx.wrap_socket(s, do_handshake_on_connect=False)
            s.close()  # Free associated resources now from thread, not in a GC pass
        print("done", _thread.get_ident())
        if set_done_flag:
            self.done = True


if __name__ == "__main__":
    unittest.main()
