# SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT
"""Host test: web workflow recovers on its own from a wedged ESP32. Runs
esp_wedge.py on the board through run_on_board.py and, from the moment the
board prints WEDGED, polls web workflow until it answers again.
Usage: esp_wedge_host.py <board ip> [runs]"""

import os
import subprocess
import sys
import threading
import time
import urllib.request

ip = sys.argv[1]
runs = int(sys.argv[2]) if len(sys.argv) > 2 else 1
here = os.path.dirname(os.path.abspath(__file__))


def up():
    try:
        with urllib.request.urlopen("http://%s/cp/version.json" % ip, timeout=1) as r:
            return r.status == 200
    except OSError:
        return False


passed = 0
for run in range(runs):
    time.sleep(2)
    print("run %d: web workflow up before: %s" % (run + 1, up()))
    p = subprocess.Popen(
        [
            sys.executable,
            os.path.join(here, "run_on_board.py"),
            os.path.join(here, "esp_wedge.py"),
            "70",
        ],
        stdout=subprocess.PIPE,
        text=True,
    )
    result = {}

    def poll(t0):
        while time.monotonic() - t0 < 40:
            if up():
                result["back"] = time.monotonic() - t0
                return
            time.sleep(0.2)

    poller = None
    board_pass = False
    for line in p.stdout:
        line = line.rstrip()
        if line:
            print("  board:", line)
        board_pass = board_pass or line == "PASS"
        if line == "WEDGED" and poller is None:
            poller = threading.Thread(target=poll, args=(time.monotonic(),))
            poller.start()
    p.wait()
    if poller:
        poller.join()
    print(
        "  web workflow back %s"
        % ("after %.1f s" % result["back"] if "back" in result else "NOT within 40 s")
    )
    passed += board_pass and "back" in result
print("%d of %d passed" % (passed, runs))
