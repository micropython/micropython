# Test multiple concurrent CAN instances using loopback.
# Initialising in any order shouldn't break TX, RX or filtering.
#
# This test is ported from tests/ports/stm32/pyb_can_instances.py

try:
    from machine import CAN

    CAN(2, 125_000)  # skip any board which doesn't have at least 2 CAN peripherals
except (ImportError, ValueError):
    print("SKIP")
    raise SystemExit

import time
import unittest

# Some boards have 3x CAN peripherals, test all three
HAS_CAN3 = True
try:
    CAN(3, 125_000)
except ValueError:
    HAS_CAN3 = False


class Test(unittest.TestCase):
    def test_can12(self):
        self._test_pairs([(1, 2), (2, 1)])

    @unittest.skipUnless(HAS_CAN3, "no CAN3")
    def test_can3(self):
        self._test_pairs([(1, 3), (3, 1), (2, 3), (3, 2)])

    def _test_pairs(self, seq):
        for id_a, id_b in seq:
            with self.subTest("Testing CAN pair", id_a=id_a, id_b=id_b):
                self._test_controller_pair(id_a, id_b)

    def _test_controller_pair(self, id_a, id_b):
        # Setting up each CAN peripheral independently is deliberate here, to catch
        # catch cases where initialising CAN2 breaks CAN1 or vice versa
        can_a = CAN(id_a, 125_000, mode=CAN.MODE_LOOPBACK)
        can_a.set_filters([(0x100, 0x700, 0)])

        can_b = CAN(id_b, 125_000, mode=CAN.MODE_LOOPBACK)
        can_b.set_filters([(0x000, 0x7F0, 0)])

        try:
            # Drain any old messages in RX FIFOs
            for can in (can_a, can_b):
                while can.recv():
                    pass

            for which, id, can in (("A", id_a, can_a), ("B", id_b, can_b)):
                # print("testing config", which, "with controller", can)
                # message1 should only receive on can_a, message2 on can_b
                can.send(0x123, "message1", 0)
                can.send(0x003, "message2", 0)
                time.sleep_ms(10)
                n_recv = 0
                while res := can.recv():
                    n_recv += 1
                    # print(res)
                    if can == can_a:
                        self.assertEqual(res[1], b"message1", "can_a should receive message1 only")
                    if can == can_b:
                        self.assertEqual(res[1], b"message2", "can_b should receive message2 only")
                self.assertEqual(n_recv, 1, "Each instance should receive exactly 1 message")
        finally:
            can_a.deinit()
            can_b.deinit()


if __name__ == "__main__":
    unittest.main()
