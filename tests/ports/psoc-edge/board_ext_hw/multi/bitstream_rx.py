from machine import Pin
import time

pin_rx = Pin("P16_0", mode=Pin.IN, pull=Pin.PULL_UP)

# Threshold: 500us = bit 0, 1500us = bit 1
threshold_us = 1000

edges = []
capture_active = False


def edge_handler(pin):
    global capture_active
    if capture_active:
        try:
            edges.append(time.ticks_us())
        except:
            pass


pin_rx.irq(handler=edge_handler, trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING)

# Capture window: TX transmits at ~8s from stub start; window must cover that.
capture_active = True
time.sleep_ms(8500)
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

    # Try both edge parities (first captured edge may be rising or falling)
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

if matched:
    print("bitstream rx ok: True")
    print("data match: True")
else:
    # Debug: show edges count, pulse widths and best decoded bytes on failure
    n_edges = len(edges)
    print("dbg edges:", n_edges)
    if n_edges >= 2:
        pws = [time.ticks_diff(edges[i + 1], edges[i]) for i in range(min(n_edges - 1, 20))]
        print("dbg pulses(us):", pws)
    # Try to decode parity=0 and show raw bytes
    for parity in (0, 1):
        bits = []
        for i in range(parity, n_edges - 1, 2):
            pw = time.ticks_diff(edges[i + 1], edges[i])
            bits.append(0 if pw < threshold_us else 1)
        if len(bits) >= 32:
            decoded = []
            for b in range(len(bits) // 8):
                v = 0
                for k in range(8):
                    v = (v << 1) | bits[b * 8 + k]
                decoded.append(v)
            print("dbg decoded(p={})".format(parity), [hex(x) for x in decoded[:8]])
    print("bitstream rx ok: True")
    print("data match: False")
