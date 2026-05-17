# Regression test for DMA for DCache coherency bugs with cache line
# written originally for https://github.com/micropython/micropython/issues/13471

# IMPORTANT: This test requires SPI2 MISO (pin Y8 on Pyboard D) to be connected to GND

import unittest
from machine import SPI

SPI_NUM = 2
BAUDRATE = 5_000_000


class Test(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.spi = SPI(SPI_NUM, baudrate=BAUDRATE)
        cls.skip_slow_test = False

    def test_variable_offset_fixed_length(self):
        buf = bytearray(1024)
        for offs in range(0, len(buf)):
            v = memoryview(buf)[offs : offs + 128]
            self.spi.readinto(v, 0xFF)
            ok = all(b == 0x00 for b in v)
            if not ok:
                print(offs, v.hex())
                self.skip_slow_test = True
            self.assertTrue(ok)

    def test_variable_offset_and_lengths(self):
        # this takes around 30s to run, so skipped if already failing
        if self.skip_slow_test:
            self.skipTest("already failing")

        buf = bytearray(1024)
        for op_len in range(1, 66):
            print(op_len)
            wr = b"\xff" * op_len
            for offs in range(1, len(buf) - op_len - 1):
                # Place some "sentinel" values before and after the DMA buffer
                before = offs & 0xFF
                after = (~offs) & 0xFF
                buf[offs - 1] = before
                buf[offs + op_len] = after
                v = memoryview(buf)[offs : offs + op_len]
                self.spi.write_readinto(wr, v)
                ok = (
                    all(b == 0x00 for b in v)
                    and buf[offs - 1] == before
                    and buf[offs + op_len] == after
                )
                if not ok:
                    print(v.hex())
                    print(hex(op_len), hex(offs), hex(buf[offs - 1]), hex(buf[offs + op_len]))
                self.assertTrue(ok)


if __name__ == "__main__":
    unittest.main()
