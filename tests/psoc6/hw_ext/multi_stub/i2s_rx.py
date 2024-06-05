import os
from machine import I2S, Pin
import binascii
import time
import array
import struct

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    sck_rx_pin = "P5_4"
    ws_rx_pin = "P5_5"
    sd_rx_pin = "P5_6"
    rx_ready_signal_pin_name = "P13_4"
elif "CY8CPROTO-063-BLE" in machine:
    # These would be the right pins for this test, but unfortunately
    # the P5_1 is allocated for the UART serial comm terminal communication.
    # Thus this tests is not currently possible for this board.
    # sck_rx_pin = "P5_4"
    # ws_rx_pin = "P5_5"
    # sd_rx_pin = "P5_6"
    print("SKIP")
    raise SystemExit

rx_ready_signal_pin = None


def rx_ready_signal_init():
    global rx_ready_signal_pin
    rx_ready_signal_pin = Pin(rx_ready_signal_pin_name, Pin.OUT, value=0)
    rx_ready_signal_pin.low()


def rx_ready_signal_deinit():
    global rx_ready_signal_pin
    rx_ready_signal_pin.deinit()


def notify_readiness_to_tx():
    global rx_ready_signal_pin
    rx_ready_signal_pin.high()
    time.sleep(0.5)
    rx_ready_signal_pin.low()


def make_expected_sequence():
    len = 0xFF
    expected_frame = bytearray(len)
    for i in range(len):
        expected_frame[i] = i

    return expected_frame


def find_sublist_in_list(full_list, sublist):
    for i in range(len(full_list) - len(sublist) - 1):
        if full_list[i : i + len(sublist)] == sublist:
            return True

    return False


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
print("1. tx-rx data for all formats, rates and bit resolution")

test_rates = [8000, 16000, 32000, 48000, 22050, 44100]
test_formats = [I2S.MONO, I2S.STEREO]
test_bits = [16, 32]
test_bits_resolution = [8, 12, 16, 20, 32]

rx_ready_signal_init()
exp_seq = make_expected_sequence()

for _format in test_formats:
    for _bits in test_bits:
        for _rate in test_rates:
            audio_in = I2S(
                0,
                sck=sck_rx_pin,
                ws=ws_rx_pin,
                sd=sd_rx_pin,
                mode=I2S.RX,
                bits=_bits,
                format=_format,
                rate=_rate,
                ibuf=20000,
            )

            # Try 3 times to overcome synch mismatches
            for i in range(3):
                # Read i2s data
                rx_buf = bytearray([0] * 2560)
                num_read = audio_in.readinto(rx_buf)

                # validate data
                is_seq_received = find_sublist_in_list(rx_buf, exp_seq)
                if is_seq_received:
                    break

            audio_in.deinit()
            print(
                f"data received for format = {_format}, bits = {_bits}, rate = {_rate} : {is_seq_received}"
            )

            notify_readiness_to_tx()
            # print_i2s_format(rx_buf, 16)

            # Give some time to the i2s_tx to setup
            time.sleep(2)

rx_ready_signal_deinit()

###############################################################################
print("\n2. irq non-blocking read implementation ")

rx_done = False


def rx_complete_irq(obj):
    global rx_done
    rx_done = True


audio_in = I2S(
    0,
    sck=sck_rx_pin,
    ws=ws_rx_pin,
    sd=sd_rx_pin,
    mode=I2S.RX,
    bits=16,
    format=I2S.MONO,
    rate=8000,
    ibuf=20000,
)

rx_buf = bytearray([0] * 50)
audio_in.irq(rx_complete_irq)
num_read = audio_in.readinto(rx_buf)

while not rx_done:
    pass

# if we get pass this rx_done flag has been
# modified by the interrupt

print("rx blocking done")
audio_in.deinit()


###############################################################################
print("\n3. shift ")

buf = bytearray(b"\xFF\xFF\xFF\xFF")
I2S.shift(buf=buf, bits=16, shift=3)
print(binascii.hexlify(buf))
I2S.shift(buf=buf, bits=16, shift=-3)
print(binascii.hexlify(buf))
