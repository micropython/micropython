# test machine memory read/write
# requires: slice

try:
    import machine

    machine.mem32
except:
    print("SKIP")
    raise SystemExit

try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

import sys
import unittest

if sys.platform == "linux":
    # The unix port on Linux treats machine.mem* addresses as physical
    # addresses and maps them via /dev/mem, so a buffer can't be used.
    print("SKIP")
    raise SystemExit


class TestReadWrite(unittest.TestCase):
    def setUp(self):
        # Over-allocate and align up to 8 bytes; addressof() gives no alignment guarantee.
        self.buf = bytearray(16)
        base = uctypes.addressof(self.buf)
        self.addr = (base + 7) & ~7
        self.off = self.addr - base

    def check_buf(self, value, size):
        self.assertEqual(
            bytes(self.buf[self.off : self.off + size]), value.to_bytes(size, sys.byteorder)
        )

    def test_mem8(self):
        machine.mem8[self.addr] = 0x5A
        self.assertEqual(machine.mem8[self.addr], 0x5A)
        self.check_buf(0x5A, 1)
        # Stores truncate to the access width.
        machine.mem8[self.addr] = 0x1A5
        self.assertEqual(machine.mem8[self.addr], 0xA5)
        machine.mem8[self.addr] = -1
        self.assertEqual(machine.mem8[self.addr], 0xFF)

    def test_mem16(self):
        machine.mem16[self.addr] = 0xBEEF
        self.assertEqual(machine.mem16[self.addr], 0xBEEF)
        self.check_buf(0xBEEF, 2)
        machine.mem16[self.addr] = -1
        self.assertEqual(machine.mem16[self.addr], 0xFFFF)

    def test_mem32(self):
        # mem32 reads are signed on ports with 32-bit ints, so mask the result.
        machine.mem32[self.addr] = 0xDEADBEEF
        self.assertEqual(machine.mem32[self.addr] & 0xFFFFFFFF, 0xDEADBEEF)
        self.check_buf(0xDEADBEEF, 4)
        machine.mem32[self.addr] = -1
        self.assertEqual(machine.mem32[self.addr] & 0xFFFFFFFF, 0xFFFFFFFF)

    def test_widths_agree(self):
        machine.mem32[self.addr] = 0x11223344
        if sys.byteorder == "little":
            self.assertEqual(machine.mem8[self.addr], 0x44)
            self.assertEqual(machine.mem16[self.addr + 2], 0x1122)
        else:
            self.assertEqual(machine.mem8[self.addr], 0x11)
            self.assertEqual(machine.mem16[self.addr + 2], 0x3344)

    def test_alignment_message(self):
        try:
            (lambda: 0)(0)
        except TypeError as e:
            if not str(e):
                # MICROPY_ERROR_REPORTING_NONE gives exceptions no message text.
                self.skipTest("no error messages")
        # A misaligned address with the top address bit set: the error message must
        # report the whole address, not just its low 32 bits.
        ptr_bits = 8 * uctypes.sizeof({"p": (uctypes.PTR | 0, uctypes.UINT8)})
        addr = (1 << (ptr_bits - 1)) | 1
        with self.assertRaises(ValueError) as cm:
            machine.mem16[addr]
        self.assertEqual(str(cm.exception), "address %08x is not aligned to 2 bytes" % addr)


if __name__ == "__main__":
    unittest.main()
