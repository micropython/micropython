# Test that SLEEP_ENx registers are preserved over a call to machine.lightsleep().

import sys
from machine import mem32, lightsleep
import unittest

is_rp2350 = "RP2350" in sys.implementation._machine

if is_rp2350:
    CLOCK_BASE = 0x40010000
    SLEEP_EN0 = CLOCK_BASE + 0xB4
    SLEEP_EN1 = CLOCK_BASE + 0xB8
    TO_DISABLE_EN0 = 1 << 30  # SHA256
    TO_DISABLE_EN1 = 1 << 4  # SRAM0
else:
    CLOCK_BASE = 0x40008000
    SLEEP_EN0 = CLOCK_BASE + 0xA8
    SLEEP_EN1 = CLOCK_BASE + 0xAC
    TO_DISABLE_EN0 = 1 << 28  # SRAM0
    TO_DISABLE_EN1 = 1 << 0  # SRAM4


class Test(unittest.TestCase):
    def setUp(self):
        self.orig_sleep_en0 = mem32[SLEEP_EN0]
        self.orig_sleep_en1 = mem32[SLEEP_EN1]

    def tearDown(self):
        mem32[SLEEP_EN0] = self.orig_sleep_en0
        mem32[SLEEP_EN1] = self.orig_sleep_en1

    def test_sleep_en_regs(self):
        print()

        # Disable some bits so the registers aren't just 0xffff.
        mem32[SLEEP_EN0] &= ~TO_DISABLE_EN0
        mem32[SLEEP_EN1] &= ~TO_DISABLE_EN1

        # Get the registers before the lightsleep.
        sleep_en0_before = mem32[SLEEP_EN0] & 0xFFFFFFFF
        sleep_en1_before = mem32[SLEEP_EN1] & 0xFFFFFFFF
        print(hex(sleep_en0_before), hex(sleep_en1_before))

        # Do a lightsleep.
        lightsleep(100)

        # Get the registers after a lightsleep.
        sleep_en0_after = mem32[SLEEP_EN0] & 0xFFFFFFFF
        sleep_en1_after = mem32[SLEEP_EN1] & 0xFFFFFFFF
        print(hex(sleep_en0_after), hex(sleep_en1_after))

        # Check the registers have not changed.
        self.assertEqual(sleep_en0_before, sleep_en0_after)
        self.assertEqual(sleep_en1_before, sleep_en1_after)


if __name__ == "__main__":
    unittest.main()
