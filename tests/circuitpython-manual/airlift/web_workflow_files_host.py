# SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT
"""Host test: the web workflow file API over the ESP32 co-processor.
Usage: web_workflow_files_host.py <board ip> <path to CIRCUITPY/settings.toml> [upload dir]
Lists /, downloads boot_out.txt and compares it with the copy next to
settings.toml (when the drive is mounted), then uploads a 20 KB file to the
upload dir, reads it back, and deletes it.

The upload dir defaults to /saves/, which stays writable over web workflow while
the host has CIRCUITPY mounted. Pass / on boards without a saves partition, with
CIRCUITPY ejected; otherwise the upload gets 409 "USB storage active"."""

import base64
import json
import os
import sys
import time
import urllib.error
import urllib.request

ip, settings = sys.argv[1], sys.argv[2]
upload_dir = sys.argv[3] if len(sys.argv) > 3 else "/saves/"
upload_dir = "/" + upload_dir.strip("/") + "/" if upload_dir.strip("/") else "/"
password = None
for line in open(settings):
    if line.strip().startswith("CIRCUITPY_WEB_API_PASSWORD"):
        password = line.split("=", 1)[1].strip().strip('"')
auth = "Basic " + base64.b64encode((":" + password).encode()).decode()


def req(method, path, data=None, headers=None):
    h = {"Authorization": auth}
    h.update(headers or {})
    r = urllib.request.Request("http://%s%s" % (ip, path), data=data, method=method, headers=h)
    t = time.monotonic()
    try:
        with urllib.request.urlopen(r, timeout=30) as resp:
            body = resp.read()
            code = resp.status
    except urllib.error.HTTPError as e:
        body, code = e.read(), e.code
    return code, body, time.monotonic() - t


results = []

code, body, dt = req("GET", "/fs/", headers={"Accept": "application/json"})
listing = json.loads(body) if code == 200 else {"files": []}
print("list /: %d, %d entries, %.2f s" % (code, len(listing["files"]), dt))
results.append(code == 200)

code, body, dt = req("GET", "/fs/boot_out.txt")
local = os.path.join(os.path.dirname(settings), "boot_out.txt")
same = open(local, "rb").read() == body if os.path.exists(local) else None
print("get boot_out.txt: %d, %d bytes, %.2f s, matches local: %s" % (code, len(body), dt, same))
results.append(code == 200 and same is not False)

path = "/fs" + upload_dir + "airlift_upload_test.bin"
payload = bytes((i * 7 + (i >> 8)) & 0xFF for i in range(20000))
code, body, dt = req(
    "PUT", path, data=payload, headers={"X-Timestamp": str(int(time.time() * 1000))}
)
print("put 20000 bytes: %d, %.2f s (%.1f KB/s) %s" % (code, dt, 20000 / 1024 / dt, body[:80]))
results.append(code in (201, 204))
code, body, dt = req("GET", path)
print("read back: %d, %d bytes, matches: %s" % (code, len(body), body == payload))
results.append(code == 200 and body == payload)
code, body, dt = req("DELETE", path)
print("delete: %d" % code)
results.append(code == 204)
print("PASS" if all(results) else "FAIL")
