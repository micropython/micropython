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


# ---------------------------------------------------------------------------
# RP2350 PIO v1 C-layer feature tests
# ---------------------------------------------------------------------------


class TestPIOV1Constants(unittest.TestCase):
    def test_pio_version(self):
        # PIO.version() returns the hardware version: 0 on RP2040, 1 on RP2350.
        expected = 1 if is_rp2350 else 0
        self.assertEqual(rp2.PIO(0).version(), expected)
        self.assertEqual(rp2.PIO(1).version(), expected)

    def test_join_rp2350_constants(self):
        # JOIN_RX_GET and JOIN_RX_PUT are RP2350-only FIFO mode constants.
        if not is_rp2350:
            return
        self.assertEqual(rp2.PIO.JOIN_RX_GET, 4)
        self.assertEqual(rp2.PIO.JOIN_RX_PUT, 8)

    def test_dreq_constants_pio0_pio1(self):
        # DREQ constants for PIO0 and PIO1 are contiguous starting at 0 and 8.
        self.assertEqual(rp2.DREQ_PIO0_TX0, 0)
        self.assertEqual(rp2.DREQ_PIO0_TX1, 1)
        self.assertEqual(rp2.DREQ_PIO0_TX2, 2)
        self.assertEqual(rp2.DREQ_PIO0_TX3, 3)
        self.assertEqual(rp2.DREQ_PIO0_RX0, 4)
        self.assertEqual(rp2.DREQ_PIO0_RX1, 5)
        self.assertEqual(rp2.DREQ_PIO0_RX2, 6)
        self.assertEqual(rp2.DREQ_PIO0_RX3, 7)
        self.assertEqual(rp2.DREQ_PIO1_TX0, 8)
        self.assertEqual(rp2.DREQ_PIO1_TX1, 9)
        self.assertEqual(rp2.DREQ_PIO1_TX2, 10)
        self.assertEqual(rp2.DREQ_PIO1_TX3, 11)
        self.assertEqual(rp2.DREQ_PIO1_RX0, 12)
        self.assertEqual(rp2.DREQ_PIO1_RX1, 13)
        self.assertEqual(rp2.DREQ_PIO1_RX2, 14)
        self.assertEqual(rp2.DREQ_PIO1_RX3, 15)

    def test_dreq_constants_pio2(self):
        # PIO2 DREQ constants exist only on RP2350, starting at 16.
        if not is_rp2350:
            with self.assertRaises(AttributeError):
                _ = rp2.DREQ_PIO2_TX0
            return
        self.assertEqual(rp2.DREQ_PIO2_TX0, 16)
        self.assertEqual(rp2.DREQ_PIO2_TX1, 17)
        self.assertEqual(rp2.DREQ_PIO2_TX2, 18)
        self.assertEqual(rp2.DREQ_PIO2_TX3, 19)
        self.assertEqual(rp2.DREQ_PIO2_RX0, 20)
        self.assertEqual(rp2.DREQ_PIO2_RX1, 21)
        self.assertEqual(rp2.DREQ_PIO2_RX2, 22)
        self.assertEqual(rp2.DREQ_PIO2_RX3, 23)


# FJOIN_RX_PUT (bit 15): RX FIFO replaced by 4 random-access slots.
# CPU has read access via putget(); SM has write access via put instruction.
@rp2.asm_pio(fifo_join=8)
def prog_fjoin_rx_put():
    nop()


# FJOIN_RX_GET (bit 14): RX FIFO replaced by 4 random-access slots.
# CPU has write access via putget(); SM has read access via get instruction.
@rp2.asm_pio(fifo_join=4)
def prog_fjoin_rx_get():
    nop()


class TestPIOV1Hardware(unittest.TestCase):
    def test_putget_absent_on_rp2040(self):
        # putget() must not exist on RP2040.
        if is_rp2350:
            return
        sm = rp2.StateMachine(0, prog_loopback)
        with self.assertRaises(AttributeError):
            sm.putget(0)
        rp2.PIO(0).remove_program()

    def test_putget_read_returns_int(self):
        # In FJOIN_RX_PUT mode the CPU has read access to the 4 PUTGET slots.
        # Verify each slot returns an integer (contents may be non-zero if
        # the SM was used previously in this session).
        if not is_rp2350:
            return
        sm = rp2.StateMachine(0, prog_fjoin_rx_put)
        for i in range(4):
            val = sm.putget(i)
            self.assertIsInstance(val, int)
        sm.active(False)
        rp2.PIO(0).remove_program()

    def test_putget_write_no_exception(self):
        # In FJOIN_RX_GET mode the CPU has write access to the 4 PUTGET slots.
        # Verify writes succeed without raising.
        if not is_rp2350:
            return
        sm = rp2.StateMachine(0, prog_fjoin_rx_get)
        for i in range(4):
            sm.putget(i, 0xA0000000 | i)
        sm.active(False)
        rp2.PIO(0).remove_program()

    def test_putget_index_bounds(self):
        # putget() rejects indices outside 0-3.
        if not is_rp2350:
            return
        sm = rp2.StateMachine(0, prog_fjoin_rx_put)
        with self.assertRaises(ValueError):
            sm.putget(-1)
        with self.assertRaises(ValueError):
            sm.putget(4)
        sm.active(False)
        rp2.PIO(0).remove_program()

    def test_sm_mask_enable(self):
        # PIO.sm_mask_enable() atomically enables SMs via CTRL register.
        # Enabling sm_mask=0 is a no-op that should not raise.
        if not is_rp2350:
            pio = rp2.PIO(0)
            with self.assertRaises(AttributeError):
                pio.sm_mask_enable(0)
            return
        pio = rp2.PIO(0)
        sm = rp2.StateMachine(0, prog_loopback)
        # Enable SM0 via mask, then verify it's running.
        pio.sm_mask_enable(0x1)
        sm.put(0xCAFE)
        self.assertEqual(sm.get(), 0xCAFE)
        sm.active(False)
        rp2.PIO(0).remove_program()


if __name__ == "__main__":
    unittest.main()
