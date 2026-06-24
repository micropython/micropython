# Test behaviour when array module is provided out of bounds values
#
# This test is intended to also pass on CPython.

try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    import unittest
except MemoryError:
    print("SKIP-TOO-LARGE")  # some small boards can't fit unittest in RAM
    raise SystemExit


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

    def _set_out_of_range(self, arr, index, value):
        old_val = arr[index]
        with self.assertRaises(OverflowError):
            arr[index] = value
        self.assertEqual(arr[index], old_val)  # should be unchanged after exception

    @unittest.skipIf(not is_v2, "no overflow checks on v1")
    def test_setter_overflow_Q(self):
        arr = array("Q", [1, 2, 3, 4])
        self._set_out_of_range(arr, 0, -1)
        self._set_out_of_range(arr, 1, 1 << 65)
        self._set_out_of_range(arr, 2, -(2**65))

    @unittest.skipIf(not is_v2, "no overflow checks on v1")
    def test_setter_overflow_q(self):
        arr = array("q", [1, 2, 3, 4])
        self._set_out_of_range(arr, 1, 1 << 65)
        self._set_out_of_range(arr, 2, -(2**65))
        self._set_out_of_range(arr, 3, (1 << 64) - 1)  # negative in 64-bit 2s complement

    @unittest.skipIf(not is_v2, "no overflow checks on v1")
    def test_setter_overflow_I(self):
        arr = array("I", [5, 6, 7, 8])
        self._set_out_of_range(arr, 0, -1)
        self._set_out_of_range(arr, 1, 1 << 33)
        self._set_out_of_range(arr, 2, -(2**72))
        self._set_out_of_range(arr, 3, 1 << 65)

    @unittest.skipIf(not is_v2, "no overflow checks on v1")
    def test_setter_overflow_i(self):
        arr = array("i", [5, 6, 7, 8])
        self._set_out_of_range(arr, 0, 1 << 33)
        self._set_out_of_range(arr, 1, -(2**72))
        self._set_out_of_range(arr, 2, 0xFFFF_FFFF)  # negative in 32-bit 2s complement
        self._set_out_of_range(arr, 3, 1 << 65)


if __name__ == "__main__":
    unittest.main()
