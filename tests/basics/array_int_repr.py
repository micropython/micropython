# Test array integer representations in memory
#
# This has to be a unit test because correct internal representation depends on
# native endianness
#
# These test cases should pass on both CPython and MicroPython.

try:
    from array import array
    from sys import byteorder
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

# Ports without bigint support don't support typecode 'q'
try:
    array('q', [])
    array_has_typecode_q = True
except:
    array_has_typecode_q = False

class TestIntReprs(unittest.TestCase):
    def _test_repr(self, typecode, values):
        # create an array with the specified typecode and list of values
        a = array(typecode, values)
        a_hex = memoryview(a).hex()
        print(a, a_hex)

        self.assertEqual(len(a_hex) % len(values), 0)
        # no array.itemsize in MicroPython, so calculate item size
        sz = len(a_hex) // 2 // len(values)
        if hasattr(a, "itemsize"):
            self.assertEqual(a.itemsize, sz)

        # build alternative hex representation of the array using int.to_bytes()
        # on each value
        values_hex = ""
        for v in values:
            v_bytes = v.to_bytes(sz, byteorder=byteorder, signed=typecode.islower())
            values_hex += v_bytes.hex()

        # compare with the raw array contents
        self.assertEqual(a_hex, values_hex)

    def test_smaller_ints(self):
        for (typecode, initialiser) in (
                ('b', [1, -1, 120, -120]),
                ('B', [1, 5, 220]),
                ('h', [5, -1, 32_000, -32_000]),
                ('H', [5, 1, 32_000, 65_535]),
                ('i', [5, -1, 32_000, -32_000]),  # CPython only guarantees min 2 bytes, C style!
                ('I', [5, 1, 32_000, 65_535]),
                ('l', [5, -1, 2_000_000, -2_000_000, 0x7FFF_FFFF]),
                ('L', [5, 1, 65_536, 2_000_000, 0x7FFF_FFFF, 0xFFFF_FFFF]),
                ):
            self._test_repr(typecode, initialiser)

    @unittest.skipIf(not array_has_typecode_q, "port has no bigint support")
    def test_bigints(self):
        # Note: need to be careful not to write any literal expressions that can't be compiled on non-bigint MP
        a = 0x1FFF_FFF
        b = 62

        try:
            # this calculation will trigger OverflowError if bigint is set to long long
            max_uint64 = [2 ** (b + 1)]
        except OverflowError:
            max_uint64 = []

        for (typecode, initialiser) in (
                ('q', [a * 5, -a * 10, 2 ** b, (2 ** b) * -1]),
                ('Q', [a * 5, a * 10, 2 ** b, (2 ** b) - 1, (2 ** b) + 1] + max_uint64)
        ):
            self._test_repr(typecode, initialiser)


if __name__ == "__main__":
    unittest.main()
