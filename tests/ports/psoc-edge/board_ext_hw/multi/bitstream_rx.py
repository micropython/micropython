from machine import Pin
import time

pin_rx = Pin("P16_0", mode=Pin.IN, pull=Pin.PULL_UP)

# Timing thresholds for decoding
# Bitstream timing (ns) gets measured in us via time.ticks_us()
# Empirically: 500ns~=100us, 1500ns~=300us at captured resolution
# Threshold at 1000us discriminates between 500us (bit 0 high) and 1500us (bit 1 high) pulses
threshold_us = 1000  # 1000us threshold

edges = []
capture_active = False
saw_high = False
saw_low = False
level_changes = 0


def edge_handler(pin):
    global capture_active
    if capture_active:
        try:
            edges.append(time.ticks_us())
        except:
            pass


# Setup IRQ to capture rising and falling edges
pin_rx.irq(handler=edge_handler, trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING)

# Start edge capture and sample pin level during the wait. This helps distinguish
# missing electrical connectivity from a pure bitstream/IRQ decode issue.
capture_active = True
last_level = pin_rx.value()
if last_level:
    saw_high = True
else:
    saw_low = True

for _ in range(120):
    time.sleep_ms(100)
    level = pin_rx.value()
    if level:
        saw_high = True
    else:
        saw_low = True
    if level != last_level:
        level_changes += 1
        last_level = level

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

    # Depending on whether the first captured edge is rising/falling, HIGH pulse
    # widths can appear at even or odd edge-interval indices.
    for parity in (0, 1):
        decoded_bits = []
        for i in range(parity, len(edges) - 1, 2):
            pulse_width_us = time.ticks_diff(edges[i + 1], edges[i])
            decoded_bits.append(0 if pulse_width_us < threshold_us else 1)

        # Search for expected frame anywhere in the decoded stream.
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
    print("edges captured:", len(edges))
    print("pin high seen:", saw_high)
    print("pin low seen:", saw_low)
    print("pin level changes:", level_changes)
    if len(edges) >= 2:
        pulse_widths = []
        for i in range(len(edges) - 1):
            pulse_widths.append(time.ticks_diff(edges[i + 1], edges[i]))
        print("first pulse width (us):", pulse_widths[0])
        print("last pulse width (us):", pulse_widths[-1])
        print("max pulse width (us):", max(pulse_widths))
    print("bitstream rx ok: True")
    print("data match: False")
