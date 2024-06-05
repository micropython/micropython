import os
from machine import I2S, Pin
import binascii
import time
import array
import struct

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    sck_tx_pin = "P13_1"
    ws_tx_pin = "P13_2"
    sd_tx_pin = "P13_3"
    wait_signal_pin_name = "P0_4"
elif "CY8CPROTO-063-BLE" in machine:
    # This would be the right pins for this test, but unfortunately
    # the P5_1 is allocated for the UART serial comm terminal communication.
    # So this tests is not currently possible for this board.
    # sck_tx_pin = "P5_1"
    # ws_tx_pin = "P5_2"
    # sd_tx_pin = "P5_3"
    print("SKIP")
    raise SystemExit

signal_received = False
rising_edge_count = 0
wait_signal_pin = None


def signal_irq(arg):
    global signal_received
    global rising_edge_count
    rising_edge_count += 1
    signal_received = True


def wait_signal_init():
    global wait_signal_pin
    wait_signal_pin = Pin(wait_signal_pin_name, Pin.IN)
    wait_signal_pin.irq(handler=signal_irq, trigger=Pin.IRQ_RISING)


def wait_signal_deinit():
    global wait_signal_pin
    wait_signal_pin.deinit()


def is_rx_done():
    # The second interrupt notifies rx completion
    global signal_received

    if signal_received:
        # Clear flag
        signal_received = False
        return True
    else:
        return False


def make_i2s_tx_frame_detect_pattern():
    max_value = 0x100
    reps = 10
    buf = bytearray([0] * (max_value) * reps)
    for i in range(reps):
        for j in range(max_value):
            buf[i * max_value + j] = j

    # for i in range(reps):
    #     print(binascii.hexlify(buf[i * max_value : (i + 1) * max_value]))

    return buf


def print_i2s_format(raw_buf, bits):
    samples_buf = []
    if bits == 16:
        bytes_per_sample = 2
        format = "<H"
    elif bits == 32:
        bytes_per_sample = 4
        format = "<I"

    num_bytes = len(raw_buf)
    i = 0
    while num_bytes > 0:
        sample = struct.unpack(format, raw_buf[i * bytes_per_sample : (i + 1) * bytes_per_sample])[
            0
        ]
        if i % 2 == 0:
            sample_l = sample
        else:
            sample_r = sample
            # sample_l is available from previous
            sample_tuple = (hex(sample_l), hex(sample_r))
            samples_buf.append(sample_tuple)

        num_bytes -= bytes_per_sample
        i += 1

    print("(Left channel, Right channel)")
    for s_tuple in samples_buf:
        print(s_tuple)


###############################################################################
test_rates = [8000, 16000, 32000, 48000, 22050, 44100]
test_formats = [I2S.MONO, I2S.STEREO]
test_bits = [16, 32]
test_bits_resolution = [8, 12, 16, 20, 32]


buf = make_i2s_tx_frame_detect_pattern()
wait_signal_init()

for _format in test_formats:
    for _bits in test_bits:
        for _rate in test_rates:
            audio_out = I2S(
                0,
                sck=sck_tx_pin,
                ws=ws_tx_pin,
                sd=sd_tx_pin,
                mode=I2S.TX,
                bits=_bits,
                format=_format,
                rate=_rate,
                ibuf=20000,
            )

            rx_done = False

            while not rx_done:
                audio_out.write(buf)
                time.sleep(1)
                rx_done = is_rx_done()

            print(f"irq : {rising_edge_count}")
            audio_out.deinit()

wait_signal_deinit()
