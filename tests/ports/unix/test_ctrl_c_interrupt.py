#!/usr/bin/env python3
"""
Test that Ctrl-C interrupts running code in the REPL.

This test spawns micropython in a PTY, starts a long-running loop,
and sends SIGINT to verify it generates KeyboardInterrupt.
"""

# This test must be run with CPython as it spawns a subprocess
try:
    import pty
except ImportError:
    print("SKIP")
    raise SystemExit

import os
import subprocess
import select
import time
import signal
import sys
import termios


def read_until_quiet(master, timeout=0.05):
    """Read from PTY until no data for timeout seconds."""
    output = b""
    while True:
        ready = select.select([master], [], [], timeout)
        if ready[0]:
            data = os.read(master, 1024)
            if not data:
                break
            output += data
        else:
            break
    return output


def main():
    # Get micropython path from command line or use default
    micropython = sys.argv[1] if len(sys.argv) > 1 else "../ports/unix/build-standard/micropython"

    master, slave = pty.openpty()

    # Ensure the PTY has ISIG enabled (for Ctrl-C to generate SIGINT)
    attrs = termios.tcgetattr(slave)
    attrs[3] |= termios.ISIG  # Enable signal generation
    attrs[6][termios.VINTR] = 3  # Set Ctrl-C as interrupt character
    termios.tcsetattr(slave, termios.TCSANOW, attrs)

    def setup_controlling_terminal():
        """Set up the child process with the PTY as controlling terminal."""
        # Create a new session and set the PTY as the controlling terminal
        os.setsid()
        # Make the PTY the controlling terminal
        import fcntl

        fcntl.ioctl(0, termios.TIOCSCTTY, 0)

    p = subprocess.Popen(
        [micropython],
        stdin=slave,
        stdout=slave,
        stderr=subprocess.STDOUT,
        bufsize=0,
        preexec_fn=setup_controlling_terminal,
    )

    # Wait for banner
    time.sleep(0.2)
    banner = read_until_quiet(master)
    if b"MicroPython" not in banner:
        print("FAIL: No banner")
        return 1

    # Start a long-running operation
    os.write(master, b"import time;time.sleep(30)\n")
    time.sleep(0.1)  # Give it time to start sleeping

    # Send Ctrl-C byte (0x03) through the terminal (not as a signal)
    # This tests that the terminal mode allows SIGINT generation
    os.write(master, b"\x03")

    # Read output
    time.sleep(0.2)
    output = read_until_quiet(master, timeout=0.1)

    # Check for KeyboardInterrupt
    if b"KeyboardInterrupt" in output:
        print("PASS: Ctrl-C interrupted execution")
        success = True
    else:
        print("FAIL: No KeyboardInterrupt found")
        success = False

    # Verify REPL still works
    os.write(master, b"print('OK')\n")
    time.sleep(0.1)
    output = read_until_quiet(master)

    if b"OK" in output:
        print("PASS: REPL functional after interrupt")
    else:
        print("FAIL: REPL not functional")
        success = False

    # Clean up
    os.write(master, b"\x04")  # Ctrl-D to exit
    try:
        p.wait(timeout=1)
    except subprocess.TimeoutExpired:
        p.kill()
    os.close(master)
    os.close(slave)

    return 0 if success else 1


if __name__ == "__main__":
    sys.exit(main())
