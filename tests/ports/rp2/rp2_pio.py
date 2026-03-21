# Test rp2.PIO and rp2.StateMachine functionality.

import sys
import time
import rp2
import unittest

is_rp2350 = "RP2350" in sys.implementation._machine


# ---------------------------------------------------------------------------
# Shared programs used across tests
# ---------------------------------------------------------------------------


@rp2.asm_pio()
def prog_loopback():
    # Copies each word from TX FIFO to RX FIFO.
    pull()
    mov(isr, osr)
    push()


@rp2.asm_pio()
def prog_irq():
    # Fires SM-relative IRQ 0 once, then loops on nop.
    irq(rel(0))
    wrap_target()
    nop()


# ---------------------------------------------------------------------------
# Assembler encoding tests  (no hardware state needed)
# ---------------------------------------------------------------------------


class TestPIOAssemblerEncoding(unittest.TestCase):
    def test_nop(self):
        # nop() is mov(y, y)
        self.assertEqual(rp2.asm_pio_encode("nop()", 0), 0xA042)

    def test_wait_gpio(self):
        self.assertEqual(rp2.asm_pio_encode("wait(1, gpio, 5)", 0), 0x2085)

    def test_wait_pin(self):
        self.assertEqual(rp2.asm_pio_encode("wait(1, pin, 5)", 0), 0x20A5)

    def test_wait_irq(self):
        self.assertEqual(rp2.asm_pio_encode("wait(0, irq, 3)", 0), 0x2043)

    def test_wait_jmp_pin(self):
        # RP2350: wait(polarity, jmp_pin, offset) uses src=3 in instruction bits [6:5].
        if not is_rp2350:
            return
        # 0x2000 | polarity<<7 | 3<<5 | offset
        self.assertEqual(rp2.asm_pio_encode("wait(1, jmp_pin, 2)", 0), 0x20E2)
        self.assertEqual(rp2.asm_pio_encode("wait(0, jmp_pin, 0)", 0), 0x2060)
        self.assertEqual(rp2.asm_pio_encode("wait(1, jmp_pin, 3)", 0), 0x20E3)

    def test_irq_plain(self):
        self.assertEqual(rp2.asm_pio_encode("irq(0)", 0), 0xC000)
        self.assertEqual(rp2.asm_pio_encode("irq(clear, 3)", 0), 0xC043)
        self.assertEqual(rp2.asm_pio_encode("irq(rel(0))", 0), 0xC010)

    def test_irq_cross_pio(self):
        # RP2350: next_pio(n) sets bits [4:3]=01 in the irq index field.
        if not is_rp2350:
            return
        # irq(next_pio(2)) -> index = 2 | 0x08 = 0x0A -> 0xC00A
        self.assertEqual(rp2.asm_pio_encode("irq(next_pio(2))", 0), 0xC00A)
        # irq(prev_pio(1)) -> index = 1 | 0x18 = 0x19 -> 0xC019
        self.assertEqual(rp2.asm_pio_encode("irq(prev_pio(1))", 0), 0xC019)
        # Combined with clear modifier
        self.assertEqual(rp2.asm_pio_encode("irq(clear, next_pio(0))", 0), 0xC048)
        self.assertEqual(rp2.asm_pio_encode("irq(clear, prev_pio(3))", 0), 0xC05B)

    def test_mov_pindirs(self):
        # RP2350: pindirs is a valid MOV destination (dest=4).
        # mov(pindirs, null) -> 0xA000 | 4<<5 | 3 = 0xA083
        self.assertEqual(rp2.asm_pio_encode("mov(pindirs, null)", 0), 0xA083)
        self.assertEqual(rp2.asm_pio_encode("mov(pindirs, invert(null))", 0), 0xA08B)

    def test_in_count_zero_default(self):
        @rp2.asm_pio()
        def p():
            nop()

        self.assertEqual(p[5] & 0x1F, 0)

    def test_in_count_nonzero(self):
        # RP2350: in_count encodes into shiftctrl bits [4:0]; other bits unaffected.
        if not is_rp2350:
            return

        @rp2.asm_pio(in_count=5)
        def p():
            nop()

        shiftctrl = p[5]
        self.assertEqual(shiftctrl & 0x1F, 5)
        # Must not bleed into fifo_join bits
        self.assertEqual((shiftctrl >> 14) & 0x3, 0)
        self.assertEqual((shiftctrl >> 30) & 0x3, 0)

    def test_in_count_max(self):
        # RP2350: Maximum useful in_count is 32 (5 bits); 32 & 0x1F == 0
        if not is_rp2350:
            return

        @rp2.asm_pio(in_count=31)
        def p():
            nop()

        self.assertEqual(p[5] & 0x1F, 31)

    def test_fifo_join_legacy(self):
        # JOIN_TX and JOIN_RX must still encode into shiftctrl bits [31:30].
        @rp2.asm_pio(fifo_join=rp2.PIO.JOIN_TX)
        def p_tx():
            nop()

        sc = p_tx[5]
        self.assertEqual((sc >> 30) & 0x3, 1)
        self.assertEqual((sc >> 14) & 0x3, 0)

        @rp2.asm_pio(fifo_join=rp2.PIO.JOIN_RX)
        def p_rx():
            nop()

        sc = p_rx[5]
        self.assertEqual((sc >> 30) & 0x3, 2)
        self.assertEqual((sc >> 14) & 0x3, 0)

    def test_fifo_join_rp2350_modes(self):
        # RP2350 TXGET(4)/TXPUT(8)/PUTGET(12) encode into shiftctrl bits [15:14].
        if not is_rp2350:
            return
        for join_val, expected_bits14 in ((4, 1), (8, 2), (12, 3)):

            @rp2.asm_pio(fifo_join=join_val)
            def p():
                nop()

            sc = p[5]
            self.assertEqual(
                (sc >> 14) & 0x3,
                expected_bits14,
                "fifo_join={} bits[15:14]".format(join_val),
            )
            self.assertEqual(
                (sc >> 30) & 0x3,
                0,
                "fifo_join={} bits[31:30] should be 0".format(join_val),
            )

    def test_in_count_with_fifo_join(self):
        # RP2350: Verify in_count and fifo_join can coexist in shiftctrl.
        if not is_rp2350:
            return

        @rp2.asm_pio(fifo_join=rp2.PIO.JOIN_TX, in_count=7)
        def p():
            nop()

        sc = p[5]
        self.assertEqual((sc >> 30) & 0x3, 1)  # JOIN_TX
        self.assertEqual(sc & 0x1F, 7)  # in_count


# ---------------------------------------------------------------------------
# StateMachine hardware tests
# ---------------------------------------------------------------------------


class TestStateMachine(unittest.TestCase):
    def setUp(self):
        self.pio = rp2.PIO(0)
        self.sm = rp2.StateMachine(0, prog_loopback)

    def tearDown(self):
        self.sm.active(False)
        self.sm.irq(None)
        self.pio.remove_program()

    def test_printing(self):
        self.assertEqual(str(self.sm), "StateMachine(0)")

    def test_pio_printing(self):
        self.assertEqual(str(self.pio), "PIO(0)")

    def test_active(self):
        sm = self.sm
        self.assertFalse(sm.active())
        sm.active(True)
        self.assertTrue(sm.active())
        sm.active(False)
        self.assertFalse(sm.active())

    def test_loopback_values(self):
        sm = self.sm
        sm.active(True)
        for val in (0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678):
            sm.put(val)
            self.assertEqual(sm.get(), val)

    def test_tx_rx_fifo_counts(self):
        sm = self.sm
        self.assertEqual(sm.tx_fifo(), 0)
        self.assertEqual(sm.rx_fifo(), 0)

        sm.active(True)
        sm.put(0xABCD)

        deadline = time.ticks_add(time.ticks_ms(), 100)
        while sm.rx_fifo() == 0 and time.ticks_diff(deadline, time.ticks_ms()) > 0:
            pass

        self.assertEqual(sm.rx_fifo(), 1)
        sm.get()
        self.assertEqual(sm.rx_fifo(), 0)

    def test_exec(self):
        sm = self.sm
        sm.active(True)
        # Inject instructions to push a known value via the RX FIFO.
        sm.exec("set(x, 21)")
        sm.exec("in_(x, 32)")
        sm.exec("push()")
        self.assertEqual(sm.get(), 21)

    def test_irq(self):
        sm = self.sm
        sm.init(prog_irq)

        irq_fired = [False]

        def handler(sm):
            irq_fired[0] = True

        sm.irq(handler)
        sm.active(True)

        deadline = time.ticks_add(time.ticks_ms(), 100)
        while not irq_fired[0] and time.ticks_diff(deadline, time.ticks_ms()) > 0:
            pass

        sm.active(False)
        self.assertTrue(irq_fired[0])

    def test_restart(self):
        sm = self.sm
        sm.active(True)
        sm.put(0x11)
        self.assertEqual(sm.get(), 0x11)
        sm.restart()
        sm.put(0x22)
        self.assertEqual(sm.get(), 0x22)

    def test_add_remove_program_explicit(self):
        # add_program/remove_program on a second PIO instance.
        pio1 = rp2.PIO(1)

        @rp2.asm_pio()
        def small():
            nop()

        pio1.add_program(small)
        pio1.remove_program(small)
        # Adding again after removal should succeed.
        pio1.add_program(small)
        pio1.remove_program(small)

    def test_program_memory_reuse(self):
        # Initialising two SMs with the same program should not duplicate
        # it in instruction memory.
        sm0 = rp2.StateMachine(0, prog_loopback)
        sm1 = rp2.StateMachine(1, prog_loopback)
        sm0.active(True)
        sm1.active(True)
        sm0.put(0xAA)
        sm1.put(0xBB)
        self.assertEqual(sm0.get(), 0xAA)
        self.assertEqual(sm1.get(), 0xBB)
        sm0.active(False)
        sm1.active(False)


# ---------------------------------------------------------------------------
# RP2350-specific hardware tests
# ---------------------------------------------------------------------------


class TestPIORp2350(unittest.TestCase):
    def test_pio_count(self):
        # RP2350 has 3 PIO blocks; RP2040 has 2.
        if is_rp2350:
            p = rp2.PIO(2)
            self.assertEqual(str(p), "PIO(2)")
        else:
            with self.assertRaises(ValueError):
                rp2.PIO(2)

    def test_sm_count(self):
        # RP2350 has SMs 0-11; RP2040 has SMs 0-7.
        if is_rp2350:
            sm = rp2.StateMachine(8)
            self.assertEqual(str(sm), "StateMachine(8)")
            rp2.PIO(2).remove_program()
        else:
            with self.assertRaises(ValueError):
                rp2.StateMachine(8)

    def test_pio_constants(self):
        self.assertEqual(rp2.PIO.JOIN_NONE, 0)
        self.assertEqual(rp2.PIO.JOIN_TX, 1)
        self.assertEqual(rp2.PIO.JOIN_RX, 2)
        self.assertEqual(rp2.PIO.IRQ_SM0, 0x100)
        self.assertEqual(rp2.PIO.IRQ_SM1, 0x200)
        self.assertEqual(rp2.PIO.IRQ_SM2, 0x400)
        self.assertEqual(rp2.PIO.IRQ_SM3, 0x800)

    def test_pio2_loopback(self):
        # On RP2350, verify that PIO(2) / SM(8..11) can run a program.
        if not is_rp2350:
            return
        sm = rp2.StateMachine(8, prog_loopback)
        sm.active(True)
        sm.put(0x5A5A5A5A)
        self.assertEqual(sm.get(), 0x5A5A5A5A)
        sm.active(False)
        rp2.PIO(2).remove_program()


if __name__ == "__main__":
    unittest.main()
