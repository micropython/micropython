# Test behaviour when array module is provided out of bounds values
#
# This test is intended to also pass on CPython.

try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

# MicroPython V2.0 will enforce bounds on items (same as CPython), V1.x truncates
#
# Note: once <V2.0 behaviour is dropped, this could be changed to a simple test
# that compares output with CPython
import sys

is_v2 = hasattr(sys.implementation, "_v2") or "cpython" in str(sys.implementation)

UNSIGNED_TYPES = ("Q", "L", "I", "H", "B")


class Test(unittest.TestCase):
    def _test_invalid(self, typecode, value, truncated_value):
        # Test creating an array with the specified invalid value
        # for the given typecode (MP 1.x will verify the value truncates
        # to the expected 'truncated_value'.)
        if is_v2:
            with self.assertRaises(OverflowError):
                array(typecode, [value])
        else:
            a = array(typecode, [value])
            self.assertEqual(a[0], truncated_value)

    def _test_valid(self, typecode, value):
        # Test creating an array with the specified valid value
        # for the given typecode
        a = array(typecode, [value])
        self.assertEqual(a[0], int(value))

    def test_bigint_overflow(self):
        # big int -1
        self._test_invalid("Q", -(2**64) // 2**64, 0xFFFFFFFFFFFFFFFF)
        # self._test_invalid("L", -(2**64) // 2**64)  # 32-bit vs 64-bit results differ
        self._test_invalid("I", -(2**64) // 2**64, 0xFFFFFFFF)
        self._test_invalid("H", -(2**64) // 2**64, 0xFFFF)
        self._test_invalid("B", -(2**64) // 2**64, 0xFF)

        # big int 2**63
        self._test_invalid("q", 2**63, -0x8000000000000000)
        # self._test_invalid("l", 2**63)  # 32-bit vs 64-bit results differ
        self._test_invalid("i", 2**63, 0)
        self._test_invalid("h", 2**63, 0)
        self._test_invalid("b", 2**63, 0)

    def test_unsigned_negative(self):
        self._test_invalid("Q", -1, 0xFFFFFFFFFFFFFFFF)
        # self._test_invalid("L", -(2**64) // 2**64)  # 32-bit vs 64-bit results differ
        self._test_invalid("I", -1, 0xFFFFFFFF)
        self._test_invalid("H", -1, 0xFFFF)
        self._test_invalid("B", -1, 0xFF)

    def test_zero(self):
        for code in UNSIGNED_TYPES:
            self._test_valid(code, 0)

    def test_one(self):
        for code in UNSIGNED_TYPES:
            self._test_valid(code, 1)

    def test_array(self):
        self._test_valid("b", True)
        self._test_valid("b", False)


if __name__ == "__main__":
    unittest.main()
