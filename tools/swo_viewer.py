"""This prints out all parsed ITM packets.

Connect a USB to Serial converter to the SWO pin and then provide the serial
device to this script. It should be 1MBaud SWO signal. CTRL-C when you've
captured enough data and then it'll process and output.

pip install pysigrok-libsigrokdecode
python tools/swo_viewer.py /dev/ttyACM0
"""

import serial
import sys
import sigrokdecode
import time
import json

decoder = sigrokdecode.get_decoder("arm_itm")()

decoder.reset()
decoder.options = {"objdump": "", "elffile": ""}
decoder.start()

dwt_timestamp = 0
last_dwt_timestamp = 0
streak = 0

stack = []


def decoder_cb(ss, es, data):
    global streak
    global last_dwt_timestamp
    print(dwt_timestamp, ss, es, data)


decoder.add_callback(sigrokdecode.OUTPUT_ANN, None, decoder_cb)

s = serial.Serial(sys.argv[-2], 1000000)

buffers = []
while True:
    try:
        start_ts = time.monotonic_ns()
        b = s.read(s.in_waiting)
        if b:
            end_ts = time.monotonic_ns()
            buffers.append((start_ts, end_ts, b))
            # print(len(b))
            # if len(buffers) > 10:
            #     break
    except KeyboardInterrupt:
        break

time_per_bit = 1_000_000_000 / 1000000

min_gap = 100000000
total_bytes = 0
for start_ts, end_ts, buf in buffers:
    # print(total_bytes, start_ts, end_ts, buf)
    ts_per_byte = (end_ts - start_ts) / len(buf)
    for i, b in enumerate(buf):
        # print(total_bytes, hex(b))
        total_bytes += 1
        decoder.decode(
            start_ts + ts_per_byte * i, start_ts + ts_per_byte * (i + 1), ("DATA", None, (b,))
        )
        dwt_timestamp = decoder.dwt_timestamp
