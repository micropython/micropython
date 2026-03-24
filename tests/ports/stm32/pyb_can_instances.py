# Test that initialising CAN instances in any order doesn't break
# TX, RX or filtering
try:
    from pyb import CAN

    CAN(2)  # skip any board which doesn't have at least 2 CAN peripherals
except (ImportError, ValueError):
    print("SKIP")
    raise SystemExit

import unittest
import errno

# Classic CAN (aka bxCAN) hardware has a different filter API
# and some different behaviours to newer FDCAN hardware
IS_CLASSIC = hasattr(CAN, "MASK16")

# Some boards have 3x CAN peripherals, test all three
HAS_CAN3 = True
try:
    CAN(3)
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
        try:
            can_a = CAN(id_a, CAN.LOOPBACK)
            if IS_CLASSIC:
                can_a.setfilter(0, CAN.LIST16, 0, (123, 124, 125, 126))
            else:
                can_a.setfilter(0, CAN.RANGE, 0, (123, 126))

            can_b = CAN(id_b, CAN.LOOPBACK)
            if IS_CLASSIC:
                can_b.setfilter(0, CAN.LIST16, 0, (3, 4, 5, 6))
            else:
                can_b.setfilter(0, CAN.RANGE, 0, (3, 6))

            # Drain any old messages in RX FIFOs
            for can in (can_a, can_b):
                while can.any(0):
                    can.recv(0)

            for which, id, can in (("A", id_a, can_a), ("B", id_b, can_b)):
                print("testing config", which, "with controller", can)
                # message1 should only receive on can_a, message2 on can_b
                can.send("message1", 123)
                can.send("message2", 3)
                n_recv = 0
                try:
                    while True:
                        res = can.recv(0, timeout=50)
                        n_recv += 1
                        print("received", res)
                        if can == can_a:
                            self.assertEqual(
                                res[4], b"message1", "can_a should receive message1 only"
                            )
                        if can == can_b:
                            self.assertEqual(
                                res[4], b"message2", "can_b should receive message2 only"
                            )
                except OSError as e:
                    if e.errno != errno.ETIMEDOUT:
                        raise
                    print("recv timed out")
                self.assertEqual(n_recv, 1, "Each instance should receive exactly 1 message")
        finally:
            can_a.deinit()
            can_b.deinit()


if __name__ == "__main__":
    unittest.main()
