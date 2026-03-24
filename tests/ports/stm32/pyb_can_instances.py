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
    def test_controller_pairs(self):
        sequences = [(1, 2), (2, 1)]
        if HAS_CAN3:
            # Still test CAN controllers in pairs, but add ID 3
            sequences += [(1, 3), (3, 1), (2, 3), (3, 2)]

        for identifiers in sequences:
            id_a, id_b = identifiers
            with self.subTest("Testing CAN pair", id_a=id_a, id_b=id_b):
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
                        did_recv = False
                        try:
                            while True:
                                res = can.recv(0, timeout=50)
                                print("received", res)
                                if can == can_a:
                                    self.assertEqual(
                                        res[4], b"message1", "can_a should receive message1 only"
                                    )
                                if can == can_b:
                                    self.assertEqual(
                                        res[4], b"message2", "can_b should receive message2 only"
                                    )
                                self.assertFalse(
                                    did_recv, "Each instance should only receive 1x message"
                                )
                                did_recv = True
                        except OSError as e:
                            if e.errno != errno.ETIMEDOUT:
                                raise
                            if not did_recv:
                                self.fail("no rx on can_{} peripheral {}".format(which, can))
                            # otherwise it's OK as we've received our message already, and no more
                finally:
                    can_a.deinit()
                    can_b.deinit()


if __name__ == "__main__":
    unittest.main()
