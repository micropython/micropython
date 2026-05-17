# Test deprecation of co_lnotab

try:
    (lambda: 0).__code__.co_code
except AttributeError:
    print("SKIP")
    raise SystemExit


import unittest
import sys


mpy_is_v2 = getattr(sys.implementation, '_v2', False)


def f():
    pass


class Test(unittest.TestCase):

    @unittest.skipIf(mpy_is_v2, "Removed in MicroPython v2 and later.")
    def test_co_lnotab_exists(self):
        self.assertIsInstance(f.__code__.co_lnotab, bytes)

    @unittest.skipUnless(mpy_is_v2, "Not removed before MicroPython v2.")
    def test_co_lnotab_removed(self):
        with self.assertRaises(AttributeError):
            f.__code__.co_lnotab


if __name__ == "__main__":
    unittest.main()
