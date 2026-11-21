# test machine.mem64
# requires: int-big slice

try:
    import machine

    machine.mem64
except:
    print("SKIP")
    raise SystemExit

import sys
import unittest

try:
    import uctypes
except ImportError:
    uctypes = None


class Test(unittest.TestCase):
    def test_print(self):
        self.assertEqual(repr(machine.mem64), "<64-bit memory>")

    def test_alignment(self):
        for addr in (1, 4):
            with self.assertRaises(ValueError):
                machine.mem64[addr]
            with self.assertRaises(ValueError):
                machine.mem64[addr] = 1


class TestReadWrite(unittest.TestCase):
    # These tests access real memory, using a buffer owned by this process.
    # Each test calls this first, rather than using setUp(), because
    # micropython-lib's unittest doesn't handle SkipTest raised by setUp().
    def setup_buf(self):
        if uctypes is None:
            self.skipTest("needs uctypes")
        if sys.platform == "linux":
            # The unix port on Linux treats machine.mem* addresses as physical
            # addresses and maps them via /dev/mem, so a buffer can't be used.
            self.skipTest("addresses are physical on linux")
        # Over-allocate and align up to 8 bytes; addressof() gives no alignment guarantee.
        self.buf = bytearray(16)
        base = uctypes.addressof(self.buf)
        self.addr = (base + 7) & ~7
        self.off = self.addr - base

    def test_read_write(self):
        self.setup_buf()
        # Small value, large value, and values with the top bit set.
        for value in (0x5A, 0x1122334455667788, 0xDEADBEEFCAFEBABE, 0xFFFFFFFFFFFFFFFF):
            machine.mem64[self.addr] = value
            self.assertEqual(machine.mem64[self.addr], value)
            self.assertEqual(
                bytes(self.buf[self.off : self.off + 8]), value.to_bytes(8, sys.byteorder)
            )

    def test_truncation(self):
        self.setup_buf()
        # Stores truncate to the access width.
        machine.mem64[self.addr] = -1
        self.assertEqual(machine.mem64[self.addr], 0xFFFFFFFFFFFFFFFF)
        machine.mem64[self.addr] = (1 << 70) | 0x1234
        self.assertEqual(machine.mem64[self.addr], 0x1234)

    def test_widths_agree(self):
        self.setup_buf()
        machine.mem64[self.addr] = 0x1122334455667788
        lo, hi = machine.mem32[self.addr], machine.mem32[self.addr + 4]
        if sys.byteorder == "big":
            lo, hi = hi, lo
        self.assertEqual((lo, hi), (0x55667788, 0x11223344))

    def test_store_non_int(self):
        self.setup_buf()
        with self.assertRaises(TypeError):
            machine.mem64[self.addr] = "hello"


if __name__ == "__main__":
    unittest.main()
