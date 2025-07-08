# Test deprecation of co_lnotab

try:
    (lambda: 0).__code__.co_code
except AttributeError:
    print("SKIP")
    raise SystemExit


import unittest
import sys


def f():
    pass


class Test(unittest.TestCase):
    @unittest.skipIf(getattr(sys.implementation,'_v2',False), "Removed in Micropython v2 and later.")
    def test_co_lnotab_exists(self):
        self.assertIsInstance(f.__code__.co_lnotab, bytes)
    
    @unittest.skipUnless(getattr(sys.implementation,'_v2',False), "Not removed before Micropython v2.")
    def test_co_lnotab_removed(self):
        with self.assertRaises(AttributeError):
            f.__code__.co_lnotab


if __name__ == "__main__":
    unittest.main()