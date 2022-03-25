# Test machine.I2S on PYB/PYBD
#
# Writes some bytes over I2S and checks they are received by the other device.
# Requires wiring up WS, SCK, SD pins between two devices.

import os
from machine import Pin, I2S

# Configure pins based on the board.
if "F405" in os.uname().machine:
    i2s_id = 2
    ws_pin = Pin("Y5")
    sck_pin = Pin("Y6")
    sd_pin = Pin("Y8")
else:
    i2s_id = 1
    ws_pin = Pin("X5")
    sck_pin = Pin("X6")
    sd_pin = Pin("X8")

TEST_BYTES = b"0123456789ABCDEF"
RATE = 8000  # frames/sec

def buf_len(bits_per_sample, frame_format):
    if frame_format == I2S.MONO:
        channels = 1
    else:
        channels = 2
    bits_per_frame = bits_per_sample * channels
    buf_len_100ms = bits_per_frame // 8 * RATE // 10
    assert buf_len_100ms % len(TEST_BYTES) == 0
    return buf_len_100ms

def test_tx(mode, bits_per_sample, frame_format, num_tx):
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

    data = TEST_BYTES * (buf_len(bits_per_sample, frame_format) // len(TEST_BYTES))
    print("tx start")
    for i in range(num_tx):
        n = i2s.write(data)
        print("write", n)
    print("tx end")
    i2s.deinit()


def test_rx(mode, bits_per_sample, frame_format, num_rx, num_rx_extra, expected):
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

    buf = bytearray(buf_len(bits_per_sample, frame_format))
    wanted_bytes = expected * 8

    print("rx start")

    for _ in range(num_rx):
        n = i2s.readinto(buf)
        if wanted_bytes in buf:
            print("read", n, True)
        else:
            print("read", n, buf[:16], buf[-16:])

    # Extra reads for the case we are driving the clock, so the passive side finishes.
    for _ in range(num_rx_extra):
        i2s.readinto(buf)

    print("rx end")

    i2s.deinit()


def instance0():
    multitest.next()
    for bits_per_sample, frame_format, expected in (
            # TX and RX transfer full 32-bit stereo
            (32, I2S.STEREO, b"0123456789ABCDEF0123456789ABCDEF"),
            # TX writes different formats, RX receives 32-bit stereo
            (32, I2S.STEREO, b"012301234567456789AB89ABCDEFCDEF"),
            (32, I2S.STEREO, b"23016745AB89EFCD23016745AB89EFCD"),
            (32, I2S.STEREO, b"01012323454567678989ABABCDCDEFEF"),  # 16-bit mono (L=R)
            # TX writes 32-bit stereo, RX receives different formats
            (32, I2S.MONO, b"012389AB012389AB012389AB012389AB"),  # 32-bit, L channel only
            (16, I2S.STEREO, b"014589CD014589CD014589CD014589CD"),
            (16, I2S.MONO, b"23AB23AB23AB23AB23AB23AB23AB23AB"),
        ):
        test_rx(I2S.RX_PASSIVE, bits_per_sample, frame_format, 4, 0, expected)
        multitest.wait("tx done")
        test_rx(I2S.RX, bits_per_sample, frame_format, 4, 1, expected)
        multitest.broadcast("rx done")


def instance1():
    multitest.next()
    for bits_per_sample, frame_format, num_tx in (
            # TX and RX transfer full 32-bit stereo
            (32, I2S.STEREO, 4),
            # TX writes different formats, RX receives 32-bit stereo
            (32, I2S.MONO, 4),
            (16, I2S.STEREO, 8),
            (16, I2S.MONO, 8),
            # TX writes 32-bit stereo, RX receives different formats
            (32, I2S.STEREO, 4),
            (32, I2S.STEREO, 4),
            (32, I2S.STEREO, 4),
        ):
        test_tx(I2S.TX, bits_per_sample, frame_format, num_tx + 1)
        multitest.broadcast("tx done")
        test_tx(I2S.TX_PASSIVE, bits_per_sample, frame_format, num_tx)
        multitest.wait("rx done")
