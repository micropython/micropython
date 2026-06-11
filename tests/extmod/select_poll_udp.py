# test select.poll on UDP sockets

try:
    import socket, select
except ImportError:
    print("SKIP")
    raise SystemExit

# Some targets (eg PYBV10) have the socket module but are unable to create
# UDP sockets without a registered NIC.
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(socket.getaddrinfo("127.0.0.1", 8000)[0][-1])
except OSError:
    print("SKIP")
    raise SystemExit

import unittest


class Test(unittest.TestCase):
    def test_poll(self):
        poll = select.poll()

        # UDP socket should not be readable.
        poll.register(s, select.POLLIN)
        res = poll.poll(0)
        self.assertEqual(len(res), 0)

        # UDP socket should be writable.
        poll.modify(s, select.POLLOUT)
        res = poll.poll(0)
        self.assertEqual(res[0][1], select.POLLOUT)

    @unittest.skipUnless(hasattr(select, "select"), "no select")
    def test_select(self):
        # UDP socket should only be writable.
        r, w, e = select.select([s], [s], [s], 0)
        self.assertEqual(r, [])
        self.assertEqual(w, [s])
        self.assertEqual(e, [])


if __name__ == "__main__":
    unittest.main()

s.close()
