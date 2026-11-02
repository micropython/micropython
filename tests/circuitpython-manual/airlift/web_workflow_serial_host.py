# SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT
"""Host test: the web workflow serial websocket over the ESP32 co-processor.
Usage: web_workflow_serial_host.py <board ip> <path to CIRCUITPY/settings.toml>
Opens /cp/serial/, interrupts to the REPL, evaluates an expression, prints a
long line from the VM, then soft-reboots through the websocket and checks the
same connection still carries the REPL afterwards. code.py may be anything,
including a program that never ends."""

import base64
import os
import socket
import sys
import time

ip, settings = sys.argv[1], sys.argv[2]
password = [
    l.split("=", 1)[1].strip().strip('"')
    for l in open(settings)
    if l.strip().startswith("CIRCUITPY_WEB_API_PASSWORD")
][0]


def connect():
    s = socket.create_connection((ip, 80), timeout=10)
    key = base64.b64encode(os.urandom(16)).decode()
    auth = base64.b64encode((":" + password).encode()).decode()
    s.sendall(
        (
            "GET /cp/serial/ HTTP/1.1\r\nHost: %s\r\nUpgrade: websocket\r\n"
            "Connection: Upgrade\r\nSec-WebSocket-Key: %s\r\nSec-WebSocket-Version: 13\r\n"
            "Authorization: Basic %s\r\n\r\n" % (ip, key, auth)
        ).encode()
    )
    head = b""
    while b"\r\n\r\n" not in head:
        head += s.recv(1)
    if b" 101 " not in head.split(b"\r\n")[0]:
        sys.exit("upgrade failed: %r" % head)
    return s


def send(s, text):
    data = text.encode()
    mask = os.urandom(4)
    s.sendall(
        bytes([0x81, 0x80 | len(data)]) + mask + bytes(b ^ mask[i % 4] for i, b in enumerate(data))
    )


buf = b""
# Text received after the last marker found, for the next read_until().
pending = ""


def read_until(s, marker, timeout):
    """Collects text frames until marker (a string or a tuple of strings) appears.
    Returns the text up to and including the first one found."""
    global buf, pending
    markers = marker if isinstance(marker, tuple) else (marker,)
    text = pending
    end = time.monotonic() + timeout
    s.settimeout(0.2)
    while time.monotonic() < end:
        try:
            chunk = s.recv(4096)
            if not chunk:
                break
            buf += chunk
        except socket.timeout:
            pass
        while len(buf) >= 2:
            n = buf[1] & 0x7F
            off = 2
            if n == 126:
                if len(buf) < 4:
                    break
                n = int.from_bytes(buf[2:4], "big")
                off = 4
            if len(buf) < off + n:
                break
            if buf[0] & 0xF == 1:
                text += buf[off : off + n].decode("utf-8", "replace")
            buf = buf[off + n :]
        found = [(text.index(m) + len(m), m) for m in markers if m in text]
        if found:
            i = min(found)[0]
            text, pending = text[:i], text[i:]
            return text, True
    pending = ""
    return text, False


def to_repl(s):
    """Ctrl-C, then a key if code.py ended at "Press any key to enter the REPL"."""
    send(s, "\x03")
    text, ok = read_until(s, (">>> ", "Press any key"), 10)
    if ok and text.endswith("Press any key"):
        send(s, "\r")
        text, ok = read_until(s, ">>> ", 10)
    return ok


s = connect()
print("ctrl-c -> prompt:", to_repl(s))
send(s, "print(6*7)\r")
t = time.monotonic()
text, ok = read_until(s, "42\r\n", 5)
print("eval 6*7 -> 42:", ok, "%.0f ms" % ((time.monotonic() - t) * 1000))
send(s, "print('x' * 3000)\r")
text, ok = read_until(s, "x" * 3000, 10)
print("3000-char line arrives whole:", ok)
send(s, "\x04")
text, ok = read_until(s, "soft reboot", 10)
print("ctrl-d -> soft reboot seen:", ok)
text, ok = read_until(s, "code.py output:", 15)
print("code.py ran after reload:", ok)
time.sleep(2)
print("same websocket at the REPL again:", to_repl(s))
send(s, "print(7*6+1)\r")
text, ok = read_until(s, "43\r\n", 5)
print("eval after reload -> 43:", ok)
s.close()
