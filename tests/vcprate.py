#!/usr/bin/env python
#
# Performance and reliability test for USB CDC.
#
# Usage:
#   vcprate.py [cdc-device]
#
# The `cdc-device` will default to /dev/ttyACM0.

import sys
import time
import argparse
import serial


def drain_input(ser):
    time.sleep(0.1)
    while ser.inWaiting() > 0:
        data = ser.read(ser.inWaiting())
        time.sleep(0.1)


read_test_script = """
vcp_id = %u
led = None
try:
    import pyb
    pyb.LED(1).on()
    led = pyb.LED(2)
    assert pyb.USB_VCP(vcp_id).isconnected()
    wr=pyb.USB_VCP(vcp_id).send
except:
    import sys
    wr=sys.stdout.buffer.write
b=bytearray(%u)
for i in range(len(b)):
    b[i] = i & 0xff
for _ in range(%d):
    if led:
        led.toggle()
    n = wr(b)
"""


def read_test(ser_repl, ser_data, usb_vcp_id, bufsize, nbuf):
    assert bufsize % 256 == 0  # for verify to work

    # Load and run the read_test_script.
    ser_repl.write(b"\x03\x01\x04")  # break, raw-repl, soft-reboot
    drain_input(ser_repl)
    ser_repl.write(bytes(read_test_script % (usb_vcp_id, bufsize, nbuf), "ascii"))
    ser_repl.write(b"\x04")  # eof
    ser_repl.flush()
    response = ser_repl.read(2)
    assert response == b"OK", response

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
            print("ERROR: timeout waiting for data")
            return 0
        to_read = min(ser_data.inWaiting(), remain)
        data = ser_data.read(to_read)
        # verify bytes coming in are in sequence
        # if last_byte is not None:
        #    if data[0] != (last_byte + 1) & 0xff:
        #        print('ERROR: first byte is not in sequence:', last_byte, data[0])
        # last_byte = data[-1]
        # for i in range(1, len(data)):
        #    if data[i] != (data[i - 1] + 1) & 0xff:
        #        print('ERROR: data not in sequence at position %d:' % i, data[i - 1], data[i])
        remain -= len(data)
        print(n, nbuf * bufsize, end="\r")
        total_data[n : n + len(data)] = data
        n += len(data)
    t_end = time.time()
    for i in range(0, len(total_data)):
        if total_data[i] != i & 0xFF:
            print("fail", i, i & 0xFF, total_data[i])
    ser_repl.write(b"\x03")  # break
    t = t_end - t_start

    # Print results.
    print(
        "DATA IN: bufsize=%u, read %u bytes in %.2f msec = %.2f kibytes/sec = %.2f MBits/sec"
        % (bufsize, n, t * 1000, n / 1024 / t, n * 8 / 1000000 / t)
    )

    return t


write_test_script_verified = """
import sys
vcp_id = %u
led=None
try:
    import pyb
    pyb.LED(1).on()
    led=pyb.LED(2)
    assert pyb.USB_VCP(vcp_id).isconnected()
    rd=pyb.USB_VCP(vcp_id).recv
except:
    rd=sys.stdin.readinto
b=bytearray(%u)
for _ in range(%u):
    if led:
        led.toggle()
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
vcp_id = %u
led=None
try:
    import pyb
    pyb.LED(1).on()
    led=pyb.LED(2)
    assert pyb.USB_VCP(vcp_id).isconnected()
    rd=pyb.USB_VCP(vcp_id).recv
except:
    rd=sys.stdin.readinto
b=bytearray(%u)
for _ in range(%u):
    if led:
        led.toggle()
    n = rd(b)
    if n != len(b):
        sys.stdout.write(b'ER%%04u' %% n)
    else:
        sys.stdout.write(b'OK%%04u' %% n)
"""


def write_test(ser_repl, ser_data, usb_vcp_id, bufsize, nbuf, verified):
    # Load and run the write_test_script.
    ser_repl.write(b"\x03\x01\x04")  # break, raw-repl, soft-reboot
    drain_input(ser_repl)
    if verified:
        script = write_test_script_verified
    else:
        script = write_test_script_unverified
    ser_repl.write(bytes(script % (usb_vcp_id, bufsize, nbuf), "ascii"))
    ser_repl.write(b"\x04")  # eof
    ser_repl.flush()
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
        # while ser_data.inWaiting() == 0:
        #    time.sleep(0.001)
        # response = ser_data.read(ser_data.inWaiting())
        response = ser_repl.read(6)
        if response != b"OK%04u" % bufsize:
            print("bad response, expecting OK%04u, got %r" % (bufsize, response))
    t_end = time.time()
    ser_repl.write(b"\x03")  # break
    t = t_end - t_start

    # Print results.
    print(
        "DATA OUT: verify=%d, bufsize=%u, wrote %u bytes in %.2f msec = %.2f kibytes/sec = %.2f MBits/sec"
        % (verified, bufsize, n, t * 1000, n / 1024 / t, n * 8 / 1000000 / t)
    )

    return t


def do_test(dev_repl, dev_data=None, usb_vcp_id=None):
    if dev_data is None:
        print("REPL and data on", dev_repl)
        ser_repl = serial.Serial(dev_repl, baudrate=115200)
        ser_data = ser_repl
        usb_vcp_id = 0
    else:
        print("REPL on", dev_repl)
        print("data on", dev_data)
        print("USB VCP", usb_vcp_id)
        ser_repl = serial.Serial(dev_repl, baudrate=115200)
        ser_data = serial.Serial(dev_data, baudrate=115200)

    if 0:
        for i in range(1000):
            print("======== TEST %04u ========" % i)
            read_test(ser_repl, ser_data, usb_vcp_id, 8000, 32)
            write_test(ser_repl, ser_data, usb_vcp_id, 8000, 32, True)
        return

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

    for bufsize, nbuf in read_test_params:
        t = read_test(ser_repl, ser_data, usb_vcp_id, bufsize, nbuf)
        if t > 4:
            break

    for bufsize, nbuf in write_test_params:
        t = write_test(ser_repl, ser_data, usb_vcp_id, bufsize, nbuf, True)
        if t > 4:
            break

    for bufsize, nbuf in write_test_params:
        t = write_test(ser_repl, ser_data, usb_vcp_id, bufsize, nbuf, False)
        if t > 4:
            break

    ser_repl.close()
    ser_data.close()


def main():
    dev_repl = "/dev/ttyACM0"
    dev_data = None
    usb_vcp_id = None
    if len(sys.argv) >= 2:
        dev_repl = sys.argv[1]
    if len(sys.argv) >= 3:
        assert len(sys.argv) >= 4
        dev_data = sys.argv[2]
        usb_vcp_id = int(sys.argv[3])
    do_test(dev_repl, dev_data, usb_vcp_id)


if __name__ == "__main__":
    main()
