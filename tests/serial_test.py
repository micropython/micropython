#!/usr/bin/env python
#
# Performance and reliability test for serial port communication.
#
# Basic usage:
#   serial_test.py [-t serial-device]
#
# The `serial-device` will default to /dev/ttyACM0.

import argparse
import serial
import sys
import time

run_tests_module = __import__("run-tests")

read_test_script = """
bin = True
try:
    wr=__import__("pyb").USB_VCP(0).send
except:
    import sys
    if hasattr(sys.stdout,'buffer'):
        wr=sys.stdout.buffer.write
    else:
        wr=sys.stdout.write
        bin = False
b=bytearray(%u)
if bin:
    wr('BIN')
    for i in range(len(b)):
        b[i] = i & 0xff
else:
    wr('TXT')
    for i in range(len(b)):
        b[i] = 0x20 + (i & 0x3f)
for _ in range(%d):
    wr(b)
"""


write_test_script_verified = """
import sys
try:
    rd=__import__("pyb").USB_VCP(0).recv
except:
    rd=sys.stdin.readinto
b=bytearray(%u)
for _ in range(%u):
    n = rd(b)
    fail = 0
    for i in range(n):
        if b[i] != 32 + (i & 0x3f):
            fail += 1
    if fail:
        sys.stdout.write(b'ER%%05u' %% fail)
    else:
        sys.stdout.write(b'OK%%05u' %% n)
"""

write_test_script_unverified = """
import sys
try:
    rd=__import__("pyb").USB_VCP(0).recv
except:
    rd=sys.stdin.readinto
b=bytearray(%u)
for _ in range(%u):
    n = rd(b)
    if n != len(b):
        sys.stdout.write(b'ER%%05u' %% n)
    else:
        sys.stdout.write(b'OK%%05u' %% n)
"""


class TestError(Exception):
    pass


def drain_input(ser):
    time.sleep(0.1)
    while ser.inWaiting() > 0:
        data = ser.read(ser.inWaiting())
        time.sleep(0.1)


def send_script(ser, script):
    chunk_size = 32
    for i in range(0, len(script), chunk_size):
        ser.write(script[i : i + chunk_size])
        time.sleep(0.01)
    ser.write(b"\x04")  # eof
    ser.flush()
    response = ser.read(2)
    if response != b"OK":
        response += ser.read(ser.inWaiting())
        raise TestError("could not send script", response)


def read_test(ser_repl, ser_data, bufsize, nbuf):
    global test_passed

    assert bufsize % 256 == 0  # for verify to work

    # how long to wait for data from device
    # (if UART TX is also enabled then it can take 1.4s to send
    # out a 16KB butter at 115200bps)
    READ_TIMEOUT_S = 2

    # Load and run the read_test_script.
    ser_repl.write(b"\x03\x01\x04")  # break, raw-repl, soft-reboot
    drain_input(ser_repl)
    script = bytes(read_test_script % (bufsize, nbuf), "ascii")
    send_script(ser_repl, script)

    # Read from the device the type of data that it will send (BIN or TXT).
    data_type = ser_data.read(3)

    # Read data from the device, check it is correct, and measure throughput.
    n = 0
    last_byte = None
    t_start = time.time()
    remain = nbuf * bufsize
    total_data = bytearray(remain)
    while remain:
        t0 = time.monotonic_ns()
        while ser_data.inWaiting() == 0:
            if time.monotonic_ns() - t0 > READ_TIMEOUT_S * 1e9:
                # timeout waiting for data from device
                break
            time.sleep(0.0001)
        if not ser_data.inWaiting():
            test_passed = False
            print("ERROR: timeout waiting for data")
            print(total_data[:n])
            return 0
        to_read = min(ser_data.inWaiting(), remain)
        data = ser_data.read(to_read)
        remain -= len(data)
        print(f"{n} / {nbuf * bufsize}", end="\r")
        total_data[n : n + len(data)] = data
        n += len(data)
    t_end = time.time()
    for i in range(0, len(total_data)):
        if data_type == b"BIN":
            wanted = i & 0xFF
        else:
            wanted = 0x20 + (i & 0x3F)
        if total_data[i] != wanted:
            test_passed = False
            print("ERROR: data mismatch:", i, wanted, total_data[i])
    ser_repl.write(b"\x03")  # break
    t = t_end - t_start

    # Print results.
    print(
        "DATA IN: bufsize=%u, read %u bytes in %.2f msec = %.2f kibytes/sec = %.2f MBits/sec"
        % (bufsize, n, t * 1000, n / 1024 / t, n * 8 / 1000000 / t)
    )

    return n / t


def write_test(ser_repl, ser_data, bufsize, nbuf, verified):
    global test_passed

    # Load and run the write_test_script.
    ser_repl.write(b"\x03\x01\x04")  # break, raw-repl, soft-reboot
    drain_input(ser_repl)
    if verified:
        script = write_test_script_verified
    else:
        script = write_test_script_unverified
    script = bytes(script % (bufsize, nbuf), "ascii")
    send_script(ser_repl, script)
    drain_input(ser_repl)

    # Write data to the device, check it is correct, and measure throughput.
    n = 0
    t_start = time.time()
    buf = bytearray(bufsize)
    for i in range(len(buf)):
        buf[i] = 32 + (i & 0x3F)  # don't want to send ctrl chars!
    for i in range(nbuf):
        ser_data.write(buf)
        n += len(buf)
        print(f"{n} / {nbuf * bufsize}", end="\r")
        response = ser_repl.read(7)
        if response != b"OK%05u" % bufsize:
            test_passed = False
            print("ERROR: bad response, expecting OK%05u, got %r" % (bufsize, response))
    t_end = time.time()
    ser_repl.write(b"\x03")  # break
    t = t_end - t_start

    # Print results.
    print(
        "DATA OUT: verify=%d, bufsize=%u, wrote %u bytes in %.2f msec = %.2f kibytes/sec = %.2f MBits/sec"
        % (verified, bufsize, n, t * 1000, n / 1024 / t, n * 8 / 1000000 / t)
    )

    return n / t


def do_test(dev_repl, dev_data=None, time_per_subtest=1):
    if dev_data is None:
        print("REPL and data on", dev_repl)
        ser_repl = serial.Serial(dev_repl, baudrate=115200, timeout=1)
        ser_data = ser_repl
    else:
        print("REPL on", dev_repl)
        print("data on", dev_data)
        ser_repl = serial.Serial(dev_repl, baudrate=115200, timeout=1)
        ser_data = serial.Serial(dev_data, baudrate=115200, timeout=1)

    for test_func, test_args, bufsize in (
        (read_test, (), 256),
        (write_test, (True,), 128),
        (write_test, (False,), 128),
    ):
        nbuf = 128
        while bufsize <= 16384:
            rate = test_func(ser_repl, ser_data, bufsize, nbuf, *test_args)
            bufsize *= 2
            if rate:
                # Adjust the amount of data based on the rate, to keep each subtest
                # at around time_per_subtest seconds long.
                nbuf = max(min(128, int(rate * time_per_subtest / bufsize)), 1)

    ser_repl.close()
    ser_data.close()


def main():
    global test_passed

    cmd_parser = argparse.ArgumentParser(
        description="Test performance and reliability of serial port communication.",
        epilog=run_tests_module.test_instance_epilog,
        formatter_class=argparse.RawTextHelpFormatter,
    )
    cmd_parser.add_argument(
        "-t",
        "--test-instance",
        default="a0",
        help="MicroPython instance to test",
    )
    cmd_parser.add_argument(
        "--time-per-subtest", default="1", help="approximate time to take per subtest (in seconds)"
    )
    args = cmd_parser.parse_args()

    dev_repl = run_tests_module.convert_device_shortcut_to_real_device(args.test_instance)

    test_passed = True
    try:
        do_test(dev_repl, None, float(args.time_per_subtest))
    except TestError as er:
        test_passed = False
        print("ERROR:", er)

    if not test_passed:
        sys.exit(1)


if __name__ == "__main__":
    main()
