# Test machine.I2S data transfer rate, for both TX and RX.

try:
    from machine import Pin, I2S
except ImportError:
    print("SKIP")
    raise SystemExit

import time, sys

MAX_DELTA_MS = 4

# Configure pins based on the board.
# Tuples of (i2s_id, sck_pin, ws_pin, sd_tx_pin, sd_rx_pin)
# A board must have at least one instance to test, both TX and RX mode.
if "pyboard" in sys.platform:
    i2s_instances = ((2, Pin("Y6"), Pin("Y5"), Pin("Y8"), Pin("Y8")),)
elif "rp2" in sys.platform:
    i2s_instances = (
        (0, Pin(0), Pin(1), Pin(2), Pin(2)),
        (1, Pin(0), Pin(1), Pin(2), Pin(2)),
    )
elif "mimxrt" in sys.platform:
    i2s_instances = (
        (1, Pin("D26"), Pin("D27"), Pin("D7"), Pin("D8")),
        (2, Pin("D4"), Pin("D3"), Pin("D2"), None),
    )
elif "esp32" in sys.platform:
    try:
        i2s_instances = ((0, Pin(18), Pin(19), Pin(21), Pin(14)),)
    except ValueError:
        # fallback to lower pin number for ESP32-C3
        i2s_instances = ((0, Pin(6), Pin(7), Pin(10), Pin(11)),)
    # Allow for small additional RTOS overhead
    MAX_DELTA_MS = 8

TEST_BYTES = b"01234567"
RATE = 11025  # frames/sec


def test(i2s_id, sck_pin, ws_pin, sd_pin, mode, bits_per_sample, frame_format):
    if sd_pin is None:
        return

    i2s = I2S(
        i2s_id,
        sck=sck_pin,
        ws=ws_pin,
        sd=sd_pin,
        mode=mode,
        bits=bits_per_sample,
        format=frame_format,
        rate=RATE,
        ibuf=200,
    )

    if frame_format == I2S.MONO:
        channels = 1
    else:
        channels = 2
    bits_per_frame = bits_per_sample * channels
    buf_len_200ms = bits_per_frame // 8 * RATE // 5

    # Create test data and preload I2S buffers.
    if mode == I2S.TX:
        mode_str = "TX"
        data = TEST_BYTES * (buf_len_200ms // len(TEST_BYTES))
        i2s.write(data)
    else:
        mode_str = "RX"
        data = bytearray(len(TEST_BYTES) * (buf_len_200ms // len(TEST_BYTES)))
        i2s.readinto(data)

    # Time how long it takes to read/write 2 lots of data.
    t0 = time.ticks_ms()
    for i in range(2):
        if mode == I2S.TX:
            i2s.write(data)
        else:
            i2s.readinto(data)
    t1 = time.ticks_ms()
    dt = time.ticks_diff(t1, t0)

    i2s.deinit()

    # Time should be in range of 400ms.
    time_delta = abs(dt - 400)
    time_in_range = time_delta <= MAX_DELTA_MS

    # Print out test result if requested, or if time not in range.
    if print_results or not time_in_range:
        print(mode_str, bits_per_sample, channels, time_in_range or time_delta)


print_results = True

for i2s_id, sck_pin, ws_pin, sd_tx_pin, sd_rx_pin in i2s_instances:
    test(i2s_id, sck_pin, ws_pin, sd_tx_pin, I2S.TX, 16, I2S.MONO)
    test(i2s_id, sck_pin, ws_pin, sd_tx_pin, I2S.TX, 16, I2S.STEREO)
    test(i2s_id, sck_pin, ws_pin, sd_tx_pin, I2S.TX, 32, I2S.MONO)
    test(i2s_id, sck_pin, ws_pin, sd_tx_pin, I2S.TX, 32, I2S.STEREO)
    test(i2s_id, sck_pin, ws_pin, sd_rx_pin, I2S.RX, 16, I2S.MONO)
    test(i2s_id, sck_pin, ws_pin, sd_rx_pin, I2S.RX, 16, I2S.STEREO)
    test(i2s_id, sck_pin, ws_pin, sd_rx_pin, I2S.RX, 32, I2S.MONO)
    test(i2s_id, sck_pin, ws_pin, sd_rx_pin, I2S.RX, 32, I2S.STEREO)

    # For any remaining tests, don't print the results if they pass.
    # This is to have the same output on all boards regardless of
    # how many I2S instances they test.
    print_results = False
