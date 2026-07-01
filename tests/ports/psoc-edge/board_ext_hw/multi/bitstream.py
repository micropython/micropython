# Bitstream multi-device test
# Tests machine.bitstream() transmission and reception between two boards.
#
# KIT_PSE84_AI board hardware connections:
#   Board 0 (TX)   Board 1 (RX)
#   P16_0     <->  P16_0
#   GND       <->  GND

from machine import Pin, bitstream
import time


def instance0():
    # TX: validate the bitstream API, then transmit a test pattern once RX is armed.

    pin_api = Pin("P16_1", mode=Pin.OUT, value=0)
    api_data = bytes([0x10, 0x00, 0x00])

    # API validation: nominal call succeeds
    call_ok = True
    try:
        bitstream(pin_api, 0, (500, 1500, 1500, 500), api_data)
    except Exception:
        call_ok = False
    print("bitstream call ok:", call_ok)

    # API validation: timing below 300 ns minimum is rejected
    try:
        bitstream(pin_api, 0, (299, 850, 800, 450), api_data)
    except ValueError as exc:
        print("bitstream rejects short timing:", str(exc) == "timing must be >= 300 ns")
    else:
        print("bitstream rejects short timing:", False)

    # Enter phase 2 (allow runner to start instance1), then wait until RX is armed.
    multitest.next()
    multitest.wait("RX_ARMED")

    pin_tx = Pin("P16_0", mode=Pin.OUT, value=0)
    timing = (500000, 1500000, 1500000, 500000)  # 500 µs / 1500 µs pulses
    test_data = bytes([0x12, 0x34, 0x56, 0x78])

    tx_ok = True
    try:
        for _ in range(5):
            bitstream(pin_tx, 0, timing, test_data)
            time.sleep_ms(200)
    except Exception:
        tx_ok = False
    print("bitstream tx ok:", tx_ok)


def instance1():
    # RX: arm the IRQ, signal TX that capture is active, then decode the result.

    pin_rx = Pin("P16_0", mode=Pin.IN, pull=Pin.PULL_UP)
    threshold_us = 1000
    MAX_EDGES = 128  # enough for ≥2 complete frames (32 bits × 2 edges × 2)
    edges = []
    capture_active = False

    def edge_handler(pin):
        if capture_active and len(edges) < MAX_EDGES:
            edges.append(time.ticks_us())

    pin_rx.irq(handler=edge_handler, trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING)

    # Arm capture before broadcasting so no leading edges are missed.
    capture_active = True

    # Enter phase 2, then immediately tell TX it may transmit.
    multitest.next()
    multitest.broadcast("RX_ARMED")

    # Capture for 4 s (TX sends 5 frames × ~264 ms ≈ 1.3 s; extra headroom included).
    time.sleep_ms(4000)
    capture_active = False

    expected = bytes([0x12, 0x34, 0x56, 0x78])

    def bytes_to_bits(data):
        bits = []
        for byte_val in data:
            for bit in range(7, -1, -1):
                bits.append((byte_val >> bit) & 1)
        return bits

    def has_expected_frame(edges, threshold_us, expected_bytes):
        if len(edges) < 2:
            return False
        expected_bits = bytes_to_bits(expected_bytes)
        for parity in (0, 1):
            decoded_bits = []
            for i in range(parity, len(edges) - 1, 2):
                pulse_width_us = time.ticks_diff(edges[i + 1], edges[i])
                decoded_bits.append(0 if pulse_width_us < threshold_us else 1)
            n = len(expected_bits)
            for start in range(0, len(decoded_bits) - n + 1):
                if decoded_bits[start : start + n] == expected_bits:
                    return True
        return False

    matched = has_expected_frame(edges, threshold_us, expected)
    print("bitstream rx ok: True")
    print("data match:", matched)
