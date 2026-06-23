# test non-blocking UDP sockets

try:
    import socket, errno
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
    def test_nonblocking(self):
        s.settimeout(0)

        with self.assertRaises(OSError) as ctx:
            s.recv(1)
        self.assertEqual(ctx.exception.errno, errno.EAGAIN)


if __name__ == "__main__":
    unittest.main()

s.close()
