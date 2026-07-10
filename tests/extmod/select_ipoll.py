# Test select.ipoll().

try:
    import socket, select
except ImportError:
    print("SKIP")
    raise SystemExit

# Use a new UDP socket for tests, which should be writable but not readable.
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
    def test_ipoll_single_socket(self):
        poller = select.poll()

        # Register socket.
        poller.register(s)

        # Basic polling.
        self.assertEqual(list(poller.ipoll(0)), [(s, 4)])

        # Pass in flags=1 for one-shot behaviour.
        self.assertEqual(list(poller.ipoll(0, 1)), [(s, 4)])

        # Socket should be deregistered and poll should return nothing.
        self.assertEqual(list(poller.ipoll(0)), [])

    def test_ipoll_multiple_sockets(self):
        # Create a second socket.
        s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s2.bind(socket.getaddrinfo("127.0.0.1", 8001)[0][-1])

        poller = select.poll()

        # Register both sockets.
        poller.register(s)
        poller.register(s2)

        # Basic polling with two sockets.
        self.assertEqual(list(poller.ipoll(0)), [(s2, 4), (s2, 4)])

        # Unregister the first socket, to test polling the remaining one.
        poller.unregister(s)
        self.assertEqual(list(poller.ipoll(0)), [(s2, 4)])

        # Unregister the second socket, to test polling none.
        poller.unregister(s2)
        self.assertEqual(list(poller.ipoll(0)), [])

        s2.close()


if __name__ == "__main__":
    unittest.main()

s.close()
