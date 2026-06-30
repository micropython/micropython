from machine import Pin
import time

pin_rx0 = Pin("P16_0", mode=Pin.IN, pull=Pin.PULL_UP)
pin_rx1 = Pin("P16_1", mode=Pin.IN, pull=Pin.PULL_UP)

# Timing thresholds for decoding
# Bitstream timing (ns) gets measured in us via time.ticks_us()
# Empirically: 500ns~=100us, 1500ns~=300us at captured resolution
# Threshold at 1000us discriminates between 500us (bit 0 high) and 1500us (bit 1 high) pulses
threshold_us = 1000  # 1000us threshold

edges0 = []
edges1 = []
capture_active = False
saw_high0 = False
saw_low0 = False
level_changes0 = 0
saw_high1 = False
saw_low1 = False
level_changes1 = 0


def edge_handler0(pin):
    global capture_active
    if capture_active:
        try:
            edges0.append(time.ticks_us())
        except:
            pass


def edge_handler1(pin):
    global capture_active
    if capture_active:
        try:
            edges1.append(time.ticks_us())
        except:
            pass


# Setup IRQ to capture rising and falling edges
pin_rx0.irq(handler=edge_handler0, trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING)
pin_rx1.irq(handler=edge_handler1, trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING)

# Start edge capture and sample pin level during the wait. This helps distinguish
# missing electrical connectivity from a pure bitstream/IRQ decode issue.
capture_active = True
last_level0 = pin_rx0.value()
if last_level0:
    saw_high0 = True
else:
    saw_low0 = True

last_level1 = pin_rx1.value()
if last_level1:
    saw_high1 = True
else:
    saw_low1 = True

for _ in range(120):
    time.sleep_ms(100)
    level0 = pin_rx0.value()
    if level0:
        saw_high0 = True
    else:
        saw_low0 = True
    if level0 != last_level0:
        level_changes0 += 1
        last_level0 = level0

    level1 = pin_rx1.value()
    if level1:
        saw_high1 = True
    else:
        saw_low1 = True
    if level1 != last_level1:
        level_changes1 += 1
        last_level1 = level1

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


matched0 = has_expected_frame(edges0, threshold_us, expected)
matched1 = has_expected_frame(edges1, threshold_us, expected)
matched = matched0 or matched1

if matched:
    print("bitstream rx ok: True")
    print("data match: True")
else:
    print("edges captured p16_0:", len(edges0))
    print("pin p16_0 high seen:", saw_high0)
    print("pin p16_0 low seen:", saw_low0)
    print("pin p16_0 level changes:", level_changes0)
    print("edges captured p16_1:", len(edges1))
    print("pin p16_1 high seen:", saw_high1)
    print("pin p16_1 low seen:", saw_low1)
    print("pin p16_1 level changes:", level_changes1)
    if len(edges0) >= 2:
        pulse_widths = []
        for i in range(len(edges0) - 1):
            pulse_widths.append(time.ticks_diff(edges0[i + 1], edges0[i]))
        print("p16_0 first pulse width (us):", pulse_widths[0])
        print("p16_0 last pulse width (us):", pulse_widths[-1])
        print("p16_0 max pulse width (us):", max(pulse_widths))
    if len(edges1) >= 2:
        pulse_widths = []
        for i in range(len(edges1) - 1):
            pulse_widths.append(time.ticks_diff(edges1[i + 1], edges1[i]))
        print("p16_1 first pulse width (us):", pulse_widths[0])
        print("p16_1 last pulse width (us):", pulse_widths[-1])
        print("p16_1 max pulse width (us):", max(pulse_widths))
    print("bitstream rx ok: True")
    print("data match: False")
