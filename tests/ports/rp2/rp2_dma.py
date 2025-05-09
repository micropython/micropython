# Test rp2.DMA functionality.

import sys
import time
import machine
import rp2
import unittest

is_rp2350 = "RP2350" in sys.implementation._machine

SRC = bytes(i & 0xFF for i in range(16 * 1024))


class Test(unittest.TestCase):
    def setUp(self):
        self.dma = rp2.DMA()

    def tearDown(self):
        self.dma.close()

    def test_printing(self):
        dma = self.dma
        self.assertEqual(str(dma), "DMA({})".format(dma.channel))

    def test_pack_unpack_ctrl(self):
        dma = self.dma
        ctrl_dict = rp2.DMA.unpack_ctrl(dma.pack_ctrl())
        if is_rp2350:
            self.assertEqual(len(ctrl_dict), 18)
            self.assertTrue("inc_read_rev" in ctrl_dict)
            self.assertTrue("inc_write_rev" in ctrl_dict)
        else:
            self.assertEqual(len(ctrl_dict), 16)
        self.assertEqual(ctrl_dict["ahb_err"], 0)
        self.assertEqual(ctrl_dict["bswap"], 0)
        self.assertEqual(ctrl_dict["busy"], 0)
        self.assertEqual(ctrl_dict["chain_to"], dma.channel)
        self.assertEqual(ctrl_dict["enable"], 1)
        self.assertEqual(ctrl_dict["high_pri"], 0)
        self.assertEqual(ctrl_dict["inc_read"], 1)
        self.assertEqual(ctrl_dict["inc_write"], 1)
        self.assertEqual(ctrl_dict["irq_quiet"], 1)
        self.assertEqual(ctrl_dict["read_err"], 0)
        self.assertEqual(ctrl_dict["ring_sel"], 0)
        self.assertEqual(ctrl_dict["ring_size"], 0)
        self.assertEqual(ctrl_dict["size"], 2)
        self.assertEqual(ctrl_dict["sniff_en"], 0)
        self.assertEqual(ctrl_dict["treq_sel"], 63)
        self.assertEqual(ctrl_dict["write_err"], 0)

    def test_register_access(self):
        dma = self.dma
        dma.read = 0
        dma.write = 0
        dma.count = 0
        dma.ctrl = dma.pack_ctrl()
        self.assertEqual(dma.read, 0)
        self.assertEqual(dma.write, 0)
        self.assertEqual(dma.count, 0)
        self.assertEqual(dma.ctrl & 0x01F, 25)
        self.assertIn(dma.channel, range(16))
        self.assertIsInstance(dma.registers, memoryview)

    def test_close(self):
        dma = self.dma
        dma.close()

        # Test closing when already closed.
        dma.close()

        # Test using when closed.
        with self.assertRaises(ValueError):
            dma.active()

    def test_simple_memory_copy(self):
        dma = self.dma
        dest = bytearray(1024)
        dma.config(read=SRC, write=dest, count=len(dest) // 4, ctrl=dma.pack_ctrl(), trigger=False)
        self.assertFalse(any(dest))
        dma.active(True)
        while dma.active():
            pass
        self.assertEqual(dest[:8], SRC[:8])
        self.assertEqual(dest[-8:], SRC[-8:])

    def test_time_taken_for_large_memory_copy(self):
        def run_and_time_dma(dma):
            ticks_us = time.ticks_us
            active = dma.active
            irq_state = machine.disable_irq()
            t0 = ticks_us()
            active(True)
            while active():
                pass
            t1 = ticks_us()
            machine.enable_irq(irq_state)
            return time.ticks_diff(t1, t0)

        num_average = 10
        dt_sum = 0
        for _ in range(num_average):
            dma = self.dma
            dest = bytearray(16 * 1024)
            dma.read = SRC
            dma.write = dest
            dma.count = len(dest) // 4
            dma.ctrl = dma.pack_ctrl()
            dt_sum += run_and_time_dma(dma)
            self.assertEqual(dest[:8], SRC[:8])
            self.assertEqual(dest[-8:], SRC[-8:])
            self.tearDown()
            self.setUp()
        dt = dt_sum // num_average
        self.assertIn(dt, range(30, 80))

    def test_config_trigger(self):
        # Test using .config(trigger=True) to start DMA immediately.
        dma = self.dma
        dest = bytearray(1024)
        dma.config(read=SRC, write=dest, count=len(dest) // 4, ctrl=dma.pack_ctrl(), trigger=True)
        while dma.active():
            pass
        self.assertEqual(dest[:8], SRC[:8])
        self.assertEqual(dest[-8:], SRC[-8:])

    def test_irq(self):
        def callback(dma):
            nonlocal irq_flags
            print("irq fired")
            irq_flags = dma.irq().flags()

        dma = self.dma
        irq_flags = None
        dest = bytearray(1024)
        dma.irq(callback)
        dma.config(
            read=SRC,
            write=dest,
            count=len(dest) // 4,
            ctrl=dma.pack_ctrl(irq_quiet=0),
            trigger=True,
        )
        while dma.active():
            pass
        time.sleep_ms(1)  # when running as native code, give the scheduler a chance to run
        self.assertEqual(irq_flags, 1)
        self.assertEqual(dest[:8], SRC[:8])
        self.assertEqual(dest[-8:], SRC[-8:])


if __name__ == "__main__":
    unittest.main()
