# Test machine.CAN timings results
#
# Single device test, assumes no connections to the CAN pins

try:
    from machine import CAN
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

from target_wiring import can_args, can_kwargs


class TestTimings(unittest.TestCase):
    def test_bitrate(self):
        for bitrate in (125_000, 250_000, 500_000, 1_000_000):
            can = CAN(*can_args, bitrate=bitrate, **can_kwargs)
            print(can)
            timings = can.get_timings()
            print(timings)
            # Actual bitrate may not be exactly equal to requested rate
            self.assertAlmostEqual(timings[0], bitrate, delta=1_000)
            can.deinit()

    def test_sample_point(self):
        # Verify that tseg1 and tseg2 are set correctly from the sample_point argument
        for sample_point in (66, 75, 95):
            can = CAN(*can_args, bitrate=500_000, sample_point=sample_point, **can_kwargs)
            _bitrate, _sjw, tseg1, tseg2, _fd, _port = can.get_timings()
            print(f"sample_point={sample_point}, tseg1={tseg1}, tseg2={tseg2}")
            self.assertAlmostEqual(sample_point / 100, tseg1 / (tseg1 + tseg2), delta=0.05)
            can.deinit()

    def test_tseg_args(self):
        # Verify that tseg1 and tseg2 are set correctly and sample_point is ignored if these are provided
        for tseg1, tseg2 in ((5, 2), (16, 8), (16, 5), (15, 5)):
            print(f"tseg1={tseg1} tseg2={tseg2}")
            can = CAN(
                *can_args, bitrate=250_000, tseg1=tseg1, tseg2=tseg2, sample_point=99, **can_kwargs
            )
            bitrate, _sjw, ret_tseg1, ret_tseg2, _fd, _port = can.get_timings()
            self.assertEqual(ret_tseg1, tseg1)
            self.assertEqual(ret_tseg2, tseg2)

    def test_invalid_timing_args(self):
        # Test various kwargs out of their allowed value ranges
        with self.assertRaises(ValueError):
            CAN(*can_args, bitrate=250_000, tseg1=55, **can_kwargs)
        with self.assertRaises(ValueError):
            CAN(*can_args, bitrate=500_000, tseg2=9, **can_kwargs)
        with self.assertRaises(ValueError):
            CAN(*can_args, bitrate=-1, **can_kwargs)
        with self.assertRaises(ValueError):
            CAN(*can_args, bitrate=500_000, sample_point=101, **can_kwargs)


if __name__ == "__main__":
    unittest.main()
