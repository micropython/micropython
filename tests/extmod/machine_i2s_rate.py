# Test machine.I2S data transfer rate, for both TX and RX.

try:
    from machine import Pin, I2S
except ImportError:
    print("SKIP")
    raise SystemExit

import time, sys

# Configure pins based on the board.
if "pyboard" in sys.platform:
    i2s_id = 2
    sck_pin = Pin("Y6")
    ws_pin = Pin("Y5")
    sd_tx_pin = sd_rx_pin = Pin("Y8")
elif "rp2" in sys.platform:
    i2s_id = 1
    sck_pin = Pin(0)
    ws_pin = Pin(1)
    sd_tx_pin = sd_rx_pin = Pin(2)
elif "mimxrt" in sys.platform:
    i2s_id = 1
    sck_pin = Pin(26)
    ws_pin = Pin(27)
    sd_tx_pin = Pin(7)
    sd_rx_pin = Pin(8)

TEST_BYTES = b"01234567"
RATE = 11025  # frames/sec


def test(mode, sd_pin, bits_per_sample, frame_format):
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
    buf_len_250ms = bits_per_frame // 8 * RATE // 4

    # Create test data and preload I2S buffers.
    if mode == I2S.TX:
        mode_str = "TX"
        data = TEST_BYTES * (buf_len_250ms // len(TEST_BYTES))
        i2s.write(data)
    else:
        mode_str = "RX"
        data = bytearray(len(TEST_BYTES) * (buf_len_250ms // len(TEST_BYTES)))
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

    # Print out test result, time should be in range of 500ms.
    print(mode_str, bits_per_sample, channels, abs(dt - 500) <= 4)


test(I2S.TX, sd_tx_pin, 16, I2S.MONO)
test(I2S.TX, sd_tx_pin, 16, I2S.STEREO)
test(I2S.TX, sd_tx_pin, 32, I2S.MONO)
test(I2S.TX, sd_tx_pin, 32, I2S.STEREO)
test(I2S.RX, sd_rx_pin, 16, I2S.MONO)
test(I2S.RX, sd_rx_pin, 16, I2S.STEREO)
test(I2S.RX, sd_rx_pin, 32, I2S.MONO)
test(I2S.RX, sd_rx_pin, 32, I2S.STEREO)
