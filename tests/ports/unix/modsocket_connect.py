# Test socket.socket.connect() for both IPv4 and IPv6.

try:
    import socket
except ImportError:
    print("SKIP")
    raise SystemExit
import os
import unittest


TEST_HOSTNAME = "example.com"
TEST_PORT = 80


class Test(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # If the IPv6 address for example.com fails then skip AF_INET6 tests.
        try:
            entries = socket.getaddrinfo(TEST_HOSTNAME, TEST_PORT, socket.AF_INET6)
            cls._has_ipv6 = len(entries) > 0
        except:
            cls._has_ipv6 = False

        # The GitHub CI runner just doesn't work when it comes to IPv6.
        if os.getenv("GITHUB_ACTIONS") == "true":
            cls._has_ipv6 = False

        # If the IPv4 address for example.com fails then skip AF_INET tests.
        try:
            entries = socket.getaddrinfo(TEST_HOSTNAME, TEST_PORT, socket.AF_INET)
            cls._has_ipv4 = len(entries) > 0
        except:
            cls._has_ipv4 = False

    def test_hostname_connect_ipv4(self):
        if not self._has_ipv4:
            self.skipTest(
                "IPv4 connectivity may not be available or temporary name resolution failure detected"
            )

        s = socket.socket(socket.AF_INET)
        try:
            s.connect((TEST_HOSTNAME, TEST_PORT))
            s.send(b"GET /\n\n")
            data = s.recv(8)
            self.assertTrue(len(data) > 0)
        finally:
            s.close()

    def test_hostname_connect_ipv6(self):
        if not self._has_ipv6:
            self.skipTest(
                "IPv6 connectivity may not be available or temporary name resolution failure detected"
            )

        s = socket.socket(socket.AF_INET6)
        try:
            s.connect((TEST_HOSTNAME, TEST_PORT))
            s.send(b"GET /\n\n")
            data = s.recv(8)
            self.assertTrue(len(data) > 0)
        finally:
            s.close()

    def test_sockaddr_connect_ipv4(self):
        if not self._has_ipv4:
            self.skipTest(
                "IPv4 connectivity may not be available or temporary name resolution failure detected"
            )

        sockaddr = socket.getaddrinfo(TEST_HOSTNAME, TEST_PORT, socket.AF_INET)[0][-1]
        s = socket.socket(socket.AF_INET)
        try:
            s.connect(sockaddr)
            s.send(b"GET /\n\n")
            data = s.recv(8)
            self.assertTrue(len(data) > 0)
        finally:
            s.close()

    def test_sockaddr_connect_ipv6(self):
        if not self._has_ipv6:
            self.skipTest(
                "IPv6 connectivity may not be available or temporary name resolution failure detected"
            )

        sockaddr = socket.getaddrinfo(TEST_HOSTNAME, TEST_PORT, socket.AF_INET6)[0][-1]
        s = socket.socket(socket.AF_INET6)
        try:
            s.connect(sockaddr)
            s.send(b"GET /\n\n")
            data = s.recv(8)
            self.assertTrue(len(data) > 0)
        finally:
            s.close()


if __name__ == "__main__":
    unittest.main()
