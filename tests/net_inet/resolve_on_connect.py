# Test that the socket module performs DNS resolutions on bind and connect.
# Currenty only the esp32 port does this, so the test is restricted to that port.

import sys

if sys.implementation.name == "micropython" and sys.platform != "esp32":
    print("SKIP")
    raise SystemExit

import socket
import unittest


class Test(unittest.TestCase):
    def test_bind_resolves_0_0_0_0(self):
        s = socket.socket()
        self.assertEqual(s.bind(("0.0.0.0", 31245)), None)
        s.close()

    def test_bind_resolves_localhost(self):
        s = socket.socket()
        self.assertEqual(s.bind(("localhost", 31245)), None)
        s.close()

    def test_connect_resolves(self):
        s = socket.socket()
        self.assertEqual(s.connect(("micropython.org", 80)), None)
        s.close()

    def test_connect_non_existent(self):
        s = socket.socket()
        with self.assertRaises(OSError):
            s.connect(("nonexistent.example.com", 80))
        s.close()


if __name__ == "__main__":
    unittest.main()
