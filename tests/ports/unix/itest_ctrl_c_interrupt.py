"""
Test that Ctrl-C interrupts running code in the REPL.

This test is run by run-tests.py which sets up a PTY with ISIG enabled
and spawns MicroPython with the PTY as its controlling terminal.

Globals provided by run-tests.py:
- master: PTY master file descriptor
- read_until_quiet: Function to read from PTY until no data for timeout seconds
"""

import os
import time


# Wait for banner
time.sleep(0.2)
banner = read_until_quiet(master)

if b"MicroPython" not in banner:
    print("SKIP")
    raise SystemExit

# Check if time module is available
os.write(master, b"import time\n")
time.sleep(0.05)
check_output = read_until_quiet(master, timeout=0.05)
if b"ImportError" in check_output:
    print("SKIP")
    raise SystemExit

# Start a long-running operation
os.write(master, b"time.sleep(30)\n")
time.sleep(0.1)  # Give it time to start sleeping

# Send Ctrl-C byte (0x03) through the terminal
# This tests that the terminal mode allows SIGINT generation
os.write(master, b"\x03")

# Read output
time.sleep(0.2)
output = read_until_quiet(master, timeout=0.1)

# Check for KeyboardInterrupt
if b"KeyboardInterrupt" in output:
    print("PASS: Ctrl-C interrupted execution")
else:
    print("FAIL: No KeyboardInterrupt found")

# Verify REPL still works
os.write(master, b"print('OK')\n")
time.sleep(0.1)
output = read_until_quiet(master)

if b"OK" in output:
    print("PASS: REPL functional after interrupt")
else:
    print("FAIL: REPL not functional")
