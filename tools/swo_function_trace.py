"""This prints out Chrome Trace Formatted json that can be viewed in Perfetto or Spall.
https://ui.perfetto.dev/
https://gravitymoth.com/spall/spall.html

Format:
https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/preview#

Connect a USB to Serial converter to the SWO pin and then provide the serial
device to this script. It should be 1MBaud SWO signal. CTRL-C when you've captured enough data and
then it'll process and output.

pip install pysigrok-libsigrokdecode
python tools/swo_function_trace.py /dev/ttyACM0 build-metro_m7_1011/firmware.elf > trace.json
"""

import serial
import sys
import sigrokdecode
import time
import json

from elftools.elf.elffile import ELFFile

f = open(sys.argv[-1], "rb")
ef = ELFFile(f)

symtab = ef.get_section_by_name(".symtab")
symbols = {}
for s in symtab.iter_symbols():
    addr = s.entry["st_value"]
    symbols[addr] = s.name
f.close()

# sys.exit(0)

decoder = sigrokdecode.get_decoder("arm_itm")()

decoder.reset()
decoder.options = {"objdump": "", "elffile": ""}
decoder.start()

dwt_timestamp = 0
last_dwt_timestamp = 0
streak = 0
print("[")

stack = []


def emit(ts, addr, channel):
    s = None
    if addr in symbols:
        s = symbols[addr]
    else:
        s = hex(addr)
    if addr < 0x6000_0000:
        s = "R:" + s
    else:
        s = "F:" + s
    if channel[0] == "3":
        stack.append(addr)
    else:
        if not stack or stack[-1] != addr:
            return
        stack.pop()
    event = {
        "name": s,
        "ph": "B" if channel[0] == "3" else "E",
        "ts": ts,
        "pid": 0,
        "tid": 0,
    }
    print(json.dumps(event), ",")


def decoder_cb(ss, es, data):
    global streak
    global last_dwt_timestamp
    # print(ss, es, data)
    ptype = data[0]
    ts = (dwt_timestamp + (streak * 32)) / 500
    if ptype == 0:
        event = {"name": data[1][0], "ph": "i", "ts": ts, "pid": 0, "tid": 0, "s": "g"}
        print(json.dumps(event), ",")
        if data[1][0] == "Overflow":
            while stack:
                emit(ts, stack[-1], "4:")

    if ptype in (0, 1):
        return
    if ptype == 2 and (data[1][0].startswith("3:") or data[1][0].startswith("4:")):
        channel, addr = data[1][0].split()
        addr = int(addr[2:], 16)
        # if addr & 0x1 != 0:
        #     addr -= 1
        # print(dwt_timestamp + streak, channel, symbols[addr], hex(addr))
        emit(ts, addr, channel)
    else:
        # print(dwt_timestamp + streak, data)
        pass
    if dwt_timestamp == last_dwt_timestamp:
        streak += 1
    else:
        streak = 0

    if last_dwt_timestamp > dwt_timestamp:
        raise RuntimeError()
    last_dwt_timestamp = dwt_timestamp


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
