#!/usr/bin/env python
#
# Performance and reliability test for serial port communication.
#
# Usage:
#   serial_test.py [serial-device]
#
# The `serial-device` will default to /dev/ttyACM0.

import argparse
import serial
import sys
import time

# Stop running the test when it takes longer than this.
TIME_LIMIT_S = 4

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
        sys.stdout.write(b'ER%%04u' %% fail)
    else:
        sys.stdout.write(b'OK%%04u' %% n)
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
        sys.stdout.write(b'ER%%04u' %% n)
    else:
        sys.stdout.write(b'OK%%04u' %% n)
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
            if time.monotonic_ns() - t0 > 1e9:
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
        print(n, nbuf * bufsize, end="\r")
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

    return t


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
        print(n, nbuf * bufsize, end="\r")
        response = ser_repl.read(6)
        if response != b"OK%04u" % bufsize:
            test_passed = False
            print("ERROR: bad response, expecting OK%04u, got %r" % (bufsize, response))
    t_end = time.time()
    ser_repl.write(b"\x03")  # break
    t = t_end - t_start

    # Print results.
    print(
        "DATA OUT: verify=%d, bufsize=%u, wrote %u bytes in %.2f msec = %.2f kibytes/sec = %.2f MBits/sec"
        % (verified, bufsize, n, t * 1000, n / 1024 / t, n * 8 / 1000000 / t)
    )

    return t


def do_test(dev_repl, dev_data=None, fast=False):
    if dev_data is None:
        print("REPL and data on", dev_repl)
        ser_repl = serial.Serial(dev_repl, baudrate=115200, timeout=1)
        ser_data = ser_repl
    else:
        print("REPL on", dev_repl)
        print("data on", dev_data)
        ser_repl = serial.Serial(dev_repl, baudrate=115200, timeout=1)
        ser_data = serial.Serial(dev_data, baudrate=115200, timeout=1)

    read_test_params = [
        (256, 128),
        (512, 64),
        (1024, 64),
        (2048, 64),
        (4096, 64),
        (8192, 64),
        (16384, 64),
    ]
    write_test_params = [
        (128, 32),
        (256, 16),
        (512, 16),
        (1024, 16),
        (2048, 16),
        (4096, 16),
        (8192, 32),
        (9999, 64),
    ]

    if fast:
        read_test_params = (read_test_params[-1],)
        write_test_params = (write_test_params[-1],)

    for bufsize, nbuf in read_test_params:
        t = read_test(ser_repl, ser_data, bufsize, nbuf)
        if t > TIME_LIMIT_S:
            break

    for bufsize, nbuf in write_test_params:
        t = write_test(ser_repl, ser_data, bufsize, nbuf, True)
        if t > TIME_LIMIT_S:
            break

    for bufsize, nbuf in write_test_params:
        t = write_test(ser_repl, ser_data, bufsize, nbuf, False)
        if t > TIME_LIMIT_S:
            break

    ser_repl.close()
    ser_data.close()


def main():
    global test_passed

    cmd_parser = argparse.ArgumentParser(
        description="Performance and reliability test for serial port communication",
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
        "--quick", action="store_true", help="only test with the largest chunk size"
    )
    args = cmd_parser.parse_args()

    dev_repl = run_tests_module.convert_device_shortcut_to_real_device(args.test_instance)

    test_passed = True
    try:
        do_test(dev_repl, None, args.quick)
    except TestError as er:
        test_passed = False
        print("ERROR:", er)

    if not test_passed:
        sys.exit(1)


if __name__ == "__main__":
    main()
