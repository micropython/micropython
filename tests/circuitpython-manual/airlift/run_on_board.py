# SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT
"""Run a local .py file on the board through the raw REPL and stream its output.
Usage: run_on_board.py <script> [timeout_s]. Set AIRLIFT_TEST_SERIAL to pick the port;
the default is the first Fruit Jam in /dev/serial/by-id. Needs pyserial."""

import glob
import os
import sys
import time

import serial

PORT = (
    os.getenv("AIRLIFT_TEST_SERIAL")
    or (sorted(glob.glob("/dev/serial/by-id/*Fruit_Jam*")) + [""])[0]
)


def main():
    src = open(sys.argv[1], "rb").read()
    timeout = float(sys.argv[2]) if len(sys.argv) > 2 else 120
    s = serial.Serial(PORT, 115200, timeout=0.1)
    for _ in range(3):
        s.write(b"\r\x03")
        time.sleep(0.3)
    s.reset_input_buffer()
    s.write(b"\r\x01")
    buf = b""
    t = time.monotonic()
    while b"raw REPL; CTRL-B to exit\r\n>" not in buf:
        buf += s.read(256)
        if time.monotonic() - t > 5:
            sys.exit("no raw REPL: %r" % buf[-200:])
    # Soft reboot so objects left by an interrupted run (pins!) are released.
    s.write(b"\x04")
    buf = b""
    t = time.monotonic()
    while not (b"soft reboot" in buf and buf.endswith(b"raw REPL; CTRL-B to exit\r\n>")):
        buf += s.read(256)
        if time.monotonic() - t > 10:
            sys.exit("no soft reboot: %r" % buf[-200:])
    for i in range(0, len(src), 256):
        s.write(src[i : i + 256])
        time.sleep(0.01)
    s.write(b"\x04")
    ok = s.read(2)
    if ok != b"OK":
        sys.exit("bad ack %r" % (ok + s.read(200)))
    out = b""
    t = time.monotonic()
    while time.monotonic() - t < timeout:
        chunk = s.read(256)
        if chunk:
            out += chunk
            sys.stdout.write(chunk.decode("utf-8", "replace").replace("\x04", "\n[end]\n"))
            sys.stdout.flush()
            if out.count(b"\x04") >= 2:
                break
    else:
        s.write(b"\x03")
        print("\n[timeout]")
    s.write(b"\x02")
    s.close()


main()
