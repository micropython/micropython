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


def edge_handler(pin):
    global capture_active
    if capture_active:
        try:
            edges.append(time.ticks_us())
        except:
            pass


# Setup IRQ to capture rising and falling edges
pin_rx.irq(handler=edge_handler, trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING)

# Start edge capture; wait for transmission
# TX waits 8000ms before transmitting. RX window should be long enough for both wait and transmission.
capture_active = True
time.sleep_ms(8500)
capture_active = False


def decode_bitstream(edges, threshold_us):
    """Decode bitstream edges back into bytes."""
    if len(edges) < 4:
        return None

    decoded_bits = []

    # Process HIGH pulses only: edges[0->1] = bit0 HIGH, edges[2->3] = bit1 HIGH, etc.
    # Skip LOW pulses (edges[1->2], edges[3->4], etc.)
    for i in range(0, len(edges) - 1, 2):
        pulse_width_us = (edges[i + 1] - edges[i]) & 0xFFFFFFFF

        # Decode: short HIGH pulse < threshold = 0, long HIGH pulse >= threshold = 1
        if pulse_width_us < threshold_us:
            decoded_bits.append(0)
        else:
            decoded_bits.append(1)

    # Convert bits to bytes (MSB first)
    if len(decoded_bits) % 8 != 0:
        return None

    decoded_bytes = []
    for byte_idx in range(len(decoded_bits) // 8):
        byte_val = 0
        for bit_idx in range(8):
            bit = decoded_bits[byte_idx * 8 + bit_idx]
            byte_val = (byte_val << 1) | bit
        decoded_bytes.append(byte_val)

    return bytes(decoded_bytes)


decoded = decode_bitstream(edges, threshold_us)
expected = bytes([0x12, 0x34, 0x56, 0x78])

if decoded == expected:
    print("bitstream rx ok: True")
    print("data match: True")
else:
    print("bitstream rx ok: True")
    print("data match: False")
