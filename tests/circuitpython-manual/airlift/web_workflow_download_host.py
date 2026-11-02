# SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT
"""Host test: download integrity through web workflow over the ESP32 co-processor.
Uploads one file to /saves (writable over web workflow while the host has
CIRCUITPY mounted), then GETs it n times and checks every body. A bad body is
reported with where it first differs and whether what follows is a duplicate
of earlier data or skips ahead (a hole).
Usage: web_workflow_download_host.py <board ip> <path to settings.toml> <n> [size]"""

import base64
import http.client
import os
import sys
import time

ip, settings, n = sys.argv[1], sys.argv[2], int(sys.argv[3])
size = int(sys.argv[4]) if len(sys.argv) > 4 else 100000
pw = [
    l.split("=", 1)[1].strip().strip('"')
    for l in open(settings)
    if l.strip().startswith("CIRCUITPY_WEB_API_PASSWORD")
][0]
auth = "Basic " + base64.b64encode((":" + pw).encode()).decode()
data = os.urandom(size)


def req(method, path, body=None):
    c = http.client.HTTPConnection(ip, timeout=60)
    c.request(
        method,
        path,
        body=body,
        headers={"Authorization": auth, "X-Timestamp": str(int(time.time() * 1000))},
    )
    r = c.getresponse()
    try:
        b = r.read()
    except http.client.IncompleteRead as e:
        b = e.partial
    c.close()
    return r.status, b


print("PUT", req("PUT", "/fs/saves/airlift_download_test.bin", data)[0], flush=True)
bad = 0
for i in range(n):
    t = time.time()
    try:
        code, b = req("GET", "/fs/saves/airlift_download_test.bin")
    except Exception as e:
        print("%d %.3f %s %s" % (i, t, type(e).__name__, e), flush=True)
        bad += 1
        continue
    if code != 200 or b != data:
        first = next((j for j in range(min(len(b), size)) if b[j] != data[j]), None)
        dup = None
        if first is not None:
            # Is what follows the mismatch a copy of earlier data (duplicate) or later data (hole)?
            probe = b[first : first + 32]
            dup = [j for j in range(max(0, first - 8192), first) if data[j : j + 32] == probe][:1]
            hole = [j for j in range(first, min(size, first + 8192)) if data[j : j + 32] == probe][
                :1
            ]
            dup = "dup from %s, hole to %s" % (dup, hole)
        print(
            "%d %.3f code %d len %d (short %d) first mismatch %s %s"
            % (i, t, code, len(b), size - len(b), first, dup),
            flush=True,
        )
        bad += 1
print("%d of %d bad" % (bad, n), flush=True)
print("PASS" if bad == 0 else "FAIL")
req("DELETE", "/fs/saves/airlift_download_test.bin")
