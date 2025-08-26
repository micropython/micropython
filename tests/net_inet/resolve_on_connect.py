# Test that the socket module performs DNS resolutions on bind and connect
# Only works on the esp32 port at the moment

import sys

if sys.implementation.name == "micropython" and sys.platform != "esp32":
    print("SKIP")
    raise SystemExit

import socket
import unittest


class Test(unittest.TestCase):
    def test_bind_resolves_0_0_0_0(self):
        s = socket.socket()
        s.bind(("0.0.0.0", 31245))
        s.close()

    def test_bind_resolves_localhost(self):
        s = socket.socket()
        s.bind(("localhost", 31245))
        s.close()

    def test_connect_resolves(self):
        s = socket.socket()
        s.connect(("micropython.org", 80))
        s.close()

    def test_connect_non_existent(self):
        s = socket.socket()
        with self.assertRaises(OSError):
            s.connect(("nonexistent.example.com", 80))
        s.close()


if __name__ == "__main__":
    unittest.main()
