# Test machine.Encoder implementation
#
# IMPORTANT: This test requires hardware connections:
# - out0_pin and in0_pin must be wired together.
# - out1_pin and in1_pin must be wired together.

try:
    from machine import Encoder
except ImportError:
    print("SKIP")
    raise SystemExit

import sys
import unittest
from machine import Pin
from target_wiring import encoder_loopback_id, encoder_loopback_out_pins, encoder_loopback_in_pins

PRINT = False
PIN_INIT_VALUE = 1

id = encoder_loopback_id
out0_pin, out1_pin = encoder_loopback_out_pins
in0_pin, in1_pin = encoder_loopback_in_pins

out0_pin = Pin(out0_pin, mode=Pin.OUT)
in0_pin = Pin(in0_pin, mode=Pin.IN)
out1_pin = Pin(out1_pin, mode=Pin.OUT)
in1_pin = Pin(in1_pin, mode=Pin.IN)


class TestEncoder(unittest.TestCase):
    def setUp(self):
        out0_pin(PIN_INIT_VALUE)
        out1_pin(PIN_INIT_VALUE)
        self.enc = Encoder(id, in0_pin, in1_pin, phases=1)
        self.enc2 = Encoder(id + 1, in0_pin, in1_pin, phases=2)
        self.enc4 = Encoder(id + 2, in0_pin, in1_pin, phases=4)
        self.pulses = 0  # track the expected encoder position in software
        if PRINT:
            print(
                "\nout0_pin() out1_pin() enc.value() enc2.value() enc4.value() |",
                out0_pin(),
                out1_pin(),
                "|",
                self.enc.value(),
                self.enc2.value(),
                self.enc4.value(),
            )

    def tearDown(self):
        self.enc.deinit()
        try:
            self.enc2.deinit()
        except:
            pass
        try:
            self.enc4.deinit()
        except:
            pass

    def rotate(self, pulses):
        for _ in range(abs(pulses)):
            self.pulses += 1 if (pulses > 0) else -1
            if pulses > 0:
                if self.pulses % 2:
                    out0_pin(not out0_pin())
                else:
                    out1_pin(not out1_pin())
            else:
                if self.pulses % 2:
                    out1_pin(not out1_pin())
                else:
                    out0_pin(not out0_pin())
        if PRINT:
            print(
                "out0_pin() out1_pin() enc.value() enc2.value() enc4.value() pulses self.pulses |",
                out0_pin(),
                out1_pin(),
                "|",
                self.enc.value(),
                self.enc2.value(),
                self.enc4.value(),
                "|",
                pulses,
                self.pulses,
            )

    def assertPosition(self, value, value2=None, value4=None):
        self.assertEqual(self.enc.value(), value)
        if not value2 is None:
            self.assertEqual(self.enc2.value(), value2)
        if not value4 is None:
            self.assertEqual(self.enc4.value(), value4)
        pass

    @unittest.skipIf(sys.platform == "mimxrt", "cannot read back the pin")
    def test_connections(self):
        # Test the hardware connections are correct. If this test fails, all tests will fail.
        for ch, outp, inp in ((0, out0_pin, in0_pin), (1, out1_pin, in1_pin)):
            print("Testing channel ", ch)
            outp(1)
            self.assertEqual(1, inp())
            outp(0)
            self.assertEqual(0, inp())

    def test_basics(self):
        self.assertPosition(0)
        self.rotate(100)
        self.assertPosition(100 // 4, 100 // 2, 100)
        self.rotate(-100)
        self.assertPosition(0)

    def test_partial(self):
        # With phase=1 (default), need 4x pulses to count a rotation
        self.assertPosition(0)
        self.rotate(1)
        self.assertPosition(1, 1, 1)
        self.rotate(1)
        self.assertPosition(1, 1, 2)
        self.rotate(1)
        self.assertPosition(1, 2, 3)
        self.rotate(1)
        self.assertPosition(1, 2, 4)  # +4
        self.rotate(1)
        self.assertPosition(2, 3, 5)
        self.rotate(1)
        self.assertPosition(2, 3, 6)
        self.rotate(1)
        self.assertPosition(2, 4, 7)
        self.rotate(1)
        self.assertPosition(2, 4, 8)  # +4
        self.rotate(-1)
        self.assertPosition(2, 4, 7)
        self.rotate(-3)
        self.assertPosition(1, 2, 4)  # -4
        self.rotate(-4)
        self.assertPosition(0, 0, 0)  # -4
        self.rotate(-1)
        self.assertPosition(0, 0, -1)
        self.rotate(-1)
        self.assertPosition(0, -1, -2)
        self.rotate(-1)
        self.assertPosition(0, -1, -3)
        self.rotate(-1)
        self.assertPosition(-1, -2, -4)  # -4
        self.rotate(-1)
        self.assertPosition(-1, -2, -5)
        self.rotate(-3)
        self.assertPosition(-2, -4, -8)  # -4


if __name__ == "__main__":
    unittest.main()
