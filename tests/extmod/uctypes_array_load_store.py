# Test uctypes array, load and store, with array size > 1
import unittest

try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

# MicroPython V2.0 will enforce bounds on uctypes items, V1.x truncates
import sys

is_v2 = hasattr(sys.implementation, "_v2")

N = 5
PLACEHOLDER = 99


class Test(unittest.TestCase):
    def test_native_endian(self):
        self._test_endian("NATIVE")

    def test_little_endian(self):
        self._test_endian("LITTLE_ENDIAN")

    def test_big_endian(self):
        self._test_endian("BIG_ENDIAN")

    def _test_endian(self, endian):
        for item_type in (
            "INT8",
            "UINT8",
            "INT16",
            "UINT16",
            "INT32",
            "UINT32",
            "INT64",
            "UINT64",
        ):
            print(endian, item_type)
            self._test_endian_type(endian, item_type)

    def _test_endian_type(self, endian, item_type):
        print("Testing array of", item_type, "with", endian, "endianness")
        desc = {"arr": (uctypes.ARRAY | 0, getattr(uctypes, item_type) | N)}
        print(repr(desc))
        sz = uctypes.sizeof(desc)
        data = bytearray(sz)
        print(sz, repr((uctypes.addressof(data), desc, getattr(uctypes, endian))))
        s = uctypes.struct(uctypes.addressof(data), desc, getattr(uctypes, endian))
        is_unsigned = item_type.startswith("U")
        item_sz = uctypes.sizeof({"": getattr(uctypes, item_type)})

        # V2 enforces range limits when setting fields
        item_min = 0 if is_unsigned else -(2 ** (item_sz * 8 - 1)) + 1
        item_max = 2 ** (item_sz * 8 if is_unsigned else (item_sz * 8 - 1)) - 1
        print("representable range", item_min, item_max)

        for i in range(N):
            n = i - 2
            print(i, n)
            if is_v2 and (n < item_min or n > item_max):
                with self.assertRaises(OverflowError):
                    s.arr[i] = n
                s.arr[i] = PLACEHOLDER
                n = PLACEHOLDER
            else:
                s.arr[i] = n

        print(endian, item_type, sz, *(s.arr[i] for i in range(N)))

        for i in range(N):
            n = i - 2
            if is_v2 and (n < item_min or n > item_max):
                # V2 will raise OverflowError, test swaps in placeholder
                n = PLACEHOLDER
            elif is_unsigned and n < 0:
                # V1 will mask and then load as unsigned int
                n = n & ((1 << (item_sz * 8)) - 1)

            self.assertEqual(s.arr[i], n)


if __name__ == "__main__":
    unittest.main()
